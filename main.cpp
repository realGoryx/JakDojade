#include <iostream>
#include <cctype>
#include <cstring>
#include <stdbool.h>
using namespace std;

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

bool isValid(int x, int y, int w, int h) {
    return x >= 0 && x < h && y >= 0 && y < w;
}

char* findCityName(char** country, int x, int y, int w, int h) {
    int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    char* cityName = (char*)malloc((w + 1) * sizeof(char));

    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (isValid(nx, ny, w, h) && isalpha(country[nx][ny])) {
            int startCityName = -1;
            for (int j = ny; j >= 0; j--) {
                if (country[nx][j] == '.' || country[nx][j] == '#' || country[nx][j] == '*') {
                    startCityName = j + 1;
                    break;
                }
            }
            if (startCityName != -1) {
                int charPos = 0;
                for (int k = startCityName; k < w; k++) {
                    if (isalpha(country[nx][k])) {
                        cityName[charPos] = country[nx][k];
                        charPos++;
                    } else {
                        break;
                    }
                }
                cityName[charPos] = '\0';
                return cityName;
            }
        }
    }
    return nullptr;
}

bool** createVisitedArray(int w, int h) {
    bool** visited = (bool**)malloc(h * sizeof(bool*));
    for (int i = 0; i < h; i++) {
        visited[i] = (bool*)malloc(w * sizeof(bool));
        memset(visited[i], 0, w * sizeof(bool));
    }
    return visited;
}

void freeVisitedArray(bool** visited, int h) {
    for (int i = 0; i < h; i++) {
        free(visited[i]);
    }
    free(visited);
}

int bfs(char** country, int startX, int startY, int targetX, int targetY, int w, int h) {
    Point directions[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    bool** visited = createVisitedArray(w, h);

    Queue q;
    initQueue(&q);
    enqueue(&q, {startX, startY});
    visited[startX][startY] = true;

    int level = 0;
    while (!isEmpty(&q)) {
        int levelSize = q.rear - q.front + 1; // number of points at current level
        for (int i = 0; i < levelSize; i++) {
            Point p = dequeue(&q);
            int x = p.x;
            int y = p.y;
            if (x == targetX && y == targetY) {
                freeVisitedArray(visited, h);
                return level; // return level if target is found
            }
            for (int j = 0; j < 4; j++) {
                int newX = x + directions[j].x;
                int newY = y + directions[j].y;
                if (isValid(newX, newY, w, h) && !visited[newX][newY] &&
                    (country[newX][newY] == '#' || country[newX][newY] == '*')) {
                    visited[newX][newY] = true;
                    enqueue(&q, {newX, newY});
                }
            }
        }
        level++;
    }
    freeVisitedArray(visited, h);
    return -1; 
}

City* prepareAdjTable(char** country, int numberOfCities, int w, int h) {
    City* cities = (City*) malloc(numberOfCities * sizeof(City));

    if (cities == nullptr) {
        return nullptr;
    }

    int cityCounter = 0;
    for (int i = 0; i < h && cityCounter < numberOfCities; ++i) {
        for (int j = 0; j < w && cityCounter < numberOfCities; ++j) {
            if (country[i][j] == '*') {
                char* city = findCityName(country, i, j, w, h);
                if (city != nullptr) {
                    cities[cityCounter].name = city;
                    cities[cityCounter].cords = {i, j};
                    cityCounter++;
                }
            }
        }
    }

    if (cityCounter < numberOfCities) {
        cities = (City*) realloc(cities, cityCounter * sizeof(City));
    }

    return cities;
}

int main() {
    const int maxCityName = 100;
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

    City* cities = prepareAdjTable(country, numberCities, w, h);

    int** adjTable = (int**)malloc(numberCities * sizeof(int*));
    for (int i = 0; i < numberCities; i++) {
        adjTable[i] = (int*) malloc(numberCities * sizeof(int));
    }

    for(int i = 0; i < numberCities; ++i){
        for(int j = 0; j < numberCities; ++j){
            adjTable[i][i] = 0;
        }
    }

    for (int i = 0; i < numberCities; ++i) {
        for (int j = i + 1; j < numberCities; ++j) {
            auto dist = bfs(country, cities[i].cords.x, cities[i].cords.y, cities[j].cords.x, cities[j].cords.y, w, h );
            adjTable[i][j] = dist;
            adjTable[j][i] = dist;
        }
    }

    for(int i = 0; i < numberCities; i++){
        for(int j = 0; j < numberCities; j++){
            cout<< i << " " << j << " " << adjTable[i][j] << endl;
        }
    }

    int k = 0;
    cin >> k;

    char sourceCity[maxCityName];
    char targetCity[maxCityName];
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
        for(int j = 0; j < numberCities; j++){
            if(strcmp(cities[j].name, sourceCity) == 0){
                sourceCityIndex = j;
            }
            if(strcmp(cities[j].name, targetCity) == 0){
                targetCityIndex = j;
            }
        }

        if(qIdentifier == 0){
            cout << adjTable[sourceCityIndex][targetCityIndex] << endl;
        }
        else if (qIdentifier == 1){
            cout << adjTable[sourceCityIndex][targetCityIndex] /*implement path reconstruction*/ << endl;
        }
    }

    free(country);
    free(cities);
    return 0;
}
