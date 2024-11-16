#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <algorithm>
using namespace std;

int n;

vector<int> rows;
vector<int> colConflicts;
vector<int> diag;
vector<int> rev_diag;

void print_vector(const std::vector<int> &vec)
{
    cout << '[';
    for (int i = 0; i < vec.size(); ++i)
    {
        std::cout << vec[i];
        if (i < vec.size() - 1)
            cout << ' ';
    }
    cout << ']';
    cout << '\n';
}

vector<int> random_permutation(int n)
{
    std::vector<int> vec(n, 0);
    for (int i = 0; i < n; ++i)
        vec[i] = i;

    for (int i = n - 1; i > 0; --i)
    {
        int d = rand() % (i + 1);
        swap(vec[i], vec[d]);
    }
    return vec;
}

std::vector<int> minConflictsByCol(int col)
{
    vector<int> conflicts(n);
    for (int i = 0; i < n; ++i)
        conflicts[i] = rows[i] + diag[i - col + n - 1] + rev_diag[i + col];

    // cout << "Conflicts in col " << col << ": ";
    // print_vector(conflicts);

    std::vector<int> result;

    int min_value = *min_element(conflicts.begin(), conflicts.end());

    for (int i = 0; i < conflicts.size(); ++i)
        if (conflicts[i] == min_value)
            result.push_back(i);

    return result;
}

std::vector<int> argmaxs(const std::vector<int> &vec)
{
    std::vector<int> result;
    if (vec.empty())
        return result;

    int min_value = *std::max_element(vec.begin(), vec.end());

    for (int i = 0; i < vec.size(); ++i)
        if (vec[i] == min_value)
            result.push_back(i);

    return result;
}

std::vector<int> argmins(const std::vector<int> &vec)
{
    std::vector<int> result;
    if (vec.empty())
        return result;

    int min_value = *std::min_element(vec.begin(), vec.end());

    for (int i = 0; i < vec.size(); ++i)
        if (vec[i] == min_value)
            result.push_back(i);

    return result;
}

void printBoard(vector<int> &queenRowByCols)
{
    vector<int> queenColByRow = vector<int>(n);
    for (int i = 0; i < n; i++)
        queenColByRow[queenRowByCols[i]] = i;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            cout << (queenColByRow[i] == j ? '*' : '_');
        cout << '\n';
    }
}

int main()
{
    srand(time(NULL));
    cin >> n;
    int iter = 0;

    if (n == 2 || n == 3)
    {
        cout << -1 << '\n';
        return 0;
    }

    rows = vector<int>(n, 1);
    diag = vector<int>(2 * n - 1, 0);
    rev_diag = vector<int>(2 * n - 1, 0);
    colConflicts = vector<int>(n, 0);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    vector<int>
        rowPositionsByCol = random_permutation(n);
    for (int i = 0; i < n; i++)
    {
        // row: i    col: rowPositionByCol[i]
        diag[rowPositionsByCol[i] - i + n - 1]++;
        rev_diag[i + rowPositionsByCol[i]]++;
    }
    for (int i = 0; i < n; ++i)
    {
        colConflicts[i] = rows[rowPositionsByCol[i]] +
                          diag[rowPositionsByCol[i] - i + n - 1] +
                          rev_diag[i + rowPositionsByCol[i]] - 3;
    }

    int limit = 0;
    while (1)
    {
        // cout << "Iteration: " << ++limit << '\n';

        /*print_vector(rowPositionsByCol);
        print_vector(rows);
        print_vector(diag);
        print_vector(rev_diag);
        print_vector(colConflicts);*/

        vector<int> maxConflictedQueens = argmaxs(colConflicts);

        // cout << "Max conflicted cols: ";
        // print_vector(maxConflictedQueens);

        int chosenCol = maxConflictedQueens[rand() % maxConflictedQueens.size()];

        // cout << "Chosen: " << chosenCol << '\n';

        if (colConflicts[chosenCol] == 0)
        {
            // cout << "Found solution!\n";
            // printBoard(rowPositionsByCol);

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

            // cout << '[';
            // for (int i = 0; i < n; i++)
            // {
            //     cout << rowPositionsByCol[i];
            //     if (i < n - 1)
            //         cout << ", ";
            // }
            // cout << "]\n";

            std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
            break;
        }

        rows[rowPositionsByCol[chosenCol]]--;
        diag[rowPositionsByCol[chosenCol] - chosenCol + n - 1]--;
        rev_diag[rowPositionsByCol[chosenCol] + chosenCol]--;

        vector<int> chosenBest = minConflictsByCol(chosenCol);

        // cout << "Min Confilcts in col " << chosenCol << ": ";
        // print_vector(minConflictsByCol(chosenCol));

        int chosenRow = chosenBest[rand() % chosenBest.size()];

        // cout << "Chosen row: " << chosenRow << '\n';

        rowPositionsByCol[chosenCol] = chosenRow;

        rows[rowPositionsByCol[chosenCol]]++;
        diag[rowPositionsByCol[chosenCol] - chosenCol + n - 1]++;
        rev_diag[rowPositionsByCol[chosenCol] + chosenCol]++;

        for (int i = 0; i < n; ++i)
        {
            colConflicts[i] = rows[rowPositionsByCol[i]] +
                              diag[rowPositionsByCol[i] - i + n - 1] +
                              rev_diag[i + rowPositionsByCol[i]] - 3;
        }
    }
    // cout << "No solution found!\n";

    // print_vector(colConflicts);

    return 0;
}