
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 10 // For binary search tree

// ----------- Problem 2 Struct and Globals -----------
struct tnode {
    int key;
    struct tnode *left;
    struct tnode *right;
};

int numberOfFullNodes = 0;
int numberOfPartialNodes = 0;
int numberOfLeaves = 0;

// ---------- Function Prototypes ----------
int listSearch(int a[], int n, int x);
void runProblem1(void);
void runProblem2(void);

struct tnode *addtree(struct tnode *, int);
struct tnode *talloc(void);
void preOrderCount(struct tnode *);

int main(void) {
    int choice;
    printf("Select a problem to run:\n");
    printf("1. Find if A[i] == i in strictly increasing array (Binary Search)\n");
    printf("2. Build and traverse binary search tree (Preorder Count)\n");
    printf("Enter choice (1 or 2): ");
    scanf("%d", &choice);

    if (choice == 1) {
        runProblem1();
    } else if (choice == 2) {
        runProblem2();
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}

// --------------- Problem 1 Code -------------------
void runProblem1(void) {
    int listSize;
    printf("\nEnter the size of the list: ");
    scanf("%d", &listSize);

    int *list = (int *)calloc(listSize, sizeof(int));
    srand(time(NULL));
    list[0] = pow(-1, rand() % 2) * (rand() % 50);

    for (int i = 1; i < listSize; i++) {
        list[i] = list[i - 1] + (rand() % 10 + 1); // strictly increasing
    }

    printf("Generated Array A:\n");
    for (int i = 0; i < listSize; i++) {
        printf("A[%d] = %d\n", i, list[i]);
    }

    for (int i = 0; i < listSize; i++) {
        list[i] = list[i] - i; // Convert to B[i] = A[i] - i
    }

    int result = listSearch(list, listSize, 0);
    if (result != -1)
        printf("Found i such that A[i] == i at index: %d\n", result);
    else
        printf("No index i such that A[i] == i was found.\n");

    free(list);
}

int listSearch(int a[], int n, int x) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (a[mid] == x)
            return mid;
        else if (a[mid] < x)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

// --------------- Problem 2 Code -------------------
void runProblem2(void) {
    FILE *fPtr;
    struct tnode *root = NULL;
    int number;

    fPtr = fopen("inputList", "r");
    if (!fPtr) {
        printf("Error: inputList file not found.\n");
        return;
    }

    for (int i = 0; i < SIZE; i++) {
        fscanf(fPtr, "%d", &number);
        root = addtree(root, number);
    }
    fclose(fPtr);

    preOrderCount(root);
    printf("Number of Full Nodes (2 children): %d\n", numberOfFullNodes);
    printf("Number of Partial Nodes (1 child): %d\n", numberOfPartialNodes);
    printf("Number of Leaf Nodes (0 children): %d\n", numberOfLeaves);
}

struct tnode *addtree(struct tnode *p, int x) {
    if (p == NULL) {
        p = talloc();
        p->key = x;
        p->left = p->right = NULL;
    } else if (x < p->key) {
        p->left = addtree(p->left, x);
    } else if (x > p->key) {
        p->right = addtree(p->right, x);
    }
    return p;
}

void preOrderCount(struct tnode *t) {
    if (t != NULL) {
        int hasLeft = (t->left != NULL);
        int hasRight = (t->right != NULL);

        if (hasLeft && hasRight)
            numberOfFullNodes++;
        else if (hasLeft || hasRight)
            numberOfPartialNodes++;
        else
            numberOfLeaves++;

        preOrderCount(t->left);
        preOrderCount(t->right);
    }
}

struct tnode *talloc(void) {
    return (struct tnode *)malloc(sizeof(struct tnode));
}
