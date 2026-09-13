#!/bin/bash
# compile.sh - Copyright (c) 2026 Minetomba minetomba@proton.me
# SPDX-License-Identifier: GPL-3.0-only
musl-gcc src/main.c -static -std=c89 -pedantic -Wall -Wextra -Werror -O2 -o main
