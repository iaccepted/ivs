add_library(list SHARED
list.h
)

install(TARGETS list DESTINATION ${IVS_SO_PATH})
set_property(TARGET list PROPERTY C_STANDARD 11)
set_target_properties(list PROPERTIES LINKER_LANGUAGE C)
