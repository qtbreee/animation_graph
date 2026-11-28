#pragma once

#include <queue>
#include <vector>
#include <iostream>
#include <climits>
#include <algorithm>

struct Node {
	int value = 0;
	int weight = 0;
	Node* next = nullptr;
	Node* prev = nullptr;
};

struct FringeEntry
{
	int vertex;
	int priority;

	FringeEntry(int vertexID, int priorityValue)
	{
		vertex = vertexID;
		priority = priorityValue;
	}
};

struct ComparePriority
{
	bool operator()(const FringeEntry& first, const FringeEntry& second) const
	{
		return first.priority > second.priority;
	}
};

class BinaryHeap
{
private:
	// Keep the lowest-priority FringeEntry at the top of the queue.
	std::priority_queue<FringeEntry, std::vector<FringeEntry>, ComparePriority> heap;
	// config heap so that the entry with the smallest priority is at the top

public:
	// function insert(vertex, priority)
	void insert(int vertex, int priority)
	{
		/*
		create a new fringe entry
		store vertex inside the entry
		store priority inside the entry

		add entry to heap
		*/
		FringeEntry new_entry = { vertex, priority };
		heap.push(FringeEntry(new_entry));
	}

	FringeEntry removeMinimum()
	{
		// Save the entry at the top of heap
		FringeEntry minimum = heap.top();
		// Remove the top entry from heap
		heap.pop();
		// Return the saved entry
		return minimum;
	}

	bool isEmpty() const
	{
		// Return wheter heap contains no entries
		return heap.empty();
	}

	void clear()
	{
		// while heap is not empty
		// remove the top entry
		while (!heap.empty())
		{
			heap.pop();
		}
	}
};

class LinkedList {
public:
	Node* head = nullptr;
	int size = 0; // how many nodes exist in the linked list

	// Constructor: default constructor

	// Destructor: clean up allocated memory, destroy all elements in the list
	~LinkedList() {
		Node* current = head;
		while (current != nullptr) {
			Node* nextNode = current->next;
			delete current;
			current = nextNode;
		}
	}

	void addNode(int value, int weight) {
		Node* newNode = new Node();
		newNode->value = value;
		newNode->weight = weight;

		if (head == nullptr) { // if list is completely empty, this node becomes the head
			head = newNode;
			size++;
			return;
		}
		// appending to the beginning is faster than traversal.
		newNode->next = head;
		head->prev = newNode;
		head = newNode;
		size++;
	}

	void removeNode(int target) {
		Node* current = head;

		// Traversal
		while (current != nullptr) { // the last node in the chain will point to null, stopping the loop
			Node* prev = current->prev;
			Node* next = current->next;
			if (current->value == target) {
				// edge cases for head and tail
				if (current == head) { // head
					head = next;
					if (next != nullptr) { // safety check, if this is the last node left in the list
						next->prev = nullptr;
					}
				}
				else if (next == nullptr) { // current is tail
					if (prev == nullptr) { // safety check, if this is the last node in the list
						head = nullptr;
					}
					prev->next = nullptr;
				}
				else {
					next->prev = prev; // normal case
					prev->next = next;
				}
				delete current;
				size--;
			}
			current = next;
		}
	}
};

class Graph {
public:
	int adjSize;
	LinkedList** adjacencyList;
	int* adjLookup;
	int totalVertexes = 0;
	bool isDirected = false;

	Graph(int size, bool directed) {
		adjSize = size;
		isDirected = directed;
		// dynamically allocate the arrays
		adjacencyList = new LinkedList * [adjSize];
		adjLookup = new int[adjSize];

		std::fill_n(adjacencyList, adjSize, nullptr);
		std::fill_n(adjLookup, adjSize, -1); //-1 is a sentiel. existing vertexes can never be -1
	}

	~Graph() {
		for (int i = 0; i < adjSize; i++) {
			delete adjacencyList[i];
		}
		delete[] adjacencyList;
		delete[] adjLookup;
	}

