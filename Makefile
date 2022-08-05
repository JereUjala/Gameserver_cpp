CC=clang++
OPTS=-Wall -Wextra -Wpedantic -v # were omitting -libstdc++

make:
	# $(CC) $(OPTS) ./header/Header.h -o ./so/hdr.o
	# -c flag to only compile not link
	$(CC) $(OPTS) -c ./login/*.cpp -o ./so/loginhandling.o
	$(CC) $(OPTS) -c *.cpp -o ./so/main.o
	$(CC) $(OPTS) -c ./networking/*.cpp -o ./so/clienthandling.o
	$(CC) $(OPTS) -c ./input/*.cpp -o ./so/Playermovement.o
	$(CC) $(OPTS) -c ./server/tui.cpp -o ./so/tui.o
	$(CC) $(OPTS) -c ./server/server.cpp -o ./so/server.o
	echo 'Linking files.'
	# $(CC) $(OPTS) ./header/Header.h ./input/Playermovement.cpp ./main.cpp -o ./bin/server.aa
	# $(CC) $(OPTS) -o ./bin/server.aa ./so/main.o ./so/Playermovement.o ./so/clienthandling.o -I ./header/Header.hpp
	$(CC) $(OPTS) -o ./bin/server.aa -lncurses ./so/*.o -I ./header/Header.hpp

clean:
	rm -rf ./so/*
	rm -rf ./bin/*
backup:
	cp -r ./* ../backup
