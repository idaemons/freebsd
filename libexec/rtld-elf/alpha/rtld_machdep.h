/*-
 * Copyright (c) 1999, 2000 John D. Polstra.
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
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

#ifndef RTLD_MACHDEP_H
#define RTLD_MACHDEP_H	1

#include <sys/types.h>
#include <machine/atomic.h>

/*
 * This value of CACHE_LINE_SIZE is conservative.  The actual size
 * is 32 on the  21064, 21064A, 21066, 21066A, and 21164.  It is 64
 * on the 21264.  Compaq recommends sequestering each lock in its own
 * 128-byte block to allow for future implementations with larger
 * cache lines.
 */
#define CACHE_LINE_SIZE		128

struct Struct_Obj_Entry;

/* Return the address of the .dynamic section in the dynamic linker. */
#define rtld_dynamic(obj)	(&_DYNAMIC)

Elf_Addr reloc_jmpslot(Elf_Addr *, Elf_Addr,
		       const struct Struct_Obj_Entry *,
		       const struct Struct_Obj_Entry *,
		       const Elf_Rel *);

#define make_function_pointer(def, defobj) \
	((defobj)->relocbase + (def)->st_value)

#define call_initfini_pointer(obj, target) \
	(((InitFunc)(target))())

/* Lazy binding entry point, called via PLT. */
void _rtld_bind_start_old(void);

#define	TLS_TCB_SIZE	16

#define round(size, align) \
	(((size) + (align) - 1) & ~((align) - 1))
#define calculate_first_tls_offset(size, align) \
	round(TLS_TCB_SIZE, align)
#define calculate_tls_offset(prev_offset, prev_size, size, align) \
	round(prev_offset + prev_size, align)
#define calculate_tls_end(off, size) 	((off) + (size))

typedef struct {
    unsigned long ti_module;
    unsigned long ti_offset;
} tls_index;

extern void *__tls_get_addr(tls_index *ti);

#endif
