#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
    Fiecare thread face o operatie, unul singur afiseaza (nu main) si are voie sa 
    citeasca valorile calculate de celelalte thread-uri
*/

int *arr;
int array_size;
int num_threads;
unsigned long *calcule;

pthread_barrier_t barrier;

void *f(void *arg) {
  long id = *(long *)arg;
  
  for (int i = 0; i < array_size; i++) {
    if (id == 0) {
        calcule[0] += arr[i];
    } else if (id == 1) {
        calcule[1] *= arr[i];
    } else if (id == 2) {
        if (arr[i] < calcule[2]) {
            calcule[2] = arr[i];
        }
    } else if (id == 3) {
        if (arr[i] > calcule[3]) {
            calcule[3] = arr[i];
        }
    } else if (id == 4) {
        calcule[4] += arr[i];
    }
  }

  int r = pthread_barrier_wait(&barrier);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD) {
    printf("Eroare la asteptarea barierei\n");
    exit(-1);
  }

  if (id == num_threads -1) {
    for (int i = 0; i < num_threads; i++) {
        if (i == 0) {
            printf("sum: %ld\n", calcule[0]);
        } else if (i == 1) {
            printf("prod: %ld\n", calcule[1]);
        } else if (i == 2) {
            printf("min: %ld\n", calcule[2]);
        } else if (i == 3) {
            printf("max: %ld\n", calcule[3]);
        } else if (i == 4) {
            printf("avg: %f\n", (double) calcule[4] / (double) array_size);
        }
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Specificati dimensiunea array-ului, numarul de thread-uri si elementele vectorului\n");
    exit(-1);
  }

  array_size = atoi(argv[1]);
  num_threads = atoi(argv[2]);
  int j = 3;
  arr = malloc(array_size * sizeof(int));
  for (int i = 0; i < array_size; i++) {
    arr[i] = atoi(argv[j++]);
  }
  
  calcule = calloc(num_threads, sizeof(unsigned long));
  if (num_threads == 2) {
    calcule[1] = 1;
  }
  if (num_threads == 3) {
    calcule[1] = 1;
    calcule[2] = arr[0];
  }
  if (num_threads == 4) {
    calcule[1] = 1;
    calcule[2] = arr[0];
    calcule[3] = arr[0];
  }
  if (num_threads == 5) {
    calcule[1] = 1;
    calcule[2] = arr[0];
    calcule[3] = arr[0];
  }
  
  pthread_t threads[num_threads];
  int r;
  long id;
  void *status;
  long ids[num_threads];

  printf("[DEBUG] Vectorul citit:\n");
  for (int i = 0; i < array_size; i++) {
    printf("%d", arr[i]);
    if (i != array_size - 1) {
      printf(" ");
    } else {
      printf("\n");
    }
  }

  r = pthread_barrier_init(&barrier, NULL, num_threads);
  if (r) {
    printf("Eroare la crearea barierei\n");
    exit(-1);
  }

  for (id = 0; id < num_threads; id++) {
    ids[id] = id;
    r = pthread_create(&threads[id], NULL, f, &ids[id]);

    if (r) {
      printf("Eroare la crearea thread-ului %ld\n", id);
      exit(-1);
    }
  }

  for (id = 0; id < num_threads; id++) {
    r = pthread_join(threads[id], &status);

    if (r) {
      printf("Eroare la asteptarea thread-ului %ld\n", id);
      exit(-1);
    }
  }

  r = pthread_barrier_destroy(&barrier);
  if (r) {
    printf("Eroare la distrugerea barierei\n");
    exit(-1);
  }

  free(arr);

  return 0;
}