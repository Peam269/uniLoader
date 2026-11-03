// SPDX-License-Identifier: GPL-2.0-only
#include <lib/hisi/dtb-bundle.h>
#include <lib/debug.h>
#include <lib/libfdt/libfdt.h>
#include <main/main.h>
#include <string.h>

static unsigned char selected_fdt[CONFIG_FDT_BUF_SIZE] __attribute__((aligned(8)));
void *hisi_kirin970_select_fdt(const void *bundle, size_t size, const void *live)
{
	const void *boardid, *selected_id;
	const char *name;
	int len, ret, selected_len;

	if (!live || fdt_check_header(live))
		goto fail;

	boardid = fdt_getprop(live, 0, "hisi,boardid", &len);
	if (!boardid || len != HISI_BOARD_ID_SIZE)
		goto fail;

	ret = hisi_dtb_unpack(bundle, size, boardid, selected_fdt,
			      sizeof(selected_fdt));
	if (ret < 0) {
		printk(KERN_EMERG, "Kirin970 DTB selection failed (%d).\n", ret);
		goto fail;
	}
	if (fdt_check_header(selected_fdt) || fdt_totalsize(selected_fdt) != ret)
		goto fail;

	selected_id = fdt_getprop(selected_fdt, 0, "hisi,boardid", &selected_len);
	if (!selected_id || selected_len != HISI_BOARD_ID_SIZE ||
	    memcmp(boardid, selected_id, HISI_BOARD_ID_SIZE))
		goto fail;

	name = fdt_getprop(selected_fdt, 0, "hisi,boardname", &len);
	if (!name || len < 1 || strnlen(name, len) != (size_t)len - 1)
		goto fail;

	printk(KERN_INFO, "Selected Kirin970 DTB: %s\n", name);
	return selected_fdt;
fail:
	printk(KERN_EMERG, "No valid matching Kirin970 DTB; refusing to boot.\n");
	HANG();
	return 0;
}
