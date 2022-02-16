const e = React.createElement;


class Space extends React.Component {
    render() {
        return e('button', {className: 'col space'}, 'a');
    }
}


class Board extends React.Component {
    render() {
        let rows = [];
        for (let i = 0; i < 5; i++) {
            let row = [];
            for (let j = 0; j < 5; j++) {
                row.push(e(Space, {key: j}));
            }

            rows.push(e('div', {key: i, className: 'row'}, row));
        }

        return e('div', null, rows);
    }
}


class UI extends React.Component {
    render() {
        return e(Board);
    }
}


ReactDOM.render(
    e(UI),
    document.getElementById('root'),
)
