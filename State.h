//
// Created by sebastian on 22.04.23.
//

#ifndef INC_15PUZZLE_STATE_H
#define INC_15PUZZLE_STATE_H
#define Board unsigned long long
#define boardWidth 4
#define board_len boardWidth*boardWidth

class State {
public:
    Board board;
    unsigned short int g_cost;
    unsigned short int f_cost;
    char parentMove;
    State* parent;

    void setTile(int idx, int value);
    int getTile(int idx) const;
    void move(short mv, int blankspot);
    void printBoard() const;

    State(Board board, unsigned short int g, unsigned short int f, char parentMove, State *parent);
};


#endif //INC_15PUZZLE_STATE_H
