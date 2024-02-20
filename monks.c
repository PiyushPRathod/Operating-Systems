#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

unsigned int num_mugs;
unsigned int num_kegs;
sem_t keg_semaphore;
sem_t brew_master_mutex;
int servings_in_keg = 0;
int total_servings = 0; 
int servings_per_monk[10] = {0}; 

void *monk(void *arg) {
    int id = *(int *)arg;
    int servings_drunk = 0;

    while (servings_drunk < num_mugs) {
        sem_wait(&keg_semaphore);
        if (servings_in_keg == 0) {
            sem_wait(&brew_master_mutex);
            usleep(rand() % 1000000);
            servings_in_keg = num_mugs;
            sem_post(&brew_master_mutex);
        }
        servings_in_keg--;
        servings_drunk++;
        total_servings++; 
        servings_per_monk[id]++; 
        sem_post(&keg_semaphore);
        printf("Monk #%d drank!\n", id+1);
        usleep(rand() % 1000000);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("You must input number of mugs per keg and number of kegs\n");
        exit(-1);
    }
    num_mugs = atoi(argv[1]);
    num_kegs = atoi(argv[2]);

    srand(time(NULL));

    pthread_t threads[10];
    int ids[10];

    sem_init(&keg_semaphore, 0, 1);
    sem_init(&brew_master_mutex, 0, 1);

    for (int i = 0; i < 10; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, monk, &ids[i]);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print the number of servings consumed by each monk
    for (int i = 0; i < 10; i++) {
        printf("Monk #%d drank %d mugs of beer!\n", i+1, servings_per_monk[i]);
    }
    printf("The monks drank %d mugs of beer!\n", total_servings); 

    return 0;
}


