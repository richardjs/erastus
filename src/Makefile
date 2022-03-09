CLAGS=-std=c99 -Wall -O3


objects=state.o stateio.o layout.o


all: erastus


erastus: $(objects) main.o


test: $(objects)


api-dev:
	FLASK_APP=api FLASK_ENV=development flask run


clean:
	rm -f *.o
	rm -f test


# vim: set noexpandtab:
