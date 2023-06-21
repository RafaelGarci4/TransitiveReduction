#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

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

// Cria o fecho transitivo de um grafo
Graph transitive_closure(const Graph& G) {
    Graph closure = G;

    // Verifica se há um caminho entre todos os pares de vértices
    for (const auto& node_successors : G) {
        const string& node = node_successors.first;

        for (const auto& other_node_successors : G) {
            const string& other_node = other_node_successors.first;

            // Verifica se o vértice atual não é o mesmo e se não há uma aresta direta entre eles
            if (node != other_node && !closure.at(node).count(other_node) && hasPath(G, node, other_node)) {
                closure[node].insert(other_node);  // Adiciona uma aresta entre os vértices se houver um caminho
            }
        }
    }

    return closure;
}

// Reduz a transitividade de um grafo
Graph transitive_reduction(const Graph& G) {
    Graph reduced_graph = G;

    // Itera sobre todos os nós e seus sucessores
    for (const auto& node_successors : G) {
        const string& node = node_successors.first;
        const unordered_set<string>& successors = node_successors.second;

        vector<string> successorsVec(successors.begin(), successors.end());

        // Compara cada par de sucessores
        for (size_t i = 0; i < successorsVec.size(); i++) {
            for (size_t j = 0; j < successorsVec.size(); j++) {
                if (i != j) {
                    const string& succ1 = successorsVec[i];
                    const string& succ2 = successorsVec[j];

                    // Verifica se há um caminho entre os sucessores e se há uma aresta direta entre eles
                    if (hasPath(G, succ1, succ2) && reduced_graph[node].count(succ2) > 0) {
                        reduced_graph[node].erase(succ2);  // Remove a aresta entre os sucessores se houver um caminho
                    }
                }
            }
        }
    }

    return reduced_graph;
}

// Reduz a transitividade de um grafo baseada em caminhamento
Graph transitive_reduction_by_walk(const Graph& G) {
    Graph reduced_graph = G;

    for (const auto& node_successors : G) {
        const string& node = node_successors.first;
        const unordered_set<string>& successors = node_successors.second;

        vector<string> successorsVec(successors.begin(), successors.end());

        for (size_t i = 0; i < successorsVec.size(); i++) {
            for (size_t j = 0; j < successorsVec.size(); j++) {
                if (i != j) {
                    const string& succ1 = successorsVec[i];
                    const string& succ2 = successorsVec[j];

                    if (hasPath(G, succ1, succ2)) {
                        reduced_graph[node].erase(succ2);
                    }
                }
            }
        }
    }

    return reduced_graph;
}

// Imprime o grafo
void print_graph(const Graph& G) {
    for (const auto& node_successors : G) {
        const string& node = node_successors.first;
        const unordered_set<string>& successors = node_successors.second;

        cout << "Vertice " << node << " -> ";
        for (const string& succ : successors) {
            cout << succ << " ";  // Imprime os sucessores do nó
        }
        cout << endl;
    }
}

// Compara dois grafos
bool compare_graphs(const Graph& graph1, const Graph& graph2) {
    if (graph1.size() != graph2.size()) {
        return false;
    }

    for (const auto& node_successors : graph1) {
        const string& node = node_successors.first;
        const unordered_set<string>& successors1 = node_successors.second;
        const unordered_set<string>& successors2 = graph2.at(node);

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
    Graph reduced_graph = transitive_reduction_by_walk(graph);

 // Imprime o grafo com a redução de transitividade
    cout << "Grafo com Redução de Transitividade:" << endl;
   print_graph(reduced_graph);
   cout << endl;

   // Calcula e imprime o fecho transitivo do grafo original
    cout << "Fecho Transitivo do Grafo Original:" << endl;
    Graph closure = transitive_closure(graph);
    print_graph(closure);
    cout << endl;

    // Calcula e imprime o fecho transitivo do grafo com redução
    cout << "Fecho Transitivo do Grafo com Redução de Transitividade:" << endl;
    Graph reduced_closure = transitive_closure(reduced_graph);
    print_graph(reduced_closure);
    cout << endl;

    // Compara os fechos transitivos
    if (compare_graphs(closure, reduced_closure)) {
        cout << "Os fechos transitivos são iguais. A redução foi feita corretamente." << endl;
    } else {
        cout << "Os fechos transitivos são diferentes. A redução foi feita de forma incorreta." << endl;
    }

    return 0;
}
