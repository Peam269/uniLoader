// SPDX-License-Identifier: GPL-2.0-only
#include <lib/hisi/fdt.h>
#include <lib/debug.h>
#include <lib/libfdt/libfdt.h>
#include <string.h>

static int check_value(const void *value, int len, enum hisi_fdt_value_type type)
{
	switch (type) {
	case HISI_FDT_BYTES:
		return 0;
	case HISI_FDT_STRING:
		if (len > 0 && strnlen(value, len) == (size_t)len - 1)
			return 0;
		break;
	case HISI_FDT_U32:
		if (len == 4)
			return 0;
		break;
	case HISI_FDT_U64:
		if (len == 8)
			return 0;
		break;
	case HISI_FDT_ADDRESS:
		if (len == 4 || len == 8)
			return 0;
		break;
	}
	return -FDT_ERR_BADVALUE;
}

static int copy_property(const void *live, void *fdt,
	const struct hisi_fdt_property *property)
{
	const void *value;
	int source, target, len, ret;

	source = fdt_path_offset(live, property->path);
	if (source == -FDT_ERR_NOTFOUND)
		return 0;
	if (source < 0)
		return source;
	target = fdt_path_offset(fdt, property->path);
	if (target == -FDT_ERR_NOTFOUND)
		return 0;
	if (target < 0)
		return target;
	value = fdt_getprop(live, source, property->name, &len);
	if (!value) {
		if (len != -FDT_ERR_NOTFOUND)
			return len;
		if (!property->drop_if_missing)
			return 0;
		ret = fdt_delprop(fdt, target, property->name);
		return ret == -FDT_ERR_NOTFOUND ? 0 : ret;
	}
	ret = check_value(value, len, property->type);
	if (ret)
		return ret;
	return fdt_setprop(fdt, target, property->name, value, len);
}

int hisi_fdt_copy_properties(const void *live, void *fdt,
	const struct hisi_fdt_property *properties, size_t count)
{
	size_t i;
	int ret;

	if (!live || !fdt || live == fdt || (!properties && count))
		return -FDT_ERR_BADVALUE;
	ret = fdt_check_header(live);
	if (!ret)
		ret = fdt_check_header(fdt);
	if (ret)
		return ret;
	for (i = 0; i < count; i++) {
		if (!properties[i].path || !properties[i].name)
			return -FDT_ERR_BADVALUE;
		ret = copy_property(live, fdt, &properties[i]);
		if (ret) {
			printk(KERN_ERR, "HiSi FDT: %s/%s: %s (%d).\n",
				properties[i].path, properties[i].name,
				fdt_strerror(ret), ret);
			return ret;
		}
	}
	return 0;
}
