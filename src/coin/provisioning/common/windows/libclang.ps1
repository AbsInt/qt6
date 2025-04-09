param(
    [string]$archVer="32",
    [string]$toolchain="vs2022",
    [bool]$setDefault=$true
)
. "$PSScriptRoot\helpers.ps1"

$libclang_version="19.1.7"
Write-Output "libClang = $libclang_version" >> ~/versions.txt

# PySide versions following 5.6 use a C++ parser based on Clang (http://clang.org/).
# The Clang library (C-bindings), version 3.9 or higher is required for building.

# Starting from Qt 5.11 QDoc requires Clang to parse C++

$baseDestination = "C:\Utils\libclang-" + $libclang_version + "-" + $toolchain

function install() {

    param(
        [string]$sha1=$1,
        [string]$destination=$2
    )

    $zip = "c:\users\qt\downloads\libclang.7z"

    $script:OfficialUrl = "https://download.qt.io/development_releases/prebuilt/libclang/qt/libclang-llvmorg-$libclang_version-windows-$toolchain`_$archVer.7z"
    $script:CachedUrl = "http://ci-files01-hki.ci.qt.io/input/libclang/qt/libclang-llvmorg-$libclang_version-windows-$toolchain`_$archVer.7z"

    Download $OfficialUrl $CachedUrl $zip
    Verify-Checksum $zip $sha1
    Extract-7Zip $zip C:\Utils\
    Rename-Item C:\Utils\libclang $destination
    Remove "$zip"
}

$toolchainSuffix = ""

if ( $toolchain -eq "vs2022" ) {
    if ( $archVer -eq "64" ) {
        $sha1 = "f56057b8679e21a44b341bb1041cb03fbe6f5c0d"
    }
    elseif ( $archVer -eq "arm64" ) {
        $sha1 = "89fddd8c4bde3e8b70382e21059743637c27d38d"
    }
    else {
        $sha1 = ""
    }
    $toolchainSuffix = "msvc"
}

if ( $toolchain -eq "mingw" ) {
    if ( $archVer -eq "64" ) {
        $sha1 = "fcc1f06bd395bc133b7828d0be48e8492b9ba807"
    }
    else {
        $sha1 = ""
    }
    $toolchainSuffix = "mingw"
}


if ( $toolchain -eq "llvm-mingw" ) {
    if ( $archVer -eq "64" ) {
        $sha1 = "ee01352eb68bee252cefb1b8ff4ad086baa8ab5f"
    }
    else {
        $sha1 = ""
    }
    # Due to COIN-1137 forced to use a '_' instead of '-'
    $toolchainSuffix = "llvm_mingw"
}


install $sha1 $baseDestination-$archVer

if ( $setDefault ) {
    Set-EnvironmentVariable "LLVM_INSTALL_DIR" ($baseDestination + "-$archVer")
}
Set-EnvironmentVariable ("LLVM_INSTALL_DIR_${toolchainSuffix}") ($baseDestination + "-$archVer")
