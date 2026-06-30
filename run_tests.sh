#!/usr/bin/env bash
set -e

make tests -j 8

echo "Testing Isaac-Object-Viewer.."

./build/tests/test_runner
