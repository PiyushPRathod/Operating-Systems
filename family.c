#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_FAMILY_MEMBERS 100

unsigned int num_family_members;
sem_t chopsticks[MAX_FAMILY_MEMBERS];

void talk() {
    // Talk
    usleep(rand() % 1000000);
}

void getchopsticks(int id) {
    // Pick up chopsticks
    sem_wait(&chopsticks[id]);
    sem_wait(&chopsticks[(id + 1) % num_family_members]);
}

void eat(int id) {
    printf("Family member #%d is eating!\n", id);
    // Eat
    usleep(rand() % 1000000);
}

void putchopsticks(int id) {
    // Put down chopsticks
    sem_post(&chopsticks[id]);
    sem_post(&chopsticks[(id + 1) % num_family_members]);
}

void *family_member(void *arg) {
    int id = *(int *)arg;
    while (1) {
        talk();
        getchopsticks(id);
        eat(id);
        putchopsticks(id);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("You must input the number of family members!\n");
        exit(-1);
    }
    num_family_members = atoi(argv[1]);

    srand(time(NULL)); // Seed the random number generator

    pthread_t threads[num_family_members];
    int ids[MAX_FAMILY_MEMBERS];

    // Initialize semaphores for chopsticks
    for (int i = 0; i < num_family_members; i++) {
        sem_init(&chopsticks[i], 0, 1); // Initialize all chopsticks to 1 
    }

    // Create family member threads
    for (int i = 0; i < num_family_members; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, family_member, &ids[i]);
    }

    // Wait for family member threads to finish 
    for (int i = 0; i < num_family_members; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

