#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Pack Kirin970 DTBs into a deduplicated bundle."""

import argparse
import hashlib
import struct
from pathlib import Path

MAGIC = b"K970DTB\0"
VERSION = 1
MAX_DTB_SIZE = 1024 * 1024
HEADER = struct.Struct(">8s4I")  # magic, version, total size, count, reserved
ENTRY = struct.Struct(">16s3I")  # boardid, DTB size, recipe offset, chunk count
CHUNK = struct.Struct(">2I")  # absolute byte offset, byte length
GEAR = [
    int.from_bytes(hashlib.sha256(bytes([i])).digest()[:4], "big")
    for i in range(256)
]


def board_id(data):
    """Read the root hisi,boardid property"""
    if len(data) < 40:
        raise ValueError("truncated FDT")
    magic, total, offset, strings, _, version, _, _, slen, tlen = struct.unpack_from(
        ">10I", data
    )
    if (
        magic != 0xd00dfeed or total != len(data) or version != 17
        or offset < 40 or offset % 4 or offset + tlen > total
        or strings < 40 or strings + slen > total
    ):
        raise ValueError("invalid FDT header")
    end = offset + tlen
    depth = 0
    while offset + 4 <= end:
        token, = struct.unpack_from(">I", data, offset)
        offset += 4
        if token == 1:
            offset = (data.index(b"\0", offset, end) + 4) & ~3
            depth += 1
        elif token == 2:
            depth -= 1
        elif token == 3:
            if offset + 8 > end:
                raise ValueError("truncated FDT property")
            length, name = struct.unpack_from(">2I", data, offset)
            offset += 8
            if name >= slen or offset + length > end:
                raise ValueError("invalid FDT property")
            name_end = data.index(b"\0", strings + name, strings + slen)
            if depth == 1 and data[strings + name:name_end] == b"hisi,boardid":
                if length != 16:
                    raise ValueError("hisi,boardid must contain four cells")
                return data[offset:offset + length]
            offset = (offset + length + 3) & ~3
        elif token == 9:
            break
        elif token != 4:
            raise ValueError("invalid FDT token")
    raise ValueError("missing root hisi,boardid")


def chunks(data):
    start = rolling = 0
    for i, value in enumerate(data):
        rolling = ((rolling << 1) + GEAR[value]) & 0xffffffff
        length = i - start + 1
        if length >= 256 and ((rolling & 1023) == 0 or length >= 4096):
            yield data[start:i + 1]
            start, rolling = i + 1, 0
    if start < len(data):
        yield data[start:]


def pack(paths):
    if not paths:
        raise ValueError("no DTBs supplied")
    trees, pool, seen = [], {}, set()
    for path in sorted(paths):
        data = Path(path).read_bytes()
        ident = board_id(data)
        if ident in seen:
            raise ValueError(f"duplicate board ID in {path}: {ident.hex()}")
        if len(data) > MAX_DTB_SIZE:
            raise ValueError(f"DTB exceeds loader buffer: {path}")
        seen.add(ident)
        recipe = list(chunks(data))
        for chunk in recipe:
            pool.setdefault(chunk, 0)
        trees.append((ident, len(data), recipe))
    cursor = HEADER.size + ENTRY.size * len(trees)
    pool_offset = cursor + sum(len(recipe) * CHUNK.size for _, _, recipe in trees)
    for chunk in pool:
        pool[chunk] = pool_offset
        pool_offset += len(chunk)
    entries, recipes = bytearray(), bytearray()
    for ident, size, recipe in trees:
        entries += ENTRY.pack(ident, size, cursor, len(recipe))
        for chunk in recipe:
            recipes += CHUNK.pack(pool[chunk], len(chunk))
        cursor += len(recipe) * CHUNK.size
    return (
        HEADER.pack(MAGIC, VERSION, pool_offset, len(trees), 0)
        + entries + recipes + b"".join(pool)
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("dtbs", nargs="+", type=Path)
    args = parser.parse_args()
    try:
        data = pack(args.dtbs)
    except (ValueError, struct.error) as error:
        parser.error(str(error))
    temporary = args.output.with_suffix(args.output.suffix + ".tmp")
    temporary.write_bytes(data)
    temporary.replace(args.output)
    print(f"Packed {len(args.dtbs)} Kirin970 DTBs into {len(data)} bytes")


if __name__ == "__main__":
    main()
