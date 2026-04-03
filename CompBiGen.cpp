#include <bits/stdc++.h>
#include <omp.h>
using namespace std;
using namespace chrono;

vector<vector<int>> genBipartiteComplement(int n1, int n2, double p)
{
    int n = n1 + n2;
    vector<vector<bool>> adj(n + 1, vector<bool>(n + 1, false));

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> prob(0.0, 1.0);

    for (int u = 1; u <= n1; u++)
    {
        for (int v = n1 + 1; v <= n; v++)
        {
            if (prob(gen) < p)
            {
                adj[u][v] = true;
                adj[v][u] = true;
            }
        }
    }

    vector<vector<int>> g(n + 1);
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (adj[i][j] == 0 && i != j)
                g[i].push_back(j);
        }
    }

    return g;
}

// Tight example
vector<vector<int>> genTightExample(int k)
{
    int n = 4 * k;
    vector<vector<bool>> adj(n + 1, vector<bool>(n + 1, false));

    // A-B complete
    for (int i = 1; i <= k; i++)
    {
        for (int j = k + 1; j <= 2 * k; j++)
        {
            adj[i][j] = 1;
            adj[j][i] = 1;
        }
    }
    // B-C complete
    for (int i = k + 1; i <= 2 * k; i++)
    {
        for (int j = 2 * k + 1; j <= 3 * k; j++)
        {
            adj[i][j] = 1;
            adj[j][i] = 1;
        }
    }
    // C-D complete
    for (int i = 2 * k + 1; i <= 3 * k; i++)
    {
        for (int j = 3 * k + 1; j <= 4 * k; j++)
        {
            adj[i][j] = 1;
            adj[j][i] = 1;
        }
    }

    vector<vector<int>> g(n + 1);
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (adj[i][j] == 0 && i != j)
                g[i].push_back(j);
        }
    }

    return g;
}

int first_fit(vector<int> &order, int n, vector<vector<int>> &Graph, vector<int> &which_color)
{
    int max_color = 0;
    which_color.assign(n, 0);
    for (int adding : order)
    {
        vector<bool> used(n + 1, false);
        // colour list
        for (int j : Graph[adding])
        {
            if (which_color[j - 1] != 0)
            {
                used[which_color[j - 1]] = true;
            }
        }
        // color assignment
        for (int color = 1; color <= n; color++)
        {
            if (!used[color])
            {
                which_color[adding - 1] = color;
                max_color = max(max_color, color);
                break;
            }
        }
    }
    return max_color;
}

int WorstOrderFF(int n, vector<vector<int>> &Graph, bool print = true)
{
    vector<int> v;
    for (int i = 1; i <= n; i++)
    {
        v.push_back(i);
    }

    vector<int> worst_order;
    vector<int> worstcolor;

    int worst = 0;
    do
    {

        vector<int> which_color(n, 0);
        int ans = first_fit(v, n, Graph, which_color);

        if (worst < ans)
        {
            worst_order = v;
            worstcolor = which_color;
            worst = ans;
        }

    } while (next_permutation(v.begin(), v.end()));

    if (print)
    {
        cout << "order : ";
        for (int x : worst_order)
            cout << x << " ";
        cout << "\n";

        cout << "color : ";
        for (int x : worstcolor)
            cout << x << " ";
        cout << "\n";
    }
    return worst;
}



int WorstOrderFF_Parallel(int n, vector<vector<int>> &Graph, bool print = true)
{
    int worst = 0;
    vector<int> worst_order;
    vector<int> worstcolor;

#pragma omp parallel
    {
        int local_worst = 0;
        vector<int> local_order;
        vector<int> local_color;

#pragma omp for schedule(dynamic)
        for (int start = 1; start <= n; start++)
        {
            
            vector<int> v;
            v.push_back(start);

            for (int i = 1; i <= n; i++)
                if (i != start)
                    v.push_back(i);

            do
            {
                if (v[0] != start)
                    continue;

                vector<int> which_color(n, 0);
                int ans = first_fit(v, n, Graph, which_color);

                if (ans > local_worst)
                {
                    local_worst = ans;
                    local_order = v;
                    local_color = which_color;
                }

            } while (next_permutation(v.begin() + 1, v.end()));
        }

#pragma omp critical
        {
            if (local_worst > worst)
            {
                worst = local_worst;
                worst_order = local_order;
                worstcolor = local_color;
            }
        }
    }

    if (print)
    {
        cout << "order : ";
        for (int x : worst_order)
            cout << x << " ";
        cout << "\n";

        cout << "color : ";
        for (int x : worstcolor)
            cout << x << " ";
        cout << "\n";
    }

    return worst;
}

// Optimal coloring
bool isSafe(int v, const vector<vector<int>> &graph, const vector<int> &color, int c)
{
    for (int neighbor : graph[v])
    {
        if (color[neighbor] == c)
        {
            return false;
        }
    }
    return true;
}

bool graphColoringUtil(int v, const vector<vector<int>> &graph, vector<int> &color, int m, int n)
{
    if (v > n)
    {
        return true; // all vertices colored
    }

    for (int c = 1; c <= m; c++)
    {
        if (isSafe(v, graph, color, c))
        {
            color[v] = c;

            if (graphColoringUtil(v + 1, graph, color, m, n))
                return true;

            color[v] = 0; // backtrack
        }
    }

    return false;
}

