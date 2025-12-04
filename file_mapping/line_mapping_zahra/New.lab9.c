#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MIN_VALUE   -10000
#define MAX_VALUE    10000
#define DEFAULT_SEED 42

// function prototypes
void fillRandomArray(int a[], int n, int seed);
void copyArray(int dest[], const int src[], int n);

void buildMaxHeap(int a[], int n);
void maxHeapify(int a[], int index, int heapSize);
void heapSort(int a[], int n);

void quickSortRecursive(int a[], int low, int high);
int  partitionMiddlePivot(int a[], int low, int high);

static void swapInts(int *x, int *y);
static void maybePrintArray(const int a[], int n, const char *label);

// Utility: generate random array in [MIN_VALUE, MAX_VALUE]
void fillRandomArray(int a[], int n, int seed) {
    srand(seed);
    for (int i = 0; i < n; i++) {
        int range = MAX_VALUE - MIN_VALUE + 1;
        a[i] = MIN_VALUE + rand() % range;
    }
}

// Utility: copy one array into another
void copyArray(int dest[], const int src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

/* ===================== HEAPSORT IMPLEMENTATION ===================== */

void buildMaxHeap(int a[], int n) {
    // build heap from the bottom non-leaf nodes upwards
    for (int i = n / 2 - 1; i >= 0; i--) {
        maxHeapify(a, i, n);
    }
}

// maintain max-heap property so that a[index] is larger than its children
void maxHeapify(int a[], int index, int heapSize) {
    int largest = index;
    int left    = 2 * index + 1;
    int right   = 2 * index + 2;

    if (left < heapSize && a[left] > a[largest]) {
        largest = left;
    }
    if (right < heapSize && a[right] > a[largest]) {
        largest = right;
    }

    if (largest != index) {
        swapInts(&a[index], &a[largest]);
        maxHeapify(a, largest, heapSize);
    }
}

void heapSort(int a[], int n) {
    buildMaxHeap(a, n);

    for (int end = n - 1; end > 0; end--) {
        // move current max to the end
        swapInts(&a[0], &a[end]);
        // restore max-heap property on the reduced heap
        maxHeapify(a, 0, end);
    }
}

/* ===================== QUICKSORT IMPLEMENTATION ===================== */

// recursive quicksort using a middle-element pivot
void quickSortRecursive(int a[], int low, int high) {
    if (low < high) {
        int pivotIndex = partitionMiddlePivot(a, low, high);
        quickSortRecursive(a, low, pivotIndex - 1);
        quickSortRecursive(a, pivotIndex + 1, high);
    }
}

int partitionMiddlePivot(int a[], int low, int high) {
    // choose the middle element as pivot instead of always last
    int mid   = low + (high - low) / 2;
    int pivot = a[mid];

    // move pivot to end for standard Lomuto-like scheme
    swapInts(&a[mid], &a[high]);

    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (a[j] < pivot) {
            i++;
            swapInts(&a[i], &a[j]);
        }
    }
    swapInts(&a[i + 1], &a[high]);
    return i + 1;
}

/* ===================== SUPPORT FUNCTIONS ===================== */

static void swapInts(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

// only print array when it is reasonably small (for debugging)
static void maybePrintArray(const int a[], int n, const char *label) {
    if (n > 20) return;

    printf("%s", label);
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

/* ===================== MAIN ===================== */

int main(void) {
    int n;

    printf("Enter array size: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid size.\n");
        return 1;
    }

    int *heapArr  = (int *)malloc(n * sizeof(int));
    int *quickArr = (int *)malloc(n * sizeof(int));
    if (!heapArr || !quickArr) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(heapArr);
        free(quickArr);
        return 1;
    }

    // fill with random numbers and make a copy
    fillRandomArray(heapArr, n, DEFAULT_SEED);
    copyArray(quickArr, heapArr, n);

    maybePrintArray(heapArr, n, "Original: ");

    clock_t start, end;

    /* ---- HeapSort timing ---- */
    start = clock();
    heapSort(heapArr, n);
    end = clock();
    printf("HeapSort Time: %lf seconds\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    maybePrintArray(heapArr, n, "After HeapSort: ");

    /* ---- QuickSort timing ---- */
    start = clock();
    quickSortRecursive(quickArr, 0, n - 1);
    end = clock();
    printf("QuickSort Time: %lf seconds\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    maybePrintArray(quickArr, n, "After QuickSort: ");

    free(heapArr);
    free(quickArr);

    return 0;
}
