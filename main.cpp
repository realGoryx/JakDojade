#include <iostream>
#include <cctype>
#include <cstring>
using namespace std;

const int MAX_VERTICES = 10000;
const int INF = 1000000;

struct Edge {
	int source;
	int dest;
	int weight;
};

struct Point {
	int x;
	int y;
};

struct City {
	char* name;
	Point cords;
};

struct Queue {
	int front;
	int rear;
	Point data[MAX_VERTICES];
};

void initQueue(Queue* q) {
	q->front = -1;
	q->rear = -1;
}

bool isEmpty(Queue* q) {
	return q->front == -1;
}

void enqueue(Queue* q, Point p) {
	if (q->rear == MAX_VERTICES - 1) {
		cout << "Queue overflow\n";
		return;
	}
	if (q->front == -1) {
		q->front = 0;
	}
	q->rear++;
	q->data[q->rear] = p;
}

Point dequeue(Queue* q) {
	if (isEmpty(q)) {
		cout << "Queue underflow\n";
		Point invalid_point = { -1, -1 };
		return invalid_point;
	}
	Point p = q->data[q->front];
	if (q->front == q->rear) {
		q->front = -1;
		q->rear = -1;
	}
	else {
		q->front++;
	}
	return p;
}

bool isValid(int x, int y, int w, int h) {
	return x >= 0 && x < h && y >= 0 && y < w;
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
				else if (ny == 0) {
					startCityName = 0;
					break;
				}
			}
			if (startCityName != -1) {
				int charPos = 0;
				for (int k = startCityName; k < w; k++) {
					if (isalpha(country[nx][k])) {
						cityName[charPos] = country[nx][k];
						charPos++;
					}
					else {
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

City* getCities(char** country, int numberOfCities, int w, int h) {
	City* cities = (City*)malloc(numberOfCities * sizeof(City));

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
					cities[cityCounter].cords = { i, j };
					cityCounter++;
				}
			}
		}
	}

	if (cityCounter < numberOfCities) {
		cities = (City*)realloc(cities, cityCounter * sizeof(City));
	}

	return cities;
}

int bfs(char** country, int startX, int startY, int targetX, int targetY, const int w, const int h) {
	Point directions[4] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
	bool** visited = createVisitedArray(w, h);

	Queue q;
	initQueue(&q);
	enqueue(&q, { startX, startY });
	visited[startX][startY] = true;

	int level = 0;


	bool isCity = false;
	while (!isEmpty(&q)) {
		int levelSize = q.rear - q.front + 1;
		for (int i = 0; i < levelSize; i++) {
			Point p = dequeue(&q);
			int x = p.x;
			int y = p.y;
			if ((x != startX || y != startY) && (x != targetX || y != targetY)
				&& country[x][y] == '*') {
				for (int j = 0; j < 4; j++) {
					int markX = x + directions[j].x;
					int markY = y + directions[j].y;
					if (isValid(markX, markY, w, h)) {
						visited[markX][markY] = true;
					}
				}
			}
			else if (x == targetX && y == targetY) {
				freeVisitedArray(visited, h);
				return level;
			}
			for (int j = 0; j < 4; j++) {
				int newX = x + directions[j].x;
				int newY = y + directions[j].y;
				if (isValid(newX, newY, w, h) && !visited[newX][newY] &&
					(country[newX][newY] == '#' || country[newX][newY] == '*')) {
					visited[newX][newY] = true;
					enqueue(&q, { newX, newY });
				}
			}
		}
		level++;
	}
	freeVisitedArray(visited, h);
	return -1;
}

int dijkstra(bool trackpath, Edge* edges, int numCities, int edgeCount, int source, int dest, int* path) {
	int* dist = (int*)malloc(numCities * sizeof(int));
	bool* visited = (bool*)malloc(numCities * sizeof(bool));
	int* predecessor = (int*)malloc(numCities * sizeof(int));;

	for (int i = 0; i < numCities; i++) {
		dist[i] = INF;
		visited[i] = false;
		predecessor[i] = -1;
	}

	dist[source] = 0;

	for (int i = 0; i < numCities - 1; i++) {
		int minDistance = INF;
		int minIndex = -1;

		for (int j = 0; j < numCities; j++) {
			if (!visited[j] && dist[j] <= minDistance) {
				minDistance = dist[j];
				minIndex = j;
			}
		}

		int currentNode = minIndex;
		visited[currentNode] = true;

		for (int j = 0; j < edgeCount; j++) {
			if (edges[j].source == currentNode) {
				int neighbor = edges[j].dest;
				int weight = edges[j].weight;
				if (!visited[neighbor] && dist[currentNode] + weight < dist[neighbor]) {
					dist[neighbor] = dist[currentNode] + weight;
					predecessor[neighbor] = currentNode;
				}
			}
		}
	}

	if (trackpath) {
		int pathIndex = 0;
		for (int at = dest; at != -1; at = predecessor[at]) {
			path[pathIndex++] = at;
		}

		for (int i = 0; i < pathIndex / 2; i++) {
			int temp = path[i];
			path[i] = path[pathIndex - i - 1];
			path[pathIndex - i - 1] = temp;
		}
	}

	int result = dist[dest];

	free(dist);
	free(visited);
	free(predecessor);

	return result;
}

