#!/usr/bin/env bash

TARGET=./3dstex-test
TEST_IMAGE=images/test.png
TEST_OUTPUT=images/test.bin

FORMATS=( rgba8 rgb8 rgba5551 rgb565 rgba4 la8 hilo8 l8 a8 la4 l4 a4 )

set -x

$TARGET -h > /dev/null

for format in "${FORMATS[@]}"
do
	echo $format
	$TARGET -o $format $TEST_IMAGE $format.bin
	$TARGET -o png $format.bin $format.png
done

$TARGET -o etc1 -c1 $TEST_IMAGE etc1-c1.bin
$TARGET -o etc1 -c2 $TEST_IMAGE etc1-c2.bin
$TARGET -o etc1 -c3 $TEST_IMAGE etc1-c3.bin
$TARGET -o etc1a4 -c1 $TEST_IMAGE etc1a4-c1.bin
$TARGET -o etc1a4 -c2 $TEST_IMAGE etc1a4-c2.bin
$TARGET -o etc1a4 -c3 $TEST_IMAGE etc1a4-c3.bin

md5sum -c hash.md5
