#include <iostream>
#include <vector>
#include "TreeNode.h"

using namespace std;

void findPath(TreeNode *pRoot, int expectedSum, std::vector<int> path, 
    int currentSum) { 
    if (pRoot == NULL)
        return;
    currentSum += pRoot->value;

    path.push_back(pRoot->value);
    if (pRoot->left == NULL && pRoot->right == NULL) {
        if (currentSum == expectedSum) {
            cout << "Found a Path with Sum " << expectedSum << " ";
            std::vector<int>::iterator it = path.begin();
            while (it != path.end()) {
                cout << *it << " ";
                ++it;
            }
            cout << endl;
        }
        else {
            path.pop_back();
            currentSum -= pRoot->value;
        }
    } else {
        if (pRoot->left) {
            findPath(pRoot->left, expectedSum, path, currentSum);
        }
        if (pRoot->right) {
            findPath(pRoot->right, expectedSum, path, currentSum);
        }
    }
}

void findPathWithSum(TreeNode *pRoot, int expectedSum) {
    if (pRoot == NULL)
        return;
    int currentSum = 0;
    std::vector<int> path;
    findPath(pRoot, expectedSum, path, currentSum);
}


void testOneOrMorePaths() {
    TreeNode *root = newNode(10);
    root->left = newNode(5);
    root->left->left = newNode(4);
    root->left->right = newNode(7);
    root->right = newNode(12);
    findPathWithSum(root, 22);
    freeTree(root);
}

void testNoPath() {
    TreeNode *root = newNode(10);
    root->left = newNode(5);
    root->left->left = newNode(4);
    root->left->right = newNode(7);
    root->right = newNode(12);
    findPathWithSum(root, 23);
    freeTree(root);
}

void testOneNode() {
    TreeNode *root = newNode(10);
    findPathWithSum(root, 5);
    freeTree(root);
}

void testOnlyLeftNodes() {
    TreeNode *root = newNode(10);
    root->left = newNode(5);
    root->left->left = newNode(4);
    root->left->left->left = newNode(7);
    findPathWithSum(root, 26);
    freeTree(root);
}

void testNullRoot() {
    TreeNode *root = NULL;
    findPathWithSum(root, 23);
    freeTree(root);
}


int main(int argc, char* argv[]) {
    testOneOrMorePaths();
    testNoPath();
    testOneNode();
    testOnlyLeftNodes();
    testNullRoot();
    return 0;
}