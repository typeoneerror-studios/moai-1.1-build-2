#!/bin/bash

src="$( cd "$( dirname $0 )" && pwd )"

echo "Cleaning iOS host..."
echo ""
$src/xcode/libmoai/clean.sh -c Release -p ios

echo "Building iOS host..."
echo ""
$src/xcode/libmoai/build.sh -c Release -p ios