add_library(gladiator_shared_headers INTERFACE)

set(_gladiator_processor "${CMAKE_SYSTEM_PROCESSOR}")
if(NOT _gladiator_processor)
    set(_gladiator_processor "${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()
string(TOLOWER "${_gladiator_processor}" _gladiator_processor_lower)

set(_gladiator_id386 0)
if(_gladiator_processor_lower MATCHES "^(i[3-6]86|x86)$")
    set(_gladiator_id386 1)
endif()

set(_gladiator_idaxp 0)
if(_gladiator_processor_lower MATCHES "alpha")
    set(_gladiator_idaxp 1)
endif()

target_include_directories(gladiator_shared_headers
    INTERFACE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/shared
)

target_compile_definitions(gladiator_shared_headers
    INTERFACE
        id386=${_gladiator_id386}
        idaxp=${_gladiator_idaxp}
)
