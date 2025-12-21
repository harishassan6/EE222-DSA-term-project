#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cmath>
#include <iomanip>


using namespace std;

const int MAX_ROWS = 50;
const int MAX_COLS = 50;
const int MAX_NODES = 2500;


// ==================== NODE STRUCTURES ====================
// PURPOSE: Building blocks for linked data structures


struct ListNode {
    // PURPOSE: Node for linked list implementation (used in Stack/Queue)

    int data;
    ListNode* next;
    ListNode(int val) : data(val), next(NULL) {}

};


struct GraphNode {

    // PURPOSE: Represents a cell in the maze as a graph node
    int id;           // Unique identifier
    int row, col;     // Position in maze
    GraphNode* next;  // For linked list

    GraphNode(int i, int r, int c) : id(i), row(r), col(c), next(NULL) {}

};

struct AdjListNode {

    // PURPOSE: Edge in graph - connects one cell to another
    int dest;         // Destination node ID
    int weight;       // Edge weight (distance/cost)
    AdjListNode* next; // Next edge in adjacency list
    AdjListNode(int d, int w) : dest(d), weight(w), next(NULL) {}

};

// ==================== STACK IMPLEMENTATION ====================
// PURPOSE: LIFO (Last In First Out) data structure for DFS algorithm

class StackArray {

    // PURPOSE: Stack using array (demonstrates array-based implementation)
    // USED IN: DFS traversal, path reconstruction
private:

    int arr[MAX_NODES];
    int top;

public:

    StackArray() : top(-1) {}
    
    void push(int val) {

        // Add element to top of stack
        if (top < MAX_NODES - 1) {
            arr[++top] = val;
        }

    }
    
    int pop() {

        // Remove and return top element
        if (top >= 0) {
            return arr[top--];
        }
        return -1;

    }
    
    bool isEmpty() {

        return top == -1;

    }
    
    int peek() {

        if (top >= 0) return arr[top];
        return -1;

    }

};


class StackLinkedList {

    // PURPOSE: Stack using linked list (demonstrates dynamic memory allocation)
    // USED IN: DFS traversal alternative implementation

private:

    ListNode* top;

public:

    StackLinkedList() : top(NULL) {}

    
    void push(int val) {

        // Add element to top (insert at beginning)
        ListNode* newNode = new ListNode(val);
        newNode->next = top;
        top = newNode;

    }
    
    int pop() {

        // Remove and return top element
        if (top == NULL) return -1;

        ListNode* temp = top;
        int val = temp->data;
        top = top->next;

        delete temp;
        return val;

    }
    
    bool isEmpty() {

        return top == NULL;

    }
    
    ~StackLinkedList() {

        // Cleanup: delete all nodes
        while (top != NULL) {
            ListNode* temp = top;
            top = top->next;
            delete temp;

        }

    }

};

// ==================== QUEUE IMPLEMENTATION ====================
// PURPOSE: FIFO (First In First Out) data structure for BFS algorithm

class QueueArray {

    // PURPOSE: Queue using circular array
    // USED IN: BFS traversal (level-by-level exploration)

private:

    int arr[MAX_NODES];
    int front, rear, size;

public:

    QueueArray() : front(0), rear(-1), size(0) {}
    
    void enqueue(int val) {

        // Add element to rear of queue
        if (size < MAX_NODES) {
            rear = (rear + 1) % MAX_NODES;
            arr[rear] = val;
            size++;
        }

    }
    
    int dequeue() {

        // Remove and return front element
        if (size == 0) return -1;
        int val = arr[front];
        front = (front + 1) % MAX_NODES;
        size--;
        return val;

    }
    
    bool isEmpty() {

        return size == 0;

    }
};

class QueueLinkedList {

    // PURPOSE: Queue using linked list
    // USED IN: BFS traversal alternative implementation

private:

    ListNode* front;
    ListNode* rear;

public:

    QueueLinkedList() : front(NULL), rear(NULL) {}

    
    void enqueue(int val) {

        // Add element to rear
        ListNode* newNode = new ListNode(val);

        if (rear == NULL) {

            front = rear = newNode;

        } 
        else {

            rear->next = newNode;
            rear = newNode;

        }

    }
    
