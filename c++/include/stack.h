class Stack {
private:
    int top;
    int capacity;
    int* stack;
public:
    int id;
    Stack(int cap);
    Stack(int cap, int stack_id);
    Stack();
    void push(int value);
    int pop();
    int peek();
    int is_empty();
    void display();
};
