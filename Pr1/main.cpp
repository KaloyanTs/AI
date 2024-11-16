#include <iostream>
#include <queue>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <stack>
using namespace std;

struct VectorHash
{
    size_t operator()(const std::vector<int> &v) const
    {
        std::hash<int> hasher;
        size_t seed = 0;
        for (int i : v)
        {
            seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

int zeroPos;
int size;

unordered_set<vector<int>, VectorHash> visited;

enum Move
{
    RIGHT = 1,
    UP = -2,
    LEFT = -1,
    DOWN = 2,
    NO_MOVE = 3,
};

void showVector(vector<int> &v)
{
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
            cout << v[i * size + j] << '\t';
        cout << '\n';
    }
    cout << '\n';
}

bool isGoalState(vector<int> &board)
{
    int c = 1;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            // cout << board[i * size + j] << '^' << c << '\t';
            if (board[i * size + j] == 0 && i * size + j != zeroPos)
                return false;
            if (board[i * size + j] != 0 && board[i * size + j] != c)
                return false;
            if (board[i * size + j] != 0)
                ++c;
        }
    }
    return true;
}

struct State
{
    State *parent;
    int cost;
    vector<int> state;
    int holeRow, holeCol;
    int heuristic;
    Move parentToMe;

    void print()
    {
        cout << "State:\n";
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
                cout << state[i * size + j] << '\t';
            cout << '\n';
        }
        cout << "Hole in: " << holeRow << ',' << holeCol
             << "\nCost: " << cost << "\t Heuristic: "
             << heuristic << boolalpha << '\n'
             << "goal check: " << isGoalState(state) << '\n';
    }
};

struct CompareState
{
    bool operator()(const State *l, const State *r) const
    {
        return l->cost + l->heuristic > r->cost + r->heuristic;
    }
};

bool available(State *state, Move m)
{
    return (state->holeRow + (m / 2)) >= 0 &&
           (state->holeCol + (m % 2)) >= 0 &&
           (state->holeRow + (m / 2)) < size &&
           (state->holeCol + (m % 2)) < size;
}

int heuristic(vector<int> &board)
{
    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            // cout << board[i * size + j] << '\n';
            if (board[i * size + j] == 0)
                continue;
            int linearPos = zeroPos < board[i * size + j]
                                ? board[i * size + j]
                                : board[i * size + j] - 1;
            // cout << "Expecting on pos: " << linearPos / size << ',' << linearPos % size << '\n';
            // cout << "Actual pos: " << i << ',' << j << '\n';
            sum += abs(i - linearPos / size);
            sum += abs(j - linearPos % size);
            // cout << board[i * size + j] << " contribution is: " << sum << '\n';
        }
    }
    return sum;
}

void showMove(State *parent, int row, int col)
{
    if (parent->holeRow - row == 1)
        cout << "up\n";
    else if (parent->holeRow - row == -1)
        cout << "down\n";
    else if (parent->holeCol - col == 1)
        cout << "left\n";
    else
        cout << "right\n";
}

int inversionsDummy(vector<int> &board)
{
    int res = 0;
    for (int i = 0; i < size * size; ++i)
    {
        if (board[i] == 0)
            continue;
        for (int j = i + 1; j < size * size; ++j)
        {
            if (board[j] == 0)
                continue;
            if (board[i] > board[j])
                ++res;
        }
    }
    return res;
}

bool isSolvable(vector<int> &b, int z)
{
    if (size % 2)
    {
        return inversionsDummy(b) % 2 == 0;
    }
    return (inversionsDummy(b) + (z / size - 1)) % 2 == (zeroPos / size - 1) % 2;
}

void printPriorityQueue(priority_queue<State *, vector<State *>, CompareState> pq)
{
    cout << "==========\nQUEUE:\n";
    while (!pq.empty())
    {
        cout << pq.top()->cost + pq.top()->heuristic << '\n';
        pq.pop();
    }
    std::cout << "==========\n";
}

void A_star(vector<int> &initialState, int zero)
{
    int expanded = 0;
    priority_queue<State *, vector<State *>, CompareState> q;
    int threshold = 0;
    int newThreshold = 0;

    State *init = new State;
    init->state = initialState;
    init->holeRow = zero / size;
    init->holeCol = zero % size;
    init->heuristic = heuristic(initialState);
    init->cost = 0;
    init->parent = nullptr;
    init->parentToMe = NO_MOVE;
    bool begin = false;

    // cout << "Starting from:\n";
    // init.print();

    q.push(init);

    State *init = new State;
    init->state = initialState;
    init->holeRow = zero / size;
    init->holeCol = zero % size;
    init->heuristic = heuristic(initialState);
    init->cost = 0;
    init->parent = nullptr;
    init->parentToMe = NO_MOVE;
    q.push(init);

    while (!q.empty())
    {
        // printPriorityQueue(q);

        State *current = q.top();
        q.pop();
        if (current->cost + current->heuristic > threshold)
        {
            // cout << "Passed threshold\n";
            newThreshold = newThreshold > current->cost + current->heuristic ||
                                   newThreshold == threshold
                               ? current->cost + current->heuristic
                               : newThreshold;
            continue;
        }
        // current->print();
        // showVector(current->state);
        // cout << current->cost << " cost" << '\n';

        if (isGoalState(current->state))
        {
            // cout << "Found solution for cost: " << current->cost << '\n';
            State *i = current;
            stack<State *> path;
            while (i->parent)
            {
                path.push(i);
                i = i->parent;
            }
            int row = i->holeRow;
            int col = i->holeCol;
            cout << path.size() << '\n';
            while (!path.empty())
            {
                showMove(path.top(), row, col);
                // showVector(path.top()->state);
                row = path.top()->holeRow;
                col = path.top()->holeCol;
                path.pop();
            }
            // cout << "Expanded nodes: " << expanded << '\n';
            return;
        }
        expanded++;
        for (Move m : {RIGHT, UP, LEFT, DOWN})
        {
            if (m + current->parentToMe != 0 && available(current, m))
            {
                State *s = new State(*current);
                s->parent = current;
                swap(s->state[s->holeRow * size + s->holeCol],
                     s->state[(s->holeRow + (m / 2)) * size + (s->holeCol + (m % 2))]);
                s->holeRow += (m / 2);
                s->holeCol += (m % 2);
                s->heuristic = heuristic(s->state);
                s->cost += 1;
                s->parentToMe = m;
                q.push(s);
                // cout << "\nMove to:\n";
                // s->print();
            }
        }
    }
    // cout << "No solution with threshold " << threshold << " after " << expanded << " nodes expanded!\n";
    // cout << "new: " << newThreshold << '\n';
}

int main()
{
    int numbers;
    cin >> numbers;
    cin >> zeroPos;
    size = sqrt(numbers + 1);

    zeroPos = (zeroPos + numbers + 1) % (numbers + 1);

    vector<int> state(size * size);

    int z;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
        {
            cin >> state[i * size + j];
            if (state[i * size + j] == 0)
                z = i * size + j;
        }

    if (!isSolvable(state, z))
    {
        cout << -1 << '\n';
        return 0;
    }
    A_star(state, z);

    return 0;
}