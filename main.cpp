#include <iostream>
#include <cctype>
#include <cstring>
using namespace std;

const int MAX_VERTICES = 10000;

struct Point {
	int x;
	int y;
};

struct CityPath {
	Point start;
	Point stop;
	Point* path;
	int pathLength;
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

struct BfsResult {
	int distance;
	Point* path;
	int pathLength;
};

void initCityPath(CityPath* cp, Point start, Point stop) {
	cp->start = start;
	cp->stop = stop;
	cp->pathLength = 0;
	cp->path = (Point*)malloc(sizeof(Point));
}

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

BfsResult bfs(char** country, int startX, int startY, int targetX, int targetY, const int w, const int h) {
	Point directions[4] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
	bool** visited = createVisitedArray(w, h);
	Point** prev = (Point**)malloc(h * sizeof(Point*));
	for (int i = 0; i < h; i++) {
		prev[i] = (Point*)malloc(w * sizeof(Point));
	}

	Queue q;
	initQueue(&q);
	enqueue(&q, { startX, startY });
	visited[startX][startY] = true;

	int level = 0;
	int cityPathCounter = 0;
	bool isCity = false;
	while (!isEmpty(&q)) {
		int levelSize = q.rear - q.front + 1; // number of points at current level
		for (int i = 0; i < levelSize; i++) {
			Point p = dequeue(&q);
			int x = p.x;
			int y = p.y;
			if (x == targetX && y == targetY) {
				Point* path = (Point*)malloc((level + 1) * sizeof(Point));
				int pathIndex = level;
				while (x != startX || y != startY) {
					path[pathIndex--] = { x, y };
					Point previous = prev[x][y];
					x = previous.x;
					y = previous.y;
				}
				path[0] = { startX, startY };
				freeVisitedArray(visited, h);
				for (int i = 0; i < h; i++) {
					free(prev[i]);
				}
				free(prev);
				return { level, path, level + 1 };
			}
			for (int j = 0; j < 4; j++) {
				int newX = x + directions[j].x;
				int newY = y + directions[j].y;
				if (isValid(newX, newY, w, h) && !visited[newX][newY] &&
					(country[newX][newY] == '#' || country[newX][newY] == '*')) {
					visited[newX][newY] = true;
					prev[newX][newY] = { x, y };
					enqueue(&q, { newX, newY });
				}
			}
		}
		level++;
	}
	freeVisitedArray(visited, h);
	for (int i = 0; i < h; i++) {
		free(prev[i]);
	}
	free(prev);
	return { -1, nullptr, 0 };
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

void reconstructPath(Point source, Point target, int numCities, int pathCounter, City* cities, CityPath* cityPaths) {
	CityPath* cp = (CityPath*)malloc(sizeof(CityPath));
	for (int i = 0; i < pathCounter; i++) {
		if (cityPaths[i].start.x == source.x && cityPaths[i].start.y == source.y && cityPaths[i].stop.x == target.x && cityPaths[i].stop.y == target.y) {
			cp = &cityPaths[i];
			break;
		}
	}

	for (int i = 0; i < cp->pathLength; i++) {
		if (cp->path[i].x != source.x && cp->path[i].y != source.y && cp->path[i].x != target.x && cp->path[i].x != target.y) {
			for (int j = 0; j < numCities; j++) {
				if (cities[j].cords.x == cp->path[i].x && cities[j].cords.y == cp->path[i].y) {
					cout << " " << cities[j].name;
				}
			}
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
	CityPath* cityPaths = (CityPath*)malloc(sizeof(CityPath));

	int** adjTable = (int**)malloc(numberCities * sizeof(int*));
	for (int i = 0; i < numberCities; i++) {
		adjTable[i] = (int*)malloc(numberCities * sizeof(int));
	}

	for (int i = 0; i < numberCities; ++i) {
		for (int j = 0; j < numberCities; ++j) {
			adjTable[i][i] = 0;
		}
	}

	int pathCounter = 0;
	for (int i = 0; i < numberCities; ++i) {
		for (int j = i + 1; j < numberCities; ++j) {
			BfsResult result = bfs(country, cities[i].cords.x, cities[i].cords.y, cities[j].cords.x, cities[j].cords.y, w, h);
			adjTable[i][j] = result.distance;
			CityPath* cityPath = (CityPath*)malloc(sizeof(CityPath));
			Point source = { cities[i].cords.x, cities[i].cords.y };
			Point target = { cities[j].cords.x, cities[j].cords.y };
			initCityPath(cityPath, source, target);
			for (int k = 0; k < result.pathLength; k++) {
				Point p = result.path[k];
				if (country[p.x][p.y] == '*' && p.x != source.x && p.y != source.y && p.x != target.x && p.y != target.y) {
					cityPath->path = (Point*)realloc(cityPath->path, (cityPath->pathLength + 1) * sizeof(Point));
					cityPath->path[cityPath->pathLength] = p;
					cityPath->pathLength++;
				}

			}
			//adjTable[j][i] = dist;
			cityPaths = (CityPath*)realloc(cityPaths, (pathCounter + 1) * sizeof(CityPath));
			cityPaths[pathCounter] = *cityPath;
			pathCounter++;
		}
	}

	for (int i = numberCities - 1; i >= 0; --i) {
		for (int j = numberCities - 2; j >= 0; --j) {
			BfsResult result = bfs(country, cities[i].cords.x, cities[i].cords.y, cities[j].cords.x, cities[j].cords.y, w, h);
			adjTable[i][j] = result.distance;
			CityPath* cityPath = (CityPath*)malloc(sizeof(CityPath));
			Point source = { cities[i].cords.x, cities[i].cords.y };
			Point target = { cities[j].cords.x, cities[j].cords.y };
			initCityPath(cityPath, source, target);
			for (int k = 0; k < result.pathLength; k++) {
				Point p = result.path[k];
				if (country[p.x][p.y] == '*' && p.x != source.x && p.y != source.y && p.x != target.x && p.y != target.y) {
					cityPath->path = (Point*)realloc(cityPath->path, (cityPath->pathLength + 1) * sizeof(Point));
					cityPath->path[cityPath->pathLength] = p;
					cityPath->pathLength++;
				}

			}
			cityPaths = (CityPath*)realloc(cityPaths, (pathCounter + 1) * sizeof(CityPath));
			cityPaths[pathCounter] = *cityPath;
			pathCounter++;
		}
	}

	/*for (int i = 0; i < pathCounter; i++) {
		cout << cityPaths[i].start.x << " " << cityPaths[i].start.y << " " << cityPaths[i].stop.x << " " << cityPaths[i].stop.y << " ";
		for (int j = 0; j < cityPaths[i].pathLength; j++) {
			cout << cityPaths[i].path[j].x << " " << cityPaths[i].path[j].y << " ";
		}
		cout << endl;
	}*/

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

		adjTable[sourceCityIndex][targetCityIndex] = flightTime;
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
			cout << adjTable[sourceCityIndex][targetCityIndex] << endl;
		}
		else if (qIdentifier == 1) {
			cout << adjTable[sourceCityIndex][targetCityIndex];
			reconstructPath({ cities[sourceCityIndex].cords.x ,  cities[sourceCityIndex].cords.y }, { cities[targetCityIndex].cords.x, cities[targetCityIndex].cords.y }, numberCities, pathCounter, cities, cityPaths);
			cout << endl;
		}
	}

	free(country);
	free(cities);
	free(cityPaths);
	return 0;
}
