CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
FIND_PACKAGE(PkgConfig REQUIRED)
PROJECT(battle_type)

# Define -std=C11 para o compilador
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")


if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    ADD_DEFINITIONS(-DWIN32)
endif()

# Define todos os fontes e cabeçalhos a serem compilados
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
    src/menu_screen.h
    src/utils.c
    src/utils.h
    src/battleship.c
    src/battleship.h
    src/main.h
    src/game.c
    src/game.h
    src/server.c
    src/server.h
    src/enet_common.h
    src/client.c
    src/client.h
    src/rank.c
    src/rank.h src/sha256.c src/sha256.h)

# Cria arquivo cabeçalho para cada recurso utilizado
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

# Adiciona target
ADD_EXECUTABLE(battle_type ${SOURCE_FILES} ${COMPILED_RESOURCES})

# Define diretório de build
SET(EXECUTABLE_OUTPUT_PATH output)

# Procura pelo pacote Threads no sistema e define variaval CMAKE_THREAD_LIBS_INIT
FIND_PACKAGE(Threads REQUIRED)



if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/allegro/lib)

    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_THREAD_LIBS_INIT}) # -pthread ou equivalente
    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_LIBRARY_PATH}/liballegro-5.0.10-static-mt.a)
    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_LIBRARY_PATH}/liballegro_image-5.0.10-static-mt.a)
    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_LIBRARY_PATH}/liballegro_primitives-5.0.10-static-mt.a)
    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_LIBRARY_PATH}/liballegro_font-5.0.10-static-mt.a)
    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_LIBRARY_PATH}/liballegro_ttf-5.0.10-static-mt.a)
    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_LIBRARY_PATH}/liballegro_memfile-5.0.10-static-mt.a)
    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_LIBRARY_PATH}/liballegro_dialog-5.0.10-static-mt.a)
    TARGET_LINK_LIBRARIES(battle_type libadvapi32.a)
    TARGET_LINK_LIBRARIES(battle_type libcomctl32.a)
    TARGET_LINK_LIBRARIES(battle_type libgdi32.a)
    TARGET_LINK_LIBRARIES(battle_type libgdiplus.a)
    TARGET_LINK_LIBRARIES(battle_type libglu32.a)
    TARGET_LINK_LIBRARIES(battle_type libkernel32.a)
    TARGET_LINK_LIBRARIES(battle_type libole32.a)
    TARGET_LINK_LIBRARIES(battle_type libopengl32.a)
    TARGET_LINK_LIBRARIES(battle_type libpsapi.a)
    TARGET_LINK_LIBRARIES(battle_type libshell32.a)
    TARGET_LINK_LIBRARIES(battle_type libshlwapi.a)
    TARGET_LINK_LIBRARIES(battle_type libuser32.a)
    TARGET_LINK_LIBRARIES(battle_type libuuid.a)
    TARGET_LINK_LIBRARIES(battle_type libwinmm.a)
    TARGET_LINK_LIBRARIES(battle_type libws2_32.a)
    TARGET_LINK_LIBRARIES(battle_type libshlwapi.a)
    TARGET_LINK_LIBRARIES(battle_type stdc++)

    TARGET_LINK_LIBRARIES(battle_type libenet.a)
    TARGET_LINK_LIBRARIES(battle_type freetype.dll.a)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    # Verifica se dependencias existem
    PKG_SEARCH_MODULE(ALLEGRO REQUIRED allegro-5>=5.0)
    PKG_SEARCH_MODULE(ENET REQUIRED libenet)

    # Define derivativas ao linker
    TARGET_LINK_LIBRARIES(battle_type ${CMAKE_THREAD_LIBS_INIT}) # -pthread ou equivalente
    TARGET_LINK_LIBRARIES(battle_type m) # -lm , para usar math.h
    TARGET_LINK_LIBRARIES(battle_type enet) # -lenet , camada de comunicação em rede via UDP, static link
    TARGET_LINK_LIBRARIES(battle_type allegro) # -lallegro
    TARGET_LINK_LIBRARIES(battle_type allegro_image) # -lallegro_image
    TARGET_LINK_LIBRARIES(battle_type allegro_primitives) # -lallegro_primitives
    TARGET_LINK_LIBRARIES(battle_type allegro_font) # -lallegro_font
    TARGET_LINK_LIBRARIES(battle_type allegro_ttf) # -lallegro_ttf
    TARGET_LINK_LIBRARIES(battle_type allegro_memfile) # -lallegro_memfile
    TARGET_LINK_LIBRARIES(battle_type allegro_dialog) # -lallegro_dialog
endif()