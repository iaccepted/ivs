include_directories(${PROJECT_SOURCE_DIR}/lib)
add_executable(test_ds test_dynamic_str.c)
set_property(TARGET test_ds PROPERTY C_STANDARD 11)
target_link_libraries(test_ds ds)

add_executable(test_epoll test_epoll.c)
set_property(TARGET test_epoll PROPERTY C_STANDARD 11)
target_link_libraries(test_epoll epoll)

add_executable(test_log test_log.c)
set_property(TARGET test_log PROPERTY C_STANDARD 11)
target_link_libraries(test_log log ds)
