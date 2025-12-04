#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TREE_INPUT_SIZE 10   // number of values read from file for BST

/* ------------ Binary Search Tree Node ------------ */
struct TreeNode {
    int key;
    struct TreeNode *left;
    struct TreeNode *right;
};

/* ------------ Global Counters for Problem 2 ------------ */
static int fullNodeCount    = 0;
static int partialNodeCount = 0;
static int leafNodeCount    = 0;

/* ------------ Function Prototypes ------------ */
/* Problem 1 */
int  binarySearch(int a[], int n, int target);
void runProblem1(void);
void generateStrictlyIncreasingArray(int *arr, int size);
void transformToBArray(int *arr, int size);
void printIntArray(const int *arr, int size, const char *label);

/* Problem 2 */
void runProblem2(void);
struct TreeNode *insertNode(struct TreeNode *root, int value);
struct TreeNode *allocateNode(void);
void preorderCountNodes(struct TreeNode *root);
void resetCounters(void);
void freeTree(struct TreeNode *root);

int main(void) {
    int choice;

    printf("Select a problem to run:\n");
    printf("1. Find i such that A[i] == i in a strictly increasing array.\n");
    printf("2. Build and traverse a binary search tree (preorder count).\n");
    printf("Enter choice (1 or 2): ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    if (choice == 1) {
        runProblem1();
    } else if (choice == 2) {
        runProblem2();
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}

/* ---------------- Problem 1 Implementation ---------------- */

void runProblem1(void) {
    int listSize;

    printf("\nEnter the size of the list: ");
    if (scanf("%d", &listSize) != 1 || listSize <= 0) {
        printf("Invalid list size.\n");
        return;
    }

    int *list = (int *)calloc(listSize, sizeof(int));
    if (!list) {
        printf("Memory allocation failed.\n");
        return;
    }

    /* Generate strictly increasing A[i] */
    generateStrictlyIncreasingArray(list, listSize);
    printIntArray(list, listSize, "Generated Array A:");

    /* Convert A[i] -> B[i] = A[i] - i */
    transformToBArray(list, listSize);

    /* Search in B for value 0 (corresponds to A[i] == i) */
    int index = binarySearch(list, listSize, 0);
    if (index != -1) {
        printf("Found index i such that A[i] == i at: %d\n", index);
    } else {
        printf("No index i such that A[i] == i was found.\n");
    }

    free(list);
}

/* Generate a strictly increasing array A with some randomness */
void generateStrictlyIncreasingArray(int *arr, int size) {
    srand((unsigned int)time(NULL));

    /* Start at a random negative or positive base */
    int baseSign = (rand() % 2 == 0) ? 1 : -1;
    arr[0] = baseSign * (rand() % 50);

    for (int i = 1; i < size; i++) {
        int increment = (rand() % 10) + 1;
        arr[i] = arr[i - 1] + increment;
    }
}

/* Convert A[i] to B[i] = A[i] - i */
void transformToBArray(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = arr[i] - i;
    }
}

/* Simple binary search on an integer array */
int binarySearch(int a[], int n, int target) {
    int low  = 0;
    int high = n - 1;

    while (low <= high) {
        int mid = (low + high) / 2;

        if (a[mid] == target) {
            return mid;
        } else if (a[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

void printIntArray(const int *arr, int size, const char *label) {
    printf("%s\n", label);
    for (int i = 0; i < size; i++) {
        printf("A[%d] = %d\n", i, arr[i]);
    }
}

/* ---------------- Problem 2 Implementation ---------------- */

void runProblem2(void) {
    FILE *fPtr;
    struct TreeNode *root = NULL;
    int number;

    resetCounters();

    fPtr = fopen("inputList", "r");
    if (!fPtr) {
        printf("Error: inputList file not found.\n");
        return;
    }

    for (int i = 0; i < TREE_INPUT_SIZE; i++) {
        if (fscanf(fPtr, "%d", &number) != 1) {
            printf("Error: not enough integers in inputList.\n");
            fclose(fPtr);
            freeTree(root);
            return;
        }
        root = insertNode(root, number);
    }
    fclose(fPtr);

    preorderCountNodes(root);

    printf("Number of Full Nodes   (2 children): %d\n", fullNodeCount);
    printf("Number of Partial Nodes(1 child)   : %d\n", partialNodeCount);
    printf("Number of Leaf Nodes   (0 children): %d\n", leafNodeCount);

    freeTree(root);
}

/* Insert a value into the BST */
struct TreeNode *insertNode(struct TreeNode *root, int value) {
    if (root == NULL) {
        root = allocateNode();
        root->key   = value;
        root->left  = NULL;
        root->right = NULL;
    } else if (value < root->key) {
        root->left = insertNode(root->left, value);
    } else if (value > root->key) {
        root->right = insertNode(root->right, value);
    }
    /* duplicates are ignored */
    return root;
}

struct TreeNode *allocateNode(void) {
    struct TreeNode *node = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    return node;
}

/* Preorder traversal to count full / partial / leaf nodes */
void preorderCountNodes(struct TreeNode *root) {
    if (root != NULL) {
        int hasLeft  = (root->left  != NULL);
        int hasRight = (root->right != NULL);

        if (hasLeft && hasRight) {
            fullNodeCount++;
        } else if (hasLeft || hasRight) {
            partialNodeCount++;
        } else {
            leafNodeCount++;
        }

        preorderCountNodes(root->left);
        preorderCountNodes(root->right);
    }
}

void resetCounters(void) {
    fullNodeCount    = 0;
    partialNodeCount = 0;
    leafNodeCount    = 0;
}

/* Free all nodes in the BST */
void freeTree(struct TreeNode *root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}
