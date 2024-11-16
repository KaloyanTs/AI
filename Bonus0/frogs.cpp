#include <iostream>
#include <stack>
#include <string>
#include <chrono>
using namespace std;

enum JUMP
{
    LEFT = 1,
    DOUBLE_LEFT = 2,
    RIGHT = -1,
    DOUBLE_RIGHT = -2,
    UNDO_LEFT = 6,
    UNDO_DOUBLE_LEFT = 7,
    UNDO_RIGHT = 4,
    UNDO_DOUBLE_RIGHT = 3,
};

int sign(int x)
{
    return x > 0 ? 1 : -1;
}

int main()
{
    int n;
    cin >> n;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    int forward_after_hole = 0;
    int holePosition = n;
    bool solved = false;
    string state = "";
    for (int i = 0; i < n; ++i)
        state += '>';

    state += '_';

    for (int i = 0; i < n; ++i)
        state += '<';

    stack<JUMP> st;
    st.push(RIGHT);

    stack<JUMP> solution;

    while (!st.empty())
    {
        JUMP current = st.top();
        // current is transofrmation to be applied
        st.pop();

        if (current > 2)
        {
            // undo transformation
            JUMP to_undo = (JUMP)((int)current - 5);
            state[holePosition] = (to_undo < 0 ? '>' : '<');
            state[holePosition - to_undo] = '_';
            holePosition -= to_undo;
            switch (to_undo)
            {
            case RIGHT:
                forward_after_hole -= 1;
                break;
            case DOUBLE_RIGHT:
                forward_after_hole -= (state[holePosition - 1] == '>' ? 2 : 1);
                break;
            case DOUBLE_LEFT:
                forward_after_hole += (state[holePosition + 1] == '>' ? 1 : 0);
            }
            if (solved)
                solution.push(to_undo);
            continue;
        }

        if (solved)
            continue;

        state[holePosition] = (current < 0 ? '>' : '<');
        state[holePosition + current] = '_';
        holePosition += current;
        switch (current)
        {
        case RIGHT:
            forward_after_hole += 1;
            break;
        case DOUBLE_RIGHT:
            forward_after_hole += (state[holePosition + 1] == '>' ? 2 : 1);
            break;
        case DOUBLE_LEFT:
            forward_after_hole -= (state[holePosition - 1] == '>' ? 1 : 0);
        }

        // cout << state << ':' << forward_after_hole << '\n';

        if (forward_after_hole == n && holePosition == n)
            solved = true;
        st.push((JUMP)(current + 5));
        if (holePosition > 0 && state[holePosition + RIGHT] == '>')
            st.push(RIGHT);
        if (holePosition > 1 && state[holePosition + DOUBLE_RIGHT] == '>')
            st.push(DOUBLE_RIGHT);
        if (holePosition < 2 * n && state[holePosition + LEFT] == '<')
            st.push(LEFT);
        if (holePosition < 2 * n - 1 && state[holePosition + DOUBLE_LEFT] == '<')
            st.push(DOUBLE_LEFT);
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    state = "";
    for (int i = 0; i < n; ++i)
        state += '>';
    state += '_';
    for (int i = 0; i < n; ++i)
        state += '<';

    cout << state << '\n';

    holePosition = n;

    while (!solution.empty())
    {
        JUMP j = solution.top();
        solution.pop();
        state[holePosition] = (j < 0 ? '>' : '<');
        state[holePosition + j] = '_';
        holePosition += j;
        // cout << state << '\n';
    }

    return 0;
}