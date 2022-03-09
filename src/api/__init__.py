from subprocess import Popen, PIPE

from flask import abort, Flask, jsonify, request, Response


ERASTUS = './erastus'


app = Flask(__name__)


def check_state(state):
    if (len(state) != 34
            or (set(state[:25]) - set('01234'))
            or (set(state[25:33]) - set('xabcde12345'))
            or (state[33] not in '12')):
        abort(400)


@app.route('/actions/<state>', methods=['OPTIONS'])
def options(state):
    response = Response()
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Headers'] = 'Iterations, Mode'

    return response


@app.route('/actions/<state>')
def think(state):
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
