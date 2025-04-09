#!/usr/bin/env bash
# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

# PySide versions following 5.6 use a C++ parser based on Clang (http://clang.org/).
# The Clang library (C-bindings), version 3.9 or higher is required for building.

# This same script is used to provision libclang to Linux and macOS.
# In case of Linux, we expect to get the values as args
set -e

# shellcheck source=./check_and_set_proxy.sh
source "${BASH_SOURCE%/*}/check_and_set_proxy.sh"
# shellcheck source=./SetEnvVar.sh
source "${BASH_SOURCE%/*}/SetEnvVar.sh"
# shellcheck source=./DownloadURL.sh
source "${BASH_SOURCE%/*}/DownloadURL.sh"

PROVISIONING_DIR="$(dirname "$0")/../../"
# shellcheck source=./common.sourced.sh
source "$PROVISIONING_DIR"/common/unix/common.sourced.sh

libclang_version="19.1.7"

if uname -a |grep -q Darwin; then
    version=$libclang_version
    url="https://download.qt.io/development_releases/prebuilt/libclang/qt/libclang-llvmorg-${version}-macos-universal.7z"
    url_cached="http://ci-files01-hki.ci.qt.io/input/libclang/qt/libclang-llvmorg-${version}-macos-universal.7z"
    sha1="0b30bbe47cefe413a6d2fbc3da6b0b8ac5d84613"
elif test -f /etc/redhat-release && grep "Red Hat" /etc/redhat-release | grep "9" ; then
    version=$libclang_version
    url="https://download.qt.io/development_releases/prebuilt/libclang/qt/libclang-llvmorg-${version}-linux-Rhel9.4-gcc11.4-x86_64.7z"
    url_cached="http://ci-files01-hki.ci.qt.io/input/libclang/qt/libclang-llvmorg-${version}-linux-Rhel9.4-gcc11.4-x86_64.7z"
    sha1="1657d6a9419e9d3ecf4416cd757f488c079ec779"
elif test "$PROVISIONING_OS_ID" == "debian" && test "$PROVISIONING_ARCH" == "arm64" ; then
    version=$libclang_version
    url="https://download.qt.io/development_releases/prebuilt/libclang/qt/libclang-llvmorg-${version}-linux-Debian11.6-gcc10.0-arm64.7z"
    url_cached="http://ci-files01-hki.ci.qt.io/input/libclang/qt/libclang-llvmorg-${version}-linux-Debian11.6-gcc10.0-arm64.7z"
    sha1="2536f55987d6240c40fd1127895b0885d41148ed"
elif test "$PROVISIONING_OS_ID" == "ubuntu" && test "$PROVISIONING_ARCH" == "arm64" ; then
    version=$libclang_version
    url="https://download.qt.io/development_releases/prebuilt/libclang/qt/libclang-llvmorg-${version}-linux-Ubuntu24.04-gcc11.2-arm64.7z"
    url_cached="http://ci-files01-hki.ci.qt.io/input/libclang/qt/libclang-llvmorg-${version}-linux-Ubuntu24.04-gcc11.2-arm64.7z"
    sha1="0e1c0c492f9fcd669a77fe4480cfa271f408af9e"
elif test "$PROVISIONING_OS_ID" == "ubuntu" && test "$PROVISIONING_ARCH" == "x86_64" ; then
    version=$libclang_version
    url="https://download.qt.io/development_releases/prebuilt/libclang/qt/libclang-llvmorg-${version}-linux-Ubuntu22.04-gcc11.2-x86_64.7z"
    url_cached="http://ci-files01-hki.ci.qt.io/input/libclang/qt/libclang-llvmorg-${version}-linux-Ubuntu22.04-gcc11.2-x86_64.7z"
    sha1="eed115ea52f3b4283d02d96cd8f4fce95c5aaafe"
else
    version=$libclang_version
    url="https://download.qt.io/development_releases/prebuilt/libclang/qt/libclang-llvmorg-${version}-linux-Rhel8.10-gcc10.0-x86_64.7z"
    url_cached="http://ci-files01-hki.ci.qt.io/input/libclang/qt/libclang-llvmorg-${version}-linux-Rhel8.10-gcc10.0-x86_64.7z"
    sha1="479fa87ad804ec91a462ccb20fc9acad6982bddb"
fi

zip="/tmp/libclang.7z"
destination="/usr/local/libclang-$version"

DownloadURL $url_cached $url $sha1 $zip
if command -v 7zr &> /dev/null; then
    sudo 7zr x $zip -o/usr/local/
else
    sudo 7z x $zip -o/usr/local/
fi
sudo mv /usr/local/libclang "$destination"
rm -rf $zip


SetEnvVar "LLVM_INSTALL_DIR" "$destination"
echo "libClang = $version" >> ~/versions.txt
