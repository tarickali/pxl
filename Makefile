build:
	g++-14 -Wall -std=c++17 -I "./libs/" src/*.cpp -l SDL2 -l SDL2_image -l SDL2_ttf -l SDL2_mixer -l lua -o engine;

run:
	./engine

clean:
	rm engine