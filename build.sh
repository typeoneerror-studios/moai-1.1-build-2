#!/bin/bash

src="$( cd "$( dirname $0 )" && pwd )"

echo "Building all platforms..."

echo ""

$src"/buildios.sh"
$src"/buildandroid.sh"

echo ""
echo "Done."