include_directories(../include)

set(TRANSFORM_TEST_SOURCES test_main.cpp opencl_test.cpp)

if(TRANSFORM_HAVE_PROJ4)
	set(TRANSFORM_TEST_SOURCES ${TRANSFORM_TEST_SOURCES} proj_test.cpp)
endif()

set(ALL_LIBRARIES ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES})

add_executable(transform_tests ${TRANSFORM_TEST_SOURCES})
target_link_libraries(transform_tests ${ALL_LIBRARIES})