	void createVertex() {
		// nodes will just be named from their index number in the array. vertex 0 for example
		if (totalVertexes >= adjSize) { // is full
			std::cerr << "Adjacency List is full.\n";
			return;
		}

		int i;
		for (i = 0; i < adjSize; i++) { // loop thru and fill in any gaps first.
			if (adjacencyList[i] == nullptr) {
				break;
			}
		}

		adjacencyList[i] = new LinkedList();
		adjLookup[totalVertexes] = i;
		totalVertexes++;
	}

	void createEdge(int source, int destination, int weight) {
		if (source < 0 || source >= adjSize || destination < 0 || destination >= adjSize) { // completely out of bounds of our array
			std::cerr << "Invalid input. Source " << source << " or destination " << destination << " out of bounds.\n";
			return;
		}

		if (adjacencyList[source] == nullptr || adjacencyList[destination] == nullptr) { // if either are empty that means the connection is invalid.
			std::cerr << "Invalid input. Source " << source << " or destination " << destination << " does not exist.\n";
			return;
		}

		// weight should be nonnegative and not zero
		if (weight <= 0) {
			std::cerr << "Invalid input. Weight " << weight << " cannot be negative or zero.\n";
			return;
		}

		// no duplicate connections.
		Node* current = adjacencyList[source]->head;
		while (current != nullptr) {
			if (current->value == destination) {
				std::cerr << "Connection already exists, no duplicates.\n";
				return;
			}
			current = current->next;
		}

		adjacencyList[source]->addNode(destination, weight);
		if (!isDirected) {
			adjacencyList[destination]->addNode(source, weight);
		}
	}

	void removeEdge(int source, int destination) {
		if (source < 0 || source >= adjSize || destination < 0 || destination >= adjSize) { // completely out of bounds of our array
			std::cerr << "Invalid input. Source " << source << " or destination " << destination << " out of bounds.\n";
			return;
		}

		if (adjacencyList[source] == nullptr || adjacencyList[destination] == nullptr) { // same check from create edge
			std::cerr << "Invalid input. Source " << source << " or destination " << destination << " does not exist.\n\n";
			return;
		}

		Node* currentS = adjacencyList[source]->head;
		Node* currentD = adjacencyList[destination]->head;

		// Traversal to find the node representing the edge connection
		while (currentS != nullptr) {
			if (currentS->value == destination) {
				adjacencyList[source]->removeNode(destination);
				break;
			}
			currentS = currentS->next;
		}
		while (currentD != nullptr) {
			if (currentD->value == source) {
				adjacencyList[destination]->removeNode(source);
				break;
			}
			currentD = currentD->next;
		}
	}

	void removeVertex(int target) {
		if (target >= adjSize || target < 0) {
			std::cerr << "Invalid input: Target " << target << " out of bounds.\n";
			return;
		}

		if (adjacencyList[target] == nullptr) { // doesn't exist
			std::cerr << "Invalid input: Target " << target << " does not exist.\n";
			return;
		}

		// before deleting the vertex, follow the edges it had and remove those connections
		Node* current = adjacencyList[target]->head;
		while (current != nullptr) {
			int currentValue = current->value;
			current = current->next;
			removeEdge(target, currentValue);
		}

		delete adjacencyList[target];
		adjacencyList[target] = nullptr;

		// delete the spot in the lookup table and shift over values like [0,1,3,target,4] -> [0,1,3,4]
		int i = 0;
		while (i < totalVertexes && adjLookup[i] != target) {
			i++;
		} // goes until i is the target value or runs out of array

		for (int j = i + 1; j < totalVertexes; j++) { // main swapping loop. continues with two pointers i and j to swap places
			adjLookup[i] = adjLookup[j];
			i++;
		}
		adjLookup[i] = -1;

		totalVertexes--;
	}

	void printGraph() {
		std::cout << "Graph:\n";

		for (int i = 0; i < totalVertexes; i++) { // Traversal down the array
			int vertexAddress = adjLookup[i]; // the value in the lookup is the correct index and label
			std::cout << "Vertex " << vertexAddress << " = ";
			Node* current = adjacencyList[vertexAddress]->head;

			while (current != nullptr) { // traversal through the linked list
				std::cout << "(" << vertexAddress << " , " << current->value << ") Weight: " << current->weight << " ";
				current = current->next;
			}
			std::cout << '\n';
		}
		std::cout << "\n";
	}

