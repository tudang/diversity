#include <iostream>
using namespace std;

void traversing(int a[], int n) {
    for(int i = 0; i < n; i++)
        cout << a[i] << " ";
    cout << endl;
}

int linear_search(int a[], int n, int x) {
    for (int i = 0; i < n; i++)
        if (a[i] > x)
            return -1;
        else if (a[i] == x)
            return i;
    return -1;
}


int binary_search(int a[], int n, int x) {
    int low = 0, high = n-1;
    int mid = 0;
    int count = 0;
    while (low <= high) {
        count++;
        mid = low + (high - low) / 2;
        cout << "Count is " << count << " Mid is " << mid << endl;
        if (a[mid] == x) {
            return mid;
        }
        else if (a[mid] < x)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

int interpolation_search(int a[], int n, int x) {
    int low = 0, high = n-1, mid = -1;
    int count = 0;
    while (low <= high) {
        count++;
        mid = low + ((high - low) / (a[high] - a[low])) * (x - a[low]);
        cout << "Count is " << count << " Mid is " << mid << endl;
        if (a[mid] == x) {
            return mid;
        }
        else if (a[mid] < x)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    int list[] = {10, 14, 19, 27, 33, 35, 42, 44};
    int length = sizeof(list) / sizeof(*list);
    traversing(list, length);
    int x = 42;
    int ret = linear_search(list, length, x);
    cout << x << " is located at " << ret << endl;

    ret = binary_search(list, length, x);
    cout << x << " is located at " << ret << endl;

    ret = interpolation_search(list, length, x);
    cout << x << " is located at " << ret << endl;

    return 0;
}