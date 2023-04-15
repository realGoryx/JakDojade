#include <iostream>
#include <cctype>
#include <cstring>
using namespace std;

const int MAXN = 10000;
const int INF = 1e9;

int adj[MAXN][MAXN]; // adjacency matrix representation
int dist[MAXN]; // distance array to store the minimum distance from source to all vertices
bool visited[MAXN]; // visited array to mark the visited vertices

bool isValid(int x, int y, int w, int h) {
    return x >= 0 && x < h && y >= 0 && y < w;
}

char* findCityName(char** country, int x, int y, int w, int h) {
    int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    char* cityName = (char*) malloc( sizeof(char));

    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (isValid(nx, ny, w, h) && isalpha(country[nx][ny])) {
            for(int j = ny; j >= 0; j--){
                if(country[nx][j] == '.' || country[nx][j] == '#'){
                    int startCityName = j +1;
                    for(int k = startCityName, l = 0; k < w; k++, l++){
                        if(isalpha(country[nx][k])){
                            cityName = (char*)realloc(cityName, l + 1 * sizeof(char));
                            cityName[l] = country[nx][k];
                            cout<< "city: " << cityName << "row: " <<  nx << "l: " << l << "k: " << k << "len: " << strlen(cityName) << std::endl;
                        }
                    }
                    // trim(cityName);
                    return cityName;
                }
            }
        }
    }
    return nullptr;
}

int** prepareAdjTable(char** country, int numberOfCities, int w, int h){
    int** adjTable = (int**) malloc(numberOfCities * sizeof(int*));
    for (int i = 0; i < numberOfCities; i++) {
        adjTable[i] = (int*) malloc(numberOfCities * sizeof(int));
    }

    for(int i =0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if(country[i][j] == '*') {
                auto city = findCityName(country, i, j, w, h);
                cout << city << std::endl;
            }

        }
    }


    return adjTable;
}

void dijkstra(int start) {
    // Initialize distance array and visited array
    for (int i = 0; i < MAXN; i++) {
        dist[i] = INF;
        visited[i] = false;
    }

    // Distance to starting node is 0
    dist[start] = 0;

    // Traverse all vertices
    for (int i = 0; i < MAXN; i++) {
        // Find the vertex with the minimum distance from the starting node
        int minDist = INF;
        int minVertex;
        for (int j = 0; j < MAXN; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                minVertex = j;
            }
        }

        // Mark the selected vertex as visited
        visited[minVertex] = true;

        // Update the distances of the adjacent vertices
        for (int j = 0; j < MAXN; j++) {
            if (!visited[j] && adj[minVertex][j] > 0) {
                int newDist = dist[minVertex] + adj[minVertex][j];
                if (newDist < dist[j]) {
                    dist[j] = newDist;
                }
            }
        }
    }
}

int main() {
    int w = 0;
    int h = 0;

    cin>> w >> h;

    char** country = (char**) malloc(h * sizeof(char*));
    for (int i = 0; i < h; i++) {
        country[i] = (char*) malloc(w * sizeof(char));
    }
    int numberCities = 0;

    for(int i =0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            cin >> country[i][j];
            if(country[i][j] == '*')
                numberCities++;
        }
    }

    auto adjTable = prepareAdjTable(country, numberCities, w, h);

    // Read input
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < m; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        adj[u][v] = w;
        adj[v][u] = w; // If the graph is undirected
    }
    int start;
    cin >> start;

    // Run Dijkstra's algorithm
    dijkstra(start);

    // Output the shortest distances
    for (int i = 0; i < n; i++) {
        cout << dist[i] << " ";
    }
    cout << endl;

    return 0;
}