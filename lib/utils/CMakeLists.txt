add_library(utils SHARED
util.h
)

install(TARGETS utils DESTINATION ${IVS_SO_PATH})
set_property(TARGET utils PROPERTY C_STANDARD 11)
set_target_properties(utils PROPERTIES LINKER_LANGUAGE C)