    int dequeue() {

        // Remove and return front element
        if (front == NULL) return -1;

        ListNode* temp = front;
        int val = temp->data;
        front = front->next;

        if (front == NULL) rear = NULL;

        delete temp;
        return val;
    }
    
    bool isEmpty() {

        return front == NULL;

    }
    
    ~QueueLinkedList() {

        // Cleanup: delete all nodes
        while (front != NULL) {
            ListNode* temp = front;
            front = front->next;
            delete temp;
        }

    }

};


// ==================== HASH MAP FOR COORDINATES ====================
// PURPOSE: Fast lookup of node ID from maze coordinates (row, col)
// USED IN: Converting maze grid positions to graph node IDs in O(1) time

class HashMap {

private:

    static const int TABLE_SIZE = 1009;  // Prime number for better distribution

    struct Entry {
        // PURPOSE: Store coordinate-to-nodeID mapping
        int row, col, nodeId;
        Entry* next;  // For collision handling (chaining)
        Entry(int r, int c, int id) : row(r), col(c), nodeId(id), next(NULL) {}

    };
    Entry* table[TABLE_SIZE];
    
    int hash(int row, int col) {

        // PURPOSE: Convert (row, col) to array index
        return (row * 1000 + col) % TABLE_SIZE;

    }
    
public:
    HashMap() {

        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = NULL;

        }
    }
    
    void insert(int row, int col, int nodeId) {

        // PURPOSE: Store mapping (row, col) -> nodeId
        int idx = hash(row, col);
        Entry* newEntry = new Entry(row, col, nodeId);
        newEntry->next = table[idx];
        table[idx] = newEntry;

    }
    
    int get(int row, int col) {

        // PURPOSE: Retrieve nodeId for given (row, col)
        int idx = hash(row, col);
        Entry* curr = table[idx];
        while (curr != NULL) {
            if (curr->row == row && curr->col == col) {
                return curr->nodeId;
            }
            curr = curr->next;
        }
        return -1;  // Not found

    }
    
    ~HashMap() {

        // Cleanup: delete all entries
        for (int i = 0; i < TABLE_SIZE; i++) {
            Entry* curr = table[i];
            while (curr != NULL) {
                Entry* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }

};

// ==================== GRAPH CLASS ====================
class Graph {

private:
    AdjListNode* adjList[MAX_NODES];
    int nodeCount;
    int nodeRows[MAX_NODES];
    int nodeCols[MAX_NODES];
    
public:

    Graph() : nodeCount(0) {
        for (int i = 0; i < MAX_NODES; i++) {
            adjList[i] = NULL;
        }
    }
    
    int addNode(int row, int col) {

        int id = nodeCount;
        nodeRows[id] = row;
        nodeCols[id] = col;
        nodeCount++;
        return id;

    }
    
    void addEdge(int src, int dest, int weight) {

        AdjListNode* newNode = new AdjListNode(dest, weight);
        newNode->next = adjList[src];
        adjList[src] = newNode;

    }
    
    int getNodeCount() { 

        return nodeCount;

     }
    
    AdjListNode* getAdjList(int node) {

         return adjList[node]; 

        }
    
    void getNodeCoords(int node, int& row, int& col) {

        row = nodeRows[node];
        col = nodeCols[node];

    }
    
    ~Graph() {

        for (int i = 0; i < nodeCount; i++) {
            AdjListNode* curr = adjList[i];
            while (curr != NULL) {
                AdjListNode* temp = curr;
                curr = curr->next;
                delete temp;

            }
        }
    }
};

// ==================== MAZE CLASS ====================
class Maze {

private:

    char grid[MAX_ROWS][MAX_COLS];
    int rows, cols;
    int startRow, startCol;
    int endRow, endCol;
    
public:
    Maze() : rows(0), cols(0), startRow(-1), startCol(-1), endRow(-1), endCol(-1) {}
    
    bool loadFromFile(const char* filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Cannot open file " << filename << endl;
            return false;
        }
        
        rows = 0;
        cols = 0;
        char line[MAX_COLS + 2];
        
        while (file.getline(line, MAX_COLS + 2) && rows < MAX_ROWS) {
            int len = strlen(line);
            if (len > cols) cols = len;
            
            for (int j = 0; j < len; j++) {
                grid[rows][j] = line[j];
                if (line[j] == 'S') {
                    startRow = rows;
                    startCol = j;
                } else if (line[j] == 'E') {
                    endRow = rows;
                    endCol = j;
                }
            }
            rows++;
        }
        
        file.close();
        
        if (startRow == -1 || endRow == -1) {
            cout << "Error: Start (S) or End (E) not found in maze!" << endl;
            return false;
        }
        
        return true;
    }
    
    void display() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cout << grid[i][j];
            }
            cout << endl;
        }
    }
    
    void displayWithPath(int path[], int pathLen) {
        char tempGrid[MAX_ROWS][MAX_COLS];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                tempGrid[i][j] = grid[i][j];
            }
        }
        
        for (int i = 0; i < pathLen; i++) {
            int r = path[i] / MAX_COLS;
            int c = path[i] % MAX_COLS;
            if (tempGrid[r][c] != 'S' && tempGrid[r][c] != 'E') {
                tempGrid[r][c] = '~';
            }
        }
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cout << tempGrid[i][j];
            }
            cout << endl;
        }
    }
    
    char getCell(int r, int c) {

        if (r >= 0 && r < rows && c >= 0 && c < cols) {

            return grid[r][c];

        }

        return '#';

    }
    
    int getRows() {

         return rows;

         }

    int getCols() { 

        return cols;

     }

    int getStartRow() {

         return startRow;

         }

    int getStartCol() {

         return startCol;

         }

    int getEndRow() {

         return endRow;

         }

    int getEndCol() { 

        return endCol;

     }

};

// ==================== SORTING ALGORITHMS ====================
void bubbleSort(double arr[], int n) {

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                double temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

}

void insertionSort(int arr[], int n) {

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;

    }
}

// ==================== SOLVER CLASS ====================
class MazeSolver {
private:

    Maze* maze;
    Graph* graph;
    HashMap* coordMap;
    int startNode, endNode;
    
    void buildGraph() {

        graph = new Graph();
        coordMap = new HashMap();
        
        int rows = maze->getRows();
        int cols = maze->getCols();
        
        // Create nodes for all open cells
        for (int i = 0; i < rows; i++) {

            for (int j = 0; j < cols; j++) {

                char cell = maze->getCell(i, j);
                if (cell == ' ' || cell == 'S' || cell == 'E') {

                    int nodeId = graph->addNode(i, j);
                    coordMap->insert(i, j, nodeId);

                    
                    if (cell == 'S') startNode = nodeId;
                    if (cell == 'E') endNode = nodeId;
                }
            }
        }
        
        // Create edges (4-directional)
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int nodeId = coordMap->get(i, j);
                if (nodeId == -1) continue;
                
                for (int d = 0; d < 4; d++) {
                    int ni = i + dr[d];
                    int nj = j + dc[d];

                    int neighborId = coordMap->get(ni, nj);
                    
                    if (neighborId != -1) {
                        graph->addEdge(nodeId, neighborId, 1);
                    }
                }
            }
        }
    }
    
    int reconstructPath(int parent[], int path[]) {
        int pathLen = 0;
        int curr = endNode;
        
        StackArray pathStack;
        while (curr != -1) {

            pathStack.push(curr);
            curr = parent[curr];

        }
        
        while (!pathStack.isEmpty()) {

            int node = pathStack.pop();
            int r, c;
            graph->getNodeCoords(node, r, c);
            path[pathLen++] = r * MAX_COLS + c;

        }
        
        return pathLen;
    }
    
public:
    MazeSolver(Maze* m) : maze(m), graph(NULL), coordMap(NULL) {
        buildGraph();
    }
    
    // BFS Algorithm using Queue

    bool solveBFS(int path[], int& pathLen, int& nodesVisited) {

        int nodeCount = graph->getNodeCount();
        bool visited[MAX_NODES] = {false};
        int parent[MAX_NODES];
        for (int i = 0; i < nodeCount; i++) parent[i] = -1;
        
        QueueLinkedList q;
        q.enqueue(startNode);
        visited[startNode] = true;
        nodesVisited = 0;
        
        while (!q.isEmpty()) {

            int curr = q.dequeue();
            nodesVisited++;
            
            if (curr == endNode) {

                pathLen = reconstructPath(parent, path);
                return true;

            }
            
            AdjListNode* adj = graph->getAdjList(curr);
            while (adj != NULL) {
                int neighbor = adj->dest;
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    parent[neighbor] = curr;
                    q.enqueue(neighbor);
                }
                adj = adj->next;
            }
        }
        
        pathLen = 0;
        return false;
    }
    
    // DFS Algorithm using Stack
    bool solveDFSStack(int path[], int& pathLen, int& nodesVisited) {

        int nodeCount = graph->getNodeCount();

        bool visited[MAX_NODES] = {false};
        int parent[MAX_NODES];
        for (int i = 0; i < nodeCount; i++) parent[i] = -1;

        
        StackLinkedList s;
        s.push(startNode);
        nodesVisited = 0;
        


        while (!s.isEmpty()) {

            int curr = s.pop();
            
            if (visited[curr]) continue;
            visited[curr] = true;
            nodesVisited++;
            
            if (curr == endNode) {


                pathLen = reconstructPath(parent, path);
                return true;
            }
            
            AdjListNode* adj = graph->getAdjList(curr);
            while (adj != NULL) {
                int neighbor = adj->dest;
                if (!visited[neighbor]) {

                    parent[neighbor] = curr;
                    s.push(neighbor);

                }
                adj = adj->next;

            }
        }
        
        pathLen = 0;
        return false;
    }
    
    // DFS Recursive
    bool dfsRecursiveHelper(int curr, bool visited[], int parent[], int& nodesVisited) {
        visited[curr] = true;
        nodesVisited++;
        
        if (curr == endNode) {

            return true;

        }
        
        AdjListNode* adj = graph->getAdjList(curr);
        while (adj != NULL) {

            int neighbor = adj->dest;
            if (!visited[neighbor]) {

                parent[neighbor] = curr;
                if (dfsRecursiveHelper(neighbor, visited, parent, nodesVisited)) {
                    return true;
                }
            }
            adj = adj->next;
        }
        
        return false;
    }
    
    bool solveDFSRecursive(int path[], int& pathLen, int& nodesVisited) {

        int nodeCount = graph->getNodeCount();
        bool visited[MAX_NODES] = {false};
        int parent[MAX_NODES];
        for (int i = 0; i < nodeCount; i++) parent[i] = -1;
        
        nodesVisited = 0;
        
        if (dfsRecursiveHelper(startNode, visited, parent, nodesVisited)) {
            pathLen = reconstructPath(parent, path);
            return true;
        }
        
        pathLen = 0;
        return false;
    }
    
    ~MazeSolver() {
        delete graph;
        delete coordMap;
    }
};

// ==================== MAIN PROGRAM ====================