	void printVertex(int target) {
		if (target >= adjSize || target < 0) {
			std::cerr << "Invalid input: Target " << target << " out of bounds.\n";
			return;
		}

		if (adjacencyList[target] == nullptr) {
			std::cerr << "Invalid input: Target " << target << " does not exist.\n";
			return;
		}

		std::cout << "Vertex: " << target << " = ";
		Node* current = adjacencyList[target]->head;

		while (current != nullptr) { // traversal through the linked list
			std::cout << "(" << target << " , " << current->value << ") Weight: " << current->weight << " ";
			current = current->next;
		}
		std::cout << "\n";
	}

	// Dijkstra's Algorithm: Shortest Path between Vertexes
	int* dijkstras(int source) {
		if (source < 0 || source >= adjSize) {
			std::cerr << "Invalid input. Source " << source << " out of bounds.\n";
			return nullptr;
		}

		if (adjacencyList[source] == nullptr) {
			std::cerr << "Invalid input. Source " << source << " does not exist.\n";
			return nullptr;
		}

		bool* visited = new bool[adjSize];
		std::fill_n(visited, adjSize, false);
		int* distances = new int[adjSize];
		std::fill_n(distances, adjSize, INT_MAX);

		// set initial distances for all vertexes 'inf' for others and 0 for source
		distances[source] = 0;

		// choose unvisited vertex with the shortest distance from the start to be the current index
		for (int i = 0; i < totalVertexes; i++) {
			int minDistance = INT_MAX;
			int currentVertex = -1;

			for (int j = 0; j < totalVertexes; j++) { // search around for a vertex
				int v = adjLookup[j];
				if (visited[v] != true && distances[v] <= minDistance) {
					minDistance = distances[v];
					currentVertex = v;
				}
			}

			if (currentVertex == -1 || minDistance == INT_MAX) {
				break; // we can't find a vertex or the vertexes just aren't connected at all
			}

			visited[currentVertex] = true;

			Node* current = adjacencyList[currentVertex]->head;
			while (current != nullptr) { // make the weight calculations for the paths connected to this vertex

				if (visited[current->value] == false) { // if the vertex had not been visited
					int altDistance = distances[currentVertex] + current->weight; // calculate new distance

					if (altDistance < distances[current->value]) { // only update if this new distance is lower.
						distances[current->value] = altDistance;
					}
				}
				current = current->next;
			}
		}

		delete[] visited;
		return distances;
	}

	// dijkstraBinaryHeap
	int* dijkstrasBinaryHeap(int source)
	{

		// check whether source is in bounds
		if (source < 0 || source >= adjSize)
		{
			std::cerr << "Source is out of bounds.\n";
			return nullptr;
		}

		// check whether source exists
		if (adjacencyList[source] == nullptr)
		{
			std::cerr << "Source vertex does not exist.\n";
			return nullptr;
		}

		/*
		create distances array
		create visted array
		create BinaryHeap object
		*/
		int* distances = new int[adjSize];
		bool* visited = new bool[adjSize];

		BinaryHeap bheapfringe;

		for (int position = 0; position < adjSize; ++position)
		{
			distances[position] = INT_MAX; // rep infinity
			visited[position] = false;
		}

		distances[source] = 0;
		bheapfringe.insert(source, 0);

		// while fringe not empty
		// remove the min entry
		while (!bheapfringe.isEmpty())
		{
			FringeEntry minimum = bheapfringe.removeMinimum();

			// seperate the entry into two pieces
			int currentVertex = minimum.vertex;
			int removedDistance = minimum.priority;

			// old information dont match?
			// skip the iteration and return to the top of the outer while loop
			if (removedDistance != distances[currentVertex])
			{
				continue;
			}

			// check if already finalized
			if (visited[currentVertex] == true)
			{
				continue;
			}
			// if it hasnt been finalized, now the shortest distance to this vertex is
			visited[currentVertex] = true;

			//access the vertexs adjacency list
			Node* currentEdge = adjacencyList[currentVertex]->head;

			while (currentEdge != nullptr) {
				int neighbor = currentEdge->value;
				int edgeWeight = currentEdge->weight;
				//skip any finalized neighbors/ edge relaxation
				if (!visited[neighbor]) {
					//now calculate the alternate distance
					int alternateDistance = distances[currentVertex] + edgeWeight;

					if (alternateDistance < distances[neighbor]) {
						distances[neighbor] = alternateDistance;
						bheapfringe.insert(neighbor, alternateDistance);
					}
				}
				currentEdge = currentEdge->next;
			}
		}

		delete[] visited;
		return distances;
	}

