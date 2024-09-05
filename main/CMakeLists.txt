idf_component_register(SRCS "bluetooth.c" "common.c" "gravity.c"
                    INCLUDE_DIRS ".")
target_link_libraries(${COMPONENT_LIB} -Wl,-zmuldefs)