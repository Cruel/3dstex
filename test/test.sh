#!/usr/bin/env bash

TARGET=./3dstex-test
TEST_IMAGE=images/test.png
TEST_OUTPUT=images/test.bin

set -x

$TARGET -h > /dev/null

$TARGET -o rgba8 $TEST_IMAGE rgba8.bin
$TARGET -o rgb8 $TEST_IMAGE rgb8.bin
$TARGET -o rgba5551 $TEST_IMAGE rgba5551.bin
$TARGET -o rgb565 $TEST_IMAGE rgb565.bin
$TARGET -o rgba4 $TEST_IMAGE rgba4.bin
$TARGET -o la8 $TEST_IMAGE la8.bin
$TARGET -o hilo8 $TEST_IMAGE hilo8.bin
$TARGET -o l8 $TEST_IMAGE l8.bin
$TARGET -o a8 $TEST_IMAGE a8.bin
$TARGET -o la4 $TEST_IMAGE la4.bin
$TARGET -o l4 $TEST_IMAGE l4.bin
$TARGET -o a4 $TEST_IMAGE a4.bin
$TARGET -o etc1 -c1 $TEST_IMAGE etc1-c1.bin
$TARGET -o etc1 -c2 $TEST_IMAGE etc1-c2.bin
$TARGET -o etc1 -c3 $TEST_IMAGE etc1-c3.bin
$TARGET -o etc1a4 -c1 $TEST_IMAGE etc1a4-c1.bin
$TARGET -o etc1a4 -c2 $TEST_IMAGE etc1a4-c2.bin
$TARGET -o etc1a4 -c3 $TEST_IMAGE etc1a4-c3.bin

md5sum -c hash.md5
