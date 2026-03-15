// This code is used to generate complement of a bipartite graph

#include <bits/stdc++.h>
using namespace std;

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

int first_fit(vector<int> order, int n, vector<vector<int>> &Graph, vector<int> &which_color)
{

    int max_color = 0;
    for (int adding : order)
    {
        set<int> clours;

        // colour list
        for (int j : Graph[adding])
        {
            if (which_color[j - 1] != 0)
            {
                clours.insert(which_color[j - 1]);
            }
        }

        // color assignment
        for (int color = 1; color <= n; color++)
        {
            if (clours.count(color) == 0)
            {
                which_color[adding - 1] = color;
                max_color = max(max_color, color);
                break;
            }
        }
    }
    return max_color;
}

int func(int n, vector<vector<int>> &Graph)
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

    cout << "order : ";
    for (int x : worst_order)
        cout << x << " ";
    cout << "\n";

    cout << "color : ";
    for (int x : worstcolor)
        cout << x << " ";
    cout << "\n";

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

int graphColoring(const vector<vector<int>> &graph, int m, int n)
{
    vector<int> color(n + 1, 0);

    if (!graphColoringUtil(1, graph, color, m, n))
    {
        cout << "No feasible solution exists for " << m << " colors\n";
        return 0;
    }

    cout << "Vertex colors: ";
    for (int i = 1; i <= n; i++)
        cout << color[i] << " ";
    cout << endl;

    unordered_set<int> uniqueColors;
    for (int i = 1; i <= n; i++)
        uniqueColors.insert(color[i]);

    return uniqueColors.size();
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

int main()
{

    short mode;

    cout << "1.Random case ";
    cout << "2.Tight Example";
    cout << "Enter mode : ";
    cin >> mode;

    // genearate a Bipartite
    int n1 = 5;
    int n2 = 5;
    int k = 2;

    int n = (mode == 1) ? (n1 + n2) : (4 * k);
    vector<vector<int>> g = (mode == 1) ? genBipartiteComplement(n1, n2, 0.4) : genTightExample(k);

    // print the generated graph
    for (int i = 1; i <= n; i++)
    {
        cout << i << " : ";
        for (int x : g[i])
            cout << x << " ";
        cout << "\n";
    }

    // worst FF
    int worst = func(n, g);
    cout << "worst: " << worst << "\n";

    // OPT
    for (int i = 1;; i++)
    {
        int x = graphColoring(g, i, n);
        if (x)
        {
            cout << "OPT " << x << "\n";
            break;
        }
    }

    return 0;
}
