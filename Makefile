SRC_DIR = .
OBJ_DIR = obj
DEP_DIR = dep

SRC_FILES = $(shell find . -name '*.cpp' | sort -k 1nr | cut -f2-)
OBJ_FILES = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SRC_FILES:.cpp=.o))
	DEP_FILES = $(patsubst $(SRC_DIR)/%,$(DEP_DIR)/%,$(SRC_FILES:.cpp=.d))

OBJ_DIRS = $(sort $(dir $(OBJ_FILES)))
DEP_DIRS = $(sort $(dir $(DEP_FILES)))

CXX = g++-8
DEP_LOC = $(patsubst $(OBJ_DIR)/%,$(DEP_DIR)/%,$*.d)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_LOC)
WFLAGS = -Wall -Wextra -Werror -g
CPPFLAGS = -std=c++17 $(WFLAGS) $(DEPFLAGS) -Og -I $(SRC_DIR)
CXXFLAGS = -std=c++17 $(WFLAGS)
EXEC = main



all: objects
	${CXX} ${CXXFLAGS} -o ${EXEC} ${OBJ_FILES}

objects: configure ${OBJ_FILES}

$(OBJ_FILES):
	${CXX} $(CPPFLAGS) -c -o $@ $(patsubst $(OBJ_DIR)/%,$(SRC_DIR)/%,$*.cpp)

-include ${DEP_FILES}

.PHONY: configure

configure: ${DEP_DIRS} ${OBJ_DIRS}

$(DEP_DIRS):
	mkdir -p $@

$(OBJ_DIRS):
	mkdir -p $@

.PHONY: clean

clean:
	rm -rf ${OBJ_FILES} ${DEP_FILES} ${EXEC}
