# Make File for 8080 emulation

######### Build Params #################
DEBUG		= 1
MKDIR_P 	= mkdir -p

# Build Dir
BUILD_DIR	= build
OBJ_DIR		= obj

# Source Dirs
INC_DIR		= include
SRC_DIR		= src
DEPS		= $(INC_DIR)/cpu_8080.h $(INC_DIR)/debug.h

###### Build Specs #####################
CFLAGS					= 
COMPILER_ERROR_FLAGS 	= -Wall -Werror -Wshadow -Wextra -Wunused
LIBS 					=
# DEBUGGING is enabled by default, you can reduce binary size by disabling the
# DEBUGGING
ifeq ($(DEBUG), 1)
	DEFINE_MACROS 	= -DDEBUG -g
	OPTIMIZATION  	= -O0
else
	OPTIMIZATION 	= -O3
endif

CFLAGS += $(OPTIMIZATION) $(DEFINE_MACROS)


######### Main Build ##################
.PHONY: build setup compile clean doc extractROM

build: setup compile

compile: $(BUILD_DIR)/$(OBJ_DIR)/space.o $(BUILD_DIR)/$(OBJ_DIR)/cpu_8080.o
	$(CC) -o invaders $^ $(CFLAGS)

$(BUILD_DIR)/$(OBJ_DIR)/space.o: $(SRC_DIR)/space.c $(DEPS)
	$(CC) -c -o $@ -I$(INC_DIR) $< $(CFLAGS) $(COMPILER_ERROR_FLAGS)

$(BUILD_DIR)/$(OBJ_DIR)/cpu_8080.o: $(SRC_DIR)/cpu_8080.c $(DEPS)
	$(CC) -c -o $@ -I$(INC_DIR) $< $(CFLAGS) $(COMPILER_ERROR_FLAGS)

######### RPM Extraction Rules ########
setup:
	$(MKDIR_P) $(BUILD_DIR)
	$(MKDIR_P) $(BUILD_DIR)/$(OBJ_DIR)
	
extractROM: invaders.zip
	unzip invaders.zip -d ./invaders_rom

######### Clean UP Rules ##############
clean:
	-rm invaders
	-rm -rf $(BUILD_DIR) core