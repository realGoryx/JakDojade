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

void dfs(char** country, int sourceX, int sourceY, int targetX, int targetY, int steps, int* minSteps, char prev, int w, int h) {
	if (sourceX < 0 || sourceY < 0 || sourceX >= h || sourceY >= w || country[sourceX][sourceY] == '.' || isalpha(country[sourceX][sourceY])) {
		return;
	}

	if (sourceX == targetX && sourceY == targetY) {
		if (steps < *minSteps) {
			*minSteps = steps;
		}
		return;
	}

	if (country[sourceX][sourceY] == '#' || country[sourceX][sourceY] == '*') {
		char originalChar = country[sourceX][sourceY];
		country[sourceX][sourceY] = 'V';
		dfs(country, sourceX + 1, sourceY, targetX, targetY, steps + 1, minSteps, originalChar, w, h);
		dfs(country, sourceX - 1, sourceY, targetX, targetY, steps + 1, minSteps, originalChar, w, h);
		dfs(country, sourceX, sourceY + 1, targetX, targetY, steps + 1, minSteps, originalChar, w, h);
		dfs(country, sourceX, sourceY - 1, targetX, targetY, steps + 1, minSteps, originalChar, w, h);
		country[sourceX][sourceY] = prev;
	}
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

	return dist[dest];
}

void showpath(int* path, City* cities, int numCities) {
	for (int i = 1; i < numCities - 1; i++) {
		if (path[i] > -1) {
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
	Edge* edges = (Edge*)malloc((numberCities * (numberCities - 1) / 2) * sizeof(Edge));
	int edgeCount = 0;
	for (int i = 0; i < numberCities; i++) {
		for (int j = i + 1; j < numberCities; j++) {
			int minSteps = INF;
			dfs(country, cities[i].cords.x, cities[i].cords.y, cities[j].cords.x, cities[j].cords.y, 0, &minSteps, country[cities[i].cords.x][cities[i].cords.y], w, h);

			if (minSteps != INF) {
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
			if (edges[i].source == sourceCityIndex && edges[i].dest == targetCityIndex) {
				edges[i].weight = flightTime;
			}
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
			showpath(path, cities, numberCities);
			cout << endl;
			free(path);
		}
	}

	free(country);
	free(cities);
	return 0;
}
