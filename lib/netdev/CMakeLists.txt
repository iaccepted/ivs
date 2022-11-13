add_library(netdev SHARED
netdev_provider.c
netdev_provider.h
netdev_virtio.c
netdev_virtio.h
netdev.h
)

include_directories(${PROJECT_SOURCE_DIR}/lib)
install(TARGETS netdev DESTINATION ${IVS_SO_PATH})
set_property(TARGET netdev PROPERTY C_STANDARD 11)
