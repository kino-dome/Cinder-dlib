#!/bin/bash
read -p "Download the models as well? (y/n)?"
SHOULD_DL_MODELS=$REPLY

. scripts/formulas/dlib.sh

# init the data needed to install
TYPE=`scripts/shared/ostype.sh` # "macosx" "android" "linux64" "win"
FORMULA_DIR="$PWD/scripts/formulas"
INSTALL_DIR=$PWD
BLOCK_DIR="$PWD/.."
LIBS_DIR="$BLOCK_DIR/lib"

# make temp and jump in
mkdir -p temp
cd temp

#prepare and build dlib
download
prepare
build
copy
clean

cd $BLOCK_DIR
chmod -R 755 ../
rm -rf $INSTALL_DIR/temp

# download model to assets
if [$SHOULD_DL_MODELS=="y"] then
    cd $INSTALL_DIR/scripts
    ./download-models.sh
fi