################################################################################
# Declare some Makefile variables
################################################################################
CC = g++-14
STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I "./libs"
SRC_FILES = ./src/*.cpp \
			./src/Game/*.cpp \
			./src/Logger/*.cpp
LINKER_FLAGS = -l SDL2 -l SDL2_image -l SDL2_ttf -l SDL2_mixer -l lua
OBJ_NAME = engine

################################################################################
# Declare some Makefile rules
################################################################################
build:
	${CC} ${COMPILER_FLAGS} ${STD} ${INCLUDE_PATH} ${SRC_FILES} ${LINKER_FLAGS} -o ${OBJ_NAME};

run:
	./${OBJ_NAME}

clean:
	rm ${OBJ_NAME}