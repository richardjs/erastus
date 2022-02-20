const e = React.createElement;


function cleanHash() {
    if (location.hash.length != 35) {
        location.hash = '0000000000000000000000000xxxxxxxx1';
    }
}


function coordToSpace(coord) {
    col = {'a': 0, 'b': 1, 'c': 2, 'd': 3, 'e': 4}[coord[0].toLowerCase()];
    row = parseInt(coord[1]) - 1;
    return 5*row + col;
}


class Space extends React.Component {
    render() {
        let className = 'col fs-1 space ';
        className += ' level-' + this.props.level;

        return e('button', {
            className: className,
            disabled: !this.props.active,
        }, ['', 'X', 'O'][this.props.worker]);
    }
}


class Board extends React.Component {
    render() {
        let p1WorkerCoords = [
            coordToSpace(this.props.notation.slice(25, 27)),
            coordToSpace(this.props.notation.slice(27, 29)),
        ];
        let p2WorkerCoords = [
            coordToSpace(this.props.notation.slice(29, 31)),
            coordToSpace(this.props.notation.slice(31, 33)),
        ];

        let turn = parseInt(this.props.notation.slice(33, 34));

        let placePhase = this.props.notation.toLowerCase().includes('x');

        let rows = [];
        for (let i = 0; i < 5; i++) {
            let row = [];
            for (let j = 0; j < 5; j++) {
                let space = 5*i + j;

                let worker = null;
                if (p1WorkerCoords.includes(space)) {
                    worker = 1;
                }
                if (p2WorkerCoords.includes(space)) {
                    worker = 2;
                }

                let active;
                if (placePhase) {
                    active = worker === null;
                } else {
                    active = turn === worker;
                }

                row.push(e(Space, {key: j,
                    level: parseInt(this.props.notation[space]),
                    worker: worker,
                    active: active,
                }));
            }

            rows.push(e('div', {key: i, className: 'row'}, row));
        }

        return e('div', {className: 'board'}, rows);
    }
}


class UI extends React.Component {
    constructor(props) {
        super(props);

        cleanHash();
        window.addEventListener('hashchange', () => {
            cleanHash();
            this.forceUpdate();
        });
    }

    render() {
        return e(Board, {notation: location.hash.slice(1)});
    }
}


ReactDOM.render(
    e(UI),
    document.getElementById('root'),
)
