#!/usr/bin/env bash
# Copyright (C) 2024 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

# This script will build and install FFmpeg static libs
set -ex

# shellcheck source=../unix/InstallFromCompressedFileFromURL.sh
source "${BASH_SOURCE%/*}/../unix/InstallFromCompressedFileFromURL.sh"
# shellcheck source=../unix/SetEnvVar.sh
source "${BASH_SOURCE%/*}/../unix/SetEnvVar.sh"

version="n7.1"
url_public="https://github.com/FFmpeg/FFmpeg/archive/refs/tags/$version.tar.gz"
sha1="f008a93710a7577e3f85a90f4b632cc615164712"
url_cached="http://ci-files01-hki.ci.qt.io/input/ffmpeg/$version.tar.gz"
ffmpeg_name="FFmpeg-$version"

target_dir="$HOME"
ffmpeg_source_dir="$target_dir/$ffmpeg_name"
prefix="/usr/local/ios/ffmpeg"
dylib_regex="^@rpath/.*\.dylib$"

if [ ! -d "$ffmpeg_source_dir" ];
then
   InstallFromCompressedFileFromURL "$url_cached" "$url_public" "$sha1" "$target_dir"
fi

ffmpeg_config_options=$(cat "${BASH_SOURCE%/*}/../shared/ffmpeg_config_options.txt")

build_ffmpeg_ios() {
    local target_platform=$1
    local target_cpu_arch=""
    if [ "$target_platform" == "arm64-simulator" ]; then
        target_sdk="iphonesimulator"
        target_cpu_arch="arm64"
        minos="-mios-simulator-version-min=16.0"
    elif [ "$target_platform" == "x86_64-simulator" ]; then
        target_sdk="iphonesimulator"
        target_cpu_arch="x86_64"
        minos="-mios-simulator-version-min=16.0"
    elif [ "$target_platform" == "arm64-iphoneos" ]; then
        target_sdk="iphoneos"
        target_cpu_arch="arm64"
        minos="-miphoneos-version-min=16.0"
    else
        echo "Error when building FFmpeg for iOS. Unknown parameter given for target_platform: '${target_platform}'"
        exit 1
    fi

    local build_dir="$ffmpeg_source_dir/build_ios/$target_platform"
    sudo mkdir -p "$build_dir"
    pushd "$build_dir"

    # shellcheck disable=SC2086
    sudo "$ffmpeg_source_dir/configure" $ffmpeg_config_options \
    --sysroot="$(xcrun --sdk "$target_sdk" --show-sdk-path)" \
    --enable-cross-compile \
    --enable-optimizations \
    --prefix=$prefix \
    --arch=$target_cpu_arch \
    --cc="xcrun --sdk ${target_sdk} clang -arch $target_cpu_arch" \
    --cxx="xcrun --sdk ${target_sdk} clang++ -arch $target_cpu_arch" \
    --target-os=darwin \
    --extra-ldflags="$minos" \
    --enable-shared \
    --disable-static \
    --install-name-dir='@rpath' \
    --disable-audiotoolbox

    sudo make install DESTDIR="$build_dir/installed" -j4
    popd
}

install_ffmpeg() {
    for dir in "$@"; do
        echo "Processing files in $dir ..."
        pushd "$dir" >/dev/null
        find . -type l -name '*.*.dylib' | while read -r f; do
            dst="${f:1}"
            dstdir="$(dirname "$dst")"
            sudo mkdir -p "$dstdir"

            if [[ ! -f "$dst" ]]; then
                echo "<Copying $dir/$f to $dst"
                sudo cp -c "$f" "$dst"
                symlinkname="$(tmp=${f/*\/}; echo ${tmp/\.*}).dylib"
                sudo ln -s "$(basename -- "$f")" $dstdir/"$symlinkname"
            elif lipo -info "$f" >/dev/null 2>&1; then
                echo "Lipoing $dir/$f into $dst"
                sudo lipo -create -output "$dst" "$dst" "$f"
            elif ! diff "$f" "$dst"; then
                echo "Error: File $f in $dir doesn't match destination $dst"
                exit 1
            fi
        done
        echo "LS"
        popd >/dev/null
    done
    sudo cp -r $1$prefix/include $prefix
}

