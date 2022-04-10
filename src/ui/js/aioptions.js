function ai_player_controls(n) {
    return e('div', {
        className: 'form-check col-sm-3',
    },
        e('label', {
            htmlFor: 'player'+n+'_ai',
            className: 'form-check-label',
        }, 'Player '+n),
        e('input', {
            type: 'checkbox',
            id: 'player'+n+'_ai',
            className: 'form-check-input',
            checked: this.props['player'+n+'_ai'],
            onChange: this.handlePlayerAIChange,
        }),
    );
}


class AIOptions extends React.Component {
    constructor(props) {
        super(props);

        this.handlePlayerAIChange = this.handlePlayerAIChange.bind(this)
    }

    render() {
        return e('div', {className: 'ai-options'},
            e('h6', {className: 'lead'},
                'AI' + (this.props.waitingForAI ? ' (thinking)' : '')),
            e('div', {
                className: 'row',
            },
                e('label', {
                    className: 'form-label col-sm-2',
                    htmlFor: 'iterations'
                }, 'Iterations:'),
                e('div', {className: 'col-sm-7'},
                    e('input', {
                        type: 'range',
                        className: 'form-range',
                        min: 10000,
                        max: 200000,
                        step: 10000,
                        value: this.props.iterations,
                        onChange: this.props.handleIterationsChange,
                    }),
                ),
                e('div', {className: 'col-sm-3'},
                    e('input', {
                        type: 'number',
                        className: 'form-control-sm',
                        style: {'width': '100%'},
                        id: 'iterations',
                        min: 10000,
                        max: 200000,
                        step: 10000,
                        value: this.props.iterations,
                        onChange: this.props.handleIterationsChange,
                    }),
                ),
            ),
            e('div', {
                className: 'row',
            },
                e('label', {
                    className: 'form-label col-sm-2',
                    htmlFor: 'workers'
                }, 'Workers:'),
                e('div', {className: 'col-sm-7'},
                    e('input', {
                        type: 'range',
                        className: 'form-range',
                        min: 1,
                        max: 8,
                        step: 1,
                        value: this.props.workers,
                        onChange: this.props.handleWorkersChange,
                    }),
                ),
                e('div', {className: 'col-sm-3'},
                    e('input', {
                        type: 'number',
                        className: 'form-control-sm',
                        style: {'width': '100%'},
                        id: 'workers',
                        min: 1,
                        max: 8,
                        step: 1,
                        value: this.props.workers,
                        onChange: this.props.handleWorkersChange,
                    }),
                ),
            ),
            e('div', {
                className: 'row',
            },
                e('div', {className: 'col-sm-1'}),
                ai_player_controls.bind(this)(1),
                ai_player_controls.bind(this)(2),
                e('div', {className: 'col-sm-2'}),
                e('div', {
                    className: 'col-sm-3',
                }, 'Score: ' + this.props.score),
            ),
        );
    }

    handlePlayerAIChange(e) {
        this.props.onPlayerAIChange(e.target.id, e.target.checked);
    }
}
