
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void buildHeap(int a[], int n);// Function to build a max heap
// Function prototypes for heap sort and quick sort
void heapify(int a[], int i, int n);
void heapSort(int a[], int n);
void quickSort(int a[], int low, int high);
int partition(int a[], int low, int high);
void swap(int *x, int *y);

// Utility to generate random array
void generateArray(int a[], int n, int seed) {
    srand(seed);
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 20001 - 10000;  // to generate numbers in the range [-10000, 10000]
    }
}

// HEAPSORT FUNCTIONS for building the heap and sorting
void buildHeap(int a[], int n) {
    for (int i = n/2 - 1; i >= 0; i--) {
        heapify(a, i, n);
    }
}

void heapify(int a[], int i, int n) { // Function to maintain the heap property so that the largest element is at the root
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if (left < n && a[left] > a[largest]) { // checking if left child is larger than root
        largest = left;
    }
    if (right < n && a[right] > a[largest]) { //if right child is larger than largest so far then update largest
        largest = right;
    }

    if (largest != i) { // If largest is not root, swap and continue heapifying the affected sub-tree 
        swap(&a[i], &a[largest]);
        heapify(a, largest, n);
    }
}

void heapSort(int a[], int n) { // Function to perform heap sort sorting the array
    buildHeap(a, n);
    for (int i = n-1; i > 0; i--) { //moving the largest element to the end of the array because it is sorted
        swap(&a[0], &a[i]);
        heapify(a, 0, i);
    }
}

// QUICKSORT FUNCTIONS
void quickSort(int a[], int low, int high) {
    if (low < high) {  // Base case for recursion, if low is less than high then continue sorting 
        int pi = partition(a, low, high);
        quickSort(a, low, pi - 1);
        quickSort(a, pi + 1, high);
    }
}

int partition(int a[], int low, int high) { // Function to partition the array into two halves based on the pivot
    int pivot = a[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) { // looping through the array to rearrange elements
        if (a[j] < pivot) {
            i++;
            swap(&a[i], &a[j]); //swapping elements if they are smaller than the pivot
        }
    }
    swap(&a[i + 1], &a[high]);
    return (i + 1);
}

// SWAP FUNCTION
void swap(int *x, int *y) { // Function to swap two elements
    int temp = *x;
    *x = *y;
    *y = temp;
}

int main(void) {
    int n; // the size of the array
    scanf("%d", &n);
    int *a = (int *)malloc(n * sizeof(int)); // dynamically allocate memory for the array
    int *b = (int *)malloc(n * sizeof(int));
    generateArray(a, n, 42);  // generating random numbers in the range [-10000, 10000]
    for (int i = 0; i < n; i++) b[i] = a[i]; // copying the array for quicksort since it modifies the original array

    clock_t start, end; // variables to measure time taken by sorting algorithms

    start = clock(); // measuring time for heap sort
    heapSort(a, n); // calling heap sort function
    end = clock();
    printf("HeapSort Time: %lf seconds\n", (double)(end - start) / CLOCKS_PER_SEC); // printing the time taken by heap sort

    start = clock(); // measuring time for quick sort
    quickSort(b, 0, n - 1);
    end = clock();
    printf("QuickSort Time: %lf seconds\n", (double)(end - start) / CLOCKS_PER_SEC); // printing the time taken by quick sort

    free(a); // freeing the dynamically allocated memory
    free(b);
    return 0;
}
