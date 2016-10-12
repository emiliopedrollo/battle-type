cmake_minimum_required(VERSION 3.5)
include(FindPkgConfig)
project(battle_type)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Threads)

set(SOURCE_FILES
    alphas/allegro_samples/enet_common.h
    alphas/allegro_samples/ex_enet_client.c
    alphas/allegro_samples/ex_enet_client.h
    alphas/allegro_samples/ex_enet_server.c
    alphas/allegro_samples/ex_enet_server.h
    src/buttons.c
    src/buttons.h
    src/main.c
    src/menu_screen.c
    src/menu_screen.h)

set(RESOURCE_COMPILER "xxd")
FILE(GLOB_RECURSE COMPILED_RESOURCES
        RELATIVE ${PROJECT_SOURCE_DIR}/src/resources
        "src/resources/img/*.png"
        "src/resources/img/*.jpg"
        "src/resources/font/*.ttf")

FOREACH(INPUT_FILE ${COMPILED_RESOURCES})
    SET(OUTPUT_FILE ${INPUT_FILE}.h)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${RESOURCE_COMPILER} -i ${INPUT_FILE} ${OUTPUT_FILE}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/resources
            COMMENT "Compiling ${INPUT_FILE} to ${OUTPUT_FILE} binary")
    LIST (APPEND COMPILED_RESOURCES ${OUTPUT_FILE})
ENDFOREACH()

add_executable(battle_type ${SOURCE_FILES}  ${COMPILED_RESOURCES})




pkg_search_module(REQUIRED liballegro)
pkg_search_module(REQUIRED lanet)

target_link_libraries(battle_type ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(battle_type m)
target_link_libraries(battle_type enet)
target_link_libraries(battle_type allegro)
target_link_libraries(battle_type allegro_image)
target_link_libraries(battle_type allegro_primitives)
target_link_libraries(battle_type allegro_font)
target_link_libraries(battle_type allegro_ttf)
target_link_libraries(battle_type allegro_memfile)

SET(EXECUTABLE_OUTPUT_PATH output)