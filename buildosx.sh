#!/bin/bash

src="$( cd "$( dirname $0 )" && pwd )"

echo "Cleaning OSX host..."
echo ""
$src/xcode/osx/clean.sh -c Release

echo "Building OSX host..."
echo ""
$src/xcode/osx/build.sh -c Release