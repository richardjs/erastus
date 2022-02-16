const e = React.createElement;


function cleanHash() {
    if (location.hash.length != 31) {
        location.hash = '0000000000000000000000000xxxx0';
    }
}


class Space extends React.Component {
    render() {
        return e('button', {className: 'col space'}, this.props.notation);
    }
}


class Board extends React.Component {
    render() {
        let rows = [];
        for (let i = 0; i < 5; i++) {
            let row = [];
            for (let j = 0; j < 5; j++) {
                row.push(e(Space, {key: j,
                    notation: this.props.notation[1 + 5*i + j],
                }));
            }

            rows.push(e('div', {key: i, className: 'row'}, row));
        }

        return e('div', null, rows);
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
        return e(Board, {notation: location.hash});
    }
}


ReactDOM.render(
    e(UI),
    document.getElementById('root'),
)