#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
using namespace std;

class Graph
{
    int V;                   // Número de vértices
    vector<vector<int>> adj; // Matriz de adjacência

public:
    Graph(int V);
    Graph(vector<vector<int>> vec);
    void addEdge(int v, int w);

    vector<vector<int>> transitiveClosure();

    vector<vector<int>> transitiveReduction();

    void printGraph();
};

Graph::Graph(vector<vector<int>> vec)
{
    this->V = vec.size();
    this->adj = vector<vector<int>>(vec.size(), vector<int>(vec[0].size()));
    for (int i = 0; i < vec.size(); i++)
    {
        this->adj.push_back(vec[i]);
    }
}

Graph::Graph(int V)
{
    this->V = V;
    adj = vector<vector<int>>(V, vector<int>(V, false));
}


vector<vector<int>> readAdjacencyMatrix(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Não foi possível abrir o arquivo." << endl;
        return {};
    }

    int numVertices, numEdges;
    file >> numVertices >> numEdges;

    vector<vector<int>> adjacencyMatrix(numVertices, vector<int>(numVertices, 0));

    for (int i = 0; i < numEdges; i++) {
        int v, w;
        file >> v >> w;
        adjacencyMatrix[v - 1][w - 1] = 1;  // Ajuste para indexação baseada em 0
    }

    file.close();

    return adjacencyMatrix;
}


vector<vector<int>> removeLoops(const vector<vector<int>> &matrix)
{
    int n = matrix.size();
    vector<vector<int>> result = matrix;

    for (int i = 0; i < n; i++)
    {
        if (matrix[i][i] == 1)
        {
            result[i][i] = 0; // Remove o loop na posição da diagonal principal
        }
    }

    return result;
}

void removeCycleDFS(vector<vector<int>> &adjMatrix, vector<vector<int>> &removedEdges, int startVertex, vector<bool> &visited, stack<int> &path)
{
    visited[startVertex] = true;
    path.push(startVertex);

    for (int i = 0; i < adjMatrix.size(); i++)
    {
        if (adjMatrix[startVertex][i] == 1)
        {
            if (!visited[i])
            {
                removeCycleDFS(adjMatrix, removedEdges, i, visited, path);
            }
            else
            {
                // Remove a aresta que fecha o ciclo
                adjMatrix[startVertex][i] = 0;
                removedEdges[startVertex][i] = 1;
            }
        }
    }

    path.pop();
    visited[startVertex] = false;
}

tuple<vector<vector<int>>, vector<vector<int>>> removeCycles(const vector<vector<int>> &adjMatrix)
{
    vector<vector<int>> updatedMatrix = adjMatrix;
    vector<vector<int>> removedEdges(adjMatrix.size(), vector<int>(adjMatrix.size(), 0));
    int V = adjMatrix.size();
    vector<bool> visited(V, false);
    stack<int> path;

    for (int i = 0; i < V; i++)
    {
        if (!visited[i])
        {
            removeCycleDFS(updatedMatrix, removedEdges, i, visited, path);
        }
    }

    return make_tuple(updatedMatrix, removedEdges);
}

vector<vector<int>> unionAdjacencyMatrices(const vector<vector<int>> &matrix1, const vector<vector<int>> &matrix2)
{
    if (matrix1.size() != matrix2.size() || matrix1[0].size() != matrix2[0].size())
    {
        // As matrizes devem ter as mesmas dimensões
        cout << "As matrizes de adjacência têm dimensões diferentes." << endl;
        return {};
    }

    int n = matrix1.size();
    vector<vector<int>> result(n, vector<int>(n, 0));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            // Faz a união dos valores correspondentes nas duas matrizes
            result[i][j] = matrix1[i][j] || matrix2[i][j];
        }
    }

    return result;
}

vector<vector<int>> calculateTransitiveClosure(const vector<vector<int>> &adjMatrix)
{
    int V = adjMatrix.size();
    vector<vector<int>> closure = adjMatrix;

    for (int k = 0; k < V; k++)
    {
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                closure[i][j] = closure[i][j] || (closure[i][k] && closure[k][j]);
            }
        }
    }

    return closure;
}

vector<vector<int>> convertToIncidenceMatrix(const vector<vector<int>> &adjMatrix)
{
    int V = adjMatrix.size();
    vector<vector<int>> incidenceMatrix(V, vector<int>(V, 0));

    for (int i = 0; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            if (adjMatrix[i][j] == 1)
            {
                incidenceMatrix[i][j] = 1;
            }
        }
    }

    return incidenceMatrix;
}

