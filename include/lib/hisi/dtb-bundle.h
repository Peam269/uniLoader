/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef HISI_DTB_BUNDLE_H
#define HISI_DTB_BUNDLE_H

#include <stddef.h>

#define HISI_BOARD_ID_SIZE 16

enum hisi_dtb_error {
	HISI_DTB_INVALID = -1,
	HISI_DTB_NOT_FOUND = -2,
};

/* Returns the reconstructed DTB size or a hisi_dtb_error */
int hisi_dtb_unpack(const void *bundle, size_t size, const void *boardid,
		    void *output, size_t capacity);
void *hisi_kirin970_select_fdt(const void *bundle, size_t size, const void *live);

#endif
