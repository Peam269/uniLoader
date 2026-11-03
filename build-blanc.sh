#!/bin/bash

MKBOOTIMG=~/android/los20/kernel/huawei/kirin970/tools/mkbootimg

cd -- "$(dirname -- "$0")"
output_image="${OUTPUT_IMAGE:-uniLoader.img}"
mkbootimg="${MKBOOTIMG:-mkbootimg}"
command -v "$mkbootimg" >/dev/null

# Extract Kernel Image.gz from boot.img and compile blanc.dts into a dtb located at blob/
python3 - "${BOOT_IMAGE:-boot.img}" <<'PY'
import gzip
import struct
import sys
from pathlib import Path

boot = Path(sys.argv[1]).read_bytes()
if len(boot) < 1632 or boot[:8] != b"ANDROID!":
    sys.exit("Invalid reference boot image")
size, address, ramdisk_size = struct.unpack_from("<3I", boot, 8)
page, version = struct.unpack_from("<2I", boot, 36)
if version > 2 or page < 2048 or page & (page - 1) or not size or page + size > len(boot):
    sys.exit("Unsupported or truncated reference boot image")
if address != 0x80000 or ramdisk_size:
    sys.exit("Expected Huawei kernel at 0x80000 with separately loaded ramdisk")
kernel = boot[page:page + size]
if kernel.startswith(b"\x1f\x8b"):
    kernel = gzip.decompress(kernel)
if len(kernel) < 64 or kernel[56:60] != b"ARM\x64":
    sys.exit("Expected ARM64 Image")
offset, image_size, flags = struct.unpack_from("<3Q", kernel, 8)
if offset != 0x80000 or not image_size or not flags & 8 or flags & 1:
    sys.exit("ARM64 Image is incompatible with the Blanc load address")
cmdline = boot[64:576].split(b"\0", 1)[0] + boot[608:1632].split(b"\0", 1)[0]
if b"\n" in cmdline or b"\r" in cmdline:
    sys.exit("Invalid reference boot command line")
Path("blob/Image").write_bytes(kernel)
Path("blob/mkbootimg-cmdline").write_bytes(cmdline)
PY
dtc -q -I dts -O dtb -o blob/dtb "${DTS_SOURCE:-blanc.dts}"

make ARCH=aarch64 CROSS_COMPILE=aarch64-linux-gnu- clean
make ARCH=aarch64 CROSS_COMPILE=aarch64-linux-gnu- blanc_defconfig
make ARCH=aarch64 CROSS_COMPILE=aarch64-linux-gnu-

"$mkbootimg" \
    --header_version 1 \
    --os_version 10.0.0 \
    --os_patch_level 2020-11 \
    --kernel uniLoader.gz \
    --ramdisk /dev/null \
    --pagesize 0x00000800 \
    --base 0x00078000 \
    --kernel_offset 0x00008000 \
    --ramdisk_offset 0x37588000 \
    --second_offset 0x00e88000 \
    --tags_offset 0x37d88000 \
    --board '' \
    --cmdline "$(cat blob/mkbootimg-cmdline)" \
    -o "$output_image"
