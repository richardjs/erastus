#include "layout.h"


const uint_fast32_t ADJACENT_SPACES[25] = {
    0b0000000000000000001100010,
    0b0000000000000000011100101,
    0b0000000000000000111001010,
    0b0000000000000001110010100,
    0b0000000000000001100001000,

    0b0000000000000110001000011,
    0b0000000000001110010100111,
    0b0000000000011100101001110,
    0b0000000000111001010011100,
    0b0000000000110000100011000,

    0b0000000011000100001100000,
    0b0000000111001010011100000,
    0b0000001110010100111000000,
    0b0000011100101001110000000,
    0b0000011000010001100000000,

    0b0001100010000110000000000,
    0b0011100101001110000000000,
    0b0111001010011100000000000,
    0b1110010100111000000000000,
    0b1100001000110000000000000,

    0b0001000011000000000000000,
    0b0010100111000000000000000,
    0b0101001110000000000000000,
    0b1010011100000000000000000,
    0b0100011000000000000000000,
};