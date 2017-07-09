# trunk_path = $(shell pwd | sed "s/\/LibraCpp.*//")
#include $(trunk_path)/make.rules
CC   := gcc
CXX  := g++
AR   := ar
#Debug Version
CFLAGS  := -g -std=c++11 -Wall -pipe -fno-ident -MMD -D_GNU_SOURCE -D_REENTRNT -D_DEBUG_BUILD -fPIC -fpermissive

#Release Version
#CFLAGS  := -O3 -g -std=c++11 -Wall -pipe -fno-ident -MMD -D_GNU_SOURCE -D_REENTRNT -D_RELEASE_BUILD -fPIC

ROOT_DIR    := $(shell pwd)
SRC_DIR 	:= $(ROOT_DIR)/base \
				$(ROOT_DIR)/extra/http-parser \
				$(ROOT_DIR)/lib \
				$(ROOT_DIR)/net \
				$(ROOT_DIR)/uv

INC_DIR 	:= $(ROOT_DIR)/base \
				$(ROOT_DIR)/extra/http-parser \
				$(ROOT_DIR)/lib \
				$(ROOT_DIR)/net \
				$(ROOT_DIR)/uv

LIBUV_INC_DIR := ../libuv/include
LIBUV_LIB_DIR   := ../libuv/.libs

LIBS := -luv

LIB_DIR     := $(ROOT_DIR)/lib
#SRC_FILES := $(wildcard *.c, *.cpp, /***/***/*.cpp)
SRC_FILES  := $(wildcard $(ROOT_DIR)/base/*.cpp)
SRC_FILES  += $(wildcard $(ROOT_DIR)/uv/*.cpp)
SRC_FILES  += $(wildcard $(ROOT_DIR)/extra/*/*.cpp)
SRC_FILES  += $(wildcard $(ROOT_DIR)/net/*.cpp)
OBJ_FILES  += $(SRC_FILES:.cpp=.o)

CFLAGS += -I$(LIBUV_INC_DIR) $(addprefix -I,$(SRC_DIR)) $(addprefix -I,$(INC_DIR)) -L$(LIBUV_LIB_DIR)
#CFLAGS     := -O3 -fPIC -Werror -DCURL_STATICLIB -I$(SRC_DIR) $(addprefix -I,$(INC_DIR))

LIB_NAME   := libra
LIB_SUFFIX := .so
LIB_TARGET := $(ROOT_DIR)/lib/lib$(LIB_NAME)$(LIB_SUFFIX)


.PHONY: all clean $(dirs)

all: $(LIB_TARGET) ;

$(LIB_TARGET): $(OBJ_FILES)
	$(CXX) -shared -o $(LIB_TARGET:.a=.so) $(OBJ_FILES) 
	@echo *********Build $@ $(LIB_TARGET:.a=.so) Successful*********
	@echo

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo clean...
	@rm -rf $(LIB_TARGET) $(LIB_TARGET:.a=.so) $(OBJ_FILES) $(LIB_TARGET:.a=.bin)

