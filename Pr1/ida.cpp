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

int expanded = 0;

int rec(State *current, int threshold, int depth)
{
    ++expanded;
    current->print();
    // recursively till solution or above threshold
    // returns found cost over threshold or -1 if solution
    // maybe -2 for no children

    if (isGoalState(current->state))
    {
        cout << depth << '\n';
        cout << current->parentToMe << '\n';
        return -1;
    }
    if (current->cost + current->heuristic > threshold)
        return current->cost + current->heuristic;

    int res = threshold;
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
            int recRes = rec(s, threshold, depth + 1);
            if (recRes == -1)
            {
                cout << s->parentToMe << '\n';
                return -1;
            }
            else if (recRes > -2)
                res = res == threshold ? recRes : min(res, recRes);
        }
    }
    delete current;
    return -2;
}

void IDA_star(vector<int> &initialState, int zero)
{
    State *init = new State;
    init->state = initialState;
    init->holeRow = zero / size;
    init->holeCol = zero % size;
    init->heuristic = heuristic(initialState);
    init->cost = 0;
    init->parent = nullptr;
    init->parentToMe = NO_MOVE;

    int threshold = 0;

    cout << "init\n";

    do
    {
        threshold = rec(init, threshold, 0);
        cout << threshold << " is the new threshold\n";
    } while (threshold > -1);

    if (threshold == -2)
        cout << "No solution??? something went wrong!!!\n";

    cout << "End!\n";
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
    IDA_star(state, z);

    return 0;
}