#include <iostream>
#include "stack.h"

using namespace std;

void move_disk(int disk, Stack& src, Stack& dest, Stack& aux) {
    if (disk == 0) {
        int value = src.pop();
        dest.push(value);
        cout << "Move " << value << " from " << src.id << " to " << dest.id << endl; 
        cout << "Src " << src.id << " : ";
        src.display();
        cout << "Dst " << dest.id << " : ";
        dest.display();
        cout << "Aux " << aux.id << " : ";
        aux.display();
        return;
    }
    move_disk(disk - 1, src, aux, dest);
    dest.push(src.pop());
    cout << "Move " << disk << " from " << src.id << " to " << dest.id << endl; 
    cout << "Src " << src.id << " : ";
    src.display();
    cout << "Dst " << dest.id << " : ";
    dest.display();
    cout << "Aux " << aux.id << " : ";
    aux.display();
    move_disk(disk - 1, aux, dest, src);
}

int main() {
    int number_disks = 5;
    Stack src = Stack(number_disks, 1);
    Stack dst = Stack(number_disks, 2);
    Stack aux = Stack(number_disks, 3);

    src.push(4);
    src.push(3);
    src.push(2);
    src.push(1);
    src.push(0);

    move_disk(number_disks-1, src, dst, aux);
    return 0;
}