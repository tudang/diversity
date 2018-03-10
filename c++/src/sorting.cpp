#include <iostream>
using namespace std;

void swap(int& a, int& b) {
    int c(a);
    a = b;
    b = c;
}

void bubble_sort(int a[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j=i+1; j < n; j++)
            if (a[j] < a[i])
                swap(a[i], a[j]);
    }
}

void insertion_sort(int a[], int n) {
    int hole_postion;
    int value_to_insert;
    for (int i = 1; i < n; i++) {
        value_to_insert = a[i];
        hole_postion = i;
        while (hole_postion > 0 && a[hole_postion-1] > value_to_insert) {
            a[hole_postion] = a[hole_postion-1];
            hole_postion = hole_postion-1;
        }
        a[hole_postion] = value_to_insert;
    }
}

void selection_sort(int a[], int n) {
    int min_index;
    for (int i = 0; i < n; i++) {
        min_index = i;
        for (int j = i+1; j < n; j++)
            if (a[j] < a[min_index])
                min_index = j;
        if (min_index != i)
            swap(a[min_index], a[i]);
    }
}

void merge(int a[], int b[], int low, int mid, int high) {
    int l1, l2, i;
    for (l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
        if (a[l1] < a[l2])
            b[i] = a[l1++];
        else
            b[i] = a[l2++];
    }
    while (l1 <= mid) {
        b[i++] = a[l1++];
    }
    while (l2 <= high) {
        b[i++] = a[l2++];
    }
}

void merge_sort(int a[], int low, int high) {
    if (low == high)
        return;
    int b[high - low + 1];
    int mid = (low + high) / 2;
    merge_sort(a, low, mid);
    merge_sort(a, mid + 1, high);
    merge(a, b, low, mid, high);
    while (low <= high) {
        a[low] = b[low];
        low++;
    }
}

int partitionFunc(int a[], int left, int right, int pivot) {
    int lp = left;
    int rp = right - 1;
    while(1) {
        while(a[lp++] < pivot) {
            // do nothing
        }
        while (a[rp--] > pivot) {
            // do nothing
        }
        if (lp >= rp)
            break;
        else
            swap(a[lp], a[rp]);
    }
    // Swap the final dest with pivot
    swap(lp, right);
    return lp;
}

void quick_sort(int a[], int left, int right) {
    if (left >= right)
        return;
    int pivot = a[right];
    int partition = partitionFunc(a, left, right, pivot);
    quick_sort(a, left, partition);
    quick_sort(a, partition+1, right);
}

int main() {
    int a[] = {14, 33, 27, 10, 35, 30, 19, 42, 44};
    int length = sizeof(a) / sizeof(*a);
    for(int i = 0; i < length; i++)
        cout << a[i] << " ";
    cout << endl;

    // O(n^2)
    // bubble_sort(a, length);
    // insertion_sort(a, length);
    // selection_sort(a, length);

    // O(n log(n))
    // merge_sort(a, 0, length - 1);
    merge_sort(a, 0, length - 1);

    for(int i = 0; i < length; i++)
        cout << a[i] << " ";
    cout << endl;

    return 0;
}