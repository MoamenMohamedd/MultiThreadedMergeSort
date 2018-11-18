#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *mergeSort(void *args);

void merge(int left, int mid, int right, int *array);

struct recursiveStep {
    int low;
    int high;
    int *array;
};

int main() {
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


    struct recursiveStep *step = (struct recursiveStep *) malloc(sizeof(struct recursiveStep));
    step->low = 0;
    step->high = n - 1;
    step->array = array;

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc = pthread_create(&tid, &attr, mergeSort, (void *) step);
    if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    pthread_attr_destroy(&attr);
    rc = pthread_join(tid, NULL);
    if (rc) {
        printf("ERROR; return code from pthread_join() is %d\n", rc);
        exit(-1);
    }


    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }

    free(array);

    return 0;
}


void *mergeSort(void *args) {

    struct recursiveStep *step = (struct recursiveStep *) args;

    if (step->low < step->high) {
        int mid = (step->low + step->high) / 2;

        pthread_t tidLeft, tidRight;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        struct recursiveStep *stepLeft = (struct recursiveStep *) malloc(sizeof(struct recursiveStep));
        stepLeft->low = step->low;
        stepLeft->high = mid;
        stepLeft->array = step->array;

        int rc = pthread_create(&tidLeft, &attr, mergeSort, (void *) &stepLeft);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        struct recursiveStep *stepRight = (struct recursiveStep *) malloc(sizeof(struct recursiveStep));
        stepRight->low = mid + 1;
        stepRight->high = step->high;
        stepRight->array = step->array;

        rc = pthread_create(&tidRight, &attr, mergeSort, (void *) &stepRight);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        pthread_attr_destroy(&attr);
        rc = pthread_join(tidLeft, NULL);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        rc = pthread_join(tidRight, NULL);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }

        merge(step->low, mid, step->high, step->array);

    }

    pthread_exit(NULL);
}

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