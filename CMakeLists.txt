cmake_minimum_required(VERSION 3.10)

################################################################################
# Get version from git tag
################################################################################
set(SFML_GAMEPAD_VERSION "0.0.0")
find_package(Git)
if(Git_FOUND)
    set(GIT_HISTORY_LEVEL 0)
    while(NOT VERSION_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} tag --points-at HEAD~${GIT_HISTORY_LEVEL}
            RESULT_VARIABLE GIT_TAG_RESULT
            OUTPUT_VARIABLE GIT_TAG_VALUE
            ERROR_QUIET)

        if (NOT ${GIT_TAG_RESULT} EQUAL 0)
            break() # Command fails : no more commits in history
        endif()

        if (NOT ${GIT_TAG_VALUE} STREQUAL "")
            string(REPLACE "\n" ";" GIT_TAGS ${GIT_TAG_VALUE})

            foreach(GIT_TAG ${GIT_TAGS})
                string(STRIP ${GIT_TAG} GIT_TAG_STRIP)
                if(NOT ${GIT_TAG_STRIP} STREQUAL "")
                        string(REGEX MATCHALL "^v[0-9]+\.[0-9]+\.[0-9]+$" VERSION_TAG ${GIT_TAG_STRIP})
                        if (NOT ${VERSION_TAG} STREQUAL "")
                            message("Version tag found: ${VERSION_TAG}")
                            string(REPLACE "v" "" SFML_GAMEPAD_VERSION ${VERSION_TAG})
                            set(VERSION_FOUND on)
                            break()
                        endif()
                endif()
            endforeach()
        endif()
        math(EXPR GIT_HISTORY_LEVEL "${GIT_HISTORY_LEVEL}+1")
    endwhile()
endif()

################################################################################
# Configure project
################################################################################
project(SFMLGamepad VERSION ${SFML_GAMEPAD_VERSION})
message("Project: SFMLGamepad v${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

macro(set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")
set_option(BUILD_SHARED_LIBS TRUE BOOL "TRUE to build SFMLGamepad as shared library, FALSE to build it as static library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
endif()

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

################################################################################
# Generate library
################################################################################
add_library(sfml-gamepad SHARED
    src/Gamepad.cpp
    src/GamepadImpl.cpp
)

set_target_properties(sfml-gamepad PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(BUILD_SHARED_LIBS)
    set_target_properties(sfml-gamepad PROPERTIES DEBUG_POSTFIX -d)
endif()

target_compile_definitions(sfml-gamepad PRIVATE API_EXPORTS)
target_include_directories(sfml-gamepad PUBLIC include)
target_include_directories(sfml-gamepad PRIVATE src)

target_link_libraries(sfml-gamepad
    sfml-system
    sfml-window
)

if (WIN32)
    target_link_libraries(sfml-gamepad
        XInput
    )

    target_sources(sfml-gamepad
        PRIVATE
        src/Windows/XInput.cpp
    )
endif()

################################################################################
# Generate test app
################################################################################
file(COPY test/coolvetica\ rg.otf DESTINATION ${PROJECT_BINARY_DIR})
file(COPY test/SDL_GameControllerDB/gamecontrollerdb.txt DESTINATION ${PROJECT_BINARY_DIR})

add_executable(sfml-gamepad-test
    test/main.cpp
)

target_include_directories(sfml-gamepad-test PRIVATE test)
target_link_libraries(sfml-gamepad-test
    sfml-system
    sfml-window
    sfml-graphics
    sfml-gamepad
)

################################################################################
# Packaging
################################################################################
install(FILES include/SFMLGamepad/Gamepad.hpp
    DESTINATION include/SFMLGamepad
)

install(TARGETS sfml-gamepad
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/SFMLGamepad
)

set(CPACK_PACKAGE_NAME_SUMMARY "SFML gamepad support")
set(CPACK_PACKAGE_VENDOR "Pierre-Alexandre Pousset")
set(CPACK_PACKAGE_FILE_NAME "SFMLGamepad-${PROJECT_VERSION}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SFMLGamepad ${PROJECT_VERSION}")
set(CPACK_MONOLITHIC_INSTALL ON)

include(CPack)
