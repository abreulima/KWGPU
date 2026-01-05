CPP 	:= g++ -g -std=c++20 -DIMGUI_IMPL_WEBGPU_BACKEND_WGPU
NAME 	:= game

SRC		:= src
SRCS	:= $(wildcard $(SRC)/*.c $(SRC)/kwgpu/*.c)
SRCS	+= $(wildcard $(SRC)/*.cpp $(SRC)/kwgpu/*.cpp)

SRCS	+= $(wildcard imgui/*.cpp) 
SRCS	+= imgui/backends/imgui_impl_sdl3.cpp
SRCS	+= imgui/backends/imgui_impl_wgpu.cpp
SRCS	+= imgui/misc/cpp/imgui_stdlib.cpp


all: $(NAME)

$(NAME): $(SRCS)
	$(CPP) -Iinclude -Iimgui $(SRCS) -o $(NAME) -Llib -lwgpu_native -lSDL3

clean: game
	rm -rf ./game
	
export:
	export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH
	
run:
	./game
