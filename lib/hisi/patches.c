// SPDX-License-Identifier: GPL-2.0-only
#include <array.h>
#include <lib/hisi/fdt.h>
#include <lib/hisi/patches.h>
#include <lib/libfdt/libfdt.h>

static int copy_property(const void *live, void *fdt, const char *path,
	const char *name, enum hisi_fdt_value_type type, int drop_if_missing)
{
	const struct hisi_fdt_property property = {
		path, name, type, drop_if_missing,
	};

	return hisi_fdt_copy_properties(live, fdt, &property, 1);
}

int hisi_camera_fdt_patch(const void *live, void *fdt)
{
	return copy_property(live, fdt, "/reserved-memory/camera-mem", "status",
		HISI_FDT_STRING, 0);
}

int hisi_logo_fdt_patch(const void *live, void *fdt)
{
	return copy_property(live, fdt, "/reserved-memory/logo-buffer", "status",
		HISI_FDT_STRING, 0);
}

int hisi_lcd_panel_fdt_patch(const void *live, void *fdt)
{
	return copy_property(live, fdt, "/huawei,lcd_panel", "lcd_panel_type",
		HISI_FDT_STRING, 0);
}

int hisi_framebuffer_fdt_patch(const void *live, void *fdt)
{
	static const struct hisi_fdt_property properties[] = {
		{ "/framebuffer@E8600000", "lcd_name", HISI_FDT_STRING, 0 },
		{ "/framebuffer@E8600000", "fastboot_enable_flag", HISI_FDT_U32, 0 },
	};

	return hisi_fdt_copy_properties(live, fdt, properties, ARRAY_SIZE(properties));
}

int hisi_initrd_fdt_patch(const void *live, void *fdt)
{
	static const struct hisi_fdt_property properties[] = {
		{ "/chosen", "linux,initrd-start", HISI_FDT_ADDRESS, 0 },
		{ "/chosen", "linux,initrd-end", HISI_FDT_ADDRESS, 0 },
	};

	return hisi_fdt_copy_properties(live, fdt, properties, ARRAY_SIZE(properties));
}

int hisi_kaslr_fdt_patch(const void *live, void *fdt)
{
	return copy_property(live, fdt, "/chosen", "kaslr-seed", HISI_FDT_U64, 1);
}

int hisi_psci_fdt_patch(const void *live, void *fdt)
{
	return copy_property(live, fdt, "/psci", "method", HISI_FDT_STRING, 0);
}

int hisi_memory_fdt_patch(const void *live, void *fdt)
{
	const void *reg;
	int node, len, ret;

	/* EMUI9 firmware fills in /memory@0 and leaves /memory empty */
	node = fdt_path_offset(live, "/memory@0");
	if (node >= 0) {
		reg = fdt_getprop(live, node, "reg", &len);
		if (!reg || len <= 0 || len % 16)
			return -FDT_ERR_BADVALUE;
		ret = copy_property(live, fdt, "/memory@0", "reg",
			HISI_FDT_BYTES, 0);
		if (ret)
			return ret;
	} else if (node != -FDT_ERR_NOTFOUND) {
		return node;
	}
	return copy_property(live, fdt, "/memory", "reg", HISI_FDT_BYTES, 0);
}

int hisi_bootargs_fdt_patch(const void *live, void *fdt)
{
	return copy_property(live, fdt, "/chosen", "bootargs", HISI_FDT_STRING, 0);
}
