#include <iostream>
#include "stack.h"
using namespace std;

Stack::Stack(int cap) {
    capacity = cap;
    stack = new int[cap];
    top = -1;
}
Stack::Stack(int cap, int stack_id) {
    capacity = cap;
    stack = new int[cap];
    top = -1;
    id = stack_id;
}
Stack::Stack() {
    Stack(5);
}
void Stack::push(int value) {
    if (top == capacity-1) {
        cout << "stack is full" << endl;
        return;
    }
    top++;
    stack[top] = value;
}
int Stack::pop() {
    if (top < 0) {
        cout << "stack is empty" << endl;
        return -1;
    }
    return stack[top--];
}
int Stack::peek() {
    return stack[top];
}
int Stack::is_empty() {
    return top == -1;
}

void Stack::display() {
    if (!is_empty()) {
        for (int i=0; i <= top; i++)
            cout << stack[i] << " | ";
    }
    cout << endl;
}

