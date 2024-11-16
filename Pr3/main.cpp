#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <cmath>
using namespace std;

const int MAX_SIZE = 100;

const vector<string> names = {"Aberystwyth", "Brighton", "Edinburgh", "Exeter", "Glasgow", "Inverness", "Liverpool", "London", "Newcastle", "Nottingham", "Oxford", "Stratford"};

int n;
double graph[MAX_SIZE][MAX_SIZE];

int random(int from, int to) // inclusive : exclusive
{
    return rand() % (to - from) + from;
}

void printVector(vector<int> &v)
{
    cout << '[';
    for (int i = 0; i < v.size(); i++)
    {
        cout << v[i];
        if (i < v.size() - 1)
            cout << ' ';
    }
    cout << "]\n";
}

vector<pair<double, double>> pos;

struct Chromosome
{
    vector<int> chromosome;
    double fitness;
    Chromosome()
    {
    }
    Chromosome(const vector<int> &chromosome)
    {
        this->chromosome = chromosome;
        this->fitness = evaluate();
    }
    double evaluate()
    {
        // evaluating hamiltonian path, not cycle
        double sum = 0;
        for (int i = 0; i < n; i++)
        {
            // cout << chromosome[i] << " -> " << "(" << pos[chromosome[i]].first << ", " << pos[chromosome[i]].second << ")\n";
        }
        for (int i = 0; i < n - 1; i++)
        {
            sum += graph[chromosome[i]][chromosome[(i + 1)]];
            // cout << "(" << pos[chromosome[i]].first << ", " << pos[chromosome[i]].second << ")" << " to " << "(" << pos[chromosome[i + 1]].first << ", " << pos[chromosome[i + 1]].second << ")\n";
            // cout << names[chromosome[i]] << " to " << names[chromosome[i + 1]] << " " << graph[chromosome[i]][chromosome[(i + 1)]] << '\n';
        }
        this->fitness = sum;
        return sum;
    }
    void mutate()
    {
        // 2-opt operator
        int pos1 = random(0, n - 1);
        int pos2 = random(pos1 + 1, n);
        while (pos1 == 0 && pos2 == n - 1)
        {
            pos1 = random(0, n - 1);
            pos2 = random(pos1 + 1, n);
        }
        if (pos1 == 0)
        {
            int a = chromosome[pos1];
            int c = chromosome[pos2];
            int d = chromosome[pos2 + 1];
            if (graph[c][d] > graph[a][d])
            {
                // cout << "down by " << graph[c][d] - graph[a][d] << '\n';
                // cout << this->fitness << "\t";
                // this->evaluate();
                // cout << this->fitness << '\n';
                // cout << "start mutate!\n";
                // cout << a << " with " << c << "-" << d << '\n';
                for (int i = 0; i < pos2 + 1; i++)
                {
                    if (i >= pos2 - i)
                        break;
                    swap(chromosome[i], chromosome[pos2 - i]);
                }
            }
            return;
        }
        if (pos2 == n - 1)
        {
            int a = chromosome[pos1 - 1];
            int b = chromosome[pos1];
            int d = chromosome[pos2];
            if (graph[a][b] > graph[a][d])
            {
                // cout << "down by " << graph[a][b] - graph[a][d] << '\n';
                // cout << this->fitness << "\t";
                // this->evaluate();
                // cout << this->fitness << '\n';
                // cout << "end mutate!\n";
                // cout << a << '-' << b << " with " << d << '\n';
                for (int i = pos1; i < pos2 + 1; i++)
                {
                    if (i >= pos2 + pos1 - i)
                        break;
                    swap(chromosome[i], chromosome[pos2 + pos1 - i]);
                }
            }
            return;
        }
        int a = chromosome[pos1 - 1];
        int b = chromosome[pos1];
        int c = chromosome[pos2];
        int d = chromosome[pos2 + 1];

        if (graph[a][b] + graph[c][d] >
            graph[a][c] + graph[b][d])
        {
            // cout << "down by " << graph[a][b] + graph[c][d] - graph[a][c] - graph[b][d] << '\n';
            // cout << this->fitness << "\t";
            // this->evaluate();
            // cout << this->fitness << '\n';
            // cout << "inner mutate!\n";
            // cout << a << '-' << b << " with " << c << '-' << d << '\n';
            // cout << "positions: " << pos1 - 1 << "-" << pos1 << " -> " << pos2 << '-' << pos2 + 1 << '\n';
            // printVector(chromosome);
            for (int i = pos1; i < pos2 + 1; i++)
            {
                if (i >= pos2 + pos1 - i)
                    break;
                // cout << "swapping " << chromosome[i] << " " << chromosome[pos2 + pos1 - i] << '\n';
                swap(chromosome[i], chromosome[pos2 + pos1 - i]);
            }

            // cout << "before: " << this->fitness << '\n';
            this->evaluate();
            // cout << "after: " << this->fitness << '\n';
            // printVector(this->chromosome);
        }
    }
};

