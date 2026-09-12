#!/bin/bash
musl-gcc src/main.c -static -std=c89 -pedantic -Wall -Wextra -Werror -O2 -o main
