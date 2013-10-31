include_directories(../include)

if(TRANSFORM_HAVE_PROJ4)
	add_executable(proj_marks proj_marks.cpp)
	target_link_libraries(proj_marks ${TRANSFORM_DEPENDENT_LIBRARIES})
endif()

set(ALL_LIBRARIES ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(cpu_tmerc_sphere_marks cpu_tmerc_sphere_marks.cpp)
target_link_libraries(cpu_tmerc_sphere_marks ${ALL_LIBRARIES})



