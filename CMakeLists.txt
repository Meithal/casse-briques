cmake_minimum_required(VERSION 3.10)
project(casses_briques
        VERSION 0.0.1
        DESCRIPTION "projet esgi 3e annee moc"
        HOMEPAGE_URL "https://gitlab.com/projets-esgi1/casses-briques-esgi/programme"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)

add_executable(casses_briques-gui
        programme/client/gui/client-gui.c
        programme/client/gui/freetype-functions.c
        programme/client/gui/freetype-functions.h
        programme/client/gui/sdl-functions.c
        programme/client/gui/sdl-functions.h
        programme/common/game_rules/gameRules.c
        programme/common/game_rules/gameRules.h
        programme/common/linked_list/linked_lists.c
        programme/common/linked_list/linked_lists.h
        programme/client/callbacks.c
        programme/client/callbacks.h
        )
target_include_directories(casses_briques-gui PRIVATE programme)

add_executable(casses_briques-cli
        programme/client/cli/grille.c
        programme/common/game_rules/gameRules.c
        programme/common/game_rules/gameRules.h
        programme/client/cli/askIntInput.c
        programme/client/cli/askIntInput.h
        programme/client/cli/grilleDisplay.c
        programme/client/cli/grilleDisplay.h
        programme/client/cli/client.c
        programme/client/cli/client.h
        programme/common/linked_list/linked_lists.c
        programme/common/linked_list/linked_lists.h
        programme/client/callbacks.c
        programme/client/callbacks.h)
target_include_directories(casses_briques-cli PRIVATE programme)

add_executable(casses_briques-server
        programme/server/server.h
        programme/server/server.c
        programme/common/game_rules/gameRules.c
        programme/common/game_rules/gameRules.h
        programme/client/cli/askIntInput.c
        programme/client/cli/askIntInput.h
        programme/client/cli/grilleDisplay.c
        programme/client/cli/grilleDisplay.h
        programme/client/cli/client.c
        programme/client/cli/client.h
        programme/common/linked_list/linked_lists.c
        programme/common/linked_list/linked_lists.h
        programme/client/callbacks.c
        programme/client/callbacks.h)
target_include_directories(casses_briques-server PRIVATE programme)

if(UNIX)
    message("POSIX DETECTED")
    target_sources(casses_briques-cli PRIVATE
            programme/client/cli/linux_compatibility/conio_polyfill.c
            programme/client/cli/linux_compatibility/conio_polyfill.h)
    target_sources(casses_briques-server PRIVATE
            programme/client/cli/linux_compatibility/conio_polyfill.c
            programme/client/cli/linux_compatibility/conio_polyfill.h)

endif()

if(WIN32)
    message("WINDOWS DETECTED")
    target_link_libraries(casses_briques-gui PRIVATE wsock32 ws2_32)
    target_link_libraries(casses_briques-cli PRIVATE wsock32 ws2_32)
    target_link_libraries(casses_briques-server PRIVATE wsock32 ws2_32)
    target_sources(casses_briques-gui PRIVATE
            programme/common/network/winsock/winsock_utils.c
            programme/common/network/winsock/winsock_utils.h)
    target_sources(casses_briques-cli PRIVATE
            programme/common/network/winsock/winsock_utils.c
            programme/common/network/winsock/winsock_utils.h
            programme/client/cli/windows_compatibility/winterm.c
            programme/client/cli/windows_compatibility/winterm.h
            )
    target_sources(casses_briques-server PRIVATE
            programme/common/network/winsock/winsock_utils.c
            programme/common/network/winsock/winsock_utils.h
            programme/client/cli/windows_compatibility/winterm.c
            programme/client/cli/windows_compatibility/winterm.h
            )

    target_compile_definitions(casses_briques-server PRIVATE _UNICODE)
    target_compile_definitions(casses_briques-cli PRIVATE _UNICODE)
    target_compile_definitions(casses_briques-server PRIVATE UNICODE)
    target_compile_definitions(casses_briques-cli PRIVATE UNICODE)
endif()

if(MSVC)
    message("Including dirent polyfill.")
    target_include_directories(casses_briques-server PRIVATE ${CMAKE_SOURCE_DIR}/vendor/dirent/include)
    target_include_directories(casses_briques-cli PRIVATE ${CMAKE_SOURCE_DIR}/vendor/dirent/include)
    target_include_directories(casses_briques-gui PRIVATE ${CMAKE_SOURCE_DIR}/vendor/dirent/include)
endif()

add_custom_command(
        TARGET ${PROJECT_NAME}-gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}-gui>/assets"
        VERBATIM)
add_custom_command(
        TARGET ${PROJECT_NAME}-cli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}-gui>/assets"
        VERBATIM)
add_custom_command(
        TARGET ${PROJECT_NAME}-server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}-gui>/assets"
        VERBATIM)

execute_process(COMMAND python download-deps.py WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

add_subdirectory(vendor/sdl2 EXCLUDE_FROM_ALL)

if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(casses_briques-gui PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(casses_briques-gui PRIVATE SDL2::SDL2)

# https://stackoverflow.com/a/61708554/13577241
if (WIN32)
    # copy the .dll file to the same folder as the executable
    add_custom_command(
            TARGET ${PROJECT_NAME}-gui POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:${PROJECT_NAME}-gui>
            VERBATIM)
endif()

set(FT_DISABLE_ZLIB TRUE)
set(FT_DISABLE_BZIP2 TRUE)
set(FT_DISABLE_PNG TRUE)
set(FT_DISABLE_HARFBUZZ TRUE)
set(FT_DISABLE_BROTLI TRUE)
add_subdirectory(vendor/freetype)

target_link_libraries(casses_briques-gui PRIVATE freetype)

#add_subdirectory(vendor/json)
#target_link_libraries(casses_briques-server PUBLIC sjson)
#target_link_libraries(casses_briques-cli PUBLIC sjson)
#target_link_libraries(casses_briques-gui PUBLIC sjson)
