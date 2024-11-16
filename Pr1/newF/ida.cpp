#include <iostream>
#include <queue>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <stack>
#include <algorithm>
#include <chrono>
using namespace std;

int zeroPos;
int size;

enum Move
{
    RIGHT = 1,
    UP = -2,
    LEFT = -1,
    DOWN = 2,
    NO_MOVE = 3,
};

std::string toString(int move)
{
    switch (move)
    {
    case RIGHT:
        return "right";
    case UP:
        return "up";
    case LEFT:
        return "left";
    case DOWN:
        return "down";
    default:
        return "unknown";
    }
}

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
    int cost;
    // vector<int> state;
    int holeRow, holeCol;
    int heuristic;
    Move parentToMe;

    void print(vector<int> &board)
    {
        cerr << "State:\n";
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
                cerr << board[i * size + j] << '\t';
            cerr << '\n';
        }
        cerr << "Hole in: " << holeRow << ',' << holeCol
             << "\nCost: " << cost << "\t Heuristic: "
             << heuristic << boolalpha << '\n'
             << "goal check: " << isGoalState(board) << '\n';
    }
};

struct CompareState
{
    bool operator()(const State *l, const State *r) const
    {
        return l->cost + l->heuristic > r->cost + r->heuristic;
    }
};

bool available(State &state, Move m)
{
    return (state.holeRow + (m / 2)) >= 0 &&
           (state.holeCol + (m % 2)) >= 0 &&
           (state.holeRow + (m / 2)) < size &&
           (state.holeCol + (m % 2)) < size;
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
vector<int> path;

int rec(State &current, int threshold, int depth, vector<int> &board)
{
    // current.print(board);
    ++expanded;
    // recursively till solution or above threshold
    // returns found cost over threshold or -1 if solution
    // maybe -2 for no children

    if (isGoalState(board))
    {
        cout << depth << '\n';
        if (current.parentToMe != NO_MOVE)
            path.push_back(-current.parentToMe);
        return -1;
    }
    if (current.cost + current.heuristic > threshold)
        return current.cost + current.heuristic;

    int res = threshold;
    for (Move m : {RIGHT, UP, LEFT, DOWN})
    {
        if (m + current.parentToMe != 0 && available(current, m))
        {
            State s = current;
            swap(board[s.holeRow * size + s.holeCol],
                 board[(s.holeRow + (m / 2)) * size + (s.holeCol + (m % 2))]);
            s.holeRow += (m / 2);
            s.holeCol += (m % 2);
            s.heuristic = heuristic(board);
            s.cost += 1;
            s.parentToMe = m;
            int recRes = rec(s, threshold, depth + 1, board);
            swap(board[s.holeRow * size + s.holeCol],
                 board[(s.holeRow - (m / 2)) * size + (s.holeCol - (m % 2))]);
            if (recRes == -1)
            {
                if (current.parentToMe != NO_MOVE)
                    path.push_back(-current.parentToMe);
                return -1;
            }
            else if (recRes > -2)
                res = res == threshold ? recRes : min(res, recRes);
        }
    }
    return res == threshold ? -2 : res;
}

void IDA_star(vector<int> &initialState, int zero)
{
    State init;
    init.holeRow = zero / size;
    init.holeCol = zero % size;
    init.heuristic = heuristic(initialState);
    init.cost = 0;
    init.parentToMe = NO_MOVE;

    int threshold = init.heuristic;

    do
    {
        threshold = rec(init, threshold, 0, initialState);
    } while (threshold > -1);

    if (threshold == -2)
        cout << "No solution??? something went wrong!!!\n";
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
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    IDA_star(state, z);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    reverse(path.begin(), path.end());
    for (int x : path)
        cout << toString(x) << '\n';

    return 0;
}