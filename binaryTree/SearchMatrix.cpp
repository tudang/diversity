#include <iostream>

using namespace std;

int searchMatrix(int** mat, int rows, int cols, int value) {
    int length = rows*cols;
    int start = 0;
    int end = length - 1;
    while (start <= end) {
        int mid = start + (end - start) / 2;
        int row = mid / cols;
        int col = mid % cols;
        int v = mat[row][col];
        if (v == value)
            return mid;
        else if (v < value)
                start = mid + 1;
        else
            end = mid - 1;

    }
    return -1;
}

int main(int argc, char* argv[]) {
    int **m;
    m = new int*[3];
    for (int i = 0; i < 3; i++) {
        m[i] = new int[3];
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m[i][j] = i*6 + j*2 + 1;
            cout << m[i][j] << " ";
        }
        cout << endl;
    }


    int res1 = searchMatrix(m, 3, 3, 7);
    int res2 = searchMatrix(m, 3, 3, 13);

    cout << res1 << " " << res2 << endl;
    return 0;
}