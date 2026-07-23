#!/usr/bin/env bash

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" || exit 1; pwd)

BUILD_DIR=${SCRIPT_DIR}/build
PREFIX_DIR=${SCRIPT_DIR}/prefix

GNU_MIRROR=https://ftpmirror.gnu.org

BINUTILS_VERSION=2.46.1
BINUTILS_TARBALL=binutils-$BINUTILS_VERSION.tar.xz
BINUTILS_URL=$GNU_MIRROR/binutils/$BINUTILS_TARBALL

GCC_VERSION=16.1.0
GCC_TARBALL=gcc-$GCC_VERSION.tar.xz
GCC_URL=$GNU_MIRROR/gcc/gcc-$GCC_VERSION/$GCC_TARBALL

TARGET=x86_64-elf

download_file()
{
	local url=$1
	local destination=$2

	curl -L -C - -o "$destination" "$url"
}

prepare_sources()
{
	local url=$1
	local tarball=$2

	if [ ! -f "$tarball.dl" ]; then
		download_file "$url" "$tarball"
		touch "$tarball.dl"
	fi

	if [ ! -f "$tarball.ex" ]; then
		tar -xf "$tarball"
		touch "$tarball.ex"
	fi
}

apply_patch()
{
	local patch_file=$1
	patch -s -p1 < "$patch_file" || exit 1
}

patch_gcc()
{
	if [ ! -f "$GCC_TARBALL.patched" ]; then
		apply_patch "../libgcc_mno-red-zone.patch"
		touch "$GCC_TARBALL.patched"
	fi
}

build_binutils()
{
	if [ -f "$BINUTILS_TARBALL.bld" ]; then
		return
	fi

	mkdir binutils-build
	pushd binutils-build > /dev/null || exit 1

	../binutils-$BINUTILS_VERSION/configure \
		--target="$TARGET" \
		--prefix="$PREFIX_DIR" \
		--with-sysroot \
		--disable-nls \
		--disable-werror \
		|| exit 1

	make -j"$(nproc)" || exit 1
	make install || exit 1

	popd || exit 1

	touch "$BINUTILS_TARBALL.bld"
}

build_gcc()
{
	if [ -f "$GCC_TARBALL.bld" ]; then
		return
	fi

	mkdir gcc-build
	pushd gcc-build > /dev/null || exit 1

	../gcc-$GCC_VERSION/configure \
		--target="$TARGET" \
		--prefix="$PREFIX_DIR" \
		--disable-nls \
		--enable-languages=c,c++ \
		--without-headers \
		--disable-libquadmath --disable-libquadmath-support \
		--disable-libssp \
		--disable-hosted-libstdcxx \
		--enable-initfini-array \
		|| exit 1

	make -j"$(nproc)" || exit 1
	make install || exit 1

	popd || exit 1

	touch "$GCC_TARBALL.bld"
}

main()
{
	mkdir -p "$BUILD_DIR"
	pushd "$BUILD_DIR" > /dev/null || exit 1

	prepare_sources "$BINUTILS_URL" "$BINUTILS_TARBALL"
	prepare_sources "$GCC_URL" "$GCC_TARBALL"
	patch_gcc

	build_binutils
	build_gcc

	popd > /dev/null || exit 1
}

if [ "$0" = "${BASH_SOURCE[0]}" ]; then
	main
fi
