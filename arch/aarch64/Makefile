# SPDX-License-Identifier: GPL-2.0
#
# Copyright (c) 2025 Ivaylo Ivanov <ivo.ivanov.ivanov1@gmail.com>

obj-$(CONFIG_POSITION_INDEPENDENT) += aarch64/reloc.o
obj-y += aarch64/start.o aarch64/load-kernel.o aarch64/memcpy.o

# Optimize the MMU-off copy without unaligned or SIMD accesses
# Prevent the compiler from replacing the copy loops with assembly memcpy
CFLAGS_load-kernel.o += -O2 -mstrict-align -mgeneral-regs-only -ffreestanding -fno-tree-loop-distribute-patterns
