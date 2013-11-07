include_directories(../include)

add_executable(cpu_scale cpu_scale.cpp)
target_link_libraries(cpu_scale ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(multi_cpu multi_cpu.cpp)
target_link_libraries(multi_cpu ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(opencl_scale opencl_scale.cpp)
target_link_libraries(opencl_scale ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(opencl_tmerc opencl_tmerc.cpp)
target_link_libraries(opencl_tmerc ${TRANSFORM_LIBRARY})

if(TRANSFORM_HAVE_PROJ4)
	add_executable(proj_tmerc proj_tmerc.cpp)
	target_link_libraries(proj_tmerc ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})
endif()

if(TRANSFORM_HAVE_CUDA)
	add_executable(cuda_scale cuda_scale.cpp)
	target_link_libraries(cuda_scale ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})
endif()
