#!/bin/bash

src="$( cd "$( dirname $0 )" && pwd )"

echo "Building iOS..."
echo ""

$src/xcode/libmoai/build.sh -c Release -p ios