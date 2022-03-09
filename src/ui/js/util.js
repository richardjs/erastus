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
resizeBoard();
