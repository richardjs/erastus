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
