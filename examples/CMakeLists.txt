include_directories(../include)

add_executable(cpu_scale cpu_scale.cpp)
add_executable(multi_cpu multi_cpu.cpp)
#target_link_libraries(${TRANSFORM_TEST} ${TRANSFORM_LIBRARY} ${TRANSFORM_DEPENDENT_LIBRARIES} )
