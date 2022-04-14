from sys import stderr
from tourney import *

ENGINE = './erastus -i 10000 -w 4'
SCORE_THRESHOLD = .2

START = '0000000000000000000000000xxxxxxxx1'


def puzzle_search():
    engine = Engine(ENGINE)

    state = START
    while 1:
        action, log = engine.run(state)
        stderr.write('.')
        stderr.flush()

        top_score = None
        for line in log.split('\n'):
            if not line.startswith('alt'):
                continue
            
            _, score, _ = line.split()
            score = float(score)

            if top_score == None:
                top_score = score
                if top_score != 1:
                    break
            else:
                diff = top_score - score
                if diff >= SCORE_THRESHOLD:
                    print(f'https://erastus.schneiderbox.net/#{state}\t({diff})')
                break

        state = move_state(state, action)
        if action_count(state) == 0:
            stderr.write('\n')
            state = START


if __name__ == '__main__':
    puzzle_search()
