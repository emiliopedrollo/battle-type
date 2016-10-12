CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
INCLUDE(FindPkgConfig)
PROJECT(battle_type)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
FIND_PACKAGE(Threads)
SET(SOURCE_FILES
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
SET(RESOURCE_COMPILER "xxd")
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

ADD_EXECUTABLE(battle_type ${SOURCE_FILES}  ${COMPILED_RESOURCES})
SET(EXECUTABLE_OUTPUT_PATH output)

PKG_SEARCH_MODULE(REQUIRED liballegro)
PKG_SEARCH_MODULE(REQUIRED lanet)

TARGET_LINK_LIBRARIES(battle_type ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(battle_type m)
TARGET_LINK_LIBRARIES(battle_type enet)
TARGET_LINK_LIBRARIES(battle_type allegro)
TARGET_LINK_LIBRARIES(battle_type allegro_image)
TARGET_LINK_LIBRARIES(battle_type allegro_primitives)
TARGET_LINK_LIBRARIES(battle_type allegro_font)
TARGET_LINK_LIBRARIES(battle_type allegro_ttf)
TARGET_LINK_LIBRARIES(battle_type allegro_memfile)
