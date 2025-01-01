PKGS=sdl2
CFLAGS=-Wall -Wold-style-definition -pedantic `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)`

tetrissy: main.c
	$(CC) $(CFLAGS) -o tetrissy main.c $(LIBS)
