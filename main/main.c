// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2022, Ivaylo Ivanov <ivo.ivanov.ivanov1@gmail.com>
 */

#include <board.h>
#include <main/boot.h>
#include <main/main.h>
#include <lib/debug.h>
#include <lib/fixfdt/patch-fdt.h>
#ifdef CONFIG_HUAWEI_KIRIN970
#include <lib/hisi/dtb-bundle.h>
extern unsigned char dtb_end[];
#endif

extern struct board_data board_ops;

static void print_splash(void)
{
	printk(KERN_INFO, "             .__.____                     .___\n");
	printk(KERN_INFO, " __ __  ____ |__|    |    _________     __| _/___________\n");
	printk(KERN_INFO, "|  |  \\/    \\|  |    |   /  _ \\__  \\   / __ |/ __ \\_  __\\\n");
	printk(KERN_INFO, "|  |  /   |  \\  |    |__(  <_> ) __ \\_/ /_/ \\  ___/|  |\\/\n");
	printk(KERN_INFO, "|____/|___|  /__|_______ \\____(____  /\\____ |\\___  >__|\n");
	printk(KERN_INFO, "           \\/           \\/         \\/      \\/    \\/\n");

	printk(KERN_INFO, "passed board initialization\n");
	printk(KERN_INFO, "welcome to uniLoader %s on %s\n", VER_TAG, board_ops.name);
}

void main(void* dt, void* kernel, void* ramdisk, void *bootloader_fdt)
{
	INITCALL(board_ops.ops.early_init);
#ifdef CONFIG_HUAWEI_KIRIN970
	dt = hisi_kirin970_select_fdt(dt, dtb_end - (unsigned char *)dt,
		bootloader_fdt);
#endif
	INITCALL(board_ops.ops.drivers_init);

	print_splash();

	INITCALL(board_ops.ops.late_init);

#ifdef CONFIG_LIBFDT
	dt = patch_fdt(&board_ops, dt, bootloader_fdt);
#endif

	printk(KERN_INFO, "Booting kernel...\n");
	arch_load_kernel(kernel, dt, ramdisk);

	// todo: reset the board?
	printk(KERN_EMERG, "Something wrong happened, we shouldn't be here. Hanging....\n");
	HANG();
}