void showChromosomes(vector<Chromosome> &v)
{
    for (int x : v[0].chromosome)
    {
        cout << "(" << pos[x].first << ", " << pos[x].second << ") - ";
    }
    cout << '\n';
    for (Chromosome &x : v)
    {
        printVector(x.chromosome);
        cout << x.fitness << " fitness\n";
    }
    cout << '\n';
}

bool operator<(const Chromosome &a, const Chromosome &b)
{
    return a.fitness < b.fitness;
}

int population_size = 500;
int parents_size = 50;
int couples_count = 20;
int generations = 100;

vector<Chromosome> chromosomes;

Chromosome random_chromosome(int n)
{
    std::vector<int> vec(n);
    for (int i = 0; i < n; ++i)
        vec[i] = i;

    for (int i = n - 1; i > 0; --i)
    {
        int d = rand() % (i + 1);
        swap(vec[i], vec[d]);
    }
    return Chromosome(vec);
}

void evaluate_all(vector<Chromosome> &chromosomes)
{
    for (Chromosome &x : chromosomes)
        x.evaluate();
}

void naturalSelection()
{
    sort(chromosomes.begin(), chromosomes.end());
    // cout << "--------------------------------\n";
    // showChromosomes(chromosomes);
}

void mutate_all(vector<Chromosome> &children)
{
    for (Chromosome &x : children)
    {
        x.mutate();
        x.mutate();
    }
}

vector<Chromosome> reproduce(const Chromosome &a, const Chromosome &b)
{
    // Cycle reproduce
    // cout << "Parent 1: ";
    // for (int x : a.chromosome)
    //     cout << x << ' ';
    // cout << " -----> " << a.fitness << '\n';
    // cout << '\n';
    // cout << "Parent 2: ";
    // for (int x : b.chromosome)
    //     cout << x << ' ';
    // cout << " -----> " << b.fitness << '\n';
    // cout << '\n';

    vector<int> a_inv(n), b_inv(n);
    for (int i = 0; i < n; i++)
    {
        a_inv[a.chromosome[i]] = i;
        b_inv[b.chromosome[i]] = i;
    }

    // printVector(a_inv);
    // printVector(b_inv);

    vector<int> child1(n, -1), child2(n, -1);

    bool fromParent1 = true;
    int child1Write = 0;
    int toFill = n;
    while (toFill > 0)
    {

        // printVector(child1);
        // printVector(child2);
        if (fromParent1)
        {
            child1[child1Write] = a.chromosome[child1Write];
            child2[child1Write] = b.chromosome[child1Write];
            child1Write = a_inv[b.chromosome[child1Write]];
            toFill--;
            if (child1[child1Write] >= 0)
            {
                fromParent1 = !fromParent1;
                while (toFill && child1[child1Write] >= 0)
                {
                    child1Write = (child1Write + 1) % n;
                }
            }
        }
        else
        {
            child1[child1Write] = b.chromosome[child1Write];
            child2[child1Write] = a.chromosome[child1Write];
            child1Write = a_inv[b.chromosome[child1Write]];
            toFill--;
            if (child2[child1Write] >= 0)
            {
                fromParent1 = !fromParent1;
                while (toFill && child1[child1Write] >= 0)
                {
                    child1Write = (child1Write + 1) % n;
                }
            }
        }
    }

    return {child1, child2};
}

