#!/bin/sh

set -xe

cc -Wall -Wextra -Wold-style-definition -ggdb -pedantic `pkg-config --cflags sdl2` -o tetrissy main.c `pkg-config --libs sdl2`
