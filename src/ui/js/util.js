const e = React.createElement;


const MIN_FONT_SIZE = 6;
const MAX_FONT_SIZE = 40;
const FONT_UNDERSIZE = 0;

function resizeBoard() {
    //window.innerHeight -- don't exceed it
    //adjust .col max-width to change widths

    for (let space of document.getElementsByClassName('space')) {
        // resize space
        let height = space.getBoundingClientRect().width;
        if (height*5 > window.innerHeight) {
            height = window.innerHeight / 5;
        }
        space.style.height = height + 'px';
    }
}

window.addEventListener('resize', resizeBoard);


function coordsToSpace(coord) {
    col = {'a': 0, 'b': 1, 'c': 2, 'd': 3, 'e': 4}[coord[0].toLowerCase()];
    row = parseInt(coord[1]) - 1;
    return 5*row + col;
}


function spaceToCoords(spacei) {
    col = 'abcde'[spacei % 5];
    row = Math.floor(spacei / 5) + 1;
    return col + row;
}


// Create a tree structure from a list of actions. The nodes are the
// spaceis of the components of the action. Each leaf is the string
// representation of the action consisting of the spaceis (stored here
// for easy sending back to the API).
function buildActionTree(actions) {
    let actionTree = {};

    for (let action_string of actions) {
        // Split action string into component spaces and convert to spaceis
        let actionSpaces = action_string.split(/\&|-|\+/).map(coordsToSpace);

        if (actionTree[actionSpaces[0]] === undefined) {
            actionTree[actionSpaces[0]] = {};
        }

        if (actionTree[actionSpaces[0]][actionSpaces[1]] === undefined) {
            if (actionSpaces.length === 2) {
                actionTree[actionSpaces[0]][actionSpaces[1]] = action_string;
            } else {
                actionTree[actionSpaces[0]][actionSpaces[1]] = {};
            }
        }

        if (actionSpaces.length === 3) {
            actionTree[actionSpaces[0]][actionSpaces[1]][actionSpaces[2]] = action_string;
        }
    }

    return actionTree;
}
