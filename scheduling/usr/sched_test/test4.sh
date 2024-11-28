#!/bin/sh

cc -o test test.c

./test b &
./test b &
./test c &
