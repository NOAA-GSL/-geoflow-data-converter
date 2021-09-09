# List the source and build output directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# List the final target
EXE := $(BIN_DIR)/main

# List the source files
SRC := $(wildcard $(SRC_DIR)/*.cpp)

# From the source files, list the object files
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# List flags
# -I is a preprocessor flag
# -MMD & -MP used to generate header dependencies automatically
# -Wno-comment supresses backslash-newline warning after a // comment
# -Llib_name, -L is a linker flag
CPPFLAGS := -Iinclude -MMD -MP
CFLAGS := -O3 -Wall -Wno-comment
LDFLAGS := -L/usr/lib/x86_64-linux-gnu
LDLIBS := -lnetcdf_c++4
CC := g++

# Run these built-in targets regardless if there is a file with this name
.PHONY: all clean

# The target to build when typing make on command line
all: $(EXE)

# Recipe for building executable
# $^ = names of all prerequisites with spaces bw them and omitting duplicates
# $@ = name of target
$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Recipe for building object files
# $< = name of first prerequisite
# $@ = name of the target
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Make dirs if they do not exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Remove various files by running: make clean
clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

# GCC & Clang create .d files corresponding to .o files
# Trigger a compilation only when a header changes
# - is to silence errors if the files don't exist yet
-include $(OBJ:.o=.d)
