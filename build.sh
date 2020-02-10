#!/bin/bash

./format.sh

OUTPUT_DIR=output
BUILD_DIR=cmake-build-debug

mkdir -p $OUTPUT_DIR
mkdir -p $BUILD_DIR

# build
cd $BUILD_DIR && cmake .. && make && cd ..

# install
cp $BUILD_DIR/*.a $OUTPUT_DIR/
cp $BUILD_DIR/*.out $OUTPUT_DIR/
cp src/*.h $OUTPUT_DIR/
cp -r data $OUTPUT_DIR/
