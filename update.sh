#!/bin/bash

# get fresh toplevel module with right branch
git clone --single-branch --branch "$1" https://code.qt.io/qt/qt5.git || exit 1

cd qt5 || exit 1

# get only the really needed sub-modules
perl init-repository -f --branch --module-subset=qtbase,qtdeclarative,qttools,qtsvg || exit 1

# kill git files
find . -name ".git" -exec rm -rf --one-file-system {} \;
find . -name ".gitignore" -exec rm -f {} \;
find . -name ".gitattributes" -exec rm -f {} \;

# replace old stuff
cd .. || exit 1
rm -rf src || exit 1
mv qt5 src || exit 1

# Download some tag files. Not 100% accurate because of the version mismatch, but much simpler.
# Generating these tag files ourselves would require building qdoc, which depends on clang, etc.
rm -rf docs || exit 1
mkdir -p docs || exit 1
curl -o docs/qtcore.tags https://doc.qt.io/qt-6/qtcore.tags || exit 1
curl -o docs/qtgui.tags https://doc.qt.io/qt-6/qtgui.tags || exit 1
curl -o docs/qtnetwork.tags https://doc.qt.io/qt-6/qtnetwork.tags || exit 1
curl -o docs/qtwidgets.tags https://doc.qt.io/qt-6/qtwidgets.tags || exit 1

# add new stuff to git
git add src docs || exit 1