int main() {

    cout << "=====================================" << endl;
    cout << "   MAZE SOLVER - DSA PROJECT" << endl;
    cout << "=====================================" << endl;

    
    Maze maze;
    
    // Load maze from file
    if (!maze.loadFromFile("input_maze.txt")) {

        cout << "\nCreating sample maze file 'input_maze.txt'..." << endl;
        
        ofstream file("input_maze.txt");
        file << "##########\n";
        file << "#S   #   #\n";
        file << "# ## # # #\n";
        file << "#    #   #\n";
        file << "#### ### #\n";
        file << "#      E #\n";
        file << "##########\n";
        file.close();
        
        if (!maze.loadFromFile("input_maze.txt")) {
            return 1;
        }
    }
    
    cout << "\nOriginal Maze:" << endl;
    maze.display();
    
    MazeSolver solver(&maze);
    
    cout << "\n=====================================" << endl;
    cout << "SELECT ALGORITHM:" << endl;
    cout << "1. BFS (Breadth-First Search)" << endl;
    cout << "2. DFS (Depth-First Search - Stack)" << endl;
    cout << "3. DFS (Depth-First Search - Recursive)" << endl;
    cout << "4. Compare All Algorithms" << endl;
    cout << "=====================================" << endl;
    cout << "Enter choice: ";
    
    int choice;
    cin >> choice;
    
    int path[MAX_NODES];
    int pathLen = 0;
    int nodesVisited = 0;
    bool found = false;
    
    clock_t startTime, endTime;

    double timeTaken;
    
    if (choice >= 1 && choice <= 3) {
        startTime = clock();
        
        switch (choice) {
            case 1:

                cout << "\nRunning BFS..." << endl;
                found = solver.solveBFS(path, pathLen, nodesVisited);
                break;

            case 2:

                cout << "\nRunning DFS (Stack)..." << endl;
                found = solver.solveDFSStack(path, pathLen, nodesVisited);
                break;

            case 3:

                cout << "\nRunning DFS (Recursive)..." << endl;
                found = solver.solveDFSRecursive(path, pathLen, nodesVisited);
                break;

        }
        
        endTime = clock();

        timeTaken = double(endTime - startTime) / CLOCKS_PER_SEC * 1000;
        
        if (found) {


            cout << "\n✓ Path found!" << endl;
            cout << "Path length: " << pathLen << " steps" << endl;
            cout << "Nodes visited: " << nodesVisited << endl;
            cout << "Time taken: " << fixed << setprecision(3) << timeTaken << " ms" << endl;

            
            cout << "\nSolved Maze (path marked with *):" << endl;
            maze.displayWithPath(path, pathLen);

            
            // Save to file
            ofstream outFile("output.txt");
            outFile << "Algorithm: ";


            if (choice == 1) outFile << "BFS";
            else if (choice == 2) outFile << "DFS (Stack)";
            else outFile << "DFS (Recursive)";


            outFile << "\nPath found: Yes";
            outFile << "\nPath length: " << pathLen;
            outFile << "\nNodes visited: " << nodesVisited;
            outFile << "\nTime taken: " << timeTaken << " ms\n\n";


            outFile.close();
            
            cout << "\nResults saved to 'output.txt'" << endl;

        } else {

            cout << "\n✗ No path found!" << endl;

        }
        
        
    } else if (choice == 4) {
        cout << "\n=====================================" << endl;
        cout << "   COMPARING ALL ALGORITHMS" << endl;
        cout << "=====================================" << endl;
        
        double times[3];
        int visits[3];
        int lengths[3];
        const char* names[] = {"BFS", "DFS (Stack)", "DFS (Recursive)"};
        
        for (int i = 0; i < 3; i++) {
            startTime = clock();
            
            switch (i) {

                case 0: 
                found = solver.solveBFS(path, pathLen, nodesVisited); 

                    break;

                case 1:

                 found = solver.solveDFSStack(path, pathLen, nodesVisited);

                     break;

                case 2: 
                found = solver.solveDFSRecursive(path, pathLen, nodesVisited);

                     break;

            }
            
            endTime = clock();
            times[i] = double(endTime - startTime) / CLOCKS_PER_SEC * 1000;
            visits[i] = nodesVisited;
            lengths[i] = pathLen;
            
            cout << "\n" << names[i] << ":" << endl;
            cout << "  Path length: " << pathLen << endl;
            cout << "  Nodes visited: " << nodesVisited << endl;
            cout << "  Time: " << fixed << setprecision(3) << times[i] << " ms" << endl;
        }
        
        // Demonstrate sorting
        cout << "\n=====================================" << endl;
        cout << "SORTING ANALYSIS (by time)" << endl;
        double timesCopy[3];
        for (int i = 0; i < 3; i++) timesCopy[i] = times[i];
        
        bubbleSort(timesCopy, 3);
        
        cout << "Sorted times (Bubble Sort):" << endl;
        for (int i = 0; i < 3; i++) {
            cout << "  " << fixed << setprecision(3) << timesCopy[i] << " ms" << endl;
        }
        
        insertionSort(visits, 3);
        cout << "\nSorted nodes visited (Insertion Sort):" << endl;
        for (int i = 0; i < 3; i++) {
            cout << "  " << visits[i] << " nodes" << endl;
        }
        
        cout << "=====================================" << endl;
    }
    
    return 0;
}