include_directories(../include)

set(ALL_LIBRARIES ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

if(TRANSFORM_HAVE_PROJ4)
	add_executable(proj_marks proj_marks.cpp)
	target_link_libraries(proj_marks ${ALL_LIBRARIES})

	add_executable(proj_vs_cpu_vs_opencl proj_vs_cpu_vs_opencl.cpp)
	target_link_libraries(proj_vs_cpu_vs_opencl ${ALL_LIBRARIES})
endif()

add_executable(cpu_tmerc_sphere_marks cpu_tmerc_sphere_marks.cpp)
target_link_libraries(cpu_tmerc_sphere_marks ${ALL_LIBRARIES})

add_executable(opencl_marks opencl_marks.cpp)
target_link_libraries(opencl_marks ${ALL_LIBRARIES})



