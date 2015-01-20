/*-
 * Copyright (C) 2008 MARVELL INTERNATIONAL LTD.
 * All rights reserved.
 *
 * Developed by Semihalf.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of MARVELL nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/malloc.h>

#include <machine/bus.h>

/* Prototypes for all the bus_space structure functions */
bs_protos(generic);
bs_protos(generic_armv4);

/*
 * The bus space tag.  This is constant for all instances, so
 * we never have to explicitly "create" it.
 */
static struct bus_space _base_tag = {
	/* cookie */
	(void *) 0,

	/* mapping/unmapping */
	generic_bs_map,
	generic_bs_unmap,
	generic_bs_subregion,

	/* allocation/deallocation */
	generic_bs_alloc,
	generic_bs_free,

	/* barrier */
	generic_bs_barrier,

	/* read (single) */
	NULL,	/* bs_r_1, Use inline code in bus.h */
	NULL,	/* bs_r_2, Use inline code in bus.h */
	NULL,	/* bs_r_4, Use inline code in bus.h */
	NULL,	/* bs_r_8, Use inline code in bus.h */

	/* read multiple */
	generic_bs_rm_1,
	generic_armv4_bs_rm_2,
	generic_bs_rm_4,
	NULL,

	/* read region */
	generic_bs_rr_1,
	generic_armv4_bs_rr_2,
	generic_bs_rr_4,
	NULL,

	/* write (single) */
	NULL,	/* bs_w_1, Use inline code in bus.h */
	NULL,	/* bs_w_2, Use inline code in bus.h */
	NULL,	/* bs_w_4, Use inline code in bus.h */
	NULL,	/* bs_w_8, Use inline code in bus.h */

	/* write multiple */
	generic_bs_wm_1,
	generic_armv4_bs_wm_2,
	generic_bs_wm_4,
	NULL,

	/* write region */
	generic_bs_wr_1,
	generic_armv4_bs_wr_2,
	generic_bs_wr_4,
	NULL,

	/* set multiple */
	NULL,
	NULL,
	NULL,
	NULL,

	/* set region */
	generic_bs_sr_1,
	generic_armv4_bs_sr_2,
	generic_bs_sr_4,
	NULL,

	/* copy */
	NULL,
	generic_armv4_bs_c_2,
	NULL,
	NULL,

	/* read stream (single) */
	NULL,   /* bs_r_1_s, Use inline code in bus.h */ 
	NULL,   /* bs_r_2_s, Use inline code in bus.h */ 
	NULL,   /* bs_r_4_s, Use inline code in bus.h */ 
	NULL,   /* bs_r_8_s, Use inline code in bus.h */ 

	/* read multiple stream */
	NULL,
	generic_armv4_bs_rm_2,		/* bus_space_read_multi_stream_2 */
	NULL,
	NULL,

	/* read region stream */
	NULL,
	NULL,
	NULL,
	NULL,

	/* write stream (single) */
	NULL,   /* bs_w_1_s, Use inline code in bus.h */ 
	NULL,   /* bs_w_2_s, Use inline code in bus.h */ 
	NULL,   /* bs_w_4_s, Use inline code in bus.h */ 
	NULL,   /* bs_w_8_s, Use inline code in bus.h */ 

	/* write multiple stream */
	NULL,
	generic_armv4_bs_wm_2,		/* bus_space_write_multi_stream_2 */
	NULL,
	NULL,

	/* write region stream */
	NULL,
	NULL,
	NULL,
	NULL
};

bus_space_tag_t fdtbus_bs_tag = &_base_tag;
