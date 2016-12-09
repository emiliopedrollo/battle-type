CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
FIND_PACKAGE(PkgConfig REQUIRED)
PROJECT(battle_type)

# Define -std=C11 para o compilador
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")

# Define todos os fontes e cabeçalhos a serem compilados
SET(SOURCE_FILES
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
        src/rank.h
        src/sha256.c
        src/sha256.h)

# Cria arquivo cabeçalho para cada recurso utilizado
SET(RESOURCE_COMPILER "xxd")
FILE(GLOB_RECURSE COMPILED_RESOURCES
        RELATIVE ${PROJECT_SOURCE_DIR}/src/resources
        "src/resources/img/*.png"
        "src/resources/img/*.jpg"
        "src/resources/font/*.ttf")
FOREACH (INPUT_FILE ${COMPILED_RESOURCES})
    SET(OUTPUT_FILE ${INPUT_FILE}.h)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${RESOURCE_COMPILER} -i ${INPUT_FILE} ${OUTPUT_FILE}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/resources
            COMMENT "Compiling ${INPUT_FILE} to ${OUTPUT_FILE} binary")
    LIST(APPEND COMPILED_RESOURCES ${OUTPUT_FILE})
ENDFOREACH ()

ADD_DEFINITIONS(-D_GNU_SOURCE)

# Adiciona target
ADD_EXECUTABLE(battle_type ${SOURCE_FILES} ${COMPILED_RESOURCES})

# Define diretório de build
SET(EXECUTABLE_OUTPUT_PATH output)

# Verifica se dependencias existem
PKG_SEARCH_MODULE(ALLEGRO REQUIRED allegro-5>=5.0)
PKG_SEARCH_MODULE(ENET REQUIRED libenet)

# Procura pelo pacote Threads no sistema e define variaval CMAKE_THREAD_LIBS_INIT
FIND_PACKAGE(Threads REQUIRED)

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
