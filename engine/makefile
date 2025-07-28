# -----------------------------------------------------------------------------
# Utility stuff for the makefile
# -----------------------------------------------------------------------------
RESET  := \033[0m
RED    := \033[31m
GREEN  := \033[32m
YELLOW := \033[33m
BLUE   := \033[34m
CYAN   := \033[36m
BOLD   := \033[1m
define describe
	@printf "$(GREEN)$(BOLD)%s$(RESET) %s\n" "$(1)" "$(2)"
endef
define step
	@printf "$(CYAN)$(BOLD)%s$(RESET) %s\n" "$(1)" "$(2)"
endef
# -----------------------------------------------------------------------------
# Binary Name
# -----------------------------------------------------------------------------
EXE := renweb_app
# -----------------------------------------------------------------------------
# OS info
# -----------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    OS_NAME := Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OS_NAME := Linux
    else ifeq ($(UNAME_S),Darwin)
        OS_NAME := Apple
    else
        $(error Unknown operating system detected. Don't know how to proceed :/)
    endif
endif
# -----------------------------------------------------------------------------
# Toolchain prefix
# -		ARM 32-bit (Linux): 		   CROSS_COMPILE := arm-linux-gnueabihf-
# -		ARM 64-bit (Linux): 		   CROSS_COMPILE := aarch64-linux-gnu-
# -		x86_64 (Linux, native 64-bit): CROSS_COMPILE := x86_64-linux-gnu-
# -		x86 32-bit (Linux): 		   CROSS_COMPILE := i386-linux-gnu-
# -		Windows 32-bit (MinGW): 	   CROSS_COMPILE := i686-ws64-mingw32-
# -		Windows 64-bit (MinGW):		   CROSS_COMPILE := x86_64-w64-mingw32-
# -		macOS (native, clang++): 	   CROSS_COMPILE := 
# -		MIPS 32-bit (Linux): 		   CROSS_COMPILE := mipsel-linux-gnu-
# -		MIPS 64-bit (Linux):		   CROSS_COMPILE := mips64el-linux-gnu-
# -		RISC-V 64-bit (Linux): 		   CROSS_COMPILE := riscv64-linux-gnu-
# -		PowerPC (Linux): 			   CROSS_COMPILE := powerpc-linux-gnu-
# -		ARM Android (32-bit): 		   CROSS_COMPILE := arm-linux-androideabi-
# -		Android x86 (32-bit): 		   CROSS_COMPILE := i686-linux-android-
# -		32-bit x86 macOS (if needed):  CROSS_COMPILE := i386-apple-darwin-
# -		64-bit x86 macOS (native): 	   CROSS_COMPILE := x86_64-apple-darwin-
# -----------------------------------------------------------------------------
CROSS_COMPILE :=
# -----------------------------------------------------------------------------
# Compiler
# -----------------------------------------------------------------------------
CXX := $(CROSS_COMPILE)g++
CXXFLAGS += -MMD -MP
# -----------------------------------------------------------------------------
# Build mode
# -----------------------------------------------------------------------------
ifndef TARGET
	TARGET := debug
endif
ifeq ($(TARGET), debug)
	CXXFLAGS += -g -O0 -Wall -Wextra -Werror -Wno-missing-braces -Wcast-qual -Wpointer-arith -Wunused 
else
	CXXFLAGS += -O3 -flto -s
endif
# -----------------------------------------------------------------------------
# Paths 
# -----------------------------------------------------------------------------
BUILD_PATH :=  ../build
SRC_PATH :=    ./src
OBJ_PATH :=    $(SRC_PATH)/.build
INC_PATH :=    ./include
EXTERN_INC_PATHS := \
	$(addprefix -I, $(wildcard external/*/include)) \
	$(addprefix -I, external/webview/core/include) \
# -----------------------------------------------------------------------------
# Static Linked Libraries
# -----------------------------------------------------------------------------
LIBS := -lboost_system -lboost_url -lboost_thread -lboost_filesystem -lboost_program_options 
ifeq ($(OS_NAME), Windows)
	LIBS += -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32
endif
ifeq ($(OS_NAME), Apple)
	LIBS += -ldl 
endif
ifeq ($(OS_NAME), Linux)
	LIBS += -ldl 
endif
# -----------------------------------------------------------------------------
# Dynamic Linked Libraries
# -----------------------------------------------------------------------------
PKG_CONFIG := pkg-config
PKG_CFLAGS := $(shell $(PKG_CONFIG) --cflags gtk+-3.0 webkit2gtk-4.1)
PKG_LIBS   := $(shell $(PKG_CONFIG) --libs gtk+-3.0   webkit2gtk-4.1)
# -----------------------------------------------------------------------------
# Source and Object files
# -----------------------------------------------------------------------------
SRCS := $(wildcard $(SRC_PATH)/*.cpp)
OBJS := $(patsubst $(SRC_PATH)/%.cpp, $(OBJ_PATH)/%.o, $(SRCS))
# -----------------------------------------------------------------------------
# Build darget
# -----------------------------------------------------------------------------
all: $(BUILD_PATH)/$(EXE)
# -----------------------------------------------------------------------------
# RULE: Link all object files into executable
# -----------------------------------------------------------------------------
$(BUILD_PATH)/$(EXE): $(OBJS) | $(BUILD_PATH)
	$(call step,Linking Executable,$@)
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -I$(INC_PATH) $(EXTERN_INC_PATHS) $^ $(LIBS) $(PKG_LIBS ) $(LIBS) $(PKG_LIBS) -o $@
	$(call step,Linking Executable [DONE],$@)
# -----------------------------------------------------------------------------
# RULE: Compile source files to object files
# -----------------------------------------------------------------------------
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp | $(OBJ_PATH)
	$(call step,Compiling,$<)
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -I$(INC_PATH) $(EXTERN_INC_PATHS) -c $< -o $@
	$(call step,Compiling [DONE],$<)
# -----------------------------------------------------------------------------
# RULE: Make sure build directory exists
# -----------------------------------------------------------------------------
$(BUILD_PATH):
	$(call step,Build Path,Making path at $@)
	mkdir -p $@
	$(call step,Build Path [DONE],Making path at $@)
# -----------------------------------------------------------------------------
# RULE: Make sure object directory exists
# -----------------------------------------------------------------------------
$(OBJ_PATH):
	$(call step,Object Path,Making path at $@)
	mkdir -p $@
	$(call step,Object Path [DONE],Making path at $@)
# -----------------------------------------------------------------------------
# COMMAND: Remove build files and exe
# -----------------------------------------------------------------------------
clean:
	$(call step,Cleaning)
	rm -rf $(BUILD_PATH)/$(EXE) $(OBJ_PATH)/*.o
	$(call step,Cleaning [DONE])
# -----------------------------------------------------------------------------
# COMMAND: Run the program
# -----------------------------------------------------------------------------
run: $(BUILD_PATH)/$(EXE)
	$(call step,Running)
	./$(BUILD_PATH)/$(EXE) -l1
	$(call step,Running [DONE])
# -----------------------------------------------------------------------------
# COMMAND: Test the program
# -----------------------------------------------------------------------------
test: $(BUILD_PATH)/$(EXE)
	$(call step,Testing)
	ulimit -n 20000 && valgrind ./$(BUILD_PATH)/$(EXE) -l0
	$(call step,Testing [DONE])
# -----------------------------------------------------------------------------
# COMMAND: Info about the makefile
# -----------------------------------------------------------------------------
info:
	$(call step,Displaying info)
	$(call describe,OS,$(OS_NAME))
	$(call describe,App Name,$(EXE))
	$(call describe,Compiler,$(CXX))
	$(call describe,Target,$(TARGET))
	$(call describe,Build Path,$(BUILD_PATH))
	$(call describe,Source Path,$(SRC_PATH))
	$(call describe,Object Path,$(OBJ_PATH))
	$(call describe,Include Path,$(INC_PATH))
	$(call step,Displaying info [DONE])

# -----------------------------------------------------------------------------
# COMMAND: Display help info
# -----------------------------------------------------------------------------
help:
	@echo "Usage:"
	@echo "  make TARGET=debug      Build the application in debug mode"
	@echo "  make TARGET=release    Build the application in release mode"
	@echo "  make sub_modules       Builds the submodules"
	@echo "  make clean             Clean up the build directory"
	@echo "  make run               Build and run the application"
	@echo "  make test              Build and test the application"
	@echo "  make info              Displays info set in the makefile"
	@echo "  make help              Display this help message"
# -----------------------------------------------------------------------------
# Phony targets
# -----------------------------------------------------------------------------
.PHONY: all clean run help
# -----------------------------------------------------------------------------
# Includes
# -----------------------------------------------------------------------------
-include $(OBJS:.o=.d)
