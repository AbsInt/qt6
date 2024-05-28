#!/bin/bash

# get fresh toplevel module with right branch
git clone --single-branch --branch "$1" https://code.qt.io/qt/qt5.git || exit 1

cd qt5 || exit 1

# get only the really needed sub-modules
perl init-repository -f --branch --module-subset=qtbase,qtdeclarative,qttools,qtwayland,qtsvg || exit 1

# kill git files
find . -name ".git" -exec rm -rf --one-file-system {} \;
find . -name ".gitignore" -exec rm -f {} \;
find . -name ".gitattributes" -exec rm -f {} \;

# replace old stuff
cd .. || exit 1
rm -rf src || exit 1
mv qt5 src || exit 1

# patch it
#git apply absint.patch

# add new stuff to git
git add src || exit 1
