API_URL = 'http://127.0.0.1:5000'


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
        resizeBoard();
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
