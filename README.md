# Shibashiba MOAI Build


## What is this?

This is a hacked version of MOAI 1.1. We use this specicically in Shibashiba because MOAI 1.2 was pretty broken and we couldn't afford to upgrade to the latest.


## What's in this?

* Additional support for Game Center in MOAI iOS build.
* Setup and additional support for FMOD including BG music with loop points and bug fixes.
* Other patches from MOAI >= 1.2.

## Building the Moai Library for LuaJIT on OSX.

The following is cribbed from [this post](http://getmoai.com/forums/moai-luajit-t1463/#p8656).

1. Open `xcode/libmoai-luajit/libmoai.xcodeproj`. The "luajit" version of the project adds a #define, LUA_JIT, that correctly includes the correct files, etc. for building LuaJIT.
2. Build the `libmoai-osx-*` targets (excluding FMOD).
3. Using the libs built in step 2, build the mochi host.

## Building


### Building for OSX/iOS.

The MOAI build uses three Xcode projects to build the source. They are set up and configured in Xcode, but to actually compile, it works (oh so much better) to run the build scripts from the command line.


#### Relevant files

You can build the projects with the *./build\*.sh* scripts at the root of this project. Once you build a product, you can run the *./open\*.sh* scripts to open the path to where it builds the products.

You'll find all the build stuff in *./xcode/*. The project in *./xcode/osx* is used to build the OSX host and this project references *./xcode/libmoai*, which is the MOAI source code static library. The *./xcode/ios* build is NOT used. You simply use the products/targets that are built from the libmoai project. More on that below. The FMOD src code is also linked in all the Xcode projects. The source files can be found in *./FMOD*.


#### Building the OSX host.

1. Run `buildosx.sh` from the command line. This cleans the project and then rebuilds it. If you are just making tweaks to one file, a clean may not be necessary, but it is quick and safe, so it is encouraged.
2. Wait a while.
3. If no errors, run *openosx.sh* from the command line.
4. From the path that opens, copy *moai/macosx/Release/moai* and *moai-fmod/macosx/Release/moai-fmod* binaries to the Shibashiba build (one without -fmod is the untz build) in *bin/osx*.
5. cd into *bin/osx* and run the following command:

		install_name_tool -change ./libfmodex.dylib @executable_path/libfmodex.dylib moai-fmod


#### Building the iOS binaries.

1. Run `buildios.sh` from the command line. This cleans the project and then rebuilds it. If you are just making tweaks to one file, a clean may not be necessary, but it is quick and safe, so it is encouraged.
2. Wait a while.
3. If no errors, run `openios.sh` from the command line.
4. copy the four arch folders (i386, armv7, armv7s, universal) to *bin/ios* in the Shibashiba project.


### Build for Android.

Coming later...