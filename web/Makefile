EMCC = emcc


SRC_DIR = ../core
OUT = kernel.js


SRC = \
	$(SRC_DIR)/main.cpp \
	$(wildcard $(SRC_DIR)/machine/*.cpp) \
	$(wildcard $(SRC_DIR)/kernel/*.cpp)

INCLUDE = \
	-I$(SRC_DIR)/machine \
	-I$(SRC_DIR)/common \
	-I$(SRC_DIR)/kernel

CXXFLAGS = -std=c++17 -Wall -Wextra -O3

EMFLAGS = \
	--bind \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s EXPORTED_RUNTIME_METHODS=['FS'] \
	-s DISABLE_EXCEPTION_CATCHING=0 \
	-s ASYNCIFY \
	-s ASYNCIFY_IGNORE_INDIRECT=0 \
	-s ASYNCIFY_ADD=['_ZN6Kernel4initEv','_ZN6Kernel4stepEv','main']

all: $(OUT)

$(OUT): $(SRC)
	@echo "Compiling (web build)..."
	$(EMCC) $(SRC) $(INCLUDE) $(CXXFLAGS) $(EMFLAGS) -o $(OUT)
	@echo "Done! Output: $(OUT)"

clean:
	rm -f $(OUT)

.PHONY: all clean
