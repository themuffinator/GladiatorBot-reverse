add_library(botlib_ea STATIC
    ea_main.c
)

register_botlib_sources(
    TARGET botlib_ea
    SOURCES
        ea_main.c
)

target_link_libraries(botlib_ea
    PUBLIC
        botlib_common
        q2bridge
        gladiator_shared_headers
)
