#ifndef _TREENODE_H_
#define _TREENODE_H_

typedef struct TreeNode {
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode* newNode(int value);
void freeTree(TreeNode *pRoot);
void preOrderTraversal(TreeNode* pRoot);
void inOrderTraversal(TreeNode* pRoot);
void postOrderTraversal(TreeNode* pRoot);

#endif