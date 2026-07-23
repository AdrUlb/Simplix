file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS Source/*.asm Source/*.c Source/*.h Source/*.cpp Source/*.hpp Source/*.tpp)

set(CFLAGS "-masm=intel")
set(CXXFLAGS "")

add_executable(Kernel ${SOURCES})
target_include_directories(Kernel
		PRIVATE Source
		PRIVATE ${limine_protocol_SOURCE_DIR}/include
)
target_compile_options(Kernel PRIVATE
		$<$<COMPILE_LANGUAGE:C>:${CFLAGS}>
		$<$<COMPILE_LANGUAGE:CXX>:${CFLAGS} ${CXXFLAGS}>
)
target_link_libraries(Kernel KSilly)
simplix_add_kernel_options(Kernel)
simplix_add_linker_script(Kernel ${CMAKE_CURRENT_SOURCE_DIR}/Link.ld)
target_compile_options(Kernel PRIVATE -Wall -Wextra)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_definitions(Kernel PRIVATE -DKERNEL_STRIPPED)
	add_custom_command(
			TARGET Kernel POST_BUILD
			COMMAND ${CMAKE_STRIP} $<TARGET_FILE:Kernel>
	)
endif ()
