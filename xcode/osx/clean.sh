#!/bin/bash

#----------------------------------------------------------------#
# Copyright (c) 2010-2011 Zipline Games, Inc.
# All Rights Reserved.
# http://getmoai.com
#----------------------------------------------------------------#

set -e

src="$( cd "$( dirname $0 )" && pwd )"

usage="usage: $0 [-j <jobName>] [-c Debug|Release|all]"
job="default"
configurations="all"

while [ $# -gt 0 ]; do
    case "$1" in
        -j)  job="$2"; shift;;
        -c)  configurations="$2"; shift;;
        -*)
            echo >&2 \
                $usage
            exit 1;;
        *)  break;;
    esac
    shift
done

if ! [[ $job =~ ^[a-zA-Z0-9_\-]+$ ]]; then
    echo -e "*** Illegal job name specified: $job..."
    echo -e "    > Job names may only contain letters, numbers, dashes and underscores"
    echo
    exit 1
fi

if [ x"$configurations" != xDebug ] && [ x"$configurations" != xRelease ] && [ x"$configurations" != xall ]; then
    echo $usage
    exit 1
elif [ x"$configurations" = xall ]; then
    configurations="Debug Release"
fi

for config in $configurations; do

    echo "Cleaning MoaiSample/moai/macosx for $config"
    xcodebuild -configuration $config -workspace $src/MoaiSample.xcodeproj/project.xcworkspace -scheme moai -sdk macosx10.7 clean CONFIGURATION_BUILD_DIR=/tmp/osx/$job/MoaiSample/moai/macosx/$config
    echo "Done"

    echo "Cleaning MoaiSample/moai-fmod/macosx for $config"
    xcodebuild -configuration $config -workspace $src/MoaiSample.xcodeproj/project.xcworkspace -scheme moai-fmod -sdk macosx10.7 clean CONFIGURATION_BUILD_DIR=/tmp/osx/$job/MoaiSample/moai-fmod/macosx/$config
    echo "Done"

done