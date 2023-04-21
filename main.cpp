// Sebastian Woźniak - 15 puzzle

#include <iostream>
#include <bits/stdc++.h>
#include <cmath>

#define BITS_PER_TILE  4
#define MASK 0x00000000000000000fULL
#define Board unsigned long long

using namespace std;

int boardWidth = 4;


class State{
public:
    Board board;
    unsigned int g_cost;
    unsigned int f_cost;
    short parentMove;
    State* parent;

    void setTile(int idx, int value);
    int getTile(int idx) const;
    void move(short mv, int blankspot);

    State(Board board, unsigned int g, unsigned int f, short parentMove, State *parent){
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

// Moves the blank spot
// 0 - move up
// 1 - move down
// 2 - move left
// 3 - move right
void State::move(short mv, int blankspot){
    if(mv == 0){
        int val = getTile(blankspot-boardWidth);
        setTile(blankspot, val);
        setTile(blankspot - boardWidth, 0x0000);
    }
    else if(mv == 1){
        int val = getTile(blankspot+boardWidth);
        setTile(blankspot, val);
        setTile(blankspot+boardWidth, 0x0000);
    }
    else if(mv == 2){
        int val = getTile(blankspot-1);
        setTile(blankspot, val);
        setTile(blankspot-1, 0x0000);
    }
    else if(mv == 3){
        int val = getTile(blankspot+1);
        setTile(blankspot, val);
        setTile(blankspot+1, 0x0000);
    }

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

// manhattan distance
unsigned int manhattanHeuristic(State* board){
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


// says if move [ 0 - 3 ] is possible from position [ 0 - 15 ]
static const int possibleMoves[4][16] = {
        {0, 0, 0,0, 1,1,1,1, 1, 1, 1, 1, 1, 1, 1, 1},        //  move up
        {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0}, // move down
        {0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1}, //move left
        {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0} //move right
};
const short gobackmove[] = {1, 0, 3, 2};    // oposite move to move [ 0 - 3 ]

// A* algorithm to find the best solution from initialState to goal
// returns -1 if not solvable, else 0
int AstarSearch(State* initialState, Board goal){
    if(!isSolvable(initialState)) {
        cout << "Not Solvable!" << endl;
        return -1;
    }
    cout << "Solving..." << endl;
    auto cmp = [](State* left, State* right){return left->f_cost > right->f_cost;};

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
        if(q->board == goal) {
            solutionFound(q, visited);
            return 0;
        }
        int blankPos = getBlankPos(q);
        if(blankPos == -1){
            return -1;
        }
        //-------------------------- generate successors --------------------------
        for(short move=0;move<4;move++){
            if(!possibleMoves[move][blankPos]) continue;   // move impossible
            if(gobackmove[q->parentMove] != move){
                auto* successor = new State(q->board, q->g_cost + 1, 0, move, q);
                successor->move(move, blankPos);

                if(successor->board == goal){
                    solutionFound(q, visited+1);
                    return 0;
                }

                successor->f_cost = manhattanHeuristic(successor);

                auto search = closedList.find(successor);
                if (search == closedList.end()) {
                    closedList.insert(successor);
                    openList.push(successor);
                } else if (successor->f_cost < (*search)->f_cost) {
                    openList.push(successor);
                    closedList.insert(successor);
                    closedList.erase(*search);
                } else delete successor;
            }
        }
    }
    return -1;
}



int main() {
    Board goalBoard = 0x123456789abcdef0ULL;
    Board randomState = 0x123457896bcdefa0ULL;
    auto* state = new State(randomState, 0, 0, -1, nullptr);

    AstarSearch(state, goalBoard);
}