vector<vector<int>> compareMatrices(const vector<vector<int>> &matrix1, const vector<vector<int>> &matrix2)
{
    int rows = matrix1.size();
    int cols = matrix1[0].size();
    vector<vector<int>> result(rows, vector<int>(cols, 0));

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (matrix1[i][j] == 1 && matrix2[i][j] == 0)
            {
                result[i][j] = 1;
            }
        }
    }

    return result;
}

void PrintMatrix(const vector<vector<int>> &matrix)
{
    int rows = matrix.size();
    int cols = matrix[0].size();

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void Graph::addEdge(int v, int w)
{
    adj[v - 1][w - 1] = true; // Ajuste para indexação baseada em 1
}

vector<vector<int>> Graph::transitiveClosure()
{
    vector<vector<int>> reach = adj;
    for (int k = 0; k < V; k++)
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                reach[i][j] = reach[i][j] || (reach[i][k] && reach[k][j]);
    return reach;
}

vector<vector<int>> Graph::transitiveReduction()
{
    vector<vector<int>> closure = transitiveClosure();
    vector<vector<int>> reduction = adj;
    for (int i = 0; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            if (reduction[i][j])
            {
                for (int k = 0; k < V; k++)
                {
                    if (k != i && k != j && closure[i][k] && closure[k][j] &&
                        !closure[j][k])
                    {
                        reduction[i][j] = false;
                        break;
                    }
                }
            }
        }
    }
    return reduction;
}

static vector<vector<int>> MatrixMultiply(vector<vector<int>> m1, vector<vector<int>> m2)
{

    int rows_m1 = m1.size();
    int cols_m1 = m1[0].size();
    int cols_m2 = m2[0].size();

    vector<vector<int>> result(rows_m1, vector<int>(cols_m2, 0));

    for (int i = 0; i < rows_m1; ++i)
    {
        for (int j = 0; j < cols_m2; ++j)
        {
            for (int k = 0; k < cols_m1; ++k)
            {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }

    return result;
}

void Graph::printGraph()
{
    cout << "   ";
    for (int j = 0; j < V; j++)
    {
        cout << j + 1 << " ";
    }
    cout << endl;

    for (int i = 0; i < V; i++)
    {
        cout << i + 1 << "  ";
        for (int j = 0; j < V; j++)
        {
            cout << adj[i][j] << " ";
        }
        cout << endl;
    }
}

int main()
{
    vector<vector<int>> adjacencyMatrix = readAdjacencyMatrix("graphArtigo.txt"); //sess 0
    vector<vector<int>> teste{{0,1,0,0,0,0},{1,0,1,1,0,0},{0,1,0,1,1,1},{0,1,1,0,0,1},{0,0,1,0,0,1},{0,0,1,1,1,0}};
    cout << "grafo com loop :\n"
         << endl;
    PrintMatrix(adjacencyMatrix);

    vector<vector<int>> nloops = removeLoops(adjacencyMatrix); // G1 & G2 sess 2
    cout << "\n sem loops :\n"
         << endl;
    PrintMatrix(nloops); //

    tuple<vector<vector<int>>, vector<vector<int>>> dagTup = removeCycles(nloops); // sess 1
    vector<vector<int>> dag = get<0>(dagTup);
    vector<vector<int>> arestasRemovidas = get<1>(dagTup);
    cout << "\nG2 :\n"
         << endl;
    PrintMatrix(dag); // G2 == M1
    cout << "\n ciclos removidos :\n"
         << endl;
    PrintMatrix(arestasRemovidas); // G2 == M1

    vector<vector<int>> gt2 = calculateTransitiveClosure(dag); //sess 3 pt 1
    cout << "\nGT2:\n"
         << endl;
    PrintMatrix(gt2);

    vector<vector<int>> m2 = convertToIncidenceMatrix(gt2); //sess 3 pt 2
    cout << "\n M2:\n"
         << endl;
    PrintMatrix(m2);

    vector<vector<int>> m3 = MatrixMultiply(dag, m2); //sess 4
    cout << "\n M3:\n"
         << endl;
    PrintMatrix(m3);

    vector<vector<int>> gt1 = compareMatrices(dag, m3); // G1 & G3 sess 5
    cout << "\n Gt1:\n"
         << endl;
    PrintMatrix(gt1);

    vector<vector<int>> gt = unionAdjacencyMatrices(arestasRemovidas, gt1); //sess 6
    cout << "\n final :\n"
         << endl;
    PrintMatrix(gt);
    return 0;
}
