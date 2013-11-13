include_directories(../include)

add_executable(cpu_scale cpu_scale.cpp)
add_executable(multi_cpu multi_cpu.cpp)

add_executable(cpu_tmerc_e cpu_tmerc_e.cpp)
target_link_libraries(cpu_tmerc_e ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(opencl_scale opencl_scale.cpp)
target_link_libraries(opencl_scale ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(opencl_tmerc opencl_tmerc.cpp)
target_link_libraries(opencl_tmerc ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(opencl_wgs_tmerc opencl_wgs_tmerc.cpp)
target_link_libraries(opencl_wgs_tmerc ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(cpu_wgs_tmerc cpu_wgs_tmerc.cpp)
target_link_libraries(cpu_wgs_tmerc ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

if(TRANSFORM_HAVE_PROJ4)
	add_executable(proj_tmerc proj_tmerc.cpp)
	target_link_libraries(proj_tmerc ${PROJ4_LIBRARIES} ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

	add_executable(proj_wgs_tmerc proj_wgs_tmerc.cpp)
	target_link_libraries(proj_wgs_tmerc ${PROJ4_LIBRARIES} ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})
endif()
