#! /bin/bash

# Replace this with the absolute path to your FMOD stuff
export FMOD_ANDROID_SDK_ROOT="/Users/mochi/Dev/games/moai-1.1-build-2/FMOD/Android"

./build.sh -i arm -a all --use-fmod true --use-untz false --disable-adcolony --disable-billing --disable-chartboost --disable-crittercism --disable-facebook --disable-tapjoy