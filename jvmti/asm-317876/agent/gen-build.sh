#!/bin/bash -ex

mkdir -p build && cd build

cmake -G "Unix Makefiles" ..
