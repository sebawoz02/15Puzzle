//
// Created by sebastian on 22.04.23.
//

#include "State.h"
#include <iostream>


#define BITS_PER_TILE  4
#define MASK 0x00000000000000000fULL    // mask is used to reset bits, which alows to extract the value of the tile

using namespace std;


State::State(unsigned long long board, unsigned short g, unsigned short f, char parentMove, State *parent) {
    this->board = board;
    this->f_cost = f;
    this->g_cost = g;
    this->parentMove = parentMove;
    this->parent = parent;
}

void State::printBoard() const {
    for(int i = 0; i< board_len ; i++){
        cout << getTile(i) << " ";
        if(i%boardWidth == boardWidth-1) cout << endl;
    }
}

// Returns board element under given index
int State::getTile(int idx) const {
    return (this->board >> ((boardWidth*boardWidth - 1 - idx)*BITS_PER_TILE)) & MASK;
}

// Sets the new value of the board element at the given index
void State::setTile(int idx, int value){
    this->board &= ~(MASK << ((boardWidth*boardWidth - 1 - idx) * BITS_PER_TILE));
    this->board |= static_cast<Board>(value) << ((boardWidth*boardWidth - 1 - idx) * BITS_PER_TILE);
}

// Moves the blank spot
// 0 - move up
// 1 - move left
// 2 - move right
// 3 - move down
void State::move(short mv, int blankspot){
    if(mv == 0){
        int val = getTile(blankspot-boardWidth);
        setTile(blankspot, val);
        setTile(blankspot - boardWidth, 0x0000);
    }
    else if(mv == 3){
        int val = getTile(blankspot+boardWidth);
        setTile(blankspot, val);
        setTile(blankspot+boardWidth, 0x0000);
    }
    else if(mv == 1){
        int val = getTile(blankspot-1);
        setTile(blankspot, val);
        setTile(blankspot-1, 0x0000);
    }
    else if(mv == 2){
        int val = getTile(blankspot+1);
        setTile(blankspot, val);
        setTile(blankspot+1, 0x0000);
    }

}