vector<Chromosome> reproduce()
{
    double sumFitness = 0;
    vector<double> prefix = vector<double>(parents_size);
    for (int i = 0; i < parents_size; i++)
        sumFitness += 1 / (double)chromosomes[i].fitness;
    for (int i = 0; i < parents_size; i++)
        prefix[i] = 1 / (double)chromosomes[i].fitness / (double)sumFitness;
    // cout << "Prefixes\n";
    // for (double x : prefix)
    //     cout << x << ' ';
    vector<Chromosome> children;
    for (int i = 0; i < couples_count; i++)
    {
        // double rnum1 = ((double)rand() / RAND_MAX);
        // std::vector<double>::iterator it1 = std::upper_bound(prefix.begin(), prefix.end(), rnum1);

        // double rnum2 = ((double)rand() / RAND_MAX);
        // std::vector<double>::iterator it2 = std::upper_bound(prefix.begin(), prefix.end(), rnum2);

        // cout << "Combining " << it1 - prefix.begin() << " and " << it2 - prefix.begin() << '\n';
        // printVector(chromosomes[it1 - prefix.begin()].chromosome);
        // printVector(chromosomes[it2 - prefix.begin()].chromosome);

        int p1 = rand() % parents_size;
        int p2 = rand() % parents_size;

        // cout << "Combining " << p1 << " and " << p2 << '\n';

        // printVector(chromosomes[p1].chromosome);
        // printVector(chromosomes[p2].chromosome);

        vector<Chromosome> t = reproduce(chromosomes[p1], chromosomes[p2]);

        // cout << "Children:";
        // for (Chromosome &c : t)
        //     printVector(c.chromosome);

        // cout << '\n';

        children.insert(children.end(), t.begin(), t.end());
    }

    return children;
}

double dist(pair<double, double> &a, pair<double, double> &b)
{
    return sqrt((a.first - b.first) * (a.first - b.first) + (a.second - b.second) * (a.second - b.second));
}

int main()
{
    srand(time(NULL));
    // cout << "Enter size of and adjacency symmetric matrix: ";
    // cin >> n;
    // for (int i = 0; i < n; ++i)
    //     for (int j = 0; j < n; ++j)
    //         cin >> graph[i][j];

    cout << "Enter mode: ";
    int mode;
    cin >> mode;

    cout << mode << '\n';

    if (mode == 0)
    {
        double x, y;
        n = 0;
        while (!cin.eof())
        {
            cin >> x >> y;
            pos.push_back({x, y});
            ++n;
        }
    }
    else
    {
        n = mode;
        int x, y;
        for (int i = 0; i < mode; ++i)
        {
            x = rand() % 100 - 50;
            y = rand() % 100 - 50;
            cout << x << '\t' << y << '\n';
            pos.push_back({x, y});
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            graph[i][j] = dist(pos[i], pos[j]);
            // cout << graph[i][j] << ' ';
        }
        // cout << '\n';
    }

    for (int i = 0; i < population_size; i++)
        chromosomes.push_back(random_chromosome(n));

    showChromosomes(chromosomes);

    int s = generations;

    while (s-- > 0)
    {
        cout << "\n----\nGeneration: " << generations - s << "\n----\n"
             << '\n';
        for (int x : chromosomes[0].chromosome)
        {
            cout << "(" << pos[x].first << ", " << pos[x].second << ") - ";
        }
        cout << '\n';
        for (Chromosome c : chromosomes)
            cout << c.fitness << ' ';
        cout << '\n';
        vector<Chromosome> children = reproduce();
        mutate_all(children);
        // cout << "Mutated to:\n";
        // showChromosomes(children);
        evaluate_all(children);

        // cout << "Children fitness: ";
        // for (Chromosome c : children)
        //     cout << c.fitness << ' ';
        // cout << '\n';

        chromosomes.insert(chromosomes.end(), children.begin(), children.end());
        naturalSelection();

        vector<Chromosome>::const_iterator first = chromosomes.begin();
        vector<Chromosome>::const_iterator last = chromosomes.begin() + population_size;
        chromosomes = vector<Chromosome>(first, last);
    }

    cout << "\nSolution found: ";
    for (int i = 0; i < n; i++)
    {
        cout << chromosomes[0].chromosome[i] << " -> ";
    }
    cout << chromosomes[0].chromosome[0] << '\n';
    cout << "Cost: " << chromosomes[0].fitness << '\n';

    return 0;
}