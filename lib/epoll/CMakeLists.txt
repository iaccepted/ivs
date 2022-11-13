add_library(epoll SHARED
epoll.h
epoll.c
)

include_directories(${PROJECT_SOURCE_DIR}/lib)
install(TARGETS epoll DESTINATION ${IVS_SO_PATH})
set_property(TARGET epoll PROPERTY C_STANDARD 11)
target_link_libraries(epoll -lpthread)
