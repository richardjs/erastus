let scoreCache = {};

class PuzzleControls extends React.Component {
    render() {
        let unsolvedCount;
        unsolvedCount = this.props.puzzles.length;
        unsolvedCount -= this.props.solvedPuzzles.filter(x => x == true).length;

        if (this.props.puzzleid === null) {
            return e('div', {className: 'row mt-4'},
                e('div', {className: 'col-8 ps-0'},
                    e('button', {
                        className: 'btn btn-secondary',
                        onClick: this.props.onPuzzleRequest,
                        style: {width: '100%'},
                    }, unsolvedCount > 0 ? 'Load puzzle ('+unsolvedCount+' unsolved)' : 'Load puzzle (_ unsolved)'),
                ),
                e('div', {className: 'col-4 pe-0'},
                    e('button', {
                        className: 'btn btn-dark',
                        onClick: this.props.onPuzzleSubmit,
                        style: {width: '100%'},
                    }, 'Submit puzzle'),
                ),
            );
        } else {
            return e('div', {className: 'row mt-4'},
                e('div', {className: 'col ps-0'},
                    e('button', {
                        className: 'btn btn-success',
                        style: {width: '100%'},
                        onClick: this.props.onPuzzleSolve
                    }, "I solved puzzle " + (this.props.puzzleid + 1)),
                ),
                e('div', {className: 'col pe-0'},
                    e('button', {
                        className: 'btn btn-dark',
                        style: {width: '100%'},
                        onClick: this.props.onPuzzleSkip
                    }, "Skip puzzle"),
                ),
            )
        }
    }
}

class UI extends React.Component {
    constructor(props) {
        super(props);

        let solvedPuzzles = JSON.parse(localStorage.getItem('solvedPuzzles'));

        this.state = {
            actions: [],
            inputBuffer: [],
            holdAI: true,
            stateChange: false,
            waitingForAI: false,
            waitingForHint: false,
            score: '',
            hint: null,
            hintSpaces: [],
            iterations: localStorage.getItem('iterations') || 50000,
            workers: localStorage.getItem('workers') || 4,
            player1_ai: localStorage.getItem('player1_ai') ? true : false,
            player2_ai: localStorage.getItem('player2_ai') ? true : false,
            puzzleid: null,
            puzzles: [],
            solvedPuzzles: solvedPuzzles || [],
        };

        this.onAction = this.onAction.bind(this);
        this.onPlayerAIChange = this.onPlayerAIChange.bind(this);
        this.onPuzzleRequest = this.onPuzzleRequest.bind(this);
        this.onPuzzleSolve = this.onPuzzleSolve.bind(this);
        this.onPuzzleSkip = this.onPuzzleSkip.bind(this);
        this.onPuzzleSubmit = this.onPuzzleSubmit.bind(this);
        this.handleHintClick = this.handleHintClick.bind(this);
        this.handleIterationsChange = this.handleIterationsChange.bind(this);
        this.handleWorkersChange = this.handleWorkersChange.bind(this);
        this.handleClick = this.handleClick.bind(this);
        this.handleHashChange = this.handleHashChange.bind(this);
        this.handleSpaceClick = this.handleSpaceClick.bind(this);

        window.addEventListener('click', this.handleClick);
        window.addEventListener('hashchange', this.handleHashChange);
    }

    componentDidMount() {
        this.handleHashChange();

        fetch(API_URL + '/puzzles')
            .then(response => response.json())
            .then(json => {
                this.setState({puzzles: json});
            })
            .catch(console.error);
    }

