all: erastus


CFLAGS=-O3 -Wall
LDFLAGS=-lm


objects=layout.o mcts.o minimax.o state.o stateio.o


layout.o: layout.h
mcts.o: mcts.h state.h
minimax.o: minimax.h state.h
state.o: bitops.h layout.h state.h
stateio.o: state.h


erastus: $(objects) main.o


test: $(objects)


benchmark: $(objects)


api: erastus
	gunicorn -b 0.0.0.0:5000 -w $(if $(GUNICORN_WORKERS),$(GUNICORN_WORKERS),8) api:app


api-dev: erastus
	FLASK_APP=api FLASK_ENV=development flask run


clean:
	rm -f *.o
	rm -f test
	rm -f benchmark
	rm -f erastus


.phony: all api api-dev clean


# vim: set noexpandtab:
