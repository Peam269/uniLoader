// SPDX-License-Identifier: GPL-2.0-only
#include <lib/hisi/dtb-bundle.h>
#include <stdint.h>
#include <string.h>

#define BUNDLE_MAGIC "K970DTB\0"
#define BUNDLE_VERSION 1
#define HEADER_SIZE 24
#define ENTRY_SIZE 28
#define CHUNK_SIZE 8

static uint32_t read_be32(const unsigned char *p)
{
	return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 |
	       (uint32_t)p[2] << 8 | p[3];
}

int hisi_dtb_unpack(const void *bundle, size_t size, const void *boardid,
		    void *output, size_t capacity)
{
	const unsigned char *data = bundle, *entry, *selected = NULL;
	unsigned char *out = output;
	size_t count, table_end, length, recipe, chunks, written = 0;
	size_t i, offset, bytes;

	if (!data || !boardid || !out || size < HEADER_SIZE ||
	    memcmp(data, BUNDLE_MAGIC, sizeof(BUNDLE_MAGIC) - 1) ||
	    read_be32(data + 8) != BUNDLE_VERSION ||
	    read_be32(data + 12) != size || read_be32(data + 20))
		return HISI_DTB_INVALID;

	count = read_be32(data + 16);
	if (!count || count > (size - HEADER_SIZE) / ENTRY_SIZE)
		return HISI_DTB_INVALID;

	table_end = HEADER_SIZE + count * ENTRY_SIZE;
	for (i = 0; i < count; i++) {
		entry = data + HEADER_SIZE + i * ENTRY_SIZE;
		if (memcmp(entry, boardid, HISI_BOARD_ID_SIZE))
			continue;
		if (selected)
			return HISI_DTB_INVALID;
		selected = entry;
	}
	if (!selected)
		return HISI_DTB_NOT_FOUND;

	length = read_be32(selected + 16);
	recipe = read_be32(selected + 20);
	chunks = read_be32(selected + 24);
	if (!length || length > capacity || length > 0x7fffffff ||
	    recipe < table_end || recipe > size ||
	    !chunks || chunks > (size - recipe) / CHUNK_SIZE)
		return HISI_DTB_INVALID;

	for (i = 0; i < chunks; i++) {
		offset = read_be32(data + recipe + i * CHUNK_SIZE);
		bytes = read_be32(data + recipe + i * CHUNK_SIZE + 4);
		if (offset > size || !bytes || bytes > size - offset ||
		    bytes > length - written)
			return HISI_DTB_INVALID;
#ifdef __aarch64__
		/* Chunk boundaries may be unaligned while the MMU is off */
		__optimized_memcpy(out + written, data + offset, bytes);
#else
		memcpy(out + written, data + offset, bytes);
#endif
		written += bytes;
	}

	return written == length ? (int)length : HISI_DTB_INVALID;
}
