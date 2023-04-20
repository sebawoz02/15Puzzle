// Sebastian Woźniak - 15 puzzle

#include <iostream>
#include <bits/stdc++.h>

#define BITS_PER_TILE  4
#define MASK 0x00000000fULL
#define Board unsigned long long

using namespace std;

int boardWidth = 3;


class State{
public:
    Board board;
    unsigned int g_cost;
    unsigned int f_cost;
    char parentMove;
    State* parent;

    void setTile(int idx, int value);
    char getTile(int idx) const;

    State(Board board, unsigned int g, unsigned int f, char parentMove, State *parent){
        this->board = board;
        this->f_cost = f;
        this->g_cost = g;
        this->parentMove = parentMove;
        this->parent = parent;
    }
};

auto cmp = [](State* left, State* right){return left->f_cost < right->f_cost;};

// Returns board element under given index
char State::getTile(int idx) const {
    return (this->board >> (idx*BITS_PER_TILE)) & MASK;
}

// Sets new value of the board element under given index
void State::setTile(int idx, int value){
    this->board &= ~(MASK << (idx * BITS_PER_TILE));
    this->board |= static_cast<Board>(value) << (idx * BITS_PER_TILE);
}

// Function calculates count of inversions in given state.
int getInversionCount(State *state){
    int inv_count = 0;
    for(int i = 0; i<boardWidth*boardWidth; i++){
        for(int j = i+1; j<=boardWidth*boardWidth; j++){
            if(state->getTile(j) && state->getTile(i) && state->getTile(i) > state->getTile(j))
                inv_count++;
        }
    }
    return inv_count;
}

// Finds index of blank tile.
char getBlankPos(State *state){
    for(int i = boardWidth*boardWidth - 1; i>=0 ; --i){
        if(state->getTile(i) == 0) return i;
    }
    return -1;
}

// Function returns true if puzzle from given state is solvable
bool isSolvable(State *state){
    int invCount = getInversionCount(state);
    if(boardWidth & 1)return !(invCount & 1);
    else {
        char pos = getBlankPos(state);
        if(pos & 1) return !(invCount & 1);
        else return invCount & 1;
    }
}


unsigned char manhattanHeuristic(Board board){
    for(size_t i = 0; i<boardWidth*boardWidth; i++){
        return 0;
    }
    return 0;
}

// prints the solution
void solutionFound(State* state){

}

// A* algorithm to find the best solution from initialState to goal
// returns -1 if not solvable, else 0
int AstarSearch(State* initialState, Board goal){
    if(!isSolvable(initialState)) return -1;

    // open list for states to be checked
    multiset<State*, decltype(cmp)> openList(cmp);;
    // checked states
    unordered_set<State*> closedList;
    // set iterator
    openList.insert(initialState);
    while (!openList.empty()){
        // state with the lowest f_cost
        State* q = *openList.begin();
        openList.erase(q);
        closedList.insert(q);

        // generate successors
        char blankPos = getBlankPos(q);
        //-------------------------- 1 - move blank spot higher --------------------------
        if(blankPos >= boardWidth){
            auto* topSuccessor = new State(q->board, q->g_cost + 1, 0, 't', q);
            char val = topSuccessor->getTile(blankPos-boardWidth);
            topSuccessor->setTile(blankPos, val);
            topSuccessor->setTile(blankPos-boardWidth, 0x0000);
            if(topSuccessor->board == goal){
                solutionFound(topSuccessor);
                return 0;
            }
            topSuccessor->f_cost = manhattanHeuristic(topSuccessor->board) + topSuccessor->g_cost;

            for(auto itr : openList){
                if(itr->board == topSuccessor->board){
                    if(itr->f_cost>topSuccessor->f_cost){
                        for(auto itr2 : closedList){
                            if(itr2->board == topSuccessor->board){
                                if(itr->f_cost>topSuccessor->f_cost){
                                    openList.insert(topSuccessor);
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }


        }
        // -------------------------- 2 - move blank spot lower --------------------------
        if(blankPos <  (boardWidth*boardWidth)-boardWidth ){
            auto* lowSuccessor = new State(q->board, q->g_cost + 1, 0, 'd', q);
            char val = lowSuccessor->getTile(blankPos+boardWidth);
            lowSuccessor->setTile(blankPos, val);
            lowSuccessor->setTile(blankPos+boardWidth, 0x0000);
            if(lowSuccessor->board == goal){
                solutionFound(lowSuccessor);
                return 0;
            }
            lowSuccessor->f_cost = manhattanHeuristic(lowSuccessor->board) + lowSuccessor->g_cost;

            for(auto itr : openList){
                if(itr->board ==  lowSuccessor->board){
                    if(itr->f_cost> lowSuccessor->f_cost){
                        for(auto itr2 : closedList){
                            if(itr2->board ==  lowSuccessor->board){
                                if(itr->f_cost> lowSuccessor->f_cost){
                                    openList.insert( lowSuccessor);
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
        // -------------------------- 3 - move blank spot left --------------------------
        if(blankPos % boardWidth != 0){
            auto* leftSuccessor = new State(q->board, q->g_cost + 1, 0, 'l', q);
            char val = leftSuccessor->getTile(blankPos-1);
            leftSuccessor->setTile(blankPos, val);
            leftSuccessor->setTile(blankPos-1, 0x0000);
            if(leftSuccessor->board == goal){
                solutionFound(leftSuccessor);
                return 0;
            }
            leftSuccessor->f_cost = manhattanHeuristic(leftSuccessor->board) + leftSuccessor->g_cost;

            for(auto itr : openList){
                if(itr->board == leftSuccessor->board){
                    if(itr->f_cost>leftSuccessor->f_cost){
                        for(auto itr2 : closedList){
                            if(itr2->board == leftSuccessor->board){
                                if(itr->f_cost>leftSuccessor->f_cost){
                                    openList.insert(leftSuccessor);
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
        // -------------------------- 4 - move blank spot right --------------------------
        if(blankPos % boardWidth != boardWidth-1){
            auto* rightSuccessor = new State(q->board, q->g_cost + 1, 0, 'p', q);
            char val = rightSuccessor->getTile(blankPos+1);
            rightSuccessor->setTile(blankPos, val);
            rightSuccessor->setTile(blankPos+1, 0x0000);
            if(rightSuccessor->board == goal){
                solutionFound(rightSuccessor);
                return 0;
            }
            rightSuccessor->f_cost = manhattanHeuristic(rightSuccessor->board) + rightSuccessor->g_cost;

            for(auto itr : openList){
                if(itr->board == rightSuccessor->board){
                    if(itr->f_cost>rightSuccessor->f_cost){
                        for(auto itr2 : closedList){
                            if(itr2->board == rightSuccessor->board){
                                if(itr->f_cost>rightSuccessor->f_cost){
                                    openList.insert(rightSuccessor);
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    return -1;
}


int main() {
    Board goal = 0x123456780ULL;
    Board randomState = 0x125463780ULL;
    auto* state = new State(randomState, 0, 0, '\0', nullptr);

}
