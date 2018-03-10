#include <iostream>
using namespace std;

class Shape {
protected:
    int width, height;
public:
    Shape(int a, int b) {
        width = a;
        height = b;
    }
    virtual int area() {
        cout << "Area Shape" << endl;
        return 0;
    }
};

class Rectangle : public Shape {
public:
    Rectangle(int a = 0, int b = 0): Shape(a, b) {}
    int area() {
        cout << "Area Rectangle" << endl;
        return width * height;
    }
};

class Triangle : public Shape {
public:
    Triangle(int a = 0, int b = 0): Shape(a, b) {}
    int area() {
        cout << "Area Triangle" << endl;
        return width * height / 2;
    }
};

int main() {
    Shape *p;
    Rectangle rec(10, 7);
    Triangle tri(10, 5);

    p = &rec;
    p->area();

    p = &tri;
    p->area();

    return 0;
}