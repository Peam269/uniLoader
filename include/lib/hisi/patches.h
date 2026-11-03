/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef HISI_PATCHES_H_
#define HISI_PATCHES_H_

int hisi_camera_fdt_patch(const void *live, void *fdt);
int hisi_logo_fdt_patch(const void *live, void *fdt);
int hisi_lcd_panel_fdt_patch(const void *live, void *fdt);
int hisi_framebuffer_fdt_patch(const void *live, void *fdt);
int hisi_initrd_fdt_patch(const void *live, void *fdt);
int hisi_kaslr_fdt_patch(const void *live, void *fdt);
int hisi_psci_fdt_patch(const void *live, void *fdt);
int hisi_touchscreen_fdt_patch(const void *live, void *fdt);

#endif
