// Sebastian Woźniak - 15 puzzle


#include "State.h"
#include <iostream>
#include <bits/stdc++.h>
#include <cmath>
#include <random>

using namespace std;

// says if move [ 0 - 3 ] is possible from position [ 0 - 15 ]
static const int possibleMoves[4][16] = {
        {0, 0, 0,0, 1,1,1,1, 1, 1, 1, 1, 1, 1, 1, 1},//  move up
        {0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1}, //move left
        {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0} ,//move right
        {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0} // move down
};

// Function calculates count of inversions in given state.
int getInversionCount(State *state){
    int inv_count = 0;
    for(int i = 0; i<board_len-1; i++){
        for(int j = i+1; j<board_len; j++){
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
    return 0;
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

/* ------------------------- Random generators ------------------------- */
State* randomStateGenerator(){
    random_device rd{};
    mt19937 mt{rd()};
    static uniform_int_distribution<int> dist;
    dist.param(uniform_int_distribution<int>::param_type(0, 14));
    Board board = 0x0000000000000000ULL;
    auto* initialState = new State(board, 0, 0, -1, nullptr);
    unordered_set<int> added;
    for(int i = 1; i <=15; i++){
        int idx;
        do{
            idx = dist(mt);
        } while (added.find(idx)!=added.end());
        added.insert(idx);
        initialState->setTile(idx, i);
    }
    return initialState;
}

State* randomStateFromGoalGenerator(int k){
    random_device rd{};
    mt19937 mt{rd()};
    static uniform_int_distribution<int> dist;
    dist.param(uniform_int_distribution<int>::param_type(0, 3));
    Board board = 0x123456789abcdef0ULL;
    auto* initialState = new State(board, 0, 0, -1, nullptr);
    int prev = -1;
    for(int i =0; i<k;i++){
        int blank = getBlankPos(initialState);
        char move;
        do{
            move = dist(mt);
        } while (move==prev && !possibleMoves[move][blank]);
        prev = move;
        initialState->move(move, blank);
    }
    return initialState;
}

/* --------------------------------------------------------------*/

// prints the solution
int* printSolution(State* state, long visited, bool print_sol, short last_move, ::clock_t start){
    double time = (double) (::clock() - start) / CLOCKS_PER_SEC;
    cout << "[" << time <<"s]" <<endl;
    State* solution = state;
    unsigned int moves = 0;
    if(last_move != -1) moves++;
    list<short> sol;
    cout << "Visited states: " << visited << endl;
    while(solution->parent != nullptr){
        moves++;
        sol.push_front(solution->parentMove);
        solution = solution->parent;
    }
    sol.push_back(last_move);
    if(print_sol){
        for(auto m : sol){
            if(m==0)cout << "UP, ";
            else if(m==1) cout << "DOWN, ";
            else if(m==2) cout << "LEFT, ";
            else if(m==3) cout << "RIGHT, ";
        }
        cout<< endl;
    }
    cout << "Moves: " << moves << endl;
    return new int[]{static_cast<int>(moves), static_cast<int>(visited) };
}

/* ------------------------- HEURISTICS ------------------------- */

// manhattan distance
unsigned int manhattanHeuristic(State* board){
    unsigned int f_cost = 0;
    for(int i = 0; i<board_len; i++){
        int dest = board->getTile(i);
        if(dest!=0){ f_cost += (int) abs(floor(i/boardWidth) - floor((dest-1)/boardWidth)) + abs((i%boardWidth) - ((dest-1)%boardWidth));}
    }
    return f_cost + board->g_cost;
}

// counts of linear conflicts*2 + manhattandistance
unsigned short int linearConflict(State *board){
    unsigned short int conflicts = 0;
    unsigned short int distance = 0;
    for(int i = 0; i<board_len; i++) {
        int tj = board->getTile(i);
        int row = (i / boardWidth);
        int column = (i % boardWidth);
        // calculate manhattan distance
        if(tj!=0) {
            distance += (int) abs(row - floor((tj - 1) / boardWidth)) + abs(column - (tj - 1) % boardWidth);
            // row conflicts
            for (int tk_idx = i + 1; tk_idx < (row + 1) * boardWidth; tk_idx++) {
                int tk = board->getTile(tk_idx);
                if (tk != 0 && tj > tk && floor((tk - 1) / boardWidth) == row) { conflicts++; }
            }
            //column conflicts
            for (int tk_idx = i + boardWidth; tk_idx < board_len; tk_idx += boardWidth) {
                int tk = board->getTile(tk_idx);
                if (tk != 0 && tj > tk && (tk - 1) % boardWidth == column) { conflicts++; }
            }
        }

    }
    return 2*conflicts + distance + board->g_cost;
}

/* --------------------------------------------------------------*/

struct Hash{
    size_t operator()(const State* state) const{
        return hash<Board>()(state->board);
    }
};  // closedlist hashing

struct Equal{
    bool operator()(const State* obj1, const State* obj2) const{
        return obj1->board == obj2->board;
    }
};  // closedlist compare function

// A* algorithm to find the best solution from initialState to goal
int* AstarSearch(State* initialState, Board goal, bool print_sol){
    if(!isSolvable(initialState)) {
        cout << "Not Solvable!" << endl;
        return nullptr;
    }  // Not solvable
    clock_t starttime = ::clock();
    cout << "Solving..." << endl ;
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
        ++visited;
        if(q->board == goal) { return printSolution(q, visited, print_sol, -1, starttime);}
        int blankPos = getBlankPos(q);

        //-------------------------- generate successors --------------------------
        for(char move=0;move<4;move++){
            if(!possibleMoves[move][blankPos]) continue;   // move impossible
            if(3 != q->parentMove + move){  // is not going back to the parent
                auto* successor = new State(q->board, q->g_cost + 1, 0, move, q);
                successor->move(move, blankPos);
                if(successor->board == goal){   // successor == goal
                    ++visited;
                    closedList.clear();
                    return printSolution(q, visited, print_sol, move, starttime);
                }

                successor->f_cost = linearConflict(successor);

                auto search = closedList.find(successor);
                if (search == closedList.end()) {   // not checked yet
                    closedList.insert(successor);
                    openList.push(successor);
                } else if (successor->f_cost < (*search)->f_cost) { // checked but f_cost is lower
                    openList.push(successor);
                    closedList.erase(*search);
                    closedList.insert(successor);
                } else delete successor;
            }
        }
    }
    return nullptr;
}

int main() {
    unsigned long long goalBoard = 0x123456789abcdef0ULL;
    auto *state = randomStateGenerator();
    state->printBoard();
    AstarSearch(state, goalBoard, true);
}
