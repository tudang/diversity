#include <iostream>
using namespace std;

struct node {
    int data;
    struct node *left;
    struct node *right;
};

class Tree {
private:
    struct node *root;
public:
    Tree() { root = NULL; }
    struct node* getRoot() {
        return root;
    }

    void insert(int data) {
        struct node *tmp_node = (struct node*) malloc(sizeof(struct node));
        tmp_node->data = data;
        tmp_node->left = NULL;
        tmp_node->right = NULL;

        if (root == NULL) {
            root = tmp_node;
            return;
        }
        struct node *current = root;
        struct node *parent = NULL;
        while (1) {
            parent = current;
            if (parent->data < data) {
                current = current->right;
                if (current == NULL) {
                    parent->right = tmp_node;
                    return;
                }
            }
            else if (parent->data > data) {
                current = current->left;
                if (current == NULL) {
                    parent->left = tmp_node;
                    return;
                }
            }
            else
                return;
        }
    }
    struct node* search(int data) {
        struct node *current = root;
        while (current != NULL) {
            if (current->data == data)
                return current;
            else if (current->data < data)
                current = current->right;
            else
                current = current->left;
        }
        return NULL;
    }
    void preorder(struct node* current) {
        if (current == NULL)
            return;
        cout << current->data << " ";
        preorder(current->left);
        preorder(current->right);
    }
    void preorderTraversal() {
        preorder(root);
    }
    void inorder(struct node* current) {
        if (current == NULL)
            return;
        inorder(current->left);
        cout << current->data << " ";
        inorder(current->right);
    }
    void inorderTraversal() {
        inorder(root);
    }
    void postOrder(struct node* current) {
        if (current == NULL)
            return;
        postOrder(current->left);
        postOrder(current->right);
        cout << current->data << " ";
    }
    void postorderTraversal() {
        postOrder(root);
    }
};

int main() {
    Tree tree = Tree();
    tree.insert(5);
    tree.insert(3);
    tree.insert(2);
    tree.insert(4);
    tree.insert(8);
    tree.insert(7);
    tree.insert(10);

    cout << "PreOrder Traversal: ";
    tree.preorderTraversal();
    cout << endl << "InOrder Traversal: ";
    tree.inorderTraversal();
    cout << endl << "Postorder Traversal: ";
    tree.postorderTraversal();

    struct node* r = tree.search(4);
    cout << endl << "Found r->data " << r->data << endl;


    return 0;
}