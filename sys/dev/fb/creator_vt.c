/*-
 * Copyright (c) 2014 Nathan Whitehorn
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
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/fbio.h>

#include <dev/vt/vt.h>
#include <dev/vt/hw/fb/vt_fb.h>
#include <dev/vt/colors/vt_termcolors.h>

#include <machine/bus.h>
#include <machine/bus_private.h>

#include <dev/ofw/openfirm.h>
#include "creatorreg.h"

static vd_probe_t	creatorfb_probe;
static vd_init_t	creatorfb_init;
static vd_blank_t	creatorfb_blank;
static vd_bitbltchr_t	creatorfb_bitbltchr;

static const struct vt_driver vt_creatorfb_driver = {
	.vd_name	= "creatorfb",
	.vd_probe	= creatorfb_probe,
	.vd_init	= creatorfb_init,
	.vd_blank	= creatorfb_blank,
	.vd_bitbltchr	= creatorfb_bitbltchr,
	.vd_fb_ioctl	= vt_fb_ioctl,
	.vd_fb_mmap	= vt_fb_mmap,
	.vd_priority	= VD_PRIORITY_SPECIFIC
};

struct creatorfb_softc {
	struct fb_info fb;
	struct bus_space_tag memt[1];
	bus_space_handle_t memh;
};

static struct creatorfb_softc creatorfb_conssoftc;
VT_DRIVER_DECLARE(vt_creatorfb, vt_creatorfb_driver);

static int
creatorfb_probe(struct vt_device *vd)
{
	phandle_t chosen, node;
	ihandle_t stdout;
	char type[64], name[64];

	chosen = OF_finddevice("/chosen");
	OF_getprop(chosen, "stdout", &stdout, sizeof(stdout));
	node = OF_instance_to_package(stdout);
	if (node == -1) {
		/*
		 * The "/chosen/stdout" does not exist try
		 * using "screen" directly.
		 */
		node = OF_finddevice("screen");
	}
	OF_getprop(node, "device_type", type, sizeof(type));
	if (strcmp(type, "display") != 0)
		return (CN_DEAD);

	OF_getprop(node, "name", name, sizeof(name));
	if (strcmp(name, "SUNW,ffb") != 0 && strcmp(name, "SUNW,afb") != 0)
		return (CN_DEAD);

	/* Looks OK... */
	return (CN_INTERNAL);
}

static int
creatorfb_init(struct vt_device *vd)
{
	struct creatorfb_softc *sc;
	phandle_t chosen;
	phandle_t node;
	ihandle_t handle;
	uint32_t height, width;
	char type[64], name[64];
	bus_addr_t phys;
	int space;

	/* Initialize softc */
	vd->vd_softc = sc = &creatorfb_conssoftc;

	chosen = OF_finddevice("/chosen");
	OF_getprop(chosen, "stdout", &handle, sizeof(ihandle_t));
	node = OF_instance_to_package(handle);
	if (node == -1) {
		/*
		 * The "/chosen/stdout" does not exist try
		 * using "screen" directly.
		 */
		node = OF_finddevice("screen");
		handle = OF_open("screen");
	}
	OF_getprop(node, "device_type", type, sizeof(type));
	if (strcmp(type, "display") != 0)
		return (CN_DEAD);

	OF_getprop(node, "name", name, sizeof(name));
	if (strcmp(name, "SUNW,ffb") != 0 && strcmp(name, "SUNW,afb") != 0)
		return (CN_DEAD);

	/* Make sure we have needed properties */
	if (OF_getproplen(node, "height") != sizeof(height) ||
	    OF_getproplen(node, "width") != sizeof(width))
		return (CN_DEAD);

	OF_getprop(node, "height", &height, sizeof(height));
	OF_getprop(node, "width", &width, sizeof(width));

	sc->fb.fb_height = height;
	sc->fb.fb_width = width;
	sc->fb.fb_bpp = sc->fb.fb_depth = 32;
	sc->fb.fb_stride = 8192; /* Fixed */
	sc->fb.fb_size = sc->fb.fb_height * sc->fb.fb_stride;

	/* Map linear framebuffer */
	if (OF_decode_addr(node, FFB_DFB24, &space, &phys) != 0)
		return (CN_DEAD);
	sc->fb.fb_pbase = phys;
	sc->memh = sparc64_fake_bustag(space, phys, &sc->memt[0]);

	/* 32-bit VGA palette */
	vt_generate_vga_palette(sc->fb.fb_cmap, COLOR_FORMAT_RGB,
	    255, 16, 255, 8, 255, 0);

	vt_fb_init(vd);

	return (CN_INTERNAL);
}

static void
creatorfb_blank(struct vt_device *vd, term_color_t color)
{
	struct creatorfb_softc *sc;
	uint32_t c;
	int i;

	sc = vd->vd_softc;
	c = sc->fb.fb_cmap[color];

	for (i = 0; i < sc->fb.fb_height; i++)
		bus_space_set_region_4(sc->memt, sc->memh, i*sc->fb.fb_stride,
		    c, sc->fb.fb_width);
}

static void
creatorfb_bitbltchr(struct vt_device *vd, const uint8_t *src,
    const uint8_t *mask, int bpl, vt_axis_t top, vt_axis_t left,
    unsigned int width, unsigned int height, term_color_t fg, term_color_t bg)
{
	struct creatorfb_softc *sc = vd->vd_softc;
	u_long line;
	uint32_t fgc, bgc;
	int c;
	uint8_t b, m;

	fgc = sc->fb.fb_cmap[fg];
	bgc = sc->fb.fb_cmap[bg];
	b = m = 0;

	/* Don't try to put off screen pixels */
	if (((left + width) > vd->vd_width) || ((top + height) >
	    vd->vd_height))
		return;

	line = (sc->fb.fb_stride * top) + 4*left;
	for (; height > 0; height--) {
		for (c = 0; c < width; c++) {
			if (c % 8 == 0)
				b = *src++;
			else
				b <<= 1;
			if (mask != NULL) {
				if (c % 8 == 0)
					m = *mask++;
				else
					m <<= 1;
				/* Skip pixel write if mask not set. */
				if ((m & 0x80) == 0)
					continue;
			}
			bus_space_write_4(sc->memt, sc->memh, line + 4*c,
			    (b & 0x80) ? fgc : bgc);
		}
		line += sc->fb.fb_stride;
	}
}

