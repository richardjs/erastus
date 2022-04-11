class UI extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            actions: [],
            inputBuffer: [],
            holdAI: true,
            stateChange: false,
            waitingForAI: false,
            score: 'n\\a',
            iterations: localStorage.getItem('iterations') || 50000,
            workers: localStorage.getItem('workers') || 4,
            player1_ai: localStorage.getItem('player1_ai') ? true : false,
            player2_ai: localStorage.getItem('player2_ai') ? true : false,
        };

        this.onAction = this.onAction.bind(this);
        this.onPlayerAIChange = this.onPlayerAIChange.bind(this);
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
                    score: this.state.score,
                    handleIterationsChange: this.handleIterationsChange,
                    handleWorkersChange: this.handleWorkersChange,
                    onPlayerAIChange: this.onPlayerAIChange,
                }),
            ),
            e('div', {className: 'row mt-4'},
                e('button', {
                    className: 'btn btn-danger',
                    onClick: () => {if (confirm('New game?')) window.location = ''},
                }, "New game"),
            ),
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
            this.setState({holdAI: true});
        }
        this.setState({stateChange: false});

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
                let score = 'n\\a';
                for (let i = 0; i < logLines.length; i++) {
                    let line = logLines[i];
                    if (!line.startsWith('score')) continue;

                    score = line.split('\t').at(-1);
                    break;
                }

                this.setState({
                    score: score,
                    waitingForAI: false
                });
                this.setState({stateChange: true});
                location.hash = json.state;
            })
            .catch(console.error)
    }
}


ReactDOM.render(
    e(UI),
    document.getElementById('root'),
)
