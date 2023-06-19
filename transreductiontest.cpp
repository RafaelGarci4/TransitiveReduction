#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

using namespace std;

using Graph = unordered_map<string, unordered_set<string>>;

// Cria um grafo com base nos vértices e arestas fornecidos
Graph create_graph(const vector<string>& vertices, const vector<pair<string, string>>& edges) {
    Graph G;

    // Adiciona os vértices ao grafo
    for (const string& vertex : vertices) {
        G[vertex] = {};
    }

    // Adiciona as arestas ao grafo
    for (const auto& edge : edges) {
        const string& u = edge.first;
        const string& v = edge.second;
        G[u].insert(v);
    }

    return G;
}

// Verifica se há um caminho entre dois vértices no grafo
bool hasPath(const Graph& G, const string& start, const string& end) {
    unordered_set<string> visited;
    queue<string> q;
    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        string curr = q.front();
        q.pop();

        if (curr == end) {
            return true;  // Se o vértice atual é o destino, retorna verdadeiro (há um caminho)
        }

        for (const string& neighbor : G.at(curr)) {
            if (visited.find(neighbor) == visited.end()) {
                q.push(neighbor);  // Adiciona o vizinho à fila para visitar posteriormente
                visited.insert(neighbor);  // Marca o vizinho como visitado
            }
        }
    }

    return false;  // Se nenhum caminho foi encontrado, retorna falso
}

// Reduz a transitividade de um grafo
// Reduz a transitividade de um grafo
void reduceTransitivity(Graph& graph) {
    for (auto& node_successors : graph) {
        const string& node = node_successors.first;
        unordered_set<string>& successors = graph[node];

        unordered_set<string> transitively_reachable;

        for (const string& successor : successors) {
            transitively_reachable.insert(successor);
            const unordered_set<string>& successors_of_successor = graph[successor];

            for (const string& transitive_successor : successors_of_successor) {
                transitively_reachable.insert(transitive_successor);
            }
        }

        for (auto& node_successors : graph) {
            unordered_set<string>& successors_of_successor = node_successors.second;

            for (const string& transitive_successor : transitively_reachable) {
                successors_of_successor.erase(transitive_successor);
            }
        }
    }
}


// Imprime o grafo
void print_graph(const Graph& G) {
    for (const auto& node_successors : G) {
        const string& node = node_successors.first;
        const unordered_set<string>& successors = node_successors.second;

        cout << "Sucessores de " << node << ": ";
        for (const string& succ : successors) {
            cout << succ << " ";
        }
        cout << endl;
    }
}

// Lê o grafo a partir de um arquivo de texto
pair<vector<string>, vector<pair<string, string>>> read_graph_from_file(const string& filename) {
    vector<string> vertices;
    vector<pair<string, string>> edges;

    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            vector<string> tokens;
            stringstream ss(line);
            string token;
            while (ss >> token) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 2) {
                if (tokens[0] == "V") {
                    vertices.push_back(tokens[1]);
                } else if (tokens[0] == "E") {
                    if (tokens.size() >= 3) {
                        edges.emplace_back(tokens[1], tokens[2]);
                    }
                }
            }
        }

        file.close();
    }

    return make_pair(vertices, edges);
}

// Compara os fechos transitivos diretos de dois grafos
bool compare_transitive_closures(const Graph& closure1, const Graph& closure2) {
    if (closure1.size() != closure2.size()) {
        return false;
    }

    for (const auto& node_successors : closure1) {
        const string& node = node_successors.first;
        const unordered_set<string>& successors1 = node_successors.second;
        const unordered_set<string>& successors2 = closure2.at(node);

        if (successors1.size() != successors2.size()) {
            return false;
        }

        for (const string& succ : successors1) {
            if (successors2.find(succ) == successors2.end()) {
                return false;
            }
        }
    }

    return true;
}

// Função de fecho transitivo
Graph transitive_closure(const Graph& G) {
    Graph closure = G;

    for (const auto& node_successors : G) {
        const string& node = node_successors.first;
        unordered_set<string> visited;
        queue<string> q;
        visited.insert(node);
        q.push(node);

        while (!q.empty()) {
            string curr = q.front();
            q.pop();

            if (closure.find(curr) == closure.end()) {
                continue;
            }

            for (const string& neighbor : closure.at(curr)) {
                if (visited.find(neighbor) == visited.end()) {
                    closure[node].insert(neighbor);
                    visited.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }
    }

    return closure;
}

int main() {
    // Lê o grafo a partir do arquivo de texto
    string filename = "graph.txt";
    pair<vector<string>, vector<pair<string, string>>> graphData = read_graph_from_file(filename);
    vector<string> vertices = graphData.first;
    vector<pair<string, string>> edges = graphData.second;

    // Cria o grafo original
    Graph graph = create_graph(vertices, edges);

    // Imprime o grafo original
    cout << "Grafo Original:" << endl;
    print_graph(graph);
    cout << endl;

    // Aplica a redução de transitividade
    reduceTransitivity(graph);

    // Imprime o grafo com a redução de transitividade
    cout << "Grafo com Redução de Transitividade:" << endl;
    print_graph(graph);
    cout << endl;

    // Calcula o fecho transitivo direto do grafo original
    Graph closure_original = transitive_closure(graph);

    // Calcula o fecho transitivo direto do grafo com a redução
    Graph closure_reduced = transitive_closure(graph);

    // Imprime o fecho transitivo direto do grafo original
    cout << "Fecho Transitivo Direto do Grafo Original:" << endl;
    print_graph(closure_original);
    cout << endl;

    // Imprime o fecho transitivo direto do grafo com a redução
    cout << "Fecho Transitivo Direto do Grafo com Redução:" << endl;
    print_graph(closure_reduced);
    cout << endl;

    // Compara os fechos transitivos diretos
    if (compare_transitive_closures(closure_original, closure_reduced)) {
        cout << "Os fechos transitivos diretos são iguais. A redução foi feita corretamente." << endl;
    } else {
        cout << "Os fechos transitivos diretos são diferentes. A redução foi feita de forma incorreta." << endl;
    }

    return 0;
}
