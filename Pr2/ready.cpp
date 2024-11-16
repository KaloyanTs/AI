#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

class NQueensProblem
{
private:
    int size;
    vector<int> queens;
    vector<int> conflictsByRow;
    vector<int> conflictsByPositiveDiagonal;
    vector<int> conflictsByNegativeDiagonal;

public:
    NQueensProblem(int);
    void calculateConflicts();
    bool hasConflicts();
    int getConflicts(int);
    int getPositiveDiagonal(int, int);
    int getNegativeDiagonal(int, int);
    void initialize();
    void moveQueen(int, int);
    int getMaxConflictsCol();
    int getMinConflictsRow(int);
    void solve();
    void visualizeSolution();
};

NQueensProblem::NQueensProblem(int n)
{
    size = n;
    queens = vector<int>(size);
    conflictsByRow = vector<int>(size);
    conflictsByPositiveDiagonal = vector<int>(2 * size - 1);
    conflictsByNegativeDiagonal = vector<int>(2 * size - 1);

    srand(time(0));
}

// Get negative diagonal index of queen
int NQueensProblem::getNegativeDiagonal(int row, int col)
{
    return size + col - row - 1;
}

// Get positive diagonal index of queen
int NQueensProblem::getPositiveDiagonal(int row, int col)
{
    return col + row;
}

// Initialize queens and conflicts
void NQueensProblem::initialize()
{
    // One queen per column
    for (int col = 0; col < size; ++col)
    {
        queens[col] = col;
    }

    // Shuffle queens
    for (int i = 0; i < size - 1; i++)
    {
        int j = i + rand() % (size - i);
        std::swap(queens[i], queens[j]);
    }

    fill(conflictsByRow.begin(), conflictsByRow.end(), 0);
    fill(conflictsByPositiveDiagonal.begin(), conflictsByPositiveDiagonal.end(), 0);
    fill(conflictsByNegativeDiagonal.begin(), conflictsByNegativeDiagonal.end(), 0);
}

// Calculate all conflicts in the board
void NQueensProblem::calculateConflicts()
{
    for (int val = 0; val < size; ++val)
    {
        int queenRow = queens[val];
        ++conflictsByRow[queenRow];
        ++conflictsByPositiveDiagonal[getPositiveDiagonal(queenRow, val)];
        ++conflictsByNegativeDiagonal[getNegativeDiagonal(queenRow, val)];
    }
}

// Return column with max conflicts in the board
int NQueensProblem::getMaxConflictsCol()
{
    int maxConflicts = 0;
    int currConflicts = 0;
    vector<int> candidates(size);
    int candicatesCount = 0;

    for (int col = 0; col < size; ++col)
    {
        currConflicts = getConflicts(col);

        if (currConflicts > maxConflicts)
        {
            maxConflicts = currConflicts;
            candicatesCount = 0;
            candidates[candicatesCount++] = col;
        }
        else if (currConflicts == maxConflicts)
        {
            candidates[candicatesCount++] = col;
        }
    }

    if (candicatesCount > 1)
    {
        int candidateIndex = rand() % candicatesCount;
        return candidates[candidateIndex];
    }
    return candidates[0];
}

// Return the row with min conflicts for given column
int NQueensProblem::getMinConflictsRow(int col)
{
    int minConflicts = INT_MAX;
    int currConflicts = 0;
    vector<int> candidates(size);
    int candidatesCount = 0;

    for (int row = 0; row < size; ++row)
    {
        if (row != queens[col])
        {
            currConflicts = conflictsByRow[row] + conflictsByPositiveDiagonal[getPositiveDiagonal(row, col)] + conflictsByNegativeDiagonal[getNegativeDiagonal(row, col)];

            if (currConflicts < minConflicts)
            {
                minConflicts = currConflicts;
                candidatesCount = 0;
                candidates[candidatesCount++] = row;
            }
            else if (currConflicts == minConflicts)
            {
                candidates[candidatesCount++] = row;
            }
        }
    }

    if (candidatesCount > 1)
    {
        int candidateIndex = rand() % candidatesCount;
        return candidates[candidateIndex];
    }
    return candidates[0];
}

int NQueensProblem::getConflicts(int col)
{
    int onRow = conflictsByRow[queens[col]];
    int onPositiveDiagonal = conflictsByPositiveDiagonal[getPositiveDiagonal(queens[col], col)];
    int onNegativeDiagonal = conflictsByNegativeDiagonal[getNegativeDiagonal(queens[col], col)];

    if (onRow > 0)
        onRow--;
    if (onPositiveDiagonal > 0)
        onPositiveDiagonal--;
    if (onNegativeDiagonal > 0)
        onNegativeDiagonal--;

    return onRow + onPositiveDiagonal + onNegativeDiagonal;
}

void NQueensProblem::moveQueen(int row, int col)
{
    int currentRow = queens[col];
    --conflictsByRow[currentRow];
    --conflictsByPositiveDiagonal[getPositiveDiagonal(currentRow, col)];
    --conflictsByNegativeDiagonal[getNegativeDiagonal(currentRow, col)];

    queens[col] = row;
    ++conflictsByRow[row];
    ++conflictsByPositiveDiagonal[getPositiveDiagonal(row, col)];
    ++conflictsByNegativeDiagonal[getNegativeDiagonal(row, col)];
}

bool NQueensProblem::hasConflicts()
{
    for (int val = 0; val < size * 2 - 1; ++val)
    {
        if (val < this->size)
        {
            if (this->conflictsByRow[val] > 1)
                return true;
        }
        if (this->conflictsByPositiveDiagonal[val] > 1)
            return true;
        if (this->conflictsByNegativeDiagonal[val] > 1)
            return true;
    }
    return false;
}

void NQueensProblem::solve()
{
    int iterations = 3 * size;

    initialize();
    calculateConflicts();

    for (int i = 0; i < iterations; ++i)
    {
        int col = getMaxConflictsCol();
        int row = getMinConflictsRow(col);

        moveQueen(row, col);
        if (!hasConflicts())
        {
            // cout << "Number of moves = " << i << endl;
            return;
        }
    }

    if (hasConflicts())
    {
        // Restart
        // std::cout << "Restarted!" << endl;
        solve();
    }
}

void NQueensProblem::visualizeSolution()
{
    cout << '[';
    for (int i = 0; i < size; ++i)
    {
        cout << queens[i];
        if (i < size - 1)
            cout << ", ";
    }
    cout << ']';
    cout << '\n';
    return;

    for (int row = 0; row < size; ++row)
    {
        for (int col = 0; col < size; ++col)
        {
            if (queens[col] == row)
            {
                cout << "*";
            }
            else
            {
                cout << "_";
            }
        }
        cout << "\n";
    }
}

int main()
{
    int N;
    cout << "N = ";
    cin >> N;
    if (N < 4 && N > 1)
    {
        // cout << " Solution for N < 4 doesn't exist! Please try with higher number. " << endl;
        cout << -1 << endl;
        return 0;
    }

    NQueensProblem problem(N);

    clock_t start = clock();
    problem.solve();
    if (1)
    {
        problem.visualizeSolution();
    }

    clock_t end = clock();
    double duration = (end - start) / (double)CLOCKS_PER_SEC;
    // printf("Execution time: %f secs.\n", duration);
}