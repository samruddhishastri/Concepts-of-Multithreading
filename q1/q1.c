#define _POSIX_C_SOURCE 199309L
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>
#include<string.h>
#define ll long long int

ll n; int a[1000007], b[1000007], c[1000007];

struct arg{
	int l;
	int r;
	int *array;
};

void swap(int *xp, int *yp)  
{  
    int temp = *xp;  
    *xp = *yp;  
    *yp = temp;  
}

void selectionsort(int arr[], int l, int r)  
{  
    int i, j, min_idx;    
    for (i = l; i < r-1; i++){  
        min_idx = i;  
        for (j = i+1; j < r; j++)  
        if (arr[j] < arr[min_idx])  
            min_idx = j;   
        swap(&arr[min_idx], &arr[i]);  
    }
}

void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
    int L[n1], R[n2]; 
    for (i = 0; i < n1; i++){
        L[i] = arr[l + i];
    }
    for (j = 0; j < n2; j++){ 
        R[j] = arr[m + 1 + j]; 
    }
    i = 0;  
    j = 0;  
    k = l;
    while (i < n1 && j < n2) { 
        if (L[i] <= R[j]) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 

void mergesort(int arr[], int l, int r){
	if (r-l > 4) {
        int m = l + (r - l) / 2;  
        mergesort(arr, l, m); 
        mergesort(arr, m + 1, r); 
        merge(arr, l, m, r); 
    }
    else{
        selectionsort(arr, l, r+1);
    }
}

void concurrent_mergesort(int arr[], int l, int r){
	if (r-l > 4) {
        int m = l + (r - l) / 2;  
        int x = fork();
        if(x==0){
	        concurrent_mergesort(arr, l, m);
	        exit(0);
	    }
	    else{
                int y = fork();
             	if(y==0){
     	        	concurrent_mergesort(arr, m + 1, r);
     	        	exit(0);
     	    	}
     	    	else{
                    int status;
                    waitpid(x, &status, 0);
                    waitpid(y, &status, 0);
         	    	merge(arr, l, m, r);
         	    }
        } 
    }
    else{
    	int x = fork();
        if(x==0){
	        selectionsort(arr, l, r+1);
	        exit(0);
	    }
	    else{
                int status;
	    	    waitpid(x, &status, 0);
	    }
    }	
}

void *threaded_selectionsort(void *a){
    struct arg *args = (struct arg*) a;
    int l = args->l;
    int r = args->r;
    int *arr = args->array;
	int i, j, min_idx;    
    for (i = l; i < r-1; i++){  
        min_idx = i;  
        for (j = i+1; j < r; j++)  
        if (arr[j] < arr[min_idx])  
            min_idx = j;   
        swap(&arr[min_idx], &arr[i]);  
    }
}

void *threaded_mergesort(void *a){
	struct arg *args = (struct arg*) a;
	int l = args->l;
    int r = args->r;
    int *arr = args->array;

	if (r-l > 4) {
        int m = l + (r - l) / 2;  
		struct arg a_prime1;
		a_prime1.l = l;
    	a_prime1.r = m;
    	a_prime1.array = arr;
    	pthread_t tid1;
    	pthread_create(&tid1, NULL, threaded_mergesort, &a_prime1);

    	struct arg a_prime2;
		a_prime2.l = m+1;
    	a_prime2.r = r;
    	a_prime2.array = arr;
    	pthread_t tid2;
    	pthread_create(&tid2, NULL, threaded_mergesort, &a_prime2);
       
        pthread_join(tid1, NULL);
    	pthread_join(tid2, NULL);
        
        merge(arr, l, m, r); 
    }
    else{
        struct arg a_prime1;
		a_prime1.l = l;
    	a_prime1.r = r + 1;
    	a_prime1.array = arr;
    	pthread_t tid1;
    	pthread_create(&tid1, NULL, threaded_selectionsort, &a_prime1);
    	pthread_join(tid1, NULL);
    }
}

int main(){
	
    struct timespec ts;
	double t;
	scanf("%lld", &n);

    //create shared memory for concurrent mergesort
    key_t mem_key = IPC_PRIVATE;
    int shm_id = shmget(mem_key, sizeof(int)*(n+1), IPC_CREAT | 0666); 
    int *arr = shmat(shm_id, NULL, 0);

    //scan the input array
    for(int i=0; i<n; i++){
        scanf("%d", &a[i]);
        arr[i] = a[i];
        c[i] = a[i];
    }
	printf("\n");
    //concurrent mergesort
	printf("\033[1;36mConcurrent Mergesort: \n\033[0m");
	
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

	concurrent_mergesort(arr, 0, n-1);

	for(int i=0; i<n; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("\033[1;35mTime required:  %Lf\n\033[0m", en - st);
    long double t1 = en-st;

	printf("\n");

    //Threaded mergesort
	printf("\033[1;36mThreaded Mergesort: \n\033[0m");
	pthread_t tid;
	struct arg x;
	x.l = 0;
    x.r = n-1;
    x.array = c;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;
	pthread_create(&tid, NULL, threaded_mergesort, &x);
	pthread_join(tid, NULL);
	
	for(int i=0; i<n; i++){
        printf("%d ",x.array[i]);
    }
	printf("\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("\033[1;35mTime required: %Lf\n\033[0m", en - st);
    long double t2 = en-st;

    //normal mergesort
    printf("\n");
    printf("\033[1;36mNormal Mergesort: \n\033[0m");
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;

    mergesort(a, 0, n-1);

    for(int i=0; i<n; i++){
        printf("%d ", a[i]);
    }
    printf("\n");
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    printf("\033[1;35mTime required:  %Lf\n\n\033[0m", en - st);
    long double t3 = en-st;

	char fastest_sort[100]; double time;

	if(t1 < t2  && t1 < t3){
		strcpy(fastest_sort, "Concurrent Mergesort");
		time = t1;
	}
	if(t2 < t1  && t2 < t3){
		strcpy(fastest_sort, "Threaded Mergesort");
		time = t2;
	}
	if(t3 < t1  && t3 < t2){
		strcpy(fastest_sort, "Normal Mergesort");
		time = t3;
	}

	printf("\033[1;34mFastest: %s\n\n\033[0m", fastest_sort);
	printf("Ratio: \n");
	printf("(Concurrent mergesort : fastest_sort) : %Lf\n", t1/time);
	printf("(Threaded mergesort : fastest_sort) : %Lf\n", t2/time);
	printf("(Normal mergesort : fastest_sort) : %Lf\n", t3/time);

}