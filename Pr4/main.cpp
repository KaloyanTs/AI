#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;

#define ME 'O'
#define YOU 'X'
#define EMPTY '_'

struct TTT
{
    char board[3][3];

    TTT()
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                board[i][j] = EMPTY;
    }

    bool isFinal()
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[i][j] == EMPTY)
                    return false;
        return true;
    }

    int e(char c)
    {
        if (c == ME)
            return 1;
        return -1;
    }

    // 1 - victory, -1 - defeat, 0 - draw, 2 - not final
    int evaluate()
    {
        // cout << "evaluating\n";
        // printBoard();
        for (int i = 0; i < 3; ++i)
        {
            if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != EMPTY)
                return e(board[i][0]);
            if (board[0][i] == board[1][i] && board[2][i] == board[1][i] && board[0][i] != EMPTY)
                return e(board[0][i]);
        }
        if (board[0][0] == board[1][1] && board[2][2] == board[1][1] && board[0][0] != EMPTY)
            return e(board[0][0]);
        if (board[2][0] == board[1][1] && board[1][1] == board[0][2] && board[2][0] != EMPTY)
            return e(board[2][0]);
        if (isFinal())
            return 0;
        return 2;
    }

    vector<pair<int, int>> moves()
    {
        vector<pair<int, int>> res;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[i][j] == EMPTY)
                    res.push_back({i, j});
        return res;
    }

    void printBoard()
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
                cout << board[i][j] << '\t';
            cout << '\n';
        }
    }
};

int maxValue(TTT &state);
int minValue(TTT &state);

pair<int, int> decide(TTT &state)
{
    pair<int, int> bestMove = {-1, -1};
    int m = INT_MIN;
    for (pair<int, int> move : state.moves())
    {
        state.board[move.first][move.second] = ME;
        int r = minValue(state);
        if (m < r)
        {
            m = r;
            bestMove = move;
        }
        state.board[move.first][move.second] = EMPTY;
    }
    return bestMove;
}

int minValue(TTT &state)
{
    int ev = state.evaluate();
    if (ev != 2)
        return ev;
    int v = INT_MAX;
    for (pair<int, int> m : state.moves())
    {
        state.board[m.first][m.second] = YOU;
        v = min(v, maxValue(state));
        state.board[m.first][m.second] = EMPTY;
    }
    return v;
}

int maxValue(TTT &state)
{
    int ev = state.evaluate();
    if (ev != 2)
        return ev;
    int v = INT_MIN;
    for (pair<int, int> m : state.moves())
    {
        state.board[m.first][m.second] = ME;
        v = max(v, minValue(state));
        state.board[m.first][m.second] = EMPTY;
    }
    return v;
}

int maxValueAB(TTT &state, int &alpha, int &beta, pair<int, int> &move, int depth);
int minValueAB(TTT &state, int &alpha, int &beta, int depth);

bool d(TTT &state)
{
    return state.board[0][1] == state.board[1][1] &&
           state.board[1][1] == state.board[2][1] &&
           state.board[0][1] == ME;
}

pair<int, int> decideAB(TTT &state)
{
    int alpha = INT_MIN;
    int beta = INT_MAX;
    pair<int, int> res;
    int v = maxValueAB(state, alpha, beta, res, 0);
    return res;
}

int minValueAB(TTT &state, int &alpha, int &beta, int depth)
{
    int ev = state.evaluate();
    if (ev != 2)
    {
        // if (d(state))
        // {
        //     cout << "end " << ev << ' ' << depth << '\n';
        //     if (ev == 1 && depth < 5)
        //     {
        //         state.printBoard();
        //         cout << "found win in " << depth << "moves!\n";
        //     }
        //     cout << "finally: " << ev * (10 - depth) << '\n';
        // }
        return ev * (10 - depth);
    }
    int v = INT_MAX;
    pair<int, int> move;
    for (pair<int, int> m : state.moves())
    {
        state.board[m.first][m.second] = YOU;
        v = min(v, maxValueAB(state, alpha, beta, move, depth + 1));
        state.board[m.first][m.second] = EMPTY;
        if (v <= alpha)
        {
            // cout << "pruning\n";
            return v;
        }
        beta = min(beta, v);
    }
    return v;
}

int maxValueAB(TTT &state, int &alpha, int &beta, pair<int, int> &move, int depth)
{
    int ev = state.evaluate();
    if (ev != 2)
    {
        // if (d(state))
        // {
        //     cout << "end " << ev << ' ' << depth << '\n';
        //     if (ev == 1 && depth < 5)
        //     {
        //         state.printBoard();
        //         cout << "found win in " << depth << "moves!\n";
        //     }
        //     cout << "finally: " << ev * (10 - depth) << '\n';
        // }
        return ev * (10 - depth);
    }
    int v = INT_MIN;
    for (pair<int, int> m : state.moves())
    {
        state.board[m.first][m.second] = ME;
        // if (d(state))
        // {
        //     cout << "explore " << depth << '\n';
        //     state.printBoard();
        // }
        int p = minValueAB(state, alpha, beta, depth + 1);
        // if (d(state))
        // {
        //     cout << "res is " << p << '\n';
        // }
        if (p > v)
        {
            v = p;
            move = m;
        }
        state.board[m.first][m.second] = EMPTY;
        if (v >= beta)
        {
            // cout << "pruning\n";
            return v;
        }
        alpha = max(alpha, v);
    }
    return v;
}

// Now computer plays first
int main()
{
    TTT init;
    int x, y;
    system("cls");
    while (init.evaluate() == 2)
    {
        system("cls");
        init.printBoard();
        cout << "Your turn:\n";
        cin >> x >> y;
        init.board[x - 1][y - 1] = YOU;
        pair<int, int> m = decideAB(init);
        init.board[m.first][m.second] = ME;
        cout << "I am playing on " << m.first + 1 << ',' << m.second + 1 << '\n';
        // if (init.evaluate() != 2)
        //     break;
        // init.printBoard();
        // cout << "Your turn:\n";
        // cin >> x >> y;
        // init.board[x - 1][y - 1] = YOU;
    }
    system("cls");
    init.printBoard();

    int res = init.evaluate();
    if (res == 0)
    {
        cout << "Well played, it's a draw!\n";
    }
    else if (res == -1)
    {
        cout << "Internal server error\n";
    }
    else
        cout << "Easy win for me.\n";

    return 0;
}