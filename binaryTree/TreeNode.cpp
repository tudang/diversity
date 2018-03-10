#include <iostream>
#include "TreeNode.h"

TreeNode* newNode(int value) {
    TreeNode *node = (TreeNode*) malloc(sizeof(TreeNode));
    memset(node, 0, sizeof(TreeNode));
    node->value = value;
    return node;
}

void inOrderTraversal(TreeNode* pRoot) {
    if (pRoot == NULL)
        return;
    if (pRoot->left)
        inOrderTraversal(pRoot->left);
    printf("%d ", pRoot->value);
    if (pRoot->right)
        inOrderTraversal(pRoot->right);
}

void preOrderTraversal(TreeNode* pRoot) {
    if (pRoot == NULL)
        return;
    printf("%d ", pRoot->value);
    if (pRoot->left)
        preOrderTraversal(pRoot->left);
    if (pRoot->right)
        preOrderTraversal(pRoot->right);
}

void postOrderTraversal(TreeNode* pRoot) {
    if (pRoot == NULL)
        return;
    if (pRoot->left)
        postOrderTraversal(pRoot->left);
    if (pRoot->right)
        postOrderTraversal(pRoot->right);
    printf("%d ", pRoot->value);
}

void freeTree(TreeNode *pRoot) {
    if (pRoot == NULL)
        return;
    if (pRoot->left)
        freeTree(pRoot->left);
    if (pRoot->right)
        freeTree(pRoot->right);
    free(pRoot);
}
