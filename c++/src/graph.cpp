#include <iostream>
#include "stack.h"

using namespace std;


class Vertex {
public:
    char label;
    bool visited;
    Vertex() {}
    Vertex(char lab) {
        label = lab;
        visited = false;
    }
    void display() {
        cout << label << " ";
    }
};

class Queue {
private:
    int head;
    int tail;
    int* queue;
    int capacity;
public:
    Queue(int cap) {
        capacity = cap;
        queue = new int[cap];
        tail = -1;
    }
    Queue() {
        Queue(5);
    }

    void enqueue(int value) {
        if (is_full()) {
            cout << "queue is full" << endl;
            return;
        }
        queue[++tail] = value;
    }
    int dequeue() {
        if (is_empty()) {
            cout << "queue is empty" << endl;
            return -1;
        }
        int ret = queue[0];
        for (int i = 0; i < tail; i++) {
            queue[i] = queue[i+1];
        }
        tail--;
        return ret;
    }
    int peek() {
        return queue[0];
    }
    int is_full() {
        return (tail == capacity - 1);
    }
    int is_empty() {
        return tail == -1;
    }
};


class Graph {
private:
    Vertex* vertices;
    int** adjMatrix;
    int vertex_count;
    int capacity;
public:
    Graph(int n) {
        capacity = n;
        vertices =  new Vertex[n];
        adjMatrix = new int*[n];
        for (int i = 0; i < n; i++)
            adjMatrix[i] = new int[n];
        vertex_count = 0;
    }
    void addVertex(char label) {
        Vertex v = Vertex(label);
        vertices[vertex_count++] = v;
    }
    void addEdge(int start, int end) {
        adjMatrix[start][end] = 1;
        adjMatrix[end][start] = 1;
    }
    int getAdjUnvisitedVertex(int virtexIndex) {
        for (int i = 0; i < vertex_count; i++) {
            if (adjMatrix[virtexIndex][i] == 1 && vertices[i].visited == false)
                return i;
        }
        return -1;
    }
    void depthFirstSearch() {
        Stack stack = Stack(capacity);
        vertices[0].visited = true;
        vertices[0].display();
        stack.push(0);

        while(!stack.is_empty()) {
            int i = stack.peek();
            int j = getAdjUnvisitedVertex(i);
            if (j != -1) {
                vertices[j].visited = true;
                vertices[j].display();
                stack.push(j);
            } else {
                stack.pop();
            }
        }
    }
    void breadthFirstSearch() {
        Queue queue = Queue(capacity);
        vertices[0].visited = true;
        vertices[0].display();
        queue.enqueue(0);
        while(!queue.is_empty()) {
            int i = queue.peek();
            int j = getAdjUnvisitedVertex(i);
            if (j != -1) {
                vertices[j].visited = true;
                vertices[j].display();
                queue.enqueue(j);
            } else {
                queue.dequeue();
            }
        }
    }
};


int main() {
    Graph graph = Graph(5);
    graph.addVertex('S');
    graph.addVertex('A');
    graph.addVertex('B');
    graph.addVertex('C');
    graph.addVertex('D');

    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(0, 3);
    graph.addEdge(1, 4);
    graph.addEdge(2, 4);
    graph.addEdge(3, 4);

    cout << "Depth First Search ";
    graph.depthFirstSearch();
    // cout << "Breadth First Search ";
    // graph.breadthFirstSearch();
    return 0;
}
