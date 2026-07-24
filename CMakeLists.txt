cmake_minimum_required(VERSION 3.25)

include(Extern/Silly/Silly.cmake)
include(KernelOptions.cmake)
include(FetchContent)

project(Simplix)

FetchContent_Declare(limine_binary
		URL https://github.com/Limine-Bootloader/Limine/releases/download/v12.3.2/limine-binary.tar.xz
)
FetchContent_Declare(limine_protocol
		GIT_REPOSITORY https://github.com/Limine-Bootloader/limine-protocol
		GIT_TAG trunk
)

FetchContent_MakeAvailable(limine_binary)
FetchContent_MakeAvailable(limine_protocol)

add_custom_command(
		DEPENDS ${limine_binary_SOURCE_DIR}
		OUTPUT ${limine_binary_SOURCE_DIR}/limine
		COMMAND make -C "${limine_binary_SOURCE_DIR}"
		COMMENT "Buiilding Limine tool..."
)
add_custom_target(LimineTool DEPENDS ${limine_binary_SOURCE_DIR}/limine)

enable_language(C CXX ASM_NASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wno-unused-function -Wno-changes-meaning)
add_compile_definitions(-DSILLY_GLOBAL)

silly_library_define(Silly STATIC)
silly_library_define(KSilly STATIC)
simplix_add_kernel_options(KSilly)

add_subdirectory(Kernel)

set(ISO_FILE "${CMAKE_CURRENT_BINARY_DIR}/Simplix.iso")
set(ISO_TEMP "${CMAKE_CURRENT_BINARY_DIR}/isotemp")
set(LIMINE_CONF "${CMAKE_CURRENT_SOURCE_DIR}/limine.conf")

add_custom_command(
		OUTPUT ${ISO_FILE}
		DEPENDS Kernel
		DEPENDS LimineTool
		DEPENDS ${LIMINE_CONF}

		COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_TEMP}/boot/limine
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Kernel> ${ISO_TEMP}/boot/kernel.elf
		COMMAND ${CMAKE_COMMAND} -E copy ${LIMINE_CONF} ${ISO_TEMP}/limine.conf

		COMMAND ${CMAKE_COMMAND} -E copy ${limine_binary_SOURCE_DIR}/limine-bios.sys ${ISO_TEMP}/boot/limine/
		COMMAND ${CMAKE_COMMAND} -E copy ${limine_binary_SOURCE_DIR}/limine-bios-cd.bin ${ISO_TEMP}/boot/limine/

		COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_TEMP}/EFI/BOOT
		COMMAND ${CMAKE_COMMAND} -E copy ${limine_binary_SOURCE_DIR}/limine-uefi-cd.bin ${ISO_TEMP}/boot/limine/
		COMMAND ${CMAKE_COMMAND} -E copy ${limine_binary_SOURCE_DIR}/BOOTX64.EFI ${ISO_TEMP}/EFI/BOOT/BOOTX64.EFI

		COMMAND xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin
			-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus
			-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin
			-efi-boot-part --efi-boot-image --protective-msdos-label
			${ISO_TEMP} -o ${ISO_FILE}

		COMMAND ${limine_binary_SOURCE_DIR}/limine bios-install ${ISO_FILE}

		COMMAND ${CMAKE_COMMAND} -E rm -r ${ISO_TEMP}
)

add_custom_target(IsoImage ALL DEPENDS ${ISO_FILE})
