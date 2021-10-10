CXX = gcc
CXXFLAGS = -Wall
LDLIBS =
NAME = es-5
LDLIBS += `pkg-config --cflags --libs libpipewire-0.3`

es-5: es-5.c
	$(CXX) $(CXXFLAGS) es-5.c -o $(NAME) $(LDLIBS)

all: es-5

clean:
	rm ./$(NAME)

run: all
	./es-5

debug: all
	PIPEWIRE_DEBUG=5 ./$(NAME) 