    render() {
        if (location.hash.length == 0) {
            return e('div', null);
        }

        return e('div', null,
            e('div', {className: 'row pt-4'},
                e(Board, {
                    notation: location.hash.slice(1),
                    actions: this.state.actions,
                    inputBuffer: this.state.inputBuffer,
                    hintSpaces: this.state.hintSpaces,
                    player1_ai: this.state.player1_ai,
                    player2_ai: this.state.player2_ai,
                    holdAI: this.state.holdAI,
                    handleSpaceClick: this.handleSpaceClick,
                }),
            ),
            e('div', {className: 'row mt-4'},
                e(AIOptions, {
                    iterations: this.state.iterations,
                    workers: this.state.workers,
                    player1_ai: this.state.player1_ai,
                    player2_ai: this.state.player2_ai,
                    waitingForAI: this.state.waitingForAI,
                    waitingForHint: this.state.waitingForHint,
                    hint: this.state.hint,
                    score: this.state.score,
                    handleIterationsChange: this.handleIterationsChange,
                    handleWorkersChange: this.handleWorkersChange,
                    handleHintClick: this.handleHintClick,
                    onPlayerAIChange: this.onPlayerAIChange,
                }),
            ),
            e('div', {className: 'row mt-4'},
                e('div', {className: 'col ps-0'},
                    e('button', {
                        className: 'btn btn-danger',
                        style: {width: '100%'},
                        onClick: () => {
                            if (confirm('New game?')) window.location = ''
                        },
                    }, "New game"),
                ),
                e('div', {className: 'col pe-0'},
                    e('button', {
                        className: 'btn btn-danger',
                        style: {width: '100%'},
                        onClick: () => {
                            if (confirm('New game?')) {
                                localStorage.removeItem('player1_ai');
                                localStorage.removeItem('player2_ai');
                                if (this.state.player1_ai) {
                                    localStorage.setItem('player2_ai', this.state.player1_ai);
                                }
                                if (this.state.player2_ai) {
                                    localStorage.setItem('player1_ai', this.state.player2_ai);
                                }
                                window.location = ''
                            }
                        },
                    }, "New game (switch sides)"),
                ),
            ),
            e(PuzzleControls, {
                puzzleid: this.state.puzzleid,
                puzzles: this.state.puzzles,
                solvedPuzzles: this.state.solvedPuzzles,
                onPuzzleRequest: this.onPuzzleRequest,
                onPuzzleSolve: this.onPuzzleSolve,
                onPuzzleSkip: this.onPuzzleSkip,
                onPuzzleSubmit: this.onPuzzleSubmit,
            }),
        );
    }

    // User has entered an action using the Board component
    onAction(action) {
        this.setState({
            actions: [],
            holdAI: false,
        });

        fetch(API_URL + '/act/' + location.hash.slice(1) + '/' + action)
            .then(response => response.json())
            .then(json => {
                this.setState({stateChange: true});
                location.hash = json.state;
            })
            .catch(console.error);
    }

    // User has adjusted the AI configuration
    onPlayerAIChange(player_ai, value) {
        this.setState({[player_ai]: value});
        if (value) {
            localStorage.setItem(player_ai, value);
        } else {
            localStorage.removeItem(player_ai);
        }

        if (value && 'player'+turn()+'_ai' == player_ai) {
            this.aiMove();
        }
    }

    handleHintClick(e) {
        console.log('requesting hint');
        this.setState({waitingForHint: true});

        e.target.blur();

        fetch(API_URL + '/think/' + location.hash.slice(1), {
            headers: [
                ['Iterations', '200000'],
                ['Workers', '8']
            ]
        })
            .then(response => response.json())
            .then(json => {
                // TODO we should probably use a better way of cancelling the request
                // right now, if a new hint is requested, this one will take precedence
                if (!this.state.waitingForHint) {
                    return;
                }
                console.log(json.log);

                let logLines = json.log.split('\n');
                let score = '';
                for (let i = 0; i < logLines.length; i++) {
                    let line = logLines[i];
                    if (!line.startsWith('score')) continue;

                    score = line.split('\t').at(-1);
                    break;
                }

                this.setState({
                    score: score,
                    hint: json.action,
                    hintSpaces: json.action.split(/\&|-|\+/).map(coordsToSpace),
                    waitingForHint: false,
                });
                //scoreCache[location.hash] = score;
            })
            .catch(console.error)
    }

    handleIterationsChange(e) {
        this.setState({iterations: e.target.value});
        localStorage.setItem('iterations', e.target.value);
    }

    handleWorkersChange(e) {
        this.setState({workers: e.target.value});
        localStorage.setItem('workers', e.target.value);
    }

    handleClick() {
        // Set inputBuffer to empty
    }

