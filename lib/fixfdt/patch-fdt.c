// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2026, Igor Belwon <igor.belwon@mentallysanemainliners.org>
 * Copyright (c) 2026, Ben Grisdale <bengris32@protonmail.ch>
 */

#include <array.h>
#include <board.h>
#include <lib/fixfdt/fdt-patch.h>
#include <lib/libfdt/libfdt.h>
#include <lib/debug.h>
#include <main/main.h>

static const fdt_patch default_fdt_patches[] = {
	FDT_DEFAULT_PATCH_LIST
};

static char fdt_buf[CONFIG_FDT_BUF_SIZE];

static int do_fdt_patches(struct board_data *board, const void *fdt,
	const void* bootloader_fdt)
{
	const fdt_patch *patches;
	size_t i, n_patches;
	int ret;

	/* Fallback to default list of FDT patches if board didn't
	 * provide any.
	 */
	if (board->fdt_patches) {
		patches = board->fdt_patches;
		n_patches = board->nr_fdt_patches;
	} else {
		patches = default_fdt_patches;
		n_patches = ARRAY_SIZE(default_fdt_patches);
	}

	/* Fail immediately if there are no patches and just
	 * use the FDT as is.
	 */
	if (!n_patches)
		return -1;

	printk(KERN_INFO, "Applying %d FDT patches.\n", n_patches);

	/* Open the FDT for patching */
	ret = fdt_open_into(fdt, fdt_buf, sizeof(fdt_buf));
	if (ret < 0) {
		printk(KERN_ERR, "fdt_open_into() failed: %s\n", fdt_strerror(ret));
		if (ret == -FDT_ERR_NOSPACE)
			printk(KERN_ERR, "You probably need to increase CONFIG_FDT_BUF_SIZE!\n");
		return ret;
	}

	/* Try each FDT patch. */
	for (i = 0; i < n_patches; i++) {
		ret = patches[i](bootloader_fdt, fdt_buf);
		if (ret) {
			printk(KERN_ERR, "Failed to patch FDT!\n");
			return ret;
		}
	}

	return 0;
}

void* patch_fdt(struct board_data *board, void* fdt,
	const void* bootloader_fdt)
{
	int ret;

	ret = do_fdt_patches(board, fdt, bootloader_fdt);
	if (ret) {
#ifdef CONFIG_HUAWEI_KIRIN970
		printk(KERN_EMERG, "Kirin970 firmware fixups failed; refusing to boot.\n");
		HANG();
#endif
		/* Return the original FDT upon failure. */
		return fdt;
	}

	printk(KERN_INFO, "Successfully patched FDT!\n");
	return fdt_buf;
}
