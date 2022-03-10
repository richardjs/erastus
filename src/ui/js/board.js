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