    // URL hash has changed (either manually or after an action)
    handleHashChange() {
        // TODO better checks here
        if (location.hash.length != 35) {
            location.hash = '0000000000000000000000000xxxxxxxx1';
            return;
        }
        // TODO clear inputBuffer

        // stateChange is set when the code itself changes the hash
        // so !stateChange means the user changed the hash (e.g. back button)
        if (!this.state.stateChange) {
            let score = '';
            if (location.hash in scoreCache) {
                score = scoreCache[location.hash];
            }
            this.setState({
                // Don't hold the AI for the start state
                holdAI: location.hash != '#0000000000000000000000000xxxxxxxx1',
                score: score,
                puzzleid: null,
            });
        }
        this.setState({
            stateChange: false,
            waitingForHint: false,
            hint: null,
            hintSpaces: [],
        });

        // When the hash changes, get the actions from the server
        this.setState({actions: []});
        fetch(API_URL + '/actions/' + location.hash.slice(1))
            .then(response => response.json())
            .then(json => {
                this.setState({actions: json.actions});

                if (!this.state.holdAI && this.state['player'+turn()+'_ai']) {
                    this.aiMove();
                }
            })
            .catch(console.error);

        resizeBoard();
    }

    // User has clicked a Space component
    handleSpaceClick(spacei, e) {
        e.stopPropagation();
        let buffer = this.state.inputBuffer.slice();

        // If spacei is the last item of the input buffer, user
        // is undoing the last input by clikcing
        if (spacei === buffer[buffer.length - 1]) {
            buffer = buffer.slice(0, -1);
            this.setState({inputBuffer: buffer});
            return;
        }

        buffer.push(spacei);

        let activeSpaces = buildActionTree(this.state.actions);
        for (let inputi of buffer) {
            activeSpaces = activeSpaces[inputi];
        }

        if (typeof(activeSpaces) === 'string') {
            this.setState({inputBuffer: []});
            this.onAction(activeSpaces);
            return;
        }

        this.setState({inputBuffer: buffer});
    }

    aiMove() {
        if (this.state.actions.length === 0) {
            return;
        }

        this.setState({actions: [], waitingForAI: true});

        fetch(API_URL + '/think/' + location.hash.slice(1), {
            headers: [
                ['Iterations', this.state.iterations],
                ['Workers', this.state.workers]
            ]
        })
            .then(response => response.json())
            .then(json => {
                console.log(json.log);

                let logLines = json.log.split('\n');
                let score = '';
                for (let i = 0; i < logLines.length; i++) {
                    let line = logLines[i];
                    if (!line.startsWith('score')) continue;

                    score = line.split('\t').at(-1);
                    break;
                }

                this.setState({
                    score: score,
                    waitingForAI: false,
                    stateChange: true,
                });

                scoreCache[location.hash] = score
                location.hash = json.state;
                scoreCache[location.hash] = score
            })
            .catch(console.error)
    }

    onPuzzleRequest(e, skip, noConfirm) {
        if (!noConfirm && !confirm('Load puzzle?')) {
            return;
        }

        skip = skip || 0;
        let id;
        for (id = skip; id < this.state.puzzles.length; id++) {
            if (this.state.solvedPuzzles[id] !== true) {
                break;
            }
        }
        if (id == this.state.puzzles.length) {
            id = Math.floor(Math.random() * this.state.puzzles.length);
        }

        let puzzleState = this.state.puzzles[id]
        let player1_ai = puzzleState.slice(-1) == '2'
        let player2_ai = puzzleState.slice(-1) == '1'

        this.setState({
            puzzleid: id,
            player1_ai: player1_ai,
            player2_ai: player2_ai,
            iterations: 50000,
            workers: 4,
            stateChange: true,
        });
        localStorage.removeItem('player1_ai');
        localStorage.removeItem('player2_ai');

        location.hash = puzzleState;
    }

    onPuzzleSolve() {
        if (!confirm('Figured it out?')) {
            return;
        }
        let solvedPuzzles = this.state.solvedPuzzles;
        solvedPuzzles[this.state.puzzleid] = true;
        localStorage.setItem('solvedPuzzles', JSON.stringify(solvedPuzzles));
        this.setState({solvedPuzzles: solvedPuzzles});

        this.onPuzzleRequest(null, null, true);
    }

    onPuzzleSkip(e) {
        this.onPuzzleRequest(e, this.state.puzzleid + 1, true);
    }

    onPuzzleSubmit(e) {
        if (!confirm('Add current board to the puzzle database?')) {
            return;
        }

        fetch(API_URL + '/submit/' + location.hash.slice(1))
            .then(response => {
                if (response.status === 200) {
                    alert('Submitted!');
                }
            });
    }
}


ReactDOM.render(
    e(UI),
    document.getElementById('root'),
)
