
# Implicit Variables
CXX 		:= g++ -g -std=c++20 -DIMGUI_IMPL_WEBGPU_BACKEND_WGPU
CFLAGS	:= -Iinclude -Iimgui
CXXFLAGS := -Iinclude -Iimgui

NAME 	:= game

OBJ_DIR := obj

SRC		:= src

SRCS	:= $(wildcard $(SRC)/*.c $(SRC)/kwgpu/*.c)
SRCS	+= $(wildcard $(SRC)/*.cpp $(SRC)/kwgpu/*.cpp)
SRCS	+= $(wildcard imgui/*.cpp)
SRCS	+= imgui/backends/imgui_impl_sdl3.cpp
SRCS	+= imgui/backends/imgui_impl_wgpu.cpp
SRCS	+= imgui/misc/cpp/imgui_stdlib.cpp

OBJS	:= $(patsubst %.c,%.o, $(SRCS))
OBJS	:= $(patsubst %.cpp,%.o, $(OBJS))

all: $(NAME)

test:
	$(OBJS)

$(NAME): $(OBJS)
	$(CXX) -Iinclude -Iimgui $(OBJS) -o $(NAME) -Llib -lwgpu_native -lSDL3

$(OBJ_DIR)/%.o : %.c
	$(CXX) -o $@ $(CXXFLAGS) $<

$(OBJ_DIR)/%.o : %.c
	$(CC) -o $@ $(CFLAGS) $<

play: all
	./game

clean: game
	rm -rf ./game

fclean: clean
	find . -name "*.o" -type f -delete

export:
	export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH

run:
	./game