void showpath(int* path, City* cities, char* targetCity, int numCities) {
	for (int i = 1; i < numCities - 1; i++) {
		if (path[i] > -1 && strcmp(cities[path[i]].name, targetCity) != 0) {
			cout << " " << cities[path[i]].name;
		}
	}
}

int main() {
	const int maxCityName = 100;
	int w = 0;
	int h = 0;

	cin >> w >> h;


	char** country = (char**)malloc(h * sizeof(char*));
	for (int i = 0; i < h; i++) {
		country[i] = (char*)malloc(w * sizeof(char));
	}
	int numberCities = 0;

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			cin >> country[i][j];
			if (country[i][j] == '*')
				numberCities++;
		}
	}

	City* cities = getCities(country, numberCities, w, h);
	Edge* edges = (Edge*)malloc(numberCities * (numberCities - 1) * sizeof(Edge));
	for (int i = 0; i < numberCities * (numberCities - 1); i++) {
		edges[i].weight = - 1;
	}
	int edgeCount = 0;
	
	for (int i = 0; i < numberCities; i++) {
		for (int j = i + 1; j < numberCities; j++) {
			int minSteps = 0;
			minSteps = bfs(country, cities[i].cords.x, cities[i].cords.y, cities[j].cords.x, cities[j].cords.y, w, h);

			if (minSteps != -1 ) {
				edges[edgeCount++] = { i, j, minSteps };
				edges[edgeCount++] = { j, i, minSteps };
			}
		}
	}

	int k = 0;
	cin >> k;

	char sourceCity[maxCityName];
	char targetCity[maxCityName];
	int flightTime;
	int sourceCityIndex = -1;
	int targetCityIndex = -1;
	for (int i = 0; i < k; i++) {
		cin >> sourceCity >> targetCity >> flightTime;

		for (int i = 0; i < numberCities; i++) {
			if (strcmp(cities[i].name, sourceCity) == 0) {
				sourceCityIndex = i;
			}
			if (strcmp(cities[i].name, targetCity) == 0) {
				targetCityIndex = i;
			}
		}

        for (int i = 0; i < edgeCount; i++) {
            if (edges[i].source == sourceCityIndex && edges[i].dest == targetCityIndex ||
                edges[i].source == targetCityIndex && edges[i].dest == sourceCityIndex){
                edges[i].weight = flightTime;
            }
        }
		if (edgeCount > 0) {
			for (int i = 0; i < edgeCount; i++) {
				if (edges[i].source == sourceCityIndex && edges[i].dest == targetCityIndex) {
					edges[i].weight = flightTime;
					break;
				}
				else if (i == edgeCount - 1) {
					edges = (Edge*)realloc(edges, (edgeCount + 1) * sizeof(Edge));
					edges[edgeCount].source = sourceCityIndex;
					edges[edgeCount].dest = targetCityIndex;
					edges[edgeCount].weight = flightTime;
					++edgeCount;
				}
			}
		}
		else if (edgeCount == 0) {
				edges = (Edge*)realloc(edges, (edgeCount + 1) * sizeof(Edge));
				edges[edgeCount].source = sourceCityIndex;
				edges[edgeCount].dest = targetCityIndex;
				edges[edgeCount].weight = flightTime;
				++edgeCount;
		}
	}

	int questions = 0;
	int qIdentifier = -1;
	cin >> questions;
	for (int i = 0; i < questions; i++) {
		cin >> sourceCity >> targetCity >> qIdentifier;
		for (int j = 0; j < numberCities; j++) {
			if (strcmp(cities[j].name, sourceCity) == 0) {
				sourceCityIndex = j;
			}
			if (strcmp(cities[j].name, targetCity) == 0) {
				targetCityIndex = j;
			}
		}

		if (qIdentifier == 0) {
			int* path = (int*)malloc(numberCities * sizeof(int));
			cout << dijkstra(false, edges, numberCities, edgeCount, sourceCityIndex, targetCityIndex, path) << endl;
			free(path);
		}
		else if (qIdentifier == 1) {
			int* path = (int*)malloc(numberCities * sizeof(int));
			for (size_t i = 0; i < numberCities; i++) {
				path[i] = -1;
			}
			cout << dijkstra(true, edges, numberCities, edgeCount, sourceCityIndex, targetCityIndex, path);
			showpath(path, cities, targetCity, numberCities);
			cout << endl;
			free(path);
		}
	}

	for (int i = 0; i < h; i++) {
		free(country[i]);
	}
	free(country);
	free(cities);
	free(edges);
	return 0;
}
