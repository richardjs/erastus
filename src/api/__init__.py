from os import environ
from subprocess import Popen, PIPE

from flask import abort, Flask, jsonify, request, Response


ERASTUS = './erastus'

MAX_ITERATIONS = int(environ.get('MAX_ITERATIONS', 200000))
MIN_ITERATIONS = int(environ.get('MIN_ITERATIONS', 1000))
MAX_WORKERS = int(environ.get('MAX_WORKERS', 8))
MIN_WORKERS = int(environ.get('MIN_WORKERS', 1))


app = Flask(__name__)


def check_state(state):
    if (len(state) != 34
            or (set(state[:25]) - set('01234'))
            or (set(state[25:33]) - set('xabcde12345'))
            or (state[33] not in '12')):
        abort(400)


def check_action(action):
    if (len(action) not in [5, 6, 8]
            or (set(action) - set('abcde12345&-+#'))):
        abort(400)


@app.route('/actions/<state>', methods=['OPTIONS'])
def actions_options(state):
    response = Response()
    response.headers['Access-Control-Allow-Origin'] = '*'

    return response


@app.route('/actions/<state>')
def actions(state):
    state = state.lower()
    check_state(state)

    p = Popen([ERASTUS] + ['-l'] + [state], stdout=PIPE, stderr=PIPE)

    actions = []
    for action in p.stdout.readlines():
        actions.append(action.strip().decode('utf-8'))

    log = p.stderr.read().decode('utf-8')

    response = jsonify({
        'actions': actions,
        'log': log,
    })

    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Headers'] = 'Iterations, Move'

    return response


@app.route('/act/<state>/<action>', methods=['OPTIONS'])
def act_options(state, action):
    response = Response()
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Headers'] = 'Iterations, Mode'

    return response


@app.route('/act/<state>/<action>')
def act(state, action):
    state = state.lower()
    check_state(state)
    action = action.lower()

    # A win has a #, which gets confused for a URL hash
    # Wins are disambigous in other ways, so we'll take care of it here
    # so the API user doesn't have to worry about it
    if '-' in action and len(action) == 5:
        action += '#'

    check_action(action)

    p = Popen([ERASTUS] + ['-m'] + [action, state], stdout=PIPE, stderr=PIPE)

    state = p.stdout.read().strip().decode('utf-8')
    log = p.stderr.read().decode('utf-8')

    response = jsonify({
        'state': state,
        'log': log,
    })

    response.headers['Access-Control-Allow-Origin'] = '*'

    return response


@app.route('/think/<state>', methods=['OPTIONS'])
def think_options(state):
    response = Response()
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Headers'] = 'iterations, workers'

    return response


@app.route('/think/<state>')
def think(state):
    state = state.lower()
    check_state(state)

    iterations = int((MIN_ITERATIONS + MAX_ITERATIONS) / 2)
    workers = 1

    if 'Iterations' in request.headers:
        iterations = min(int(request.headers['Iterations']), MAX_ITERATIONS)
        iterations = max(iterations, MIN_ITERATIONS)
    if 'Workers' in request.headers:
        workers = min(int(request.headers['Workers']), MAX_WORKERS)
        workers = max(workers, MIN_WORKERS)

    p = Popen(
        [ERASTUS]
        + ['-i', str(iterations)]
        + ['-w', str(workers)]
        + [state],
    stdout=PIPE, stderr=PIPE)

    action = p.stdout.read().strip().decode('utf-8')
    log = p.stderr.read().decode('utf-8')

    check_action(action)
    p = Popen([ERASTUS] + ['-m'] + [action, state], stdout=PIPE, stderr=PIPE)
    state = p.stdout.read().strip().decode('utf-8')

    response = jsonify({
        'action': action,
        'state': state,
        'log': log,
    })

    response.headers['Access-Control-Allow-Origin']='*'
    response.headers['Access-Control-Allow-Headers']='iterations, workers'

    return response


@app.route('/puzzles', methods=['OPTIONS'])
def puzzles_options():
    response = Response()
    response.headers['Access-Control-Allow-Origin'] = '*'

    return response


@app.route('/puzzles')
def puzzles():
    try:
        response = jsonify([
            line.strip() for line in open('puzzles.txt').readlines()
        ])
    except:
        return "can't read puzzles.txt"

    response.headers['Access-Control-Allow-Origin'] = '*'
    return response


@app.route('/submit/<state>', methods=['OPTIONS'])
def submit_options():
    response = Response()
    response.headers['Access-Control-Allow-Origin'] = '*'

    return response


@app.route('/submit/<state>')
def submit(state):
    check_state(state)

    f = open('puzzles.txt', 'a')
    f.write(state + '\n')
    f.close()

    response = Response('ok')
    response.headers['Access-Control-Allow-Origin'] = '*'

    return response