build_info_plist() {
    local file_path="$1"
    local framework_name="$2"
    local framework_id="$3"

    local minimum_version_key="MinimumOSVersion"
    local minimum_os_version="16.0"
    local supported_platforms="iPhoneOS"

    # TODO: This should be filled out with the actual version of FFmpeg that we are
    # deploying.
    info_plist="<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
<plist version=\"1.0\">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>${framework_name}</string>
    <key>CFBundleIdentifier</key>
    <string>${framework_id}</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>${framework_name}</string>
    <key>CFBundlePackageType</key>
    <string>FMWK</string>
    <key>CFBundleShortVersionString</key>
    <string>7.0.2</string>
    <key>CFBundleVersion</key>
    <string>7.0.2</string>
    <key>CFBundleSignature</key>
    <string>????</string>
    <key>${minimum_version_key}</key>
    <string>${minimum_os_version}</string>
    <key>CFBundleSupportedPlatforms</key>
    <array>
        <string>${supported_platforms}</string>
    </array>
    <key>NSPrincipalClass</key>
    <string></string>
</dict>
</plist>"
    echo $info_plist | sudo tee ${file_path} 1>/dev/null
}

create_framework() {
    # Create a 'traditional' framework from the corresponding dylib.
    local framework_name="$1"
    local platform="$2" # For now it's either arm64 or arm64-simulator, see below.
    local ffmpeg_library_path="$ffmpeg_source_dir/build_ios/${platform}/installed/usr/local/ios/ffmpeg"
    local framework_complete_path="${ffmpeg_library_path}/framework/${framework_name}.framework/${framework_name}"

    sudo mkdir -p "${ffmpeg_library_path}/framework/${framework_name}.framework"
    sudo cp "${ffmpeg_library_path}/lib/${framework_name}.dylib" "${ffmpeg_library_path}/framework/${framework_name}.framework/${framework_name}"

    # Fix LC_ID_DYLIB (to be libavcodec.framework/libavcodec instead of @rpath/libavcodec.xx.yy.dylib
    sudo install_name_tool -id @rpath/${framework_name}.framework/${framework_name} "${framework_complete_path}"

    build_info_plist "${ffmpeg_library_path}/framework/${framework_name}.framework/Info.plist" "${framework_name}" "io.qt.ffmpegkit."${framework_name}

    # Fix all FFmpeg-related LC_LOAD_DYLIB, similar to how we fixed LC_ID_DYLIB above:
    otool -L "$framework_complete_path" | awk '/\t/ {print $1}' | egrep "$dylib_regex" | while read -r dependency_path; do
        found_name=$(tmp=${dependency_path/*\/}; echo ${tmp/\.*})
        if [ "$found_name" != "$framework_name" ]
        then
            sudo install_name_tool -change "$dependency_path" @rpath/${found_name}.framework/${found_name} "${framework_complete_path}"
        fi
    done
    #sudo mkdir -p "$prefix/framework/"
    #sudo cp -r "${ffmpeg_library_path}/framework/${framework_name}.framework" "$prefix/framework/"
}

create_xcframework() {
    # Create 'traditional' framework from the corresponding dylib,
    # also creating
    local framework_name="$1"
    local target_platform_a="$2"
    local target_platform_b="$3"

    local fw_a="$ffmpeg_source_dir/build_ios/${target_platform_a}/installed/usr/local/ios/ffmpeg/framework/${framework_name}.framework"
    local fw_b="$ffmpeg_source_dir/build_ios/${target_platform_b}/installed/usr/local/ios/ffmpeg/framework/${framework_name}.framework"

    sudo mkdir -p "$prefix/framework/"
    sudo xcodebuild -create-xcframework -framework $fw_a -framework $fw_b -output "${prefix}/framework/${framework_name}.xcframework"
}

build_ffmpeg_ios "x86_64-simulator"
build_ffmpeg_ios "arm64-iphoneos"

ffmpeg_libs="libavcodec libavformat libavutil libswresample libswscale"

for name in $ffmpeg_libs; do
    create_framework $name "arm64-iphoneos"
    create_framework $name "x86_64-simulator"
done

# Create corresponding (xc)frameworks containing both arm64 and arm64-simulator frameworks:
for name in $ffmpeg_libs; do
    create_xcframework $name "arm64-iphoneos" "x86_64-simulator"
done

install_ffmpeg "$ffmpeg_source_dir/build_ios/arm64-iphoneos/installed"

SetEnvVar "FFMPEG_DIR_IOS" $prefix
