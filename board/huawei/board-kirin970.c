// SPDX-License-Identifier: GPL-2.0-only
#include <aarch64/fpu.h>
#include <board.h>
#include <lib/hisi/patches.h>
#include <string.h>

#define DSS_BASE 0xE8600000U
#define DSS_LDI0_CTRL (DSS_BASE + 0x7D024)
#define DSS_LDI0_FRM_MSK (DSS_BASE + 0x7D044)

static const fdt_patch kirin970_fdt_patches[] = {
	RAMDISK_PATCH
	hisi_memory_fdt_patch,
	hisi_camera_fdt_patch,
	hisi_logo_fdt_patch,
	hisi_lcd_panel_fdt_patch,
	hisi_framebuffer_fdt_patch,
	hisi_bootargs_fdt_patch,
#ifdef CONFIG_RAMDISK_NO_COPY
	hisi_initrd_fdt_patch,
#endif
	hisi_kaslr_fdt_patch,
	hisi_psci_fdt_patch,
	hisi_touchscreen_fdt_patch,
};

static int kirin970_early_init(void)
{
	aarch64_enable_fpu();

	/* Restore LDI auto-refresh disabled by the firmware before handoff. */
	writel(readl((void *)DSS_LDI0_FRM_MSK) & ~1U, (void *)DSS_LDI0_FRM_MSK);
	writel(readl((void *)DSS_LDI0_CTRL) | 1U, (void *)DSS_LDI0_CTRL);
	return 0;
}

struct board_data board_ops = {
	.name = "huawei-kirin970",
	DEFINE_FDT_PATCHES(kirin970_fdt_patches),
	.ops = {
		.early_init = kirin970_early_init,
	},
};
