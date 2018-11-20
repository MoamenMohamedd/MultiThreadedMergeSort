#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *mergeSort(void *args);
void merge(int left, int mid, int right, int *array);

//struct for holding data at each recursive step
struct recursiveStep {
    int low;
    int high;
    int *array;
};

int main() {

    //read input file
    FILE *pInput = fopen("input.txt", "r");
    if (pInput == NULL) {
        printf("File not found");
        exit(0);
    }

    int n;

    fscanf(pInput, "%d", &n);

    int *array = (int *) malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++) {
        fscanf(pInput, "%d", &array[i]);
    }

    fclose(pInput);




    //creating the first thread that will start merge sort
    struct recursiveStep *step = (struct recursiveStep *) malloc(sizeof(struct recursiveStep));
    step->low = 0;
    step->high = n - 1;
    step->array = array;

    //unique thread identifier
    pthread_t tid;

    //attributes for thread creation
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    //specify that the thread will be joined later
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //create the thread
    int rc = pthread_create(&tid, &attr, mergeSort, (void *) step);
    if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    //free attr
    pthread_attr_destroy(&attr);

    //join created thread with main thread
    rc = pthread_join(tid, NULL);
    if (rc) {
        printf("ERROR; return code from pthread_join() is %d\n", rc);
        exit(-1);
    }


    //print sorted array
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }

    free(array);

    return 0;
}

/**
 * recursive function of merge sort
 * this is the routine each thread will execute
 *
 */
void *mergeSort(void *args) {

    struct recursiveStep *step = (struct recursiveStep *) args;

    //continue only if there is more than 2 elements
    if (step->low < step->high) {

        //finding middle element
        int mid = (step->low + step->high) / 2;

        //create two threads one to handle left part of array and other for the right part
        //two unique thread identifiers
        pthread_t tidLeft, tidRight;

        //state that the bothe the threads will be joined later after creation
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        //initialize recursive step for the left thread
        struct recursiveStep *stepLeft = (struct recursiveStep *) malloc(sizeof(struct recursiveStep));
        stepLeft->low = step->low;
        stepLeft->high = mid;
        stepLeft->array = step->array;

        //create the left thread
        int rc = pthread_create(&tidLeft, &attr, mergeSort, (void *) stepLeft);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        //initialize recursive step for the right thread
        struct recursiveStep *stepRight = (struct recursiveStep *) malloc(sizeof(struct recursiveStep));
        stepRight->low = mid + 1;
        stepRight->high = step->high;
        stepRight->array = step->array;

        //create the right thread
        rc = pthread_create(&tidRight, &attr, mergeSort, (void *) stepRight);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        //free attr
        pthread_attr_destroy(&attr);

        //join left thread with thread which created it
        rc = pthread_join(tidLeft, NULL);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }

        //join right thread with thread which created it
        rc = pthread_join(tidRight, NULL);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }

        //merge left and right
        merge(step->low, mid, step->high, step->array);

        //free this recursive step
        free(step);

    }

    pthread_exit(NULL);
}

/**
 * merge function for merge sort algorithm
 *
 * @param left
 * @param mid
 * @param right
 * @param array
 */
void merge(int left, int mid, int right, int *array) {

    int sizeLeft = mid - left + 1;
    int sizeRight = right - mid;

    int leftArray[sizeLeft];
    int rightArray[sizeRight];

    for (int i = 0; i < sizeLeft; i++)
        leftArray[i] = array[left + i];
    for (int j = 0; j < sizeRight; j++)
        rightArray[j] = array[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < sizeLeft && j < sizeRight) {
        if (leftArray[i] <= rightArray[j]) {
            array[k] = leftArray[i];
            i++;
        } else {
            array[k] = rightArray[j];
            j++;
        }

        k++;
    }

    while (i < sizeLeft) {
        array[k] = leftArray[i];
        i++;
        k++;
    }

    while (j < sizeRight) {
        array[k] = rightArray[j];
        j++;
        k++;
    }

}