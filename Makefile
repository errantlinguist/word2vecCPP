# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

## Compiler/linker settings ----------------------------------------------------
CC = clang
CFLAGS = -Wall -pedantic -ansi -march=native
CXX = clang++
CXXFLAGS = -Wall -pedantic -ansi -Weffc++ -std=c++11 -march=native

OBJ_FILE_SUFFIX = .o
LINK$(OBJ_FILE_SUFFIX) = $(CXX) $(LDFLAGS) $(TARGET_ARCH)	# Use C++ linker by default (instead of C linker)

DEBUG_FLAGS = -g -O0	# Extra flags to add for debugging
RELEASE_FLAGS = -Ofast	# Extra flags to add for release distributions

## System settings -------------------------------------------------------------
SYSTEM_INCLUDE_DIRECTORIES = /usr/include
C_HEADER_FILE_SUFFIX = .h

vpath %$(C_HEADER_FILE_SUFFIX) $(SYSTEM_INCLUDE_DIRECTORIES)

## Project settings ------------------------------------------------------------

SOURCE_FILE_SUFFIX = .cpp
SOURCE_DIR = ./src

# Recursive wildcard function <http://stackoverflow.com/a/4960296>
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
SOURCES := $(call rwildcard,$(SOURCE_DIR)/,*$(SOURCE_FILE_SUFFIX))	# The project source code files to compile

PROJECT_HEADER_FILE_SUFFIXES = $(C_HEADER_FILE_SUFFIX) .hpp
add_directory_files_to_vpath = $(foreach project_header_file_suffix,$(PROJECT_HEADER_FILE_SUFFIXES),$(call vpath,$project_header_file_suffix/,$1))

PROJECT_INCLUDE_DIRECTORIES = include
$(add_directory_files_to_vpath $(PROJECT_INCLUDE_DIRECTORIES))
# Add the system include directory after the project include directory in order to give priority to the project files if there is a name conflict
$(add_directory_files_to_vpath $(SYSTEM_INCLUDE_DIRECTORIES))
CPPFLAGS += $(addprefix -I ,$(PROJECT_INCLUDE_DIRECTORIES) $(SYSTEM_INCLUDE_DIRECTORIES))
LDLIBS =

EXECUTABLE_NAME = distance
DIST_ROOT_DIR = ./dist
OBJ_DIRNAME = obj

# Distributions ---------------------------------------------------------------
# TODO: Generalize these constants/rules by using eval function (https://www.gnu.org/software/make/manual/html_node/Eval-Function.html) and/or target-specific variable values (https://www.gnu.org/software/make/manual/make.html#Target%5F002dspecific)
DEBUG_DIST_NAME = Debug
DEBUG_CLEAN_TARGET_NAME = clean-$(DEBUG_DIST_NAME)
DEBUG_DIR = $(DIST_ROOT_DIR)/$(DEBUG_DIST_NAME)
DEBUG_EXECUTABLE = $(DEBUG_DIR)/$(EXECUTABLE_NAME)
DEBUG_OBJ_DIR = $(DEBUG_DIR)/$(OBJ_DIRNAME)
DEBUG_OBJS = $(patsubst $(SOURCE_DIR)/%$(SOURCE_FILE_SUFFIX), $(DEBUG_OBJ_DIR)/%$(OBJ_FILE_SUFFIX), $(SOURCES))	# Set of object files to be compiled

RELEASE_DIST_NAME = Release
RELEASE_CLEAN_TARGET_NAME = clean-$(RELEASE_DIST_NAME)
RELEASE_DIR = $(DIST_ROOT_DIR)/$(RELEASE_DIST_NAME)
RELEASE_EXECUTABLE = $(RELEASE_DIR)/$(EXECUTABLE_NAME)
RELEASE_OBJ_DIR = $(RELEASE_DIR)/$(OBJ_DIRNAME)
RELEASE_OBJS = $(patsubst $(SOURCE_DIR)/%$(SOURCE_FILE_SUFFIX), $(RELEASE_OBJ_DIR)/%$(OBJ_FILE_SUFFIX), $(SOURCES))	# Set of object files to be compiled

