/*
 * Copyright (c) 1995-1998 John Birrell <jb@cimlogic.com.au>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by John Birrell.
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JOHN BIRRELL AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _THREAD_SAFE
#include <pthread.h>
#include "pthread_private.h"

void _exit(int status)
{
	int		flags;
	int             i;
	struct itimerval itimer;

	/* Disable the interval timer: */
	itimer.it_interval.tv_sec  = 0;
	itimer.it_interval.tv_usec = 0;
	itimer.it_value.tv_sec     = 0;
	itimer.it_value.tv_usec    = 0;
	setitimer(_ITIMER_SCHED_TIMER, &itimer, NULL);

	/* Close the pthread kernel pipe: */
	_thread_sys_close(_thread_kern_pipe[0]);
	_thread_sys_close(_thread_kern_pipe[1]);

	/*
	 * Enter a loop to set all file descriptors to blocking
	 * if they were not created as non-blocking:
	 */
	for (i = 0; i < _thread_dtablesize; i++) {
		/* Check if this file descriptor is in use: */
		if (_thread_fd_table[i] != NULL &&
			!(_thread_fd_table[i]->flags & O_NONBLOCK)) {
			/* Get the current flags: */
			flags = _thread_sys_fcntl(i, F_GETFL, NULL);
			/* Clear the nonblocking file descriptor flag: */
			_thread_sys_fcntl(i, F_SETFL, flags & ~O_NONBLOCK);
		}
	}

	/* Call the _exit syscall: */
	_thread_sys__exit(status);
}

void
_thread_exit(char *fname, int lineno, char *string)
{
	char            s[256];

	/* Prepare an error message string: */
	strcpy(s, "Fatal error '");
	strcat(s, string);
	strcat(s, "' at line ? ");
	strcat(s, "in file ");
	strcat(s, fname);
	strcat(s, " (errno = ?");
	strcat(s, ")\n");

	/* Write the string to the standard error file descriptor: */
	_thread_sys_write(2, s, strlen(s));

	/* Force this process to exit: */
	/* XXX - Do we want abort to be conditional on _PTHREADS_INVARIANTS? */
#if defined(_PTHREADS_INVARIANTS)
	abort();
#else
	_exit(1);
#endif
}

/*
 * Only called when a thread is cancelled.  It may be more useful
 * to call it from pthread_exit() if other ways of asynchronous or
 * abnormal thread termination can be found.
 */
void
_thread_exit_cleanup(void)
{
	/*
	 * POSIX states that cancellation/termination of a thread should
	 * not release any visible resources (such as mutexes) and that
	 * it is the applications responsibility.  Resources that are
	 * internal to the threads library, including file and fd locks,
	 * are not visible to the application and need to be released.
	 */
	/* Unlock all owned fd locks: */
	_thread_fd_unlock_owned(_thread_run);

	/* Unlock all owned file locks: */
	_funlock_owned(_thread_run);

	/* Unlock all private mutexes: */
	_mutex_unlock_private(_thread_run);

	/*
	 * This still isn't quite correct because we don't account
	 * for held spinlocks (see libc/stdlib/malloc.c).
	 */
}

void
pthread_exit(void *status)
{
	pthread_t pthread;

	/* Check if this thread is already in the process of exiting: */
	if ((_thread_run->flags & PTHREAD_EXITING) != 0) {
		char msg[128];
		snprintf(msg, sizeof(msg), "Thread %p has called pthread_exit() from a destructor. POSIX 1003.1 1996 s16.2.5.2 does not allow this!",_thread_run);
		PANIC(msg);
	}

	/* Flag this thread as exiting: */
	_thread_run->flags |= PTHREAD_EXITING;

	/* Save the return value: */
	_thread_run->ret = status;

	while (_thread_run->cleanup != NULL) {
		pthread_cleanup_pop(1);
	}

	if (_thread_run->attr.cleanup_attr != NULL) {
		_thread_run->attr.cleanup_attr(_thread_run->attr.arg_attr);
	}
	/* Check if there is thread specific data: */
	if (_thread_run->specific_data != NULL) {
		/* Run the thread-specific data destructors: */
		_thread_cleanupspecific();
	}

	/* Free thread-specific poll_data structure, if allocated: */
	if (_thread_run->poll_data.fds != NULL) {
		free(_thread_run->poll_data.fds);
		_thread_run->poll_data.fds = NULL;
	}

	/*
	 * Defer signals to protect the scheduling queues from access
	 * by the signal handler:
	 */
	_thread_kern_sig_defer();

	/* Check if there are any threads joined to this one: */
	while ((pthread = TAILQ_FIRST(&(_thread_run->join_queue))) != NULL) {
		/* Remove the thread from the queue: */
		TAILQ_REMOVE(&_thread_run->join_queue, pthread, qe);

		/* Wake the joined thread and let it detach this thread: */
		PTHREAD_NEW_STATE(pthread,PS_RUNNING);
	}

	/*
	 * Undefer and handle pending signals, yielding if necessary:
	 */
	_thread_kern_sig_undefer();

	/*
	 * Lock the garbage collector mutex to ensure that the garbage
	 * collector is not using the dead thread list.
	 */
	if (pthread_mutex_lock(&_gc_mutex) != 0)
		PANIC("Cannot lock gc mutex");

	/* Add this thread to the list of dead threads. */
	TAILQ_INSERT_HEAD(&_dead_list, _thread_run, dle);

	/*
	 * Defer signals to protect the scheduling queues from access
	 * by the signal handler:
	 */
	_thread_kern_sig_defer();

	/* Remove this thread from the thread list: */
	TAILQ_REMOVE(&_thread_list, _thread_run, tle);

	/*
	 * Undefer and handle pending signals, yielding if necessary:
	 */
	_thread_kern_sig_undefer();

	/*
	 * Signal the garbage collector thread that there is something
	 * to clean up.
	 */
	if (pthread_cond_signal(&_gc_cond) != 0)
		PANIC("Cannot signal gc cond");

	/*
	 * Mark the thread as dead so it will not return if it
	 * gets context switched out when the mutex is unlocked.
	 */
	PTHREAD_SET_STATE(_thread_run, PS_DEAD);

	/* Unlock the garbage collector mutex: */
	if (pthread_mutex_unlock(&_gc_mutex) != 0)
		PANIC("Cannot lock gc mutex");

	/* This this thread will never be re-scheduled. */
	_thread_kern_sched(NULL);

	/* This point should not be reached. */
	PANIC("Dead thread has resumed");
}
#endif
