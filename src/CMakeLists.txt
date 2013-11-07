#if(TRANSFORM_STATIC_LIBRARY)
	set(TRANSFORM_LIBRARY_TYPE STATIC)
	#else()
	#set(TRANSFORM_LIBRARY_TYPE SHARED)
	#endif()

SET(TRANSFORM_LIBRARY_SOURCES
	cartographic_cpu.cpp
	cuda_detail.cpp
	opencl_loaders.cpp)

include_directories(../include)

if(CUDA_FOUND)
	# nvcc blows up with c++11 flags, so get rid of them for now
	set(TMP_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	set(CMAKE_CXX_FLAGS ${TRANSFORM_COMMON_CXX_FLAGS})
	cuda_compile(KERNELS_O cuda_kernels.cu)

	#add_library(cuda_kernels STATIC ${KERNELS_O})
	set_source_files_properties(
		${KERNELS_O} PROPERTIES
		EXTERNAL_OBJECT true
		GENERATED true
	)

#SET_TARGET_PROPERTIES(
#cuda_kernels
#PROPERTIES
#LINKER_LANGUAGE C 
#)
	set(EXTRA_OBJS ${KERNELS_O})

	set(CMAKE_CXX_FLAGS ${TMP_CXX_FLAGS})
endif()

message(STATUS "Extra objects: ${EXTRA_OBJS}")

add_library(${TRANSFORM_LIBRARY} ${TRANSFORM_LIBRARY_TYPE} ${TRANSFORM_LIBRARY_SOURCES} ${EXTRA_OBJS})

if (NOT TRANSFORM_STATIC_LIBRARY)
	target_link_libraries(${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES} ${EXTRA_OBJS})
endif()

