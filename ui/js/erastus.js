const e = React.createElement;


function cleanHash() {
    if (location.hash.length != 35) {
        location.hash = '0000000000000000000000000xxxxxxxx0';
    }
}


function coordToSpace(coord) {
    col = {'a': 0, 'b': 1, 'c': 2, 'd': 3, 'e': 4}[coord[0].toLowerCase()];
    row = parseInt(coord[1]) - 1;
    return 5*row + col;
}


class Space extends React.Component {
    render() {
        let className = 'col space ';
        className += ' level-' + this.props.level;

        return e('button', {className: className}, this.props.worker);
    }
}


class Board extends React.Component {
    render() {
        let p1WorkerCoords = [
            coordToSpace(this.props.notation.slice(25, 28)),
            coordToSpace(this.props.notation.slice(27, 30)),
        ];
        let p2WorkerCoords = [
            coordToSpace(this.props.notation.slice(29, 32)),
            coordToSpace(this.props.notation.slice(31, 34)),
        ];

        let rows = [];
        for (let i = 0; i < 5; i++) {
            let row = [];
            for (let j = 0; j < 5; j++) {
                let space = 5*i + j;

                let worker = null;
                console.log(space, p1WorkerCoords);
                if (p1WorkerCoords.includes(space)) {
                    worker = 1;
                }
                if (p2WorkerCoords.includes(space)) {
                    worker = 2;
                }

                row.push(e(Space, {key: j,
                    level: parseInt(this.props.notation[space]),
                    worker: worker,
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
