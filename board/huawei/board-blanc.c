// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2026 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <aarch64/fpu.h>
#include <board.h>
#include <drivers/framework.h>
#include <drivers/simplefb.h>
#ifdef CONFIG_LIBFDT
#include <lib/hisi/patches.h>
#endif

#define DSS_BASE		0xE8600000U
#define DSS_LDI0		(DSS_BASE + 0x7D000)

#define DSS_LDI0_CTRL		(DSS_LDI0 + 0x0024)
#define DSS_LDI0_FRM_MSK	(DSS_LDI0 + 0x0044)

#ifdef CONFIG_SIMPLE_FB
static struct video_info blanc_fb = {
	.format = FB_FORMAT_ARGB8888,
	.width = 1080,
	.height = 2160,
	.stride = 4,
	.address = (void *)0x10600000,
	.scale = 2
};
#endif

static void dss_set_reg(void* addr, uint32_t val,
			uint8_t bw, uint8_t bs)
{
	uint32_t mask = (1UL << bw) - 1UL;
	uint32_t tmp = 0;

	tmp = readl(addr);
	tmp &= ~(mask << bs);

	writel(tmp | ((val & mask) << bs), addr);
}

#ifdef CONFIG_LIBFDT
static const fdt_patch blanc_fdt_patches[] = {
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
#endif

int blanc_early_init(void)
{
	aarch64_enable_fpu();

	/* fastboot disables LDI auto-refresh before
	 * kernel handoff, so enable it back here
	 */
	dss_set_reg((void *)DSS_LDI0_FRM_MSK, 0x0, 1, 0);
	dss_set_reg((void *)DSS_LDI0_CTRL, 0x1, 1, 0);

	return 0;
}

int blanc_drv(void)
{
#ifdef CONFIG_SIMPLE_FB
	REGISTER_DRIVER("simplefb", simplefb_probe, &blanc_fb);
#endif
	return 0;
}

struct board_data board_ops = {
	.name = "huawei-blanc",
#ifdef CONFIG_LIBFDT
	DEFINE_FDT_PATCHES(blanc_fdt_patches),
#endif
	.ops = {
		.early_init = blanc_early_init,
		.drivers_init = blanc_drv
	},
};
