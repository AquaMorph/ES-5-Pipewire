CXX = gcc
CXXFLAGS = -Wall
LDLIBS =
NAME = es-5-pipewire
LDLIBS += `pkg-config --cflags --libs libpipewire-0.3`

es-5: es-5.c
	$(CXX) $(CXXFLAGS) es-5.c -o $(NAME) $(LDLIBS) -lm

all: es-5

clean:
	rm ./$(NAME)

run: all
	./$(NAME)

debug: all
	PIPEWIRE_DEBUG=5 ./$(NAME) 

install: all
	sudo mkdir -p /opt/$(NAME)
	sudo cp ./$(NAME) /opt/$(NAME)/
