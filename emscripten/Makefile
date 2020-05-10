CC = em++

COMPILER_FLAGS = -O3 -fpermissive -std=c++17 -D LLVMES_PLATFORM_WEB
ROOT_DIR = ..

OUT_DIR = .
DEBUGGER_OUT_DIR = $(OUT_DIR)/debugger
BUBBLESORT_OUT_DIR = $(OUT_DIR)/bubblesort

SRC_DIR = $(ROOT_DIR)/src/llvmes/interpreter
GUI_SRC_DIR = $(ROOT_DIR)/llvmes-gui/src/llvmes-gui
DEBUGGER_SRC_DIR = $(ROOT_DIR)/debugger/src
BUBBLESORT_SRC_DIR = $(ROOT_DIR)/playground

OBJ_DIR = obj-llvmes
GUI_OBJ_DIR = obj-llvmes-gui
DEBUGGER_OBJ_DIR = obj-debugger
BUBBLESORT_OBJ_DIR = obj-bubblesort

EM_FLAGS = -s DEMANGLE_SUPPORT=1 \
-s WASM=1 -s USE_WEBGL2=1 \
-s DEMANGLE_SUPPORT=1 -s ASSERTIONS=1 \
-s ALLOW_MEMORY_GROWTH=1 -s --memory-init-file 0 \
-s USE_GLFW=3 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s USE_PTHREADS=1 -s FULL_ES3=1

INC_FLAGS = -I $(ROOT_DIR)/src -I $(ROOT_DIR)/llmes-gui/src
GUI_INC_FLAGS = -I $(ROOT_DIR)/llvmes-gui/spdlog/include -I $(ROOT_DIR)/llvmes-gui/imgui \
-I $(ROOT_DIR)/llvmes-gui/glm -I $(ROOT_DIR)/llvmes-gui/src
DEBUGGER_INC_FLAGS = -I $(ROOT_DIR)/debugger -I $(ROOT_DIR)/debugger/src

_OBJ = cpu.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

_GUI_OBJ = application.o buffers.o draw.o log.o shader.o texture.o \
window.o imgui/imgui_layer.o imgui/imgui_renderer.o
GUI_OBJ = $(patsubst %,$(GUI_OBJ_DIR)/%,$(_GUI_OBJ))

_DEBUGGER_OBJ = debugger.o main.o
DEBUGGER_OBJ = $(patsubst %,$(DEBUGGER_OBJ_DIR)/%,$(_DEBUGGER_OBJ))

_BUBBLESORT_OBJ = bubblesort_web.o
BUBBLESORT_OBJ = $(patsubst %,$(BUBBLESORT_OBJ_DIR)/%,$(_BUBBLESORT_OBJ))

all: debugger bubblesort

$(OBJ_DIR):
	mkdir $@
$(GUI_OBJ_DIR):
	mkdir $@
	mkdir $@/imgui
$(DEBUGGER_OBJ_DIR):
	mkdir $@

$(BUBBLESORT_OBJ_DIR):
	mkdir $@

$(DEBUGGER_OUT_DIR):
	mkdir $@

$(BUBBLESORT_OUT_DIR):
	mkdir $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)
	$(CC) -c -o $@ $< $(COMPILER_FLAGS) $(EM_FLAGS) $(INC_FLAGS)

$(GUI_OBJ_DIR)/%.o: $(GUI_SRC_DIR)/%.cpp $(GUI_OBJ_DIR)
	$(CC) -c -o $@ $< $(COMPILER_FLAGS) $(EM_FLAGS) $(GUI_INC_FLAGS)

$(DEBUGGER_OBJ_DIR)/%.o: $(DEBUGGER_SRC_DIR)/%.cpp $(DEBUGGER_OBJ_DIR)
	$(CC) -c -o $@ $< $(COMPILER_FLAGS) $(EM_FLAGS) $(GUI_INC_FLAGS) $(DEBUGGER_INC_FLAGS) $(INC_FLAGS)

$(BUBBLESORT_OBJ_DIR)/%.o: $(BUBBLESORT_SRC_DIR)/%.cpp $(BUBBLESORT_OBJ_DIR)
	$(CC) -c -o $@ $< $(COMPILER_FLAGS) $(EM_FLAGS) $(GUI_INC_FLAGS) $(INC_FLAGS)

libllvmes-gui.a: $(GUI_OBJ)
	emar rcs $@ $^

libllvmes.a: $(OBJ)
	emar rcs $@ $^

debugger: libllvmes-gui.a libllvmes.a $(DEBUGGER_OBJ) $(DEBUGGER_OUT_DIR)
	$(CC) $(COMPILER_FLAGS) $(EM_FLAGS) $(DEBUGGER_INC_FLAGS) $(GUI_INC_FLAGS) $(INC_FLAGS) \
	-L . -l llvmes-gui -l llvmes -l imgui --embed-file fonts --shell-file layout.html \
	$(DEBUGGER_OBJ) -o $(DEBUGGER_OUT_DIR)/index.html

bubblesort: libllvmes-gui.a libllvmes.a $(BUBBLESORT_OBJ) $(BUBBLESORT_OUT_DIR)
	$(CC) $(COMPILER_FLAGS) $(EM_FLAGS) $(GUI_INC_FLAGS) $(INC_FLAGS) \
	-L . -l llvmes-gui -l llvmes -l imgui --embed-file fonts --embed-file bubblesort.bin --shell-file layout.html \
	$(BUBBLESORT_OBJ) -o $(BUBBLESORT_OUT_DIR)/index.html

.PHONY: all clean debugger bubblesort

clean:
	rm -rf $(DEBUGGER_OBJ_DIR) $(OBJ_DIR) $(GUI_OBJ_DIR) $(BUBBLESORT_OBJ_DIR) *.a