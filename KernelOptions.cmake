function(simplix_add_kernel_options target_name)
	set(KERNEL_CFLAGS
			-ffreestanding -nostdlib
			-fno-strict-aliasing -fno-use-cxa-atexit
			-mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-80387 -mno-avx -mno-soft-float
			-mcmodel=kernel
	)

	set(KERNEL_CXXFLAGS
			-fno-exceptions
			-fno-rtti
			-fno-unwind-tables -fno-asynchronous-unwind-tables
	)

	set(KERNEL_LDFLAGS
			-z max-page-size=0x1000
			-Wl,--build-id=none
	)

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		list(APPEND KERNEL_CFLAGS -fstack-protector-strong)
	else ()
		list(APPEND KERNEL_CFLAGS -fstack-protector-all -fno-omit-frame-pointer)
	endif ()

	set(KERNEL_COMPILE_OPTIONS
			$<$<COMPILE_LANGUAGE:C>:${KERNEL_CFLAGS}>
			$<$<COMPILE_LANGUAGE:CXX>:${KERNEL_CFLAGS} ${KERNEL_CXXFLAGS}>
	)

	set(KERNEL_LINK_OPTIONS ${KERNEL_CFLAGS} ${KERNEL_LDFLAGS})

	target_compile_options(${target_name} PRIVATE ${KERNEL_COMPILE_OPTIONS})
	target_compile_definitions(${target_name} PRIVATE -DKERNEL)
	target_link_options(${target_name} PRIVATE ${KERNEL_LINK_OPTIONS})
endfunction()

function(simplix_add_linker_script target_name linker_script)
	target_link_options(${target_name} PRIVATE -T${linker_script})
	set_target_properties(${target_name} PROPERTIES LINK_DEPENDS ${linker_script})
endfunction()
