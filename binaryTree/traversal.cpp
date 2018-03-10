#include <iostream>
#include <queue>
#include <stack>

#include "TreeNode.h"

using namespace std;


void printTopToBottom(TreeNode *pRoot) {
    if (pRoot == NULL)
        return;

    queue<TreeNode *> queueTreeNodes;
    queueTreeNodes.push(pRoot);
    int toBePrinted = 1;
    int nextLevel = 0;
    while (queueTreeNodes.size() > 0) {
        TreeNode *pNode = queueTreeNodes.front();
        queueTreeNodes.pop();

        printf("%d ", pNode->value);

        if (pNode->left) {
            queueTreeNodes.push(pNode->left);
            nextLevel++;
        }
        if (pNode->right) {
            queueTreeNodes.push(pNode->right);
            nextLevel++;
        }
        toBePrinted--;
        if (toBePrinted == 0) {
            printf("\n");
            toBePrinted = nextLevel;
            nextLevel = 0;
        }
    }
}

void printZigZag(TreeNode *pRoot) {
    if (pRoot == NULL)
        return;
    stack<TreeNode *> levels[2];
    int current = 0;
    int next = 1;

    levels[current].push(pRoot);
    while (!levels[0].empty() || !levels[1].empty()) {
        TreeNode *node = levels[current].top();
        levels[current].pop();

        printf("%d ", node->value);

        if (current == 0) {
            if (node->left) {
                levels[next].push(node->left);
            }
            if (node->right) {
                levels[next].push(node->right);
            }
        } else {
            if (node->right) {
                levels[next].push(node->right);
            }
            if (node->left) {
                levels[next].push(node->left);
            }
        }

        if (levels[current].empty()) {
            printf("\n");
            current = 1 - current;
            next = 1 - next;
        }
    }
}

int main(int argc, char* argv[]) {
    TreeNode *root = newNode(8);
    root->left = newNode(6);
    root->left->left = newNode(5);
    root->left->right = newNode(7);

    root->right = newNode(10);
    root->right->left = newNode(9);
    root->right->right = newNode(11);
    preOrderTraversal(root);
    printf(": preOrder\n");
    inOrderTraversal(root);
    printf(": inOrder\n");
    postOrderTraversal(root);
    printf(": postOrder\n");
    
    printTopToBottom(root);

    printZigZag(root);

    freeTree(root);

    return 0;
}