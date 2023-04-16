#include <iostream>
#include <cctype>
#include <cstring>
using namespace std;

const int MAXN = 10000;
const int INF = 1e9;
const int MAX_VERTICES = 10000;

struct Point{
    int x;
    int y;
};

struct City {
    char* name;
    Point cords;
};

struct Queue{
    int front;
    int rear;
    Point data[MAX_VERTICES];
};

void initQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

bool isEmpty(Queue *q) {
    return q->front == -1;
}

void enqueue(Queue *q, Point p) {
    if (q->rear == MAX_VERTICES - 1) {
        cout<<"Queue overflow\n";
        return;
    }
    if (q->front == -1) {
        q->front = 0;
    }
    q->rear++;
    q->data[q->rear] = p;
}

Point dequeue(Queue *q) {
    if (isEmpty(q)) {
        cout << "Queue underflow\n";
        Point invalid_point = {-1, -1};
        return invalid_point;
    }
    Point p = q->data[q->front];
    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
    } else {
        q->front++;
    }
    return p;
}

int adj[MAXN][MAXN]; // adjacency matrix representation
int dist[MAXN]; // distance array to store the minimum distance from source to all vertices
bool visited[MAXN]; // visited array to mark the visited vertices

bool isValid(int x, int y, int w, int h) {
    return x >= 0 && x < h && y >= 0 && y < w;
}

char* findCityName(char** country, int x, int y, int w, int h) {
    int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    char* cityName = (char*)malloc( w + 1 * sizeof(char));

    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (isValid(nx, ny, w, h) && isalpha(country[nx][ny])) {
            for (int j = ny; j >= 0; j--) {
                if (country[nx][j] == '.' || country[nx][j] == '#') {
                    int startCityName = j + 1;
                    int charPos = 0;
                    for (int k = startCityName; k < w; k++) {
                        if (isalpha(country[nx][k])) {
                            cityName[charPos] = country[nx][k];
                            charPos++;
                        }
                    }
                    cityName[charPos] = '\0';
                    return cityName;
                }
            }
        }
    }
    return nullptr;
}

int bfs(char** country, int startX, int startY, int targetX, int targetY, int w, int h) {
    Point directions[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    bool visited[h][w];
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            visited[i][j] = false;
        }
    }
    Queue q;
    initQueue(&q);
    enqueue(&q, {startX, startY});
    visited[startX][startY] = true;

    int level = 0; // initialize level to 0
    while (!isEmpty(&q)) {
        int levelSize = q.rear - q.front + 1; // number of points at current level
        for (int i = 0; i < levelSize; i++) {
            auto p = dequeue(&q);
            int x = p.x;
            int y = p.y;
            if (x == targetX && y == targetY) {
                return level; // return level if target is found
            }
            for (auto dir : directions) {
                int newX = x + dir.x;
                int newY = y + dir.y;
                if (isValid(newX, newY, w, h) && !visited[newX][newY] &&
                    (country[newX][newY] == '#' || country[newX][newY] == '*')) {
                    visited[newX][newY] = true;
                    enqueue(&q, {newX, newY});
                }
            }
        }
        level++; // increment level for next iteration
    }
    return -1; // target not found
}


City* prepareAdjTable(char** country, int numberOfCities, int w, int h) {
    City* cities = (City*) malloc(numberOfCities * sizeof(City));

    int cityCounter = 0;
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if (country[i][j] == '*') {
                auto city = findCityName(country, i, j, w, h);
                cities[cityCounter].name = city;
                cities[cityCounter].cords = {i, j};
                cityCounter++;
            }
        }
    }
    return cities;
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

    cin >> w >> h;

    char **country = (char **) malloc(h * sizeof(char *));
    for (int i = 0; i < h; i++) {
        country[i] = (char *) malloc(w * sizeof(char));
    }
    int numberCities = 0;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            cin >> country[i][j];
            if (country[i][j] == '*')
                numberCities++;
        }
    }

    auto cities = prepareAdjTable(country, numberCities, w, h);

//    for (int i = 0; i < numberCities; i++) {
//        cout << cities[i].name << std::endl;
//        cout << cities[i].cords.x << std::endl;
//        cout << cities[i].cords.y << std::endl;
//    }

    int** adjTable = (int**)malloc(numberCities * sizeof(int*));
    for (int i = 0; i < numberCities; i++) {
        adjTable[i] = (int*) malloc(numberCities * sizeof(int));
    }
    for (int i = 0; i < numberCities; ++i) {
        for (int j = i + 1; j < numberCities; ++j) {
            // (char** country, int startX, int startY, int targetX, int targetY, int w , int h){
            auto dist = bfs(country, cities[i].cords.x, cities[i].cords.y, cities[j].cords.x, cities[j].cords.y, w, h );
            adjTable[i][j] = dist;
            adjTable[j][i] = dist;
        }
    }

    for(int q = 0; q < numberCities; ++q){
        for(int r = 0; r < numberCities; ++r){
            if(q == r) adjTable[q][r] = 0;
        }
    }


    for(int i = 0; i < numberCities; i++){
        for(int j = 0; j < numberCities; j++){
            cout<< i << " " << j << " " << adjTable[i][j] << endl;
        }
    }

    int k = 0;
    cin >> k;

    char* sourceCity;
    char* targetCity;
    int flightTime;
    int sourceCityIndex = -1;
    int targetCityIndex = -1;
    for(int i = 0; i < k; i++){
        cin >> sourceCity >> targetCity >> flightTime;

        for(int i = 0; i < numberCities; i++){
            if(strcmp(cities[i].name, sourceCity) == 0){
                sourceCityIndex = i;
            }
            if(strcmp(cities[i].name, targetCity) == 0){
                targetCityIndex = i;
            }
        }

        adjTable[sourceCityIndex][targetCityIndex] = flightTime;
    }

    int questions = 0;
    int qIdentifier = -1;
    cin >> questions;
    for(int i = 0; i < questions; i++){
        cin >> sourceCity >> targetCity >> qIdentifier;
        for(int i = 0; i < numberCities; i++){
            if(strcmp(cities[i].name, sourceCity) == 0){
                sourceCityIndex = i;
            }
            if(strcmp(cities[i].name, targetCity) == 0){
                targetCityIndex = i;
            }
        }

        if(qIdentifier == 0){
            cout << adjTable[sourceCityIndex][targetCityIndex] << endl;
        }
        else if (qIdentifier == 1){
            cout << adjTable[sourceCityIndex][targetCityIndex] /*implement path reconstruction*/ << endl;
        }

    }






//    auto adjTable2 = prepareAdjTable(country, 2, w, h );

    // Read input
    /* int n, m;
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
    */
    free(country);
    free(cities);
    return 0;
}
