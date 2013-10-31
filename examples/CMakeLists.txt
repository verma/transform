include_directories(../include)

add_executable(cpu_scale cpu_scale.cpp)
add_executable(multi_cpu multi_cpu.cpp)

add_executable(opencl_scale opencl_scale.cpp)
target_link_libraries(opencl_scale ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

if(TRANSFORM_HAVE_PROJ4)
	add_executable(proj_tmerc proj_tmerc.cpp)
	target_link_libraries(proj_tmerc ${PROJ4_LIBRARIES})
endif()
