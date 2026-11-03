// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2026 Ben Grisdale <bengris32@protonmail.ch>
 */

#ifndef FDT_PATCH_H_
#define FDT_PATCH_H_

#include <array.h>

#ifndef CONFIG_RAMDISK_NO_COPY
#include <lib/fixfdt/patches/ramdisk.h>
#define RAMDISK_PATCH ramdisk_fdt_patch,
#else
#define RAMDISK_PATCH
#endif

#ifdef CMDLINE_STR
#include <lib/fixfdt/patches/cmdline.h>
#define CMDLINE_PATCH cmdline_fdt_patch,
#else
#define CMDLINE_PATCH
#endif

#define FDT_DEFAULT_PATCH_LIST \
	RAMDISK_PATCH \
	CMDLINE_PATCH

#define DEFINE_FDT_PATCHES(patches)		\
	.fdt_patches = (patches),		\
	.nr_fdt_patches = ARRAY_SIZE(patches)	\

typedef int (*fdt_patch)(const void *bootloader_fdt, void *fdt);

#endif /* FDT_PATCH_H_ */