	// eventually will need a dijkstraLinkedList
	int* dijkstrasLL(int source) {
		if (source < 0 || source >= adjSize) {
			std::cerr << "Invalid input. Source " << source << " out of bounds.\n";
			return nullptr;
		}

		if (adjacencyList[source] == nullptr) {
			std::cerr << "Invalid input. Source " << source << " does not exist.\n";
			return nullptr;
		}

		bool* visited = new bool[adjSize];
		std::fill_n(visited, adjSize, false);
		int* distances = new int[adjSize];
		std::fill_n(distances, adjSize, INT_MAX);

		LinkedList fringe = LinkedList();

		distances[source] = 0;
		fringe.addNode(source, 0);

		for (int i = 0; i < totalVertexes; i++) {
			int minDistance = INT_MAX;
			int currentVertex = -1;

			Node* currentFringe = fringe.head;
			while (currentFringe != nullptr) {
				int v = currentFringe->value;
				if (visited[v] != true && distances[v] <= minDistance) {
					minDistance = distances[v];
					currentVertex = v;
				}
				currentFringe = currentFringe->next;
			}

			if (currentVertex == -1 || minDistance == INT_MAX) {
				break;
			}

			visited[currentVertex] = true;
			fringe.removeNode(currentVertex);

			Node* current = adjacencyList[currentVertex]->head;
			while (current != nullptr) {
				if (visited[current->value] == false) {
					int distance = distances[currentVertex] + current->weight;
					if (distance < distances[current->value]) {
						distances[current->value] = distance;
						fringe.addNode(current->value, current->weight);
					}
				}
				current = current->next;
			}
		}

		delete[] visited;
		return distances;
	}

	void printDistances(int* distances) {
		if (distances == nullptr) {
			return;
		}
		std::cout << "Dijkstra's Distances:\n";
		for (int i = 0; i < adjSize; i++) {
			if (distances[i] == INT_MAX) {
				continue;
			}
			std::cout << "Vertex " << i << " Distance " << distances[i] << "\n";
		}
		std::cout << std::endl;
	}

	bool primsBinaryHeap(int source, int* parent, int* key, std::vector<int>& visitOrder) {
		// Check if the source vertex is valid
		if (source < 0 || source >= adjSize || adjacencyList[source] == nullptr) {
			std::cerr << "Invalid starting vertex.\n";
			return false;
		}
		// Initialize arrays for visited vertices, key values, and parent vertices
		bool* visited = new bool[adjSize];
		// Initialize all vertices as unvisited, with infinite key values and no parent
		for (int vertex = 0; vertex < adjSize; vertex++) {
			visited[vertex] = false;
			key[vertex] = INT_MAX;
			parent[vertex] = -1;
		}
		// Create a binary heap to store vertices based on their key values
		BinaryHeap fringe;

		key[source] = 0;
		fringe.insert(source, 0);

		// Continue until all vertices are processed
		while (!fringe.isEmpty()) {
			FringeEntry minimum = fringe.removeMinimum();
			int currentVertex = minimum.vertex;
			int removedWeight = minimum.priority;

			//ignore old heap entries
			if (removedWeight != key[currentVertex]) {
				continue;
			}
			// If the current vertex has already been visited, skip it
			if (visited[currentVertex]) {
				continue;
			}

			//mark the current vertex as visited and add it to the visit order
			visited[currentVertex] = true;
			// Add the current vertex to the visit order
			visitOrder.push_back(currentVertex);

			//explore the neighbors of the current vertex
			Node* currentEdge = adjacencyList[currentVertex]->head;

			// Traverse the adjacency list of the current vertex
			while (currentEdge != nullptr) {
				int neighbor = currentEdge->value;
				int weight = currentEdge->weight;

				// If the neighbor has not been visited and the edge weight
				// is less than the current key value, update the key and parent
				if (!visited[neighbor] && weight < key[neighbor]) {
					key[neighbor] = weight;
					parent[neighbor] = currentVertex;

					fringe.insert(neighbor, weight);
				}

				// Move to the next edge in the adjacency list
				currentEdge = currentEdge->next;
			}
		}

		// Check if all vertices were visited (ex. the graph is connected)
		bool connected = visitOrder.size() == static_cast<size_t>(totalVertexes);

		delete[] visited;
		return connected;
	}

