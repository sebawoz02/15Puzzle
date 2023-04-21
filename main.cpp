// Sebastian Woźniak - 15 puzzle

#include <iostream>
#include <bits/stdc++.h>
#include <cmath>

#define BITS_PER_TILE  4
#define MASK 0x00000000000000000fULL
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
    int getTile(int idx) const;

    State(Board board, unsigned int g, unsigned int f, char parentMove, State *parent){
        this->board = board;
        this->f_cost = f;
        this->g_cost = g;
        this->parentMove = parentMove;
        this->parent = parent;
    }
};


// Returns board element under given index
int State::getTile(int idx) const {
    return (this->board >> ((boardWidth*boardWidth - 1 - idx)*BITS_PER_TILE)) & MASK;
}

// Sets the new value of the board element at the given index
void State::setTile(int idx, int value){
    this->board &= ~(MASK << ((boardWidth*boardWidth - 1 - idx) * BITS_PER_TILE));
    this->board |= static_cast<Board>(value) << ((boardWidth*boardWidth - 1 - idx) * BITS_PER_TILE);
}

// Function calculates count of inversions in given state.
int getInversionCount(State *state){
    int inv_count = 0;
    for(int i = 0; i<boardWidth*boardWidth; i++){
        for(int j = i+1; j<=boardWidth*boardWidth; j++){
            if(state->getTile(j)!=0 && state->getTile(i)!=0 && state->getTile(i) > state->getTile(j))
                inv_count++;
        }
    }
    return inv_count;
}

// Finds index of blank tile.
int getBlankPos(State *state){
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
        int pos = getBlankPos(state);
        if(pos & 1) return !(invCount & 1);
        else return invCount & 1;
    }
}


unsigned char manhattanHeuristic(State* board){
    unsigned int f_cost = 0;
    for(int i = 0; i<boardWidth*boardWidth; i++){
        int dest = board->getTile(i);
        if(dest!=0){ f_cost += (int) abs(floor(i/boardWidth) - floor((dest-1)/boardWidth)) + abs((i%boardWidth) - ((dest-1)%boardWidth));}
    }
    return f_cost + board->g_cost;
}

// prints the solution
void solutionFound(State* state, long visited){
    cout << "Solution found! Visited states: " << visited << endl;
    State* solution = state;
    unsigned int moves = 0;
    while(solution->parent != nullptr){
        moves++;
        solution = solution->parent;
    }
    cout << "Moves: " << moves << endl;
}

struct Hash{
    size_t operator()(const State* state) const{
        return hash<Board>()(state->board);
    }
};

struct Equal{
    bool operator()(const State* obj1, const State* obj2) const{
        return obj1->board == obj2->board;
    }
};



// A* algorithm to find the best solution from initialState to goal
// returns -1 if not solvable, else 0
int AstarSearch(State* initialState, Board goal){
    if(!isSolvable(initialState)) {
        cout << "Not Solvable!" << endl;
        return -1;
    }
    cout << "Solving..." << endl;
    auto cmp = [](State* left, State* right){return left->f_cost < right->f_cost;};

    long visited = 0;
    // open list for states to be checked
    priority_queue<State*, vector<State*>,decltype(cmp)> openList(cmp);
    // checked states

    unordered_set<const State*, Hash, Equal> closedList;
    // set iterator
    openList.push(initialState);
    closedList.insert(initialState);
    while (!openList.empty()){
        // state with the lowest f_cost
        State* q = openList.top();
        openList.pop();
        closedList.insert(q);
        visited++;
        // generate successors
        int blankPos = getBlankPos(q);
        if(blankPos == -1){
            return -1;
        }
        //-------------------------- 1 - move blank spot higher --------------------------
        if(blankPos >= boardWidth && q->parentMove != 'd'){
            auto* topSuccessor = new State(q->board, q->g_cost + 1, 0, 't', q);
            int val = topSuccessor->getTile(blankPos-boardWidth);
            topSuccessor->setTile(blankPos, val);
            topSuccessor->setTile(blankPos-boardWidth, 0x0000);
            if(topSuccessor->board == goal){
                solutionFound(topSuccessor, visited);
                return 0;
            }
            topSuccessor->f_cost = manhattanHeuristic(topSuccessor);

            auto search = closedList.find(topSuccessor);
            if(search == closedList.end()){
                closedList.insert(topSuccessor);
                openList.push(topSuccessor);
            }
            else if(topSuccessor->f_cost < (*search)->f_cost){
                openList.push(topSuccessor);
                closedList.insert(topSuccessor);
                closedList.erase(search);
            } else delete topSuccessor;

        }
        // -------------------------- 2 - move blank spot lower --------------------------
        if(blankPos < (boardWidth*boardWidth-boardWidth) && q->parentMove!='t'){
            auto* lowSuccessor = new State(q->board, q->g_cost + 1, 0, 'd', q);
            int val = lowSuccessor->getTile(blankPos+boardWidth);
            lowSuccessor->setTile(blankPos, val);
            lowSuccessor->setTile(blankPos+boardWidth, 0x0000);
            if(lowSuccessor->board == goal){
                solutionFound(lowSuccessor, visited);
                return 0;
            }
            lowSuccessor->f_cost = manhattanHeuristic(lowSuccessor);

            auto search = closedList.find(lowSuccessor);
            if(search == closedList.end()){
                closedList.insert(lowSuccessor);
                openList.push(lowSuccessor);
            }
            else if(lowSuccessor->f_cost < (*search)->f_cost){
                openList.push(lowSuccessor);
                closedList.insert(lowSuccessor);
                closedList.erase(search);
            } else delete lowSuccessor;

        }
        // -------------------------- 3 - move blank spot left --------------------------
        if(blankPos % boardWidth != 0 && q->parentMove != 'p'){
            auto* leftSuccessor = new State(q->board, q->g_cost + 1, 0, 'l', q);
            int val = leftSuccessor->getTile(blankPos-1);
            leftSuccessor->setTile(blankPos, val);
            leftSuccessor->setTile(blankPos-1, 0x0000);
            if(leftSuccessor->board == goal){
                solutionFound(leftSuccessor, visited);
                return 0;
            }
            leftSuccessor->f_cost = manhattanHeuristic(leftSuccessor);

            auto search = closedList.find(leftSuccessor);
            if(search == closedList.end()){
                closedList.insert(leftSuccessor);
                openList.push(leftSuccessor);
            }
            else if(leftSuccessor->f_cost < (*search)->f_cost){
                openList.push(leftSuccessor);
                closedList.insert(leftSuccessor);
                closedList.erase(search);
            } else delete leftSuccessor;

        }
        // -------------------------- 4 - move blank spot right --------------------------
        if(blankPos % boardWidth < boardWidth-1 && q->parentMove != 'l'){
            auto* rightSuccessor = new State(q->board, q->g_cost + 1, 0, 'p', q);
            int val = rightSuccessor->getTile(blankPos+1);
            rightSuccessor->setTile(blankPos, val);
            rightSuccessor->setTile(blankPos+1, 0x0000);
            if(rightSuccessor->board == goal){
                solutionFound(rightSuccessor, visited);
                return 0;
            }
            rightSuccessor->f_cost = manhattanHeuristic(rightSuccessor);

            auto search = closedList.find(rightSuccessor);
            if(search == closedList.end()){
                closedList.insert(rightSuccessor);
                openList.push(rightSuccessor);
            }
            else if(rightSuccessor->f_cost < (*search)->f_cost){
                openList.push(rightSuccessor);
                closedList.insert(rightSuccessor);
                closedList.erase(search);
            } else delete rightSuccessor;
        }
    }
    return -1;
}



int main() {
    Board goalBoard = 0x123456780ULL;
    Board randomState = 0x1235468780ULL;
    auto* state = new State(randomState, 0, 0, '\0', nullptr);
    return AstarSearch(state, goalBoard);
}
