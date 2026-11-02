// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025, Victor Paul <vipollmail@gmail.com>
 */

#ifndef RAMDISK_H_   /* Include guard */
#define RAMDISK_H_
#ifndef CONFIG_RAMDISK_NO_COPY

#include <main/boot.h>

int ramdisk_fdt_patch(const void *bootloader_fdt, void *fdt);

#endif
#endif
