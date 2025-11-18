add_library(botlib_precomp STATIC
    l_precomp.c
    l_script.c
)

register_botlib_sources(
    TARGET botlib_precomp
    SOURCES
        l_precomp.c
        l_script.c
)

target_include_directories(botlib_precomp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(botlib_precomp
    PUBLIC
        gladiator_shared_headers
)
