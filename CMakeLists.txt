cmake_minimum_required(VERSION 3.23)
project(cases_briques
        VERSION 0.0.1
        DESCRIPTION "projet esgi 3e annee moc"
        HOMEPAGE_URL "https://gitlab.com/projets-esgi1/casses-briques-esgi/programme"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)

include_directories(programme)

execute_process(COMMAND python download-deps.py WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/programme")

add_executable(cases_briques
        programme/client/gui/client-gui.c)

add_subdirectory(programme/vendor/sdl2 EXCLUDE_FROM_ALL)

if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(cases_briques PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(cases_briques PRIVATE SDL2::SDL2)

# https://stackoverflow.com/a/61708554/13577241
if (WIN32)
    # copy the .dll file to the same folder as the executable
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            VERBATIM)
endif()