	bool primsLL(int source, int* parent, int* key, std::vector<int>& visitOrder) {
		// Check if the source vertex is valid
		if (source < 0 || source >= adjSize || adjacencyList[source] == nullptr) {
			std::cerr << "Invalid starting vertex.\n";
			return false;
		}
		// Initialize arrays for visited vertices, key values, and parent vertices
		bool* visited = new bool[adjSize];

		for (int vertex = 0; vertex < adjSize; vertex++) {
			visited[vertex] = false;
			key[vertex] = INT_MAX;
			parent[vertex] = -1;
		}

		LinkedList fringe;

		key[source] = 0;
		fringe.addNode(source, 0);

		// Continue until all vertices are processed
		while (fringe.head != nullptr)
		{
			int currentVertex = -1;
			int minimumWeight = INT_MAX;

			Node* currentFringe = fringe.head;

			// Search the linked list for the minimum.
			while (currentFringe != nullptr)
			{
				int vertex = currentFringe->value;

				if (!visited[vertex] &&
					key[vertex] < minimumWeight)
				{
					minimumWeight = key[vertex];
					currentVertex = vertex;
				}

				currentFringe = currentFringe->next;
			}

			if (currentVertex == -1)
			{
				break;
			}

			visited[currentVertex] = true;
			visitOrder.push_back(currentVertex);

			fringe.removeNode(currentVertex);

			Node* currentEdge =
				adjacencyList[currentVertex]->head;

			while (currentEdge != nullptr)
			{
				int neighbor = currentEdge->value;
				int edgeWeight = currentEdge->weight;

				if (!visited[neighbor] &&
					edgeWeight < key[neighbor])
				{
					key[neighbor] = edgeWeight;
					parent[neighbor] = currentVertex;

					fringe.addNode(
						neighbor,
						edgeWeight);
				}

				currentEdge = currentEdge->next;
			}
		}

		bool connected =
			visitOrder.size() ==
			static_cast<size_t>(totalVertexes);

		delete[] visited;

		return connected;
	}

	// Prim's Algorithm: Minimum Spanning Tree
	void prims(int source) {
		if (source < 0 || source >= adjSize ||
			adjacencyList[source] == nullptr) {
			std::cout << "Invalid starting vertex.\n";
			return;
		}

		bool* visited = new bool[adjSize];
		int* key = new int[adjSize];
		int* parent = new int[adjSize];

		for (int i = 0; i < adjSize; i++) {
			visited[i] = false;
			key[i] = INT_MAX;
			parent[i] = -1;
		}

		key[source] = 0;

		for (int count = 0; count < totalVertexes; count++) {
			int u = -1;
			int minWeight = INT_MAX;

			for (int i = 0; i < totalVertexes; i++) {
				int v = adjLookup[i];

				if (!visited[v] && key[v] < minWeight) {
					minWeight = key[v];
					u = v;
				}
			}

			if (u == -1) {
				break;
			}

			visited[u] = true;

			Node* current = adjacencyList[u]->head;

			while (current != nullptr) {
				int v = current->value;
				int weight = current->weight;

				if (!visited[v] && weight < key[v]) {
					key[v] = weight;
					parent[v] = u;
				}

				current = current->next;
			}
		}

		int totalWeight = 0;

		std::cout << "Prim's Minimum Spanning Tree:\n";

		for (int i = 0; i < totalVertexes; i++) {
			int v = adjLookup[i];

			if (parent[v] != -1) {
				std::cout << parent[v] << " - " << v
					<< ", Weight: " << key[v] << "\n";

				totalWeight += key[v];
			}
		}

		std::cout << "Total weight: " << totalWeight << "\n";

		delete[] visited;
		delete[] key;
		delete[] parent;
	}
};