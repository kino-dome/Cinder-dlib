#! /bin/bash
#
# DLIB
#
# uses CMake

# define the version
VER=19.4

# tools for git use
GIT_URL="https://github.com/davisking/dlib"
#GIT_TAG="v$VER"
GIT_TAG=

FORMULA_TYPES=( "macosx" "android" "linux64" "msw")

# download the source code and unpack it into LIB_NAME
function download() {
    git clone https://github.com/davisking/dlib.git
}

# prepare the build environment, executed inside the lib src dir
function prepare() {
    if [ "$TYPE" == "android" ] ; then
        cp -rf $FORMULA_DIR/jni/  jni
    fi
}

# executed inside the lib src dir
function build() {
    if [ "$TYPE" == "macosx" ] || [ "$TYPE" == "linux64" ] || [ "$TYPE" == "msw"] ; then
        cd dlib
        mkdir -p build
        cd build
        export MAKEFLAGS="-j$PARALLEL_MAKE -s"
        if [ "$TYPE" == "macosx" ] || [ "$TYPE" == "linux64" ] ; then
            cmake -D DLIB_NO_GUI_SUPPORT=yes -D DLIB_JPEG_SUPPORT=yes -D DLIB_PNG_SUPPORT=yes -D JPEG_INCLUDE_DIR=..\dlib\external\libjpeg -D JPEG_LIBRARY=..\dlib\external\libjpeg -D PNG_PNG_INCLUDE_DIR=..\dlib\external\libpng -D PNG_LIBRARY_RELEASE=..\dlib\external\libpng -D ZLIB_INCLUDE_DIR=..\dlib\external\zlib -D CMAKE_INSTALL_PREFIX=$INSTALL_DIR/temp/install ..
        elif [ "$TYPE" == "msw" ] ; then
            cmake -G "Visual Studio 14 2015 Win64" -D DLIB_JPEG_SUPPORT=yes -D DLIB_PNG_SUPPORT=yes -D JPEG_INCLUDE_DIR=..\dlib\external\libjpeg -D JPEG_LIBRARY=..\dlib\external\libjpeg -D PNG_PNG_INCLUDE_DIR=..\dlib\external\libpng -D PNG_LIBRARY_RELEASE=..\dlib\external\libpng -D ZLIB_INCLUDE_DIR=..\dlib\external\zlib -D ZLIB_LIBRARY_RELEASE=..\dlib\external\zlib -D CMAKE_INSTALL_PREFIX=$INSTALL_DIR/temp/install ..
        fi

        cmake --build . --config Release
        cd ../../        
    elif [ "$TYPE" == "android" ] ; then
        ${NDK_ROOT}/ndk-build -j4 NDK_DEBUG=0 NDK_PROJECT_PATH=.
    fi
}

# executed inside the lib src dir, first arg $1 is the dest libs dir root
function copy() {

    mkdir -p $LIBS_DIR/$TYPE

    if [ "$TYPE" == "macosx" ] || [ "$TYPE" == "linux64" ] ; then
        cd dlib/build
        make install
        cd -
        mv $INSTALL_DIR/temp/install/lib/libdlib.a $LIBS_DIR/$TYPE/
        mv $INSTALL_DIR/temp/install/include/dlib $BLOCK_DIR/include/
    elif [ "$TYPE" == "android" ] ; then
        cp -vr $INSTALL_DIR/temp/install/include/dlib $BLOCK_DIR/include/
        rm -rf $1/include/dlib/build
        rm -rf $1/include/dlib/test
        rm  $1/include/dlib/all_gui.cpp
        cp -vr obj/local/armeabi-v7a/libdlib.a $LIBS_DIR/android/armeabi-v7a/libdlib.a
        cp -vr obj/local/x86/libdlib.a $LIBS_DIR/android/x86/libdlib.a
    fi
}

# executed inside the lib src dir
function clean() {
    if [ "$TYPE" == "macosx" ] || [ "$TYPE" == "linux64" ] ; then
        cd dlib/build
        cmake clean .
        cd ..
        rm -rf build
        cd ..
    elif [ "$TYPE" == "android" ] ; then
        rm -rf obj
    fi
}
