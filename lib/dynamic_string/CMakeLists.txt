add_library(ds SHARED
dynamic_str.c
dynamic_str.h
)

include_directories(${PROJECT_SOURCE_DIR}/lib)
install(TARGETS ds DESTINATION ${IVS_SO_PATH})
set_property(TARGET ds PROPERTY C_STANDARD 11)
