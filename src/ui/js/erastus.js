API_URL = 'http://127.0.0.1:5000'


const e = React.createElement;


function coordsToSpace(coord) {
    col = {'a': 0, 'b': 1, 'c': 2, 'd': 3, 'e': 4}[coord[0].toLowerCase()];
    row = parseInt(coord[1]) - 1;
    return 5*row + col;
}


function spaceToCoords(spacei) {
    col = 'abcde'[spacei % 5];
    row = Math.floor(spacei / 5) + 1;
    return col + row;
}


// Create a tree structure from a list of actions. The nodes are the
// spaceis of the components of the action. Each leaf is the string
// representation of the action consisting of the spaceis (stored here
// for easy sending back to the API).
function buildActionTree(actions) {
    let actionTree = {};

    for (let action_string of actions) {
        // Split action string into component spaces and convert to spaceis
        let actionSpaces = action_string.split(/\&|-|\+/).map(coordsToSpace);

        if (actionTree[actionSpaces[0]] === undefined) {
            actionTree[actionSpaces[0]] = {};
        }

        if (actionTree[actionSpaces[0]][actionSpaces[1]] === undefined) {
            if (actionSpaces.length === 2) {
                actionTree[actionSpaces[0]][actionSpaces[1]] = action_string;
            } else {
                actionTree[actionSpaces[0]][actionSpaces[1]] = {};
            }
        }

        if (actionSpaces.length === 3) {
            actionTree[actionSpaces[0]][actionSpaces[1]][actionSpaces[2]] = action_string;
        }
    }

    return actionTree;
}


class Space extends React.Component {
    render() {
        let className = 'col fs-1 space';
        className += ' level-' + this.props.level;

        if (this.props.inInputBuffer) {
            className += ' inputBuffer';
        }

        return e('button', {
            className: className,
            disabled: !this.props.active,
            onClick: this.props.handleClick.bind(this, this.props.spacei),
        }, ['', 'X', 'O'][this.props.worker]);
    }
}


class Board extends React.Component {
    render() {
        // Pull spaceis of workers from the state notation
        let p1WorkerSpaces = [
            coordsToSpace(this.props.notation.slice(25, 27)),
            coordsToSpace(this.props.notation.slice(27, 29)),
        ];
        let p2WorkerSpaces = [
            coordsToSpace(this.props.notation.slice(29, 31)),
            coordsToSpace(this.props.notation.slice(31, 33)),
        ];

        let actionTree = buildActionTree(this.props.actions);

        let rows = [];
        for (let i = 0; i < 5; i++) {
            let row = [];
            for (let j = 0; j < 5; j++) {
                let spacei = 5*i + j;

                let worker = null;
                if (p1WorkerSpaces.includes(spacei)) {
                    worker = 1;
                } else if (p2WorkerSpaces.includes(spacei)) {
                    worker = 2;
                }

                // Walk down the actionTree with the spaces already in inputBuffer
                let activeSpaces = actionTree;
                let inputi;
                for (inputi of this.props.inputBuffer) {
                    activeSpaces = activeSpaces[inputi];
                }
                // A space is active if there is an action from our
                // current inputBuffer, or if it is the last space in
                // the inputBuffer (in which case a click is an undo)
                let active = activeSpaces[spacei] !== undefined || spacei === inputi;

                row.push(e(Space, {key: j,
                    spacei: spacei,
                    handleClick: this.props.handleSpaceClick,
                    level: parseInt(this.props.notation[spacei]),
                    worker: worker,
                    active: active,
                    inInputBuffer: this.props.inputBuffer.includes(spacei),
                }));
            }

            rows.push(e('div', {key: i, className: 'row'}, row));
        }

        return e('div', {className: 'board'}, rows);
    }
}


class AIOptions extends React.Component {
    constructor(props) {
        super(props);

        this.handleInputChange = this.handleInputChange.bind(this)
    }

    render() {
        return e('div', {className: 'ai-options'},
            e('h6', {className: 'lead'}, 'AI Options'),
            e('div', {className: 'form-check'},
                e('input', {
                    className: 'form-check-input',
                    type: 'checkbox',
                    id: 'player1_ai',
                    checked: this.props.player1_ai,
                    onChange: this.handleInputChange,
                }),
                e('label', {
                    className: 'form-check-label',
                    htmlFor: 'player1_ai',
                }, 'Player 1'),
            ),
            e('div', {className: 'form-check'},
                e('input', {
                    className: 'form-check-input',
                    type: 'checkbox',
                    id: 'player2_ai',
                    checked: this.props.player2_ai,
                    onChange: this.handleInputChange,
                }),
                e('label', {
                    className: 'form-check-label',
                    htmlFor: 'player2_ai',
                }, 'Player 2'),
            ),
        );
    }

    handleInputChange(e) {
        this.props.onPlayerAIChange(e.target.id, e.target.checked);
    }
}


class UI extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            actions: [],
            inputBuffer: [],
            player1_ai: false,
            player2_ai: true,
        };

        this.onAction = this.onAction.bind(this);
        this.onPlayerAIChange = this.onPlayerAIChange.bind(this);
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
        return e('div', null,
            e('div', {className: 'row'},
                e(Board, {
                    notation: location.hash.slice(1),
                    actions: this.state.actions,
                    inputBuffer: this.state.inputBuffer,
                    handleSpaceClick: this.handleSpaceClick,
                }),
            ),
            e('div', {className: 'row'},
                e(AIOptions, {
                    player1_ai: this.state.player1_ai,
                    player2_ai: this.state.player2_ai,
                    onPlayerAIChange: this.onPlayerAIChange,
                }),
            ),
        );
    }

    onAction(action) {
        this.setState({actions: []});

        fetch(API_URL + '/act/' + location.hash.slice(1) + '/' + action)
            .then(response => response.json())
            .then(json => {
                console.log(json.log);
                location.hash = json.state;
            })
            .catch(console.error);
    }

    onPlayerAIChange(player_ai, value) {
        this.setState({[player_ai]: value});
    }

    handleClick() {
        // Set inputBuffer to empty
    }

    handleHashChange() {
        // TODO better checks here
        if (location.hash.length != 35) {
            location.hash = '0000000000000000000000000xxxxxxxx1';
            return;
        }
        // TODO clear inputBuffer

        // When the hash changes, get the actions from the server
        this.setState({actions: []});
        fetch(API_URL + '/actions/' + location.hash.slice(1))
            .then(response => response.json())
            .then(json => {
                console.log(json.log);
                this.setState({actions: json.actions});
            })
            .catch(console.error);
    }

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
}


ReactDOM.render(
    e(UI),
    document.getElementById('root'),
)
