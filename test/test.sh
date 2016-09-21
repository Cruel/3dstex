#!/usr/bin/env bash

TARGET=./3dstex-test
TEST_IMAGE=images/test.png
TEST_OUTPUT=images/test.bin

set -x

$TARGET -h > /dev/null

$TARGET -o etc1 $TEST_IMAGE

$TARGET -o rgba8 $TEST_IMAGE

md5sum -c hash.md5
