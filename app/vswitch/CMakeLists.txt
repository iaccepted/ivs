set(SRC_LIST
main.c
)

add_executable(ivs ${SRC_LIST})
target_link_libraries(ivs log epoll list ds netdev vhost)
