add_library(memory SHARED
memory.c
memory.h
)

install(TARGETS memory DESTINATION ${IVS_SO_PATH})
set_property(TARGET memory PROPERTY C_STANDARD 11)
