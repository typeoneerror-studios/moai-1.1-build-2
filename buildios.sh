#!/bin/bash

src="$( cd "$( dirname $0 )" && pwd )"

echo "Building iOS..."
echo ""

# $src/xcode/libmoai/clean.sh -c Release -p ios
$src/xcode/libmoai/build.sh -c Release -p ios