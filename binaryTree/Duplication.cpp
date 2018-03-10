#include <iostream>

using namespace std;

void swap(int &a, int &b) {
    int temp = b;
    b = a;
    a = temp;
}

int findDuplication(int arr[], int length) {
    int found = 0;
    for (int i = 0; i < length; i++) {

        if (arr[i] != i) {
            // if m is already at m-th position, duplication has been found
            if (arr[arr[i]] == arr[i]) {
                found = 1;
                break;
            } else {
                // Swap m with value at m-th element
                swap(arr[i], arr[arr[i]]);
            }
        }
    }
    return found;
}

int main(int argc, char* argv[]) {
    int arr[] = {3, 1, 2, 0, 4, 5};
    int length = sizeof(arr) / sizeof(arr[0]);

    int res = findDuplication(arr, length);

    cout << res << endl;

    return 0;
}