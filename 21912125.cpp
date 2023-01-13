/*21912125 조관우*/

#include <stdio.h>
#include <stdlib.h>

struct FIFOScheduling {
    int arrTime;
    int teskVolume;
    int priority;
};

struct PRIScheduling {
    int arrTime;
    int teskVolume;
    int priority;
    int remainTesk;
    int runTime;
    int pointer;
};

struct PriInfo {
    int priCount;
    int waitTime;
    double aveWaitTime;
};

#define MAX_QUEUE_SIZE 1000

void FIFOscheduling(struct FIFOScheduling* sche_arr, int size);
void Priorityscheduling(struct PRIScheduling* sche_arr, int size);
void addQueue(struct FIFOScheduling data, struct FIFOScheduling* queue, int* rear, int* front);
struct FIFOScheduling delQueue(struct FIFOScheduling* queue, int* rear, int* front);
bool isQueueEmpty(int* rear, int* front);
void QueueFull();
void QueueEmpty();
void insertMaxHeap(struct PRIScheduling* heap, struct PRIScheduling data, int* n, int size);
struct PRIScheduling delMaxHeap(struct PRIScheduling* heap, int* n);
bool comparePriority(struct PRIScheduling data1, PRIScheduling data2);
bool isHeapFull(int &n, int size);
bool isHeapEmpty(int &n);

int main(void) {
    int size;
    char filename[100];
    struct FIFOScheduling* sche_arr;
    struct PRIScheduling* pri_sche_arr;

    printf("파일 이름을 입력하세요.");
    scanf("%s", filename);

    FILE* fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("파일 열기 실패\n");
        return 1;
    }
    fscanf(fp, "%d", &size);
    
    sche_arr = (struct FIFOScheduling*)malloc(size * sizeof(struct FIFOScheduling));
    pri_sche_arr = (struct PRIScheduling*)malloc(size * sizeof(struct PRIScheduling));

    while (!feof(fp)) {
        for (int i = 0; i < size; i++) {
            fscanf(fp, "%d %d %d", &sche_arr[i].arrTime, &sche_arr[i].teskVolume, &sche_arr[i].priority);
        }
    }

    for (int i = 0; i < size; i++) {
        pri_sche_arr[i].arrTime = sche_arr[i].arrTime;
        pri_sche_arr[i].teskVolume = sche_arr[i].teskVolume;
        pri_sche_arr[i].priority = sche_arr[i].priority;
        pri_sche_arr[i].remainTesk = pri_sche_arr[i].teskVolume;
        pri_sche_arr[i].runTime = 0;
        pri_sche_arr[i].pointer = i;
    }

    FIFOscheduling(sche_arr, size);
    Priorityscheduling(pri_sche_arr, size);

}

void FIFOscheduling(struct FIFOScheduling* sche_arr, int size) {
    int rear = -1, front = -1, endTime, startTime, runtime = 0, totalWaitTime = 0, compareIndex = 1;
    struct FIFOScheduling queue[MAX_QUEUE_SIZE], data;
    endTime = sche_arr[0].arrTime + sche_arr[0].teskVolume;
    runtime += endTime - sche_arr[0].arrTime;
    for (int i = 1; i < size; i++) {
        while (endTime > sche_arr[compareIndex].arrTime) {
            addQueue(sche_arr[compareIndex], queue, &rear, &front);
            compareIndex++;
        }
        if (!isQueueEmpty(&rear, &front)) {
            totalWaitTime += endTime - sche_arr[i].arrTime;
            endTime += sche_arr[i].teskVolume;
            runtime += endTime - sche_arr[i].arrTime;
        }
        else {
            data = delQueue(queue, &rear, &front);
            totalWaitTime += endTime - data.arrTime;
            endTime += data.teskVolume;
            runtime += endTime - data.arrTime;
        }
    }
    double avg_runtime = (double)runtime / size;
    double avg_waitTime = (double)totalWaitTime / size;
    printf("FIFO Scheduling의 실행 결과 :\n");
    printf("\t작업 수 : %d 종료시간:%d 평균 실행시간 : %.2f 평균 대기시간 : %.2f\n", size, endTime, avg_runtime, avg_waitTime);
}

void Priorityscheduling(struct PRIScheduling* pri_sche_arr, int size) {
    struct PriInfo* priArr;
    priArr = (struct PriInfo*)calloc(10, sizeof(PriInfo));
    struct PRIScheduling *heap;
    struct PRIScheduling item;
    int n = 0, presentTime = pri_sche_arr[0].arrTime, totTime = pri_sche_arr[0].arrTime, totrunTime = 0;
    heap = (struct PRIScheduling*)malloc(size * sizeof(struct PRIScheduling));
    
    insertMaxHeap(heap, pri_sche_arr[0], &n, size);
    priArr[pri_sche_arr[0].priority - 1].priCount++;

    for (int i = 1; i < size; i++) {
        priArr[pri_sche_arr[i].priority - 1].priCount++;
        while (true) {
            if (isHeapEmpty(n)) {
                insertMaxHeap(heap, pri_sche_arr[i], &n, size);
                presentTime = pri_sche_arr[i].arrTime;
                break;
            }
            if ((heap[1].remainTesk + presentTime) <= pri_sche_arr[i].arrTime) {
                item = delMaxHeap(heap, &n);
                presentTime += item.remainTesk;
                pri_sche_arr[item.pointer].runTime = presentTime - item.arrTime;
                priArr[item.priority - 1].waitTime += pri_sche_arr[item.pointer].runTime - item.teskVolume;
            }
            else {
                heap[1].remainTesk -= pri_sche_arr[i].arrTime - presentTime;
                presentTime = pri_sche_arr[i].arrTime;
                insertMaxHeap(heap, pri_sche_arr[i], &n, size);
                break;
            }
        }
    }
    
    while (!isHeapEmpty(n)) {
        item = delMaxHeap(heap, &n);
        presentTime += item.remainTesk;
        pri_sche_arr[item.pointer].runTime = presentTime - item.arrTime;
        priArr[item.priority - 1].waitTime += pri_sche_arr[item.pointer].runTime - item.teskVolume;
    }
    
    for (int i = 0; i < size; i++) {
        totrunTime += pri_sche_arr[i].runTime;
        totTime += pri_sche_arr[i].teskVolume;
    }

    double avg_runtime = (double)totrunTime / size;
    printf("Priority Scheduling의 실행 결과: \n");
    printf("\t작업수 = %d, 종료시간 = %d, 평균 실행시간 = %.2f\n\n", size, presentTime, avg_runtime);
    for (int i = 9; i >= 0; i--) {
        if (priArr[i].priCount != 0) {
            priArr[i].aveWaitTime = (double)priArr[i].waitTime / priArr[i].priCount;
            printf("\t우선순위 %d: 작업수 = %d, 평균 대기시간 = %.2f\n", i + 1, priArr[i].priCount, priArr[i].aveWaitTime);
        }
    }
}

void addQueue(struct FIFOScheduling data, struct FIFOScheduling *queue, int* rear, int* front) {
    *rear = (*rear + 1) % MAX_QUEUE_SIZE;
    if (isQueueEmpty(rear, front)) {
        QueueFull();
        return;
    }
    queue[*rear] = data;
}

struct FIFOScheduling delQueue(struct FIFOScheduling *queue, int* rear, int* front) {
    if (*front == *rear) {
        QueueEmpty();
        exit(1);
    }
    *front = (*front + 1) % MAX_QUEUE_SIZE;
    return queue[*front];
}
bool isQueueEmpty(int* rear, int* front) {
    if (*rear == *front)
        return true;
    else
        return false;
}

void QueueFull() {
    printf("Queue full!");
}

void QueueEmpty() {
    printf("Queue empty!");
}

void insertMaxHeap(struct PRIScheduling* heap, PRIScheduling data, int* n, int size) {
    int i;
    if (isHeapFull(*n, size)) {
        printf("Heap full!");
        exit(1);
    }
    i = ++(*n);
    while ((i != 1) && comparePriority(data, heap[i/2])) {
        heap[i] = heap[i / 2];
        i /= 2;
    }
    heap[i] = data;
}

struct PRIScheduling delMaxHeap(struct PRIScheduling* heap, int* n) {
    int parent = 1, child = 2;
    struct PRIScheduling item, temp;
    if (isHeapEmpty(*n)) { 
        printf("Heap empty!");
        exit(1); 
    }
    item = heap[1];
    temp = heap[(*n)--];
    while (child <= *n) {
        if ((child < *n) && comparePriority(heap[child+1], heap[child]))
            child++;
        if (comparePriority(temp, heap[child]))
            break;
        heap[parent] = heap[child];
        parent = child;
        child *= 2;
    }
    heap[parent] = temp;
    return item;
}

bool comparePriority(struct PRIScheduling data1, struct PRIScheduling data2) {
    if (data1.priority > data2.priority)
        return true;
    else if ((data1.priority == data2.priority) && (data1.remainTesk < data2.remainTesk))
        return true;
    else if ((data1.priority == data2.priority) && (data1.remainTesk == data2.remainTesk) && (data1.arrTime < data2.arrTime))
        return true;
    else
        return false;
}
bool isHeapFull(int &n, int size) {
    if (n == size)
        return true;
    else
        return false;
}
bool isHeapEmpty(int &n) {
    if (n > 0)
        return false;
    else
        return true;
}