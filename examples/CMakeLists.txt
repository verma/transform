include_directories(../include)

add_executable(cpu_scale cpu_scale.cpp)
add_executable(multi_cpu multi_cpu.cpp)

if(TRANSFORM_HAVE_PROJ4)
	add_executable(proj_tmerc proj_tmerc.cpp)
	target_link_libraries(proj_tmerc ${PROJ4_LIBRARIES})
endif()
