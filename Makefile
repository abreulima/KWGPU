CPP 	:= g++ -g -std=c++23
NAME 	:= game

SRC		:= src
SRCS	:= $(wildcard $(SRC)/*.c $(SRC)/kwgpu/*.c)
SRCS	+= $(wildcard $(SRC)/*.cpp $(SRC)/kwgpu/*.cpp)

all: $(NAME)

$(NAME): $(SRCS)
	$(CPP) -Iinclude $(SRCS) -o $(NAME) -lwgpu_native -lSDL3

clean: game
	rm -rf ./game
