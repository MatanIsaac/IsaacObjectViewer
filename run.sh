#!/usr/bin/env bash
set -e

make -j 8

echo "Running Isaac-Object-Viewer.."

./build/iov
