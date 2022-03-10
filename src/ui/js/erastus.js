API_URL = 'http://127.0.0.1:5000'


const e = React.createElement;


function coordToSpace(coord) {
    col = {'a': 0, 'b': 1, 'c': 2, 'd': 3, 'e': 4}[coord[0].toLowerCase()];
    row = parseInt(coord[1]) - 1;
    return 5*row + col;
}


function spaceToCoord(spacei) {
    col = 'abcde'[spacei % 5];
    row = Math.floor(spacei / 5) + 1;
    return col + row;
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
    constructor(props) {
        super(props);

        this.state = {
            inputBuffer: [],
        };

        this.handleClick = this.handleClick.bind(this);
    }


    isPlacePhase() {
        return this.props.notation.toLowerCase().includes('x');
    }


    turn() {
        return parseInt(this.props.notation.slice(33, 34));
    }


    buildActionTree() {
        let actionTree = {};

        for (let action of this.props.actions) {
            let actionSpaces = action.split(/\&|-|\+/).map(coordToSpace);

            if (actionTree[actionSpaces[0]] === undefined) {
                actionTree[actionSpaces[0]] = {};
            }

            if (actionTree[actionSpaces[0]][actionSpaces[1]] === undefined) {
                if (actionSpaces.length === 2) {
                    actionTree[actionSpaces[0]][actionSpaces[1]] = null;
                } else {
                    actionTree[actionSpaces[0]][actionSpaces[1]] = {};
                }
            }

            if (actionSpaces.length === 3) {
                actionTree[actionSpaces[0]][actionSpaces[1]][actionSpaces[2]] = null;
            }
        }

        return actionTree;
    }


    render() {
        let p1WorkerSpaces = [
            coordToSpace(this.props.notation.slice(25, 27)),
            coordToSpace(this.props.notation.slice(27, 29)),
        ];
        let p2WorkerSpaces = [
            coordToSpace(this.props.notation.slice(29, 31)),
            coordToSpace(this.props.notation.slice(31, 33)),
        ];

        let actionTree = this.buildActionTree();

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

                let activeSpaces = actionTree;
                for (let inputi of this.state.inputBuffer) {
                    activeSpaces = activeSpaces[inputi];
                }
                let active = activeSpaces[spacei] !== undefined;

                row.push(e(Space, {key: j,
                    spacei: spacei,
                    handleClick: this.handleClick,
                    level: parseInt(this.props.notation[spacei]),
                    worker: worker,
                    active: active,
                    inInputBuffer: this.state.inputBuffer.includes(spacei),
                }));
            }

            rows.push(e('div', {key: i, className: 'row'}, row));
        }

        return e('div', {className: 'board'}, rows);
    }

    handleClick(spacei, e) {
        let buffer = this.state.inputBuffer.slice();
        buffer.push(spacei);

        let activeSpaces = this.buildActionTree();
        for (let inputi of buffer) {
            activeSpaces = activeSpaces[inputi];
        }

        if (activeSpaces === null) {
            // TODO submit move to server
            console.log('move complete');
            return;
        }

        this.setState({inputBuffer: buffer});
    }
}


class UI extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            actions: [],
        };

        this.hashChange = this.hashChange.bind(this);
        window.addEventListener('hashchange', this.hashChange);
    }

    componentDidMount() {
        this.hashChange();
    }

    render() {
        return e(Board, {
            notation: location.hash.slice(1),
            actions: this.state.actions,
        });
    }

    hashChange() {
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
}


ReactDOM.render(
    e(UI),
    document.getElementById('root'),
)