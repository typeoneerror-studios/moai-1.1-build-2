#!/bin/bash

src="$( cd "$( dirname $0 )" && pwd )"

echo "Building OSX host..."
echo ""

$src/xcode/osx/build.sh -c Release