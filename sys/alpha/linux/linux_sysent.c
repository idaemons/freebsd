/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 * created from FreeBSD: src/sys/alpha/linux/syscalls.master,v 1.40.2.6 2001/10/05 07:34:36 peter Exp 
 */

#include "opt_compat.h"
#include <sys/param.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>
#include <alpha/linux/linux.h>
#include <linux_proto.h>

#define AS(name) (sizeof(struct name) / sizeof(register_t))

/* The casts are bogus but will do for now. */
struct sysent linux_sysent[] = {
	{ 0, (sy_call_t *)nosys },			/* 0 =  */
	{ AS(sys_exit_args), (sy_call_t *)sys_exit },	/* 1 = exit */
	{ 0, (sy_call_t *)linux_fork },			/* 2 = linux_fork */
	{ AS(read_args), (sy_call_t *)read },		/* 3 = read */
	{ AS(write_args), (sy_call_t *)write },		/* 4 = write */
	{ 0, (sy_call_t *)nosys },			/* 5 =  */
	{ AS(close_args), (sy_call_t *)close },		/* 6 = close */
	{ AS(osf1_wait4_args), (sy_call_t *)osf1_wait4 },	/* 7 = osf1_wait4 */
	{ 0, (sy_call_t *)nosys },			/* 8 =  */
	{ AS(linux_link_args), (sy_call_t *)linux_link },	/* 9 = linux_link */
	{ AS(linux_unlink_args), (sy_call_t *)linux_unlink },	/* 10 = linux_unlink */
	{ 0, (sy_call_t *)nosys },			/* 11 =  */
	{ AS(linux_chdir_args), (sy_call_t *)linux_chdir },	/* 12 = linux_chdir */
	{ AS(fchdir_args), (sy_call_t *)fchdir },	/* 13 = fchdir */
	{ AS(linux_mknod_args), (sy_call_t *)linux_mknod },	/* 14 = linux_mknod */
	{ AS(linux_chmod_args), (sy_call_t *)linux_chmod },	/* 15 = linux_chmod */
	{ AS(linux_chown_args), (sy_call_t *)linux_chown },	/* 16 = linux_chown */
	{ AS(linux_brk_args), (sy_call_t *)linux_brk },	/* 17 = linux_brk */
	{ 0, (sy_call_t *)nosys },			/* 18 =  */
	{ AS(linux_lseek_args), (sy_call_t *)linux_lseek },	/* 19 = linux_lseek */
	{ 0, (sy_call_t *)getpid },			/* 20 = getpid */
	{ 0, (sy_call_t *)nosys },			/* 21 =  */
	{ 0, (sy_call_t *)linux_umount },		/* 22 = linux_umount */
	{ AS(setuid_args), (sy_call_t *)setuid },	/* 23 = setuid */
	{ 0, (sy_call_t *)getuid },			/* 24 = getuid */
	{ 0, (sy_call_t *)nosys },			/* 25 =  */
	{ 0, (sy_call_t *)linux_ptrace },		/* 26 = linux_ptrace */
	{ 0, (sy_call_t *)nosys },			/* 27 =  */
	{ 0, (sy_call_t *)nosys },			/* 28 =  */
	{ 0, (sy_call_t *)nosys },			/* 29 =  */
	{ 0, (sy_call_t *)nosys },			/* 30 =  */
	{ 0, (sy_call_t *)nosys },			/* 31 =  */
	{ 0, (sy_call_t *)nosys },			/* 32 =  */
	{ AS(linux_access_args), (sy_call_t *)linux_access },	/* 33 = linux_access */
	{ 0, (sy_call_t *)nosys },			/* 34 =  */
	{ 0, (sy_call_t *)nosys },			/* 35 =  */
	{ 0, (sy_call_t *)sync },			/* 36 = sync */
	{ AS(linux_kill_args), (sy_call_t *)linux_kill },	/* 37 = linux_kill */
	{ 0, (sy_call_t *)nosys },			/* 38 =  */
	{ AS(setpgid_args), (sy_call_t *)setpgid },	/* 39 = setpgid */
	{ 0, (sy_call_t *)nosys },			/* 40 =  */
	{ AS(dup_args), (sy_call_t *)dup },		/* 41 = dup */
	{ 0, (sy_call_t *)pipe },			/* 42 = pipe */
	{ 0, (sy_call_t *)nosys },			/* 43 =  */
	{ 0, (sy_call_t *)nosys },			/* 44 =  */
	{ AS(linux_open_args), (sy_call_t *)linux_open },	/* 45 = linux_open */
	{ 0, (sy_call_t *)nosys },			/* 46 =  */
	{ 0, (sy_call_t *)getgid },			/* 47 = getgid */
	{ AS(osf1_sigprocmask_args), (sy_call_t *)osf1_sigprocmask },	/* 48 = osf1_sigprocmask */
	{ 0, (sy_call_t *)nosys },			/* 49 =  */
	{ 0, (sy_call_t *)nosys },			/* 50 =  */
	{ AS(acct_args), (sy_call_t *)acct },		/* 51 = acct */
	{ 0, (sy_call_t *)nosys },			/* 52 =  */
	{ 0, (sy_call_t *)nosys },			/* 53 =  */
	{ AS(linux_ioctl_args), (sy_call_t *)linux_ioctl },	/* 54 = linux_ioctl */
	{ 0, (sy_call_t *)nosys },			/* 55 =  */
	{ 0, (sy_call_t *)nosys },			/* 56 =  */
	{ AS(linux_symlink_args), (sy_call_t *)linux_symlink },	/* 57 = linux_symlink */
	{ AS(linux_readlink_args), (sy_call_t *)linux_readlink },	/* 58 = linux_readlink */
	{ AS(linux_execve_args), (sy_call_t *)linux_execve },	/* 59 = linux_execve */
	{ AS(umask_args), (sy_call_t *)umask },		/* 60 = umask */
	{ AS(chroot_args), (sy_call_t *)chroot },	/* 61 = chroot */
	{ 0, (sy_call_t *)nosys },			/* 62 =  */
	{ 0, (sy_call_t *)getpgrp },			/* 63 = getpgrp */
	{ 0, (sy_call_t *)nosys },			/* 64 = getpagesize */
	{ 0, (sy_call_t *)nosys },			/* 65 =  */
	{ 0, (sy_call_t *)linux_vfork },		/* 66 = linux_vfork */
	{ AS(linux_newstat_args), (sy_call_t *)linux_newstat },	/* 67 = linux_newstat */
	{ AS(linux_newlstat_args), (sy_call_t *)linux_newlstat },	/* 68 = linux_newlstat */
	{ 0, (sy_call_t *)nosys },			/* 69 =  */
	{ 0, (sy_call_t *)nosys },			/* 70 =  */
	{ AS(linux_mmap_args), (sy_call_t *)linux_mmap },	/* 71 = linux_mmap */
	{ 0, (sy_call_t *)nosys },			/* 72 =  */
	{ AS(linux_munmap_args), (sy_call_t *)linux_munmap },	/* 73 = linux_munmap */
	{ AS(linux_mprotect_args), (sy_call_t *)linux_mprotect },	/* 74 = linux_mprotect */
	{ 0, (sy_call_t *)nosys },			/* 75 = madvise */
	{ 0, (sy_call_t *)linux_vhangup },		/* 76 = linux_vhangup */
	{ 0, (sy_call_t *)nosys },			/* 77 =  */
	{ 0, (sy_call_t *)nosys },			/* 78 =  */
	{ AS(linux_setgroups_args), (sy_call_t *)linux_setgroups },	/* 79 = linux_setgroups */
	{ AS(linux_getgroups_args), (sy_call_t *)linux_getgroups },	/* 80 = linux_getgroups */
	{ 0, (sy_call_t *)nosys },			/* 81 =  */
	{ AS(linux_setpgid_args), (sy_call_t *)linux_setpgid },	/* 82 = linux_setpgid */
	{ AS(osf1_setitimer_args), (sy_call_t *)osf1_setitimer },	/* 83 = osf1_setitimer */
	{ 0, (sy_call_t *)nosys },			/* 84 =  */
	{ 0, (sy_call_t *)nosys },			/* 85 =  */
	{ 0, (sy_call_t *)nosys },			/* 86 =  */
	{ 0, (sy_call_t *)nosys },			/* 87 = gethostname */
	{ AS(sethostname_args), (sy_call_t *)osethostname },	/* 88 = osethostname */
	{ 0, (sy_call_t *)nosys },			/* 89 = getdtablesize */
	{ AS(dup2_args), (sy_call_t *)dup2 },		/* 90 = dup2 */
	{ AS(linux_newfstat_args), (sy_call_t *)linux_newfstat },	/* 91 = linux_newfstat */
	{ AS(linux_fcntl_args), (sy_call_t *)linux_fcntl },	/* 92 = linux_fcntl */
	{ AS(osf1_select_args), (sy_call_t *)osf1_select },	/* 93 = osf1_select */
	{ AS(poll_args), (sy_call_t *)poll },		/* 94 = poll */
	{ AS(fsync_args), (sy_call_t *)fsync },		/* 95 = fsync */
	{ AS(setpriority_args), (sy_call_t *)setpriority },	/* 96 = setpriority */
	{ AS(osf1_socket_args), (sy_call_t *)osf1_socket },	/* 97 = osf1_socket */
	{ AS(linux_connect_args), (sy_call_t *)linux_connect },	/* 98 = linux_connect */
	{ AS(accept_args), (sy_call_t *)oaccept },	/* 99 = accept */
	{ 0, (sy_call_t *)nosys },			/* 100 =  */
	{ AS(osend_args), (sy_call_t *)osend },		/* 101 = osend */
	{ AS(orecv_args), (sy_call_t *)orecv },		/* 102 = orecv */
	{ AS(osf1_sigreturn_args), (sy_call_t *)osf1_sigreturn },	/* 103 = osf1_sigreturn */
	{ AS(bind_args), (sy_call_t *)bind },		/* 104 = bind */
	{ AS(setsockopt_args), (sy_call_t *)setsockopt },	/* 105 = setsockopt */
	{ AS(listen_args), (sy_call_t *)listen },	/* 106 = listen */
	{ 0, (sy_call_t *)nosys },			/* 107 =  */
	{ 0, (sy_call_t *)nosys },			/* 108 =  */
	{ 0, (sy_call_t *)nosys },			/* 109 =  */
	{ 0, (sy_call_t *)nosys },			/* 110 =  */
	{ AS(osf1_sigsuspend_args), (sy_call_t *)osf1_sigsuspend },	/* 111 = osf1_sigsuspend */
	{ 0, (sy_call_t *)nosys },			/* 112 =  */
	{ 0, (sy_call_t *)nosys },			/* 113 = recvmsg */
	{ 0, (sy_call_t *)nosys },			/* 114 = sendmsg */
	{ 0, (sy_call_t *)nosys },			/* 115 =  */
	{ AS(osf1_gettimeofday_args), (sy_call_t *)osf1_gettimeofday },	/* 116 = osf1_gettimeofday */
	{ AS(osf1_getrusage_args), (sy_call_t *)osf1_getrusage },	/* 117 = osf1_getrusage */
	{ AS(getsockopt_args), (sy_call_t *)getsockopt },	/* 118 = getsockopt */
	{ 0, (sy_call_t *)nosys },			/* 119 =  */
	{ AS(readv_args), (sy_call_t *)readv },		/* 120 = readv */
	{ AS(writev_args), (sy_call_t *)writev },	/* 121 = writev */
	{ 0, (sy_call_t *)nosys },			/* 122 =  */
	{ AS(fchown_args), (sy_call_t *)fchown },	/* 123 = fchown */
	{ AS(fchmod_args), (sy_call_t *)fchmod },	/* 124 = fchmod */
	{ AS(recvfrom_args), (sy_call_t *)orecvfrom },	/* 125 = recvfrom */
	{ AS(setreuid_args), (sy_call_t *)setreuid },	/* 126 = setreuid */
	{ AS(setregid_args), (sy_call_t *)setregid },	/* 127 = setregid */
	{ AS(linux_rename_args), (sy_call_t *)linux_rename },	/* 128 = linux_rename */
	{ AS(linux_truncate_args), (sy_call_t *)linux_truncate },	/* 129 = linux_truncate */
	{ AS(oftruncate_args), (sy_call_t *)oftruncate },	/* 130 = oftruncate */
	{ AS(flock_args), (sy_call_t *)flock },		/* 131 = flock */
	{ AS(setgid_args), (sy_call_t *)setgid },	/* 132 = setgid */
	{ AS(osf1_sendto_args), (sy_call_t *)osf1_sendto },	/* 133 = osf1_sendto */
	{ AS(shutdown_args), (sy_call_t *)shutdown },	/* 134 = shutdown */
	{ 0, (sy_call_t *)nosys },			/* 135 = socketpair */
	{ AS(linux_mkdir_args), (sy_call_t *)linux_mkdir },	/* 136 = linux_mkdir */
	{ AS(linux_rmdir_args), (sy_call_t *)linux_rmdir },	/* 137 = linux_rmdir */
	{ AS(utimes_args), (sy_call_t *)utimes },	/* 138 = utimes */
	{ 0, (sy_call_t *)nosys },			/* 139 =  */
	{ 0, (sy_call_t *)nosys },			/* 140 =  */
	{ AS(ogetpeername_args), (sy_call_t *)ogetpeername },	/* 141 = ogetpeername */
	{ 0, (sy_call_t *)nosys },			/* 142 =  */
	{ 0, (sy_call_t *)nosys },			/* 143 =  */
	{ AS(linux_getrlimit_args), (sy_call_t *)linux_getrlimit },	/* 144 = linux_getrlimit */
	{ AS(linux_setrlimit_args), (sy_call_t *)linux_setrlimit },	/* 145 = linux_setrlimit */
	{ 0, (sy_call_t *)nosys },			/* 146 =  */
	{ 0, (sy_call_t *)setsid },			/* 147 = setsid */
	{ 0, (sy_call_t *)linux_quotactl },		/* 148 = linux_quotactl */
	{ 0, (sy_call_t *)nosys },			/* 149 =  */
	{ AS(getsockname_args), (sy_call_t *)ogetsockname },	/* 150 = getsockname */
	{ 0, (sy_call_t *)nosys },			/* 151 =  */
	{ 0, (sy_call_t *)nosys },			/* 152 =  */
	{ 0, (sy_call_t *)nosys },			/* 153 =  */
	{ 0, (sy_call_t *)nosys },			/* 154 =  */
	{ 0, (sy_call_t *)nosys },			/* 155 =  */
	{ AS(osf1_sigaction_args), (sy_call_t *)osf1_sigaction },	/* 156 = osf1_sigaction */
	{ 0, (sy_call_t *)nosys },			/* 157 =  */
	{ 0, (sy_call_t *)nosys },			/* 158 =  */
	{ 0, (sy_call_t *)nosys },			/* 159 =  */
	{ 0, (sy_call_t *)nosys },			/* 160 =  */
	{ 0, (sy_call_t *)nosys },			/* 161 =  */
	{ 0, (sy_call_t *)nosys },			/* 162 =  */
	{ 0, (sy_call_t *)nosys },			/* 163 =  */
	{ 0, (sy_call_t *)nosys },			/* 164 =  */
	{ 0, (sy_call_t *)nosys },			/* 165 =  */
	{ AS(setdomainname_args), (sy_call_t *)setdomainname },	/* 166 = setdomainname */
	{ 0, (sy_call_t *)nosys },			/* 167 =  */
	{ 0, (sy_call_t *)nosys },			/* 168 =  */
	{ 0, (sy_call_t *)nosys },			/* 169 =  */
	{ 0, (sy_call_t *)nosys },			/* 170 =  */
	{ 0, (sy_call_t *)nosys },			/* 171 =  */
	{ 0, (sy_call_t *)nosys },			/* 172 =  */
	{ 0, (sy_call_t *)nosys },			/* 173 =  */
	{ 0, (sy_call_t *)nosys },			/* 174 =  */
	{ 0, (sy_call_t *)nosys },			/* 175 =  */
	{ 0, (sy_call_t *)nosys },			/* 176 =  */
	{ 0, (sy_call_t *)nosys },			/* 177 =  */
	{ 0, (sy_call_t *)nosys },			/* 178 =  */
	{ 0, (sy_call_t *)nosys },			/* 179 =  */
	{ 0, (sy_call_t *)nosys },			/* 180 =  */
	{ 0, (sy_call_t *)nosys },			/* 181 =  */
	{ 0, (sy_call_t *)nosys },			/* 182 =  */
	{ 0, (sy_call_t *)nosys },			/* 183 =  */
	{ 0, (sy_call_t *)nosys },			/* 184 =  */
	{ 0, (sy_call_t *)nosys },			/* 185 =  */
	{ 0, (sy_call_t *)nosys },			/* 186 =  */
	{ 0, (sy_call_t *)nosys },			/* 187 =  */
	{ 0, (sy_call_t *)nosys },			/* 188 =  */
	{ 0, (sy_call_t *)nosys },			/* 189 =  */
	{ 0, (sy_call_t *)nosys },			/* 190 =  */
	{ 0, (sy_call_t *)nosys },			/* 191 =  */
	{ 0, (sy_call_t *)nosys },			/* 192 =  */
	{ 0, (sy_call_t *)nosys },			/* 193 =  */
	{ 0, (sy_call_t *)nosys },			/* 194 =  */
	{ 0, (sy_call_t *)nosys },			/* 195 =  */
	{ 0, (sy_call_t *)nosys },			/* 196 =  */
	{ 0, (sy_call_t *)nosys },			/* 197 =  */
	{ 0, (sy_call_t *)nosys },			/* 198 =  */
	{ 0, (sy_call_t *)nosys },			/* 199 =  */
	{ AS(linux_msgctl_args), (sy_call_t *)linux_msgctl },	/* 200 = linux_msgctl */
	{ AS(linux_msgget_args), (sy_call_t *)linux_msgget },	/* 201 = linux_msgget */
	{ AS(linux_msgrcv_args), (sy_call_t *)linux_msgrcv },	/* 202 = linux_msgrcv */
	{ AS(linux_msgsnd_args), (sy_call_t *)linux_msgsnd },	/* 203 = linux_msgsnd */
	{ AS(linux_semctl_args), (sy_call_t *)linux_semctl },	/* 204 = linux_semctl */
	{ AS(linux_semget_args), (sy_call_t *)linux_semget },	/* 205 = linux_semget */
	{ AS(linux_semop_args), (sy_call_t *)linux_semop },	/* 206 = linux_semop */
	{ 0, (sy_call_t *)nosys },			/* 207 =  */
	{ AS(linux_lchown_args), (sy_call_t *)linux_lchown },	/* 208 = linux_lchown */
	{ AS(linux_shmat_args), (sy_call_t *)linux_shmat },	/* 209 = linux_shmat */
	{ AS(linux_shmctl_args), (sy_call_t *)linux_shmctl },	/* 210 = linux_shmctl */
	{ AS(linux_shmdt_args), (sy_call_t *)linux_shmdt },	/* 211 = linux_shmdt */
	{ AS(linux_shmget_args), (sy_call_t *)linux_shmget },	/* 212 = linux_shmget */
	{ 0, (sy_call_t *)nosys },			/* 213 =  */
	{ 0, (sy_call_t *)nosys },			/* 214 =  */
	{ 0, (sy_call_t *)nosys },			/* 215 =  */
	{ 0, (sy_call_t *)nosys },			/* 216 =  */
	{ AS(linux_msync_args), (sy_call_t *)linux_msync },	/* 217 = linux_msync */
	{ 0, (sy_call_t *)nosys },			/* 218 =  */
	{ 0, (sy_call_t *)nosys },			/* 219 =  */
	{ 0, (sy_call_t *)nosys },			/* 220 =  */
	{ 0, (sy_call_t *)nosys },			/* 221 =  */
	{ 0, (sy_call_t *)nosys },			/* 222 =  */
	{ 0, (sy_call_t *)nosys },			/* 223 =  */
	{ 0, (sy_call_t *)nosys },			/* 224 =  */
	{ 0, (sy_call_t *)nosys },			/* 225 =  */
	{ 0, (sy_call_t *)nosys },			/* 226 =  */
	{ 0, (sy_call_t *)nosys },			/* 227 =  */
	{ 0, (sy_call_t *)nosys },			/* 228 =  */
	{ 0, (sy_call_t *)nosys },			/* 229 =  */
	{ 0, (sy_call_t *)nosys },			/* 230 =  */
	{ 0, (sy_call_t *)nosys },			/* 231 =  */
	{ 0, (sy_call_t *)nosys },			/* 232 =  */
	{ AS(linux_getpgid_args), (sy_call_t *)linux_getpgid },	/* 233 = linux_getpgid */
	{ AS(linux_getsid_args), (sy_call_t *)linux_getsid },	/* 234 = linux_getsid */
	{ 0, (sy_call_t *)nosys },			/* 235 = sigaltstack */
	{ 0, (sy_call_t *)nosys },			/* 236 =  */
	{ 0, (sy_call_t *)nosys },			/* 237 =  */
	{ 0, (sy_call_t *)nosys },			/* 238 =  */
	{ 0, (sy_call_t *)nosys },			/* 239 =  */
	{ 0, (sy_call_t *)nosys },			/* 240 =  */
	{ AS(osf1_sysinfo_args), (sy_call_t *)osf1_sysinfo },	/* 241 = osf1_sysinfo */
	{ 0, (sy_call_t *)nosys },			/* 242 =  */
	{ 0, (sy_call_t *)nosys },			/* 243 =  */
	{ 0, (sy_call_t *)nosys },			/* 244 =  */
	{ 0, (sy_call_t *)nosys },			/* 245 =  */
	{ 0, (sy_call_t *)nosys },			/* 246 =  */
	{ 0, (sy_call_t *)nosys },			/* 247 =  */
	{ 0, (sy_call_t *)nosys },			/* 248 =  */
	{ 0, (sy_call_t *)nosys },			/* 249 =  */
	{ 0, (sy_call_t *)nosys },			/* 250 =  */
	{ 0, (sy_call_t *)nosys },			/* 251 =  */
	{ 0, (sy_call_t *)nosys },			/* 252 =  */
	{ 0, (sy_call_t *)nosys },			/* 253 =  */
	{ 0, (sy_call_t *)nosys },			/* 254 =  */
	{ AS(linux_sysfs_args), (sy_call_t *)linux_sysfs },	/* 255 = linux_sysfs */
	{ AS(osf1_getsysinfo_args), (sy_call_t *)osf1_getsysinfo },	/* 256 = osf1_getsysinfo */
	{ AS(osf1_setsysinfo_args), (sy_call_t *)osf1_setsysinfo },	/* 257 = osf1_setsysinfo */
	{ 0, (sy_call_t *)linux_afs_syscall },		/* 258 = linux_afs_syscall */
	{ 0, (sy_call_t *)nosys },			/* 259 =  */
	{ 0, (sy_call_t *)nosys },			/* 260 =  */
	{ 0, (sy_call_t *)nosys },			/* 261 =  */
	{ 0, (sy_call_t *)nosys },			/* 262 =  */
	{ 0, (sy_call_t *)nosys },			/* 263 =  */
	{ 0, (sy_call_t *)nosys },			/* 264 =  */
	{ 0, (sy_call_t *)nosys },			/* 265 =  */
	{ 0, (sy_call_t *)nosys },			/* 266 =  */
	{ 0, (sy_call_t *)nosys },			/* 267 =  */
	{ 0, (sy_call_t *)nosys },			/* 268 =  */
	{ 0, (sy_call_t *)nosys },			/* 269 =  */
	{ 0, (sy_call_t *)nosys },			/* 270 =  */
	{ 0, (sy_call_t *)nosys },			/* 271 =  */
	{ 0, (sy_call_t *)nosys },			/* 272 =  */
	{ 0, (sy_call_t *)nosys },			/* 273 =  */
	{ 0, (sy_call_t *)nosys },			/* 274 =  */
	{ 0, (sy_call_t *)nosys },			/* 275 =  */
	{ 0, (sy_call_t *)nosys },			/* 276 =  */
	{ 0, (sy_call_t *)nosys },			/* 277 =  */
	{ 0, (sy_call_t *)nosys },			/* 278 =  */
	{ 0, (sy_call_t *)nosys },			/* 279 =  */
	{ 0, (sy_call_t *)nosys },			/* 280 =  */
	{ 0, (sy_call_t *)nosys },			/* 281 =  */
	{ 0, (sy_call_t *)nosys },			/* 282 =  */
	{ 0, (sy_call_t *)nosys },			/* 283 =  */
	{ 0, (sy_call_t *)nosys },			/* 284 =  */
	{ 0, (sy_call_t *)nosys },			/* 285 =  */
	{ 0, (sy_call_t *)nosys },			/* 286 =  */
	{ 0, (sy_call_t *)nosys },			/* 287 =  */
	{ 0, (sy_call_t *)nosys },			/* 288 =  */
	{ 0, (sy_call_t *)nosys },			/* 289 =  */
	{ 0, (sy_call_t *)nosys },			/* 290 =  */
	{ 0, (sy_call_t *)nosys },			/* 291 =  */
	{ 0, (sy_call_t *)nosys },			/* 292 =  */
	{ 0, (sy_call_t *)nosys },			/* 293 =  */
	{ 0, (sy_call_t *)nosys },			/* 294 =  */
	{ 0, (sy_call_t *)nosys },			/* 295 =  */
	{ 0, (sy_call_t *)nosys },			/* 296 =  */
	{ 0, (sy_call_t *)nosys },			/* 297 =  */
	{ 0, (sy_call_t *)nosys },			/* 298 =  */
	{ 0, (sy_call_t *)nosys },			/* 299 =  */
	{ 0, (sy_call_t *)linux_bdflush },		/* 300 = linux_bdflush */
	{ 0, (sy_call_t *)nosys },			/* 301 = sethae */
	{ 0, (sy_call_t *)linux_mount },		/* 302 = linux_mount */
	{ 0, (sy_call_t *)nosys },			/* 303 = old_adjtimex */
	{ 0, (sy_call_t *)linux_swapoff },		/* 304 = linux_swapoff */
	{ AS(linux_getdents_args), (sy_call_t *)linux_getdents },	/* 305 = linux_getdents */
	{ 0, (sy_call_t *)linux_create_module },	/* 306 = linux_create_module */
	{ 0, (sy_call_t *)linux_init_module },		/* 307 = linux_init_module */
	{ 0, (sy_call_t *)linux_delete_module },	/* 308 = linux_delete_module */
	{ 0, (sy_call_t *)linux_get_kernel_syms },	/* 309 = linux_get_kernel_syms */
	{ AS(linux_ksyslog_args), (sy_call_t *)linux_ksyslog },	/* 310 = linux_ksyslog */
	{ AS(reboot_args), (sy_call_t *)reboot },	/* 311 = reboot */
	{ AS(linux_clone_args), (sy_call_t *)linux_clone },	/* 312 = linux_clone */
	{ AS(linux_uselib_args), (sy_call_t *)linux_uselib },	/* 313 = linux_uselib */
	{ AS(mlock_args), (sy_call_t *)mlock },		/* 314 = mlock */
	{ AS(munlock_args), (sy_call_t *)munlock },	/* 315 = munlock */
	{ AS(mlockall_args), (sy_call_t *)mlockall },	/* 316 = mlockall */
	{ 0, (sy_call_t *)munlockall },			/* 317 = munlockall */
	{ 0, (sy_call_t *)linux_sysinfo },		/* 318 = linux_sysinfo */
	{ 0, (sy_call_t *)linux_sysctl },		/* 319 = linux_sysctl */
	{ 0, (sy_call_t *)linux_idle },			/* 320 = linux_idle */
	{ 0, (sy_call_t *)nosys },			/* 321 = old_umount */
	{ AS(swapon_args), (sy_call_t *)swapon },	/* 322 = swapon */
	{ AS(linux_times_args), (sy_call_t *)linux_times },	/* 323 = linux_times */
	{ AS(linux_personality_args), (sy_call_t *)linux_personality },	/* 324 = linux_personality */
	{ AS(linux_setfsuid_args), (sy_call_t *)linux_setfsuid },	/* 325 = linux_setfsuid */
	{ AS(linux_setfsgid_args), (sy_call_t *)linux_setfsgid },	/* 326 = linux_setfsgid */
	{ AS(linux_ustat_args), (sy_call_t *)linux_ustat },	/* 327 = linux_ustat */
	{ AS(linux_statfs_args), (sy_call_t *)linux_statfs },	/* 328 = linux_statfs */
	{ AS(linux_fstatfs_args), (sy_call_t *)linux_fstatfs },	/* 329 = linux_fstatfs */
	{ AS(sched_setparam_args), (sy_call_t *)sched_setparam },	/* 330 = sched_setparam */
	{ AS(sched_getparam_args), (sy_call_t *)sched_getparam },	/* 331 = sched_getparam */
	{ AS(linux_sched_setscheduler_args), (sy_call_t *)linux_sched_setscheduler },	/* 332 = linux_sched_setscheduler */
	{ AS(linux_sched_getscheduler_args), (sy_call_t *)linux_sched_getscheduler },	/* 333 = linux_sched_getscheduler */
	{ 0, (sy_call_t *)sched_yield },		/* 334 = sched_yield */
	{ AS(linux_sched_get_priority_max_args), (sy_call_t *)linux_sched_get_priority_max },	/* 335 = linux_sched_get_priority_max */
	{ AS(linux_sched_get_priority_min_args), (sy_call_t *)linux_sched_get_priority_min },	/* 336 = linux_sched_get_priority_min */
	{ AS(sched_rr_get_interval_args), (sy_call_t *)sched_rr_get_interval },	/* 337 = sched_rr_get_interval */
	{ 0, (sy_call_t *)nosys },			/* 338 = afs_syscall */
	{ AS(linux_newuname_args), (sy_call_t *)linux_newuname },	/* 339 = linux_newuname */
	{ AS(nanosleep_args), (sy_call_t *)nanosleep },	/* 340 = nanosleep */
	{ AS(linux_mremap_args), (sy_call_t *)linux_mremap },	/* 341 = linux_mremap */
	{ 0, (sy_call_t *)linux_nfsservctl },		/* 342 = linux_nfsservctl */
	{ AS(setresgid_args), (sy_call_t *)setresgid },	/* 343 = setresgid */
	{ AS(linux_getresgid_args), (sy_call_t *)linux_getresgid },	/* 344 = linux_getresgid */
	{ 0, (sy_call_t *)nosys },			/* 345 = pciconfig_read */
	{ 0, (sy_call_t *)nosys },			/* 346 = pciconfig_write */
	{ 0, (sy_call_t *)linux_query_module },		/* 347 = linux_query_module */
	{ 0, (sy_call_t *)linux_prctl },		/* 348 = linux_prctl */
	{ AS(linux_pread_args), (sy_call_t *)linux_pread },	/* 349 = linux_pread */
	{ AS(linux_pwrite_args), (sy_call_t *)linux_pwrite },	/* 350 = linux_pwrite */
	{ 0, (sy_call_t *)linux_rt_sigreturn },		/* 351 = linux_rt_sigreturn */
	{ AS(linux_rt_sigaction_args), (sy_call_t *)linux_rt_sigaction },	/* 352 = linux_rt_sigaction */
	{ AS(linux_rt_sigprocmask_args), (sy_call_t *)linux_rt_sigprocmask },	/* 353 = linux_rt_sigprocmask */
	{ 0, (sy_call_t *)linux_rt_sigpending },	/* 354 = linux_rt_sigpending */
	{ 0, (sy_call_t *)linux_rt_sigtimedwait },	/* 355 = linux_rt_sigtimedwait */
	{ 0, (sy_call_t *)linux_rt_sigqueueinfo },	/* 356 = linux_rt_sigqueueinfo */
	{ AS(linux_rt_sigsuspend_args), (sy_call_t *)linux_rt_sigsuspend },	/* 357 = linux_rt_sigsuspend */
	{ AS(linux_newselect_args), (sy_call_t *)linux_newselect },	/* 358 = linux_newselect */
	{ AS(gettimeofday_args), (sy_call_t *)gettimeofday },	/* 359 = gettimeofday */
	{ AS(settimeofday_args), (sy_call_t *)settimeofday },	/* 360 = settimeofday */
	{ AS(linux_getitimer_args), (sy_call_t *)linux_getitimer },	/* 361 = linux_getitimer */
	{ AS(linux_setitimer_args), (sy_call_t *)linux_setitimer },	/* 362 = linux_setitimer */
	{ AS(linux_utime_args), (sy_call_t *)linux_utime },	/* 363 = linux_utime */
	{ AS(getrusage_args), (sy_call_t *)getrusage },	/* 364 = getrusage */
	{ AS(linux_wait4_args), (sy_call_t *)linux_wait4 },	/* 365 = linux_wait4 */
	{ 0, (sy_call_t *)linux_adjtimex },		/* 366 = linux_adjtimex */
	{ AS(linux_getcwd_args), (sy_call_t *)linux_getcwd },	/* 367 = linux_getcwd */
	{ 0, (sy_call_t *)linux_capget },		/* 368 = linux_capget */
	{ 0, (sy_call_t *)linux_capset },		/* 369 = linux_capset */
	{ 0, (sy_call_t *)linux_sendfile },		/* 370 = linux_sendfile */
};
