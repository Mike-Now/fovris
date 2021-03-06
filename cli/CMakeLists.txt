set(LINENOISE_PATH
    "${PROJECT_SOURCE_DIR}/thirdparty/linenoise-ng"
    )
set(CMDLINE_PARSER_PATH
    "${PROJECT_SOURCE_DIR}/thirdparty/optionparser"
    )


find_program(RE2C_EXECUTABLE re2c)
if(NOT ${RE2C_EXECUTABLE} MATCHES "RE2C_EXECUTABLE-NOTFOUND")
    add_custom_command (
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/CmdLineScanner.re2c.cc"
        COMMAND re2c -i -o CmdLineScanner.re2c.cc CmdLineScanner.re 
        DEPENDS CmdLineScanner.re
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

set(HEADERS
    CmdLineScanner.h
    )

set(SRC
    CmdLineScanner.re2c.cc
    Cli.cc)

include_directories(
    "${LINENOISE_PATH}/include"
    "${CMDLINE_PARSER_PATH}/include"
    "${PROJECT_SOURCE_DIR}/include/fovris"
    )

add_executable(fovris ${SRC} ${HEADERS})
target_link_libraries(fovris libfovris linenoise)

install(TARGETS fovris 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
