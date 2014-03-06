# ============================================================
#
#   The MIT License (MIT)
#
#   Copyright (c) 2013-2014 Michał Blinkiewicz <michal.blinkiewicz@gmail.com>
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy of
#   this software and associated documentation files (the "Software"), to deal in
#   the Software without restriction, including without limitation the rights to
#   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
#   the Software, and to permit persons to whom the Software is furnished to do so,
#   subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in all
#   copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
#   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
#   COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# ============================================================

# === settings
# ============================================================

EXE_NAME := sphinx

COMPILER := g++
COMPILER_FLAGS := -std=gnu++11
INCLUDE_DIRS :=

LINKER_FLAGS :=
LIBRARY_DIRS :=
LIBRARIES :=

SRC_DIR := src

BUILD_DIR := build
OBJ_DIR := obj
DEP_DIR := dep

# === magic :) (do not change below!)
# ============================================================

OS := Other
ifeq ($(OS),Windows_NT)
	OS := Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		OS := Linux
    endif
    ifeq ($(UNAME_S),Darwin)
		OS := MacOSX
    endif
endif

SRC_DIRS := $(sort $(SRC_DIR)/ $(dir $(wildcard $(SRC_DIR)/*/))) # only one level of subdirs

SRCS := $(foreach sdir,$(SRC_DIRS),$(wildcard $(sdir)*.cpp))
DEPS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/$(DEP_DIR)/%,$(SRCS:.cpp=.d))
OBJS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/$(OBJ_DIR)/%,$(SRCS:.cpp=.o))

$(info --- --------------------------------------------------)
$(info ---       making | $(EXE_NAME) @ $(OS))
$(info --- --------------------------------------------------)
$(info ---     src dirs :)
$(foreach sdir,$(SRC_DIRS),$(info ---              - $(sdir)))
$(info ---      sources :)
$(foreach src,$(SRCS),$(info ---              - $(src)))
$(info --- dependencies :)
$(foreach dep,$(DEPS),$(info ---              - $(dep)))
$(info ---      objects :)
$(foreach obj,$(OBJS),$(info ---              - $(obj)))
$(info --- --------------------------------------------------)
$(info ---    Makefile (c) 2013-2014 Michal Blinkiewicz)
$(info --- --------------------------------------------------)

CXX = $(COMPILER)
CXXFLAGS = $(COMPILER_FLAGS) $(addprefix -I,$(INCLUDE_DIRS))
LDFLAGS = $(LINKER_FLAGS) $(addprefix -L,$(LIBRARY_DIRS)) $(addprefix -l,$(LIBRARIES))

ifeq "$(OS)" "Windows"
MD = if not exist $(subst /,\\,$(1)) mkdir $(subst /,\\,$(1))
RM = del $(1).exe ; rmdir /S /Q $(subst /,\\,$(2))
else
MD = mkdir -p $(1)
RM = rm -rf $(1) $(2)
endif

# === main part
# ============================================================

all: $(EXE_NAME)

$(EXE_NAME): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(call MD,$(@D))
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

$(BUILD_DIR)/$(DEP_DIR)/%.d: $(SRC_DIR)/%.cpp
	@$(call MD,$(@D))
	$(CXX) $(CXXFLAGS) -MF $@ -MG -MM -MP -MT $(BUILD_DIR)/$(OBJ_DIR)/$*.o -MT $@ $<

clean:
	-$(call RM,$(EXE_NAME),$(BUILD_DIR))

.PHONY: all clean

# ============================================================

