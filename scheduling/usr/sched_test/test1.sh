#!/bin/sh

cc -o test test.c

./test b &
./test c &
