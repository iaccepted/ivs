add_library(atom SHARED
atom.h
)

install(TARGETS atom DESTINATION ${IVS_SO_PATH})
set_property(TARGET atom PROPERTY C_STANDARD 11)
set_target_properties(atom PROPERTIES LINKER_LANGUAGE C)