## Rules -----------------------------------------------------------------------
# Default rule to execute
all: $(RELEASE_DIST_NAME)

$(DEBUG_DIST_NAME): $(DEBUG_EXECUTABLE)
$(RELEASE_DIST_NAME): $(RELEASE_EXECUTABLE)

compile-$(DEBUG_DIST_NAME): | $(DEBUG_OBJS)
compile-$(RELEASE_DIST_NAME): | $(RELEASE_OBJS)

# Rule to create distribution directory before linking executable
$(DEBUG_EXECUTABLE): | $(DEBUG_DIR)
$(RELEASE_EXECUTABLE): | $(RELEASE_DIR)

# Rule to create distribution directory
$(DEBUG_DIR) $(RELEASE_DIR):
	@mkdir -p $@

# Rule to link object files into executable file
$(DEBUG_DIR)/$(EXECUTABLE_NAME): $(DEBUG_OBJS)
	$(LINK$(OBJ_FILE_SUFFIX)) $^ $(LDLIBS) -o $@
$(RELEASE_DIR)/$(EXECUTABLE_NAME): $(RELEASE_OBJS)
	$(LINK$(OBJ_FILE_SUFFIX)) $^ $(LDLIBS) -o $@

# Rule to compile object file from C++ source
$(DEBUG_OBJ_DIR)/%$(OBJ_FILE_SUFFIX) : CFLAGS += $(DEBUG_FLAGS)
$(DEBUG_OBJ_DIR)/%$(OBJ_FILE_SUFFIX) : CXXFLAGS += $(DEBUG_FLAGS)
$(DEBUG_OBJ_DIR)/%$(OBJ_FILE_SUFFIX) : $(SOURCE_DIR)/%$(SOURCE_FILE_SUFFIX)
	mkdir -p $(dir $@)
	$(COMPILE$(SOURCE_FILE_SUFFIX)) $(OUTPUT_OPTION) $<
$(RELEASE_OBJ_DIR)/%$(OBJ_FILE_SUFFIX) : CFLAGS += $(RELEASE_FLAGS)
$(RELEASE_OBJ_DIR)/%$(OBJ_FILE_SUFFIX) : CXXFLAGS += $(RELEASE_FLAGS)
$(RELEASE_OBJ_DIR)/%$(OBJ_FILE_SUFFIX) : $(SOURCE_DIR)/%$(SOURCE_FILE_SUFFIX)
	mkdir -p $(dir $@)
	$(COMPILE$(SOURCE_FILE_SUFFIX)) $(OUTPUT_OPTION) $<

# Create object directory before compiling objects
$(DEBUG_OBJS): | $(DEBUG_OBJ_DIR)
$(RELEASE_OBJS): | $(RELEASE_OBJ_DIR)

# Rule to create object directory
$(DEBUG_OBJ_DIR) $(RELEASE_OBJ_DIR):
	@mkdir -p $@

# Rule to declare "phony" rules, which don't create any actual files
.PHONY: clean-all $(DEBUG_CLEAN_TARGET_NAME) $(RELEASE_CLEAN_TARGET_NAME)

# Rule to remove already-built project files
clean: clean-all
clean-all:
	rm -rf $(DIST_ROOT_DIR)
$(DEBUG_CLEAN_TARGET_NAME):
	rm -rf $(DEBUG_DIR)
# Extra target for compatibility with Code::Blocks
clean$(DEBUG_DIST_NAME): $(DEBUG_CLEAN_TARGET_NAME)
$(RELEASE_CLEAN_TARGET_NAME):
	rm -rf $(RELEASE_DIR)
# Extra target for compatibility with Code::Blocks
clean$(RELEASE_DIST_NAME): $(RELEASE_CLEAN_TARGET_NAME)
