add_library(vhost SHARED
vhost_msg.c
vhost_msg.h
vhost_socket.c
vhost_socket.h
vhost_user.c
vhost_user.h
)

include_directories(${PROJECT_SOURCE_DIR}/lib)
install(TARGETS vhost DESTINATION ${IVS_SO_PATH})
set_property(TARGET vhost PROPERTY C_STANDARD 11)
target_link_libraries(vhost memory)