bool colorableWith(int n, const vector<vector<int>> &g, int m, vector<int> &color)
{
    color.assign(n + 1, 0);
    return graphColoringUtil(1, g, color, m, n);
}

int opt(int n, vector<vector<int>> &g, bool print, vector<int> &color, int upperBound = 0)
{
    if (upperBound == 0)
        upperBound = n;

    // Binary search for minimum number of colors
    int lo = 1, hi = upperBound, ans = upperBound;
    
    while (lo <= hi)
    {
        int mid = lo + (hi - lo) / 2;
        if (colorableWith(n, g, mid, color))
        {
            if(print) cout<<"Colorable with "<<mid<<"\n";
            ans = mid;
            hi = mid - 1;
        }
        else
        {
            if(print) cout<<"Not colorable with "<<mid<<"\n";
            lo = mid + 1;
        }
    }

    // Produce the final coloring with the optimal count
    colorableWith(n, g, ans, color);

    if (print)
    {
        cout << "\nOPT number of colors " << ans << "\n";
        cout << "OPT COLORING: ";
        for (int i = 1; i <= n; i++)
            cout << color[i] << " ";
        cout << "\n";
    }
    return ans;
}

int main()
{

    short mode1, mode2;

    cout << "1.Random case \n";
    cout << "2.Tight Example \n";
    cout << "Enter mode : ";
    cin >> mode1;

    // genearate a Bipartite
    int n1 = 8;
    int n2 = 8;
    int k = 4;

    int n = (mode1 == 1) ? (n1 + n2) : (4 * k);
    vector<vector<int>> g = (mode1 == 1) ? genBipartiteComplement(n1, n2, 0.4) : genTightExample(k);

    // print the generated graph
    for (int i = 1; i <= n; i++)
    {
        cout << i << " : ";
        for (int x : g[i])
            cout << x << " ";
        cout << "\n";
    }

    cout << "1. To compare FF speed wrt to OPT\n";
    cout << "2. To verify 3/2 ratio\n";
    cout << "Enter mode: ";
    cin >> mode2;

    if (mode2 == 1)
    {

        // FF time v/s OPT coloring
        // Getting a random permutation
        vector<int> order;
        vector<int> which_color;
        for (int i = 1; i <= n; i++)
            order.push_back(i);

        random_device rd;
        mt19937 gen(rd());

        shuffle(order.begin(), order.end(), gen);

        cout<<"order : ";
        for(int num:order) cout<<num<<" ";
        cout<<"\n";

        // ---- First Fit Timing ----
        auto start_ff = high_resolution_clock::now();
        int ffcolor = first_fit(order, n, g, which_color);
        auto end_ff = high_resolution_clock::now();
        cout << "coloring : ";

        for (int color : which_color)
            cout << color << " ";
        cout << "\n";
        cout << ffcolor << " colors used\n";

        auto ff_time = duration_cast<nanoseconds>(end_ff - start_ff);

        cout << "First Fit time: " << ff_time.count() << " ns\n";

        // ---- OPT Timing ----
        auto start_opt = high_resolution_clock::now();

        int optColors = opt(n, g, false, which_color);

        auto end_opt = high_resolution_clock::now();

        auto opt_time = duration_cast<nanoseconds>(end_opt - start_opt);
        cout << "\nOPT number of colors " << optColors << "\n";
        cout << "OPT COLORING: ";
        for (int i = 1; i <= n; i++)
            cout << which_color[i] << " ";
        cout << "\n";

        cout << "OPT time: " << opt_time.count() << " ns\n";
    }
    else if (mode2 == 2)
    {
        // Showing 3/2 for complement of Bipartite
        // worst FF

        if (mode1 == 1)
        {

            int worst = WorstOrderFF_Parallel(n, g);
            cout << "worstFF: " << worst << "\n";
            // OPT
            cout << "Optimal case\n";
            vector<int> color;
            int optColoring = opt(n, g, true, color);
            cout << "\nratio " << (float)worst / optColoring << "\n";
        }

        else if (mode1 == 2)
        {
            vector<int> order;

            for (int j = k + 1; j <= 2 * k; j++)
                order.push_back(j);
            for (int j = 3 * k + 1; j <= 4 * k; j++)
                order.push_back(j);
            for (int i = 2 * k + 1; i <= 3 * k; i++)
                order.push_back(i);
            for (int i = 1; i <= k; i++)
                order.push_back(i);
            vector<int> which_color;
            int ffcolor = first_fit(order, n, g, which_color);

            cout << "WorstOrder: ";
            for (int num : order)
                cout << num << ' ';
            cout << '\n';
            cout << "WorstFF_Coloring : ";
            for (int color : which_color)
                cout << color << ' ';
            cout << '\n';
            cout << ffcolor << " colors used\n";

            // OPT
            cout << "Optimal case\n";
            int optColoring = opt(n, g, true, which_color);
            cout << "\nratio " << (float)ffcolor / optColoring << "\n";
        }
    }

    return 0;
}
