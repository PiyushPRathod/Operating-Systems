#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>
#include <time.h>

int num_patrons;
int num_stools;
sem_t semaphore;
pthread_mutex_t mutex;
int total_waiting_time = 0;
int patrons_served = 0;
int *waiting_times; // Global array to store waiting time for each patron

void *patron_thread(void *arg) {
    int patron_id = *(int*)arg;
    int wait_time;

    // Check if there is an available stool
    sem_wait(&semaphore);

    // Patron has acquired a stool
    pthread_mutex_lock(&mutex);

    // Simulate eating time
    wait_time = rand() % 5000; // Random value between 0 and 5000 milliseconds
    printf("Patron #%d is eating!", patron_id + 1);
    //fflush(stdout);

    // Update total waiting time, number of patrons served, and store the waiting time for this patron
    total_waiting_time += wait_time;
    waiting_times[patron_id] = wait_time;
    patrons_served++;

    // Exit the critical section
    pthread_mutex_unlock(&mutex);

    // Patron has finished eating and left the stool
    sem_post(&semaphore);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("You must input the number of patrons!\n");
        exit(-1);
    }

    num_patrons = atoi(argv[1]);
    num_stools = 100;

    if (num_stools <= 0) {
        printf("Number of stools should be a positive integer.\n");
        exit(-1);
    }

    // Initialize the semaphore and mutex
    sem_init(&semaphore, 0, num_stools);
    pthread_mutex_init(&mutex, NULL);

    // Seed the random number generator
    srand(time(NULL));

    // Create an array to hold thread IDs
    pthread_t patrons[num_patrons];

    // Allocate memory for the waiting times array
    waiting_times = (int*)malloc(num_patrons * sizeof(int));

    // Create threads for each patron
    for (int i = 0; i < num_patrons; i++) {
        int *patron_id = malloc(sizeof(int));
        *patron_id = i;
        if (pthread_create(&patrons[i], NULL, patron_thread, patron_id) != 0) {
            perror("Error creating thread");
            exit(-1);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_patrons; i++) {
        pthread_join(patrons[i], NULL);
    }

    // Destroy the semaphore and mutex
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);

    for (int i = 0; i < num_patrons; i++) {
        printf("Patron #%d waited %d ms\n", i + 1, waiting_times[i]);
    }

    // Calculate and print the average waiting time
    int average_waiting_time = total_waiting_time / patrons_served;
    printf("Average wait time: %d ms\n", average_waiting_time);
    printf("num_patrons: %d\n", num_patrons);

    // Free the memory allocated for waiting_times array
    free(waiting_times);

    return 0;
}

