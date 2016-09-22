#!/usr/bin/env bash

TARGET=./3dstex-test
TEST_IMAGE=images/test.png
TEST_OUTPUT=images/test.bin

FORMATS=( rgba8 rgb8 rgba5551 rgb565 rgba4 la8 hilo8 l8 a8 la4 l4 a4 )

set -x

$TARGET -h > /dev/null

for format in "${FORMATS[@]}"
do
	$TARGET -o $format $TEST_IMAGE $format.bin
done

$TARGET -o etc1 -c1 $TEST_IMAGE etc1-c1.bin
$TARGET -o etc1 -c2 $TEST_IMAGE etc1-c2.bin
$TARGET -o etc1 -c3 $TEST_IMAGE etc1-c3.bin
$TARGET -o etc1a4 -c1 $TEST_IMAGE etc1a4-c1.bin
$TARGET -o etc1a4 -c2 $TEST_IMAGE etc1a4-c2.bin
$TARGET -o etc1a4 -c3 $TEST_IMAGE etc1a4-c3.bin

$TARGET -o auto-etc1 -c1 $TEST_IMAGE auto-etc1.bin
$TARGET -o auto-l8 $TEST_IMAGE auto-l8.bin
$TARGET -o auto-l4 $TEST_IMAGE auto-l4.bin

$TARGET -bo png *.bin

! $TARGET &> /dev/null
! $TARGET arg1 &> /dev/null
! $TARGET arg1 arg2 &> /dev/null
! $TARGET arg1 arg2 arg3 &> /dev/null

md5sum -c hash.md5
