class AIOptions extends React.Component {
    constructor(props) {
        super(props);

        this.handlePlayerAIChange = this.handlePlayerAIChange.bind(this)
    }

    ai_player_controls(n) {
        return e('div', {
            className: 'form-check col-1',
        },
            e('label', {
                htmlFor: 'player'+n+'_ai',
                className: 'form-check-label',
            }, n == 1 ? 'X' : 'O'),
            e('input', {
                type: 'checkbox',
                id: 'player'+n+'_ai',
                className: 'form-check-input',
                checked: this.props['player'+n+'_ai'],
                onChange: this.handlePlayerAIChange,
            }),
        );
    }

    hintButton() {
        if (this.props.hint === null) {
            return e('div', {className: 'col-3'},
                e('button', {
                   className: 'btn btn-sm btn-secondary',
                   onClick: this.props.handleHintClick,
                   disabled: this.props.waitingForHint || this.props.waitingForAI,
                },
                    this.props.waitingForHint ? 'Thinking...' : 'Hint'
                ),
            );
        }

        return e('span', {
            className: 'col-3',
        }, this.props.hint);
    }

    render() {
        return e('div', {className: 'ai-options'},
            e('h6', {className: 'lead'},
                'AI' + (this.props.waitingForAI ? ' (thinking)' : '')),
            e('div', {
                className: 'row',
            },
                e('label', {
                    className: 'form-label col-2',
                    htmlFor: 'iterations'
                }, 'Iterations:'),
                e('div', {className: 'col-7'},
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
                e('div', {className: 'col-3'},
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
                    className: 'form-label col-2',
                    htmlFor: 'workers'
                }, 'Workers:'),
                e('div', {className: 'col-7'},
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
                e('div', {className: 'col-3'},
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
                className: 'row align-items-center mb-1',
            },
                e('div', {className: 'col-1'}),
                this.ai_player_controls(1),
                e('div', {className: 'col-1'}),
                this.ai_player_controls(2),
                e('div', {className: 'col-2'}),
                this.hintButton(),
                e('span', {
                    className: 'col-3',
                }, 'Score: ' + this.props.score),
            ),
        );
    }

    handlePlayerAIChange(e) {
        this.props.onPlayerAIChange(e.target.id, e.target.checked);
    }
}
