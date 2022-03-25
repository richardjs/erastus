CLAGS=-std=c99 -Wall -O3
LDFLAGS=-lm


objects=state.o stateio.o layout.o mcts.o minimax.o


all: erastus


erastus: $(objects) main.o


test: $(objects)


api: erastus
	gunicorn -b 0.0.0.0:5000 api:app


api-dev: erastus
	FLASK_APP=api FLASK_ENV=development flask run


clean:
	rm -f *.o
	rm -f test


.phony: all api api-dev clean


# vim: set noexpandtab:
