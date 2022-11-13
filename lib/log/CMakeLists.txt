add_library(log SHARED
log.h
log.c
)

include_directories(${PROJECT_SOURCE_DIR}/lib)
install(TARGETS log DESTINATION ${IVS_SO_PATH})
set_property(TARGET log PROPERTY C_STANDARD 11)
