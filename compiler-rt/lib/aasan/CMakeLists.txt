
include_directories(..)

set(AASAN_RTL_SOURCES
	aasan.cpp
	)

set(AASAN_RTL_HEADERS
	aasan.h
	)

set(AASAN_COMMON_FLAGS ${SANITIZER_COMMON_FLAGS})

add_compiler_rt_component(aasan)

foreach(arch ${AASAN_SUPPORTED_ARCH})
	set(AASAN_CFLAGS ${AASAN_COMMON_FLAGS})
	append_list_if(COMPILER_RT_HAS_FPIE_FLAG -fPIE AASAN_CFLAGS)
	add_compiler_rt_runtime(clang_rt.aasan
		STATIC
		ARCHS ${arch}
		SOURCES ${AASAN_RTL_SOURCES}
		$<TARGET_OBJECTS:RTInterception.${arch}>
		$<TARGET_OBJECTS:RTSanitizerCommon.${arch}>
		$<TARGET_OBJECTS:RTSanitizerCommonLibc.${arch}>
		CFLAGS ${AASAN_CFLAGS}
		PARENT_TARGET aasan)
	add_sanitizer_rt_symbols(clang_rt.aasan
		ARCHS ${arch}
		 )
	add_dependencies(aasan
		clang_rt.aasan-${arch}-symbols)
endforeach()
