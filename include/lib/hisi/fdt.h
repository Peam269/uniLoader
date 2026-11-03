/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef HISI_FDT_H_
#define HISI_FDT_H_

#include <stddef.h>

enum hisi_fdt_value_type {
	HISI_FDT_BYTES,
	HISI_FDT_STRING,
	HISI_FDT_U32,
	HISI_FDT_U64,
	HISI_FDT_ADDRESS,
};

struct hisi_fdt_property {
	const char *path;
	const char *name;
	enum hisi_fdt_value_type type;
	/* If absent in the live tree, remove a stale embedded property instead. */
	int drop_if_missing;
};

int hisi_fdt_copy_properties(const void *live, void *fdt,
	const struct hisi_fdt_property *properties, size_t count);

#endif
