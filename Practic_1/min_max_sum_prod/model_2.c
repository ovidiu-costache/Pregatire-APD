#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
    Scrieți un program paralel folosind PThreads cu exact 4 thread-uri care calculează:
    minimul, maximul, suma și produsul elementelor dintr-un vector cu N elemente.
    Atenție! Implementarea va maximiza paralelismul. O soluție care serializează părți importante
    din execuție va fi notată cu 0.
    Atenție! Fiecare thread va afișa exact una dintre cele 4 valori.
    Atenție! Unui thread îi este interzis să citească orice valoare calculată de un alt thread.
    Atenție! Thread-ul main va face doar inițializarea vectorului, pornirea și așteptarea thread-urilor
    (nu va face afișări).
    Atenție! Afișarea celor 4 valori se va face întotdeuna în ordinea minim, maxim, sumă, produs.
    Elementele vectorului vor fi inițializate cu v[i] = rand() % 100, după apelarea srand(constantă).
    Produsul va fi afișat modulo 1337.
    Exemplu de rulare:
    ./statistici 20
    minim = 4
    maxim = 95
    suma = 864
    produs = 336
*/

int *arr;
int array_size;
int num_threads;
long long *operations;
pthread_barrier_t barrier;
pthread_mutex_t mutex;
int turn;

void *f(void *arg) {
  long id = *(long *)arg;
  
  for (int i = 0; i < array_size; i++) {
    if (id == 0) {
        if (arr[i] < operations[0]) {
            operations[0] = arr[i];
        }
    } else if (id == 1) {
        if (arr[i] > operations[1]) {
            operations[1] = arr[i];
        }
    } else if (id == 2) {
        operations[2] += arr[i];
    } else if (id == 3) {
        operations[3] = (operations[3] * arr[i]) % 1337;
    }
  }

  int r = pthread_barrier_wait(&barrier);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD) {
    printf("Eroare la asteptarea barierei\n");
    exit(-1);
  }

  while (1) {
    r = pthread_mutex_lock(&mutex);
    if (r) {
      printf("Eroare la blocarea mutex-ului\n");
      exit(-1);
    }

    if (turn == id) {
      if (id == 0) {
          printf("minim = %lld\n", operations[0]);
      } else if (id == 1) {
          printf("maxim = %lld\n", operations[1]);
      } else if (id == 2) {
          printf("suma = %lld\n", operations[2]);
      } else if (id == 3) {
          printf("produs = %lld\n", operations[3] % 1337);
      }

      r = pthread_mutex_unlock(&mutex);
      if (r) {
        printf("Eroare la deblocarea mutex-ului\n");
        exit(-1);
      }

      turn++;

      break;
    }

    r = pthread_mutex_unlock(&mutex);
    if (r) {
      printf("Eroare la deblocarea mutex-ului\n");
      exit(-1);
    }

    // turn++;
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Specificati dimensiunea array-ului\n");
    exit(-1);
  }

  array_size = atoi(argv[1]);
  num_threads = 4;

  pthread_t threads[num_threads];
  int r;
  long id;
  void *status;
  long ids[num_threads];

  arr = malloc(array_size * sizeof(int));
  srand(4);
  for (int i = 0; i < array_size; i++) {
    arr[i] = rand() % 100;
    printf("%d ", arr[i]);
  }
  printf("\n");

  operations = malloc(4 * sizeof(long long));
  operations[0] = arr[0]; // minim
  operations[1] = arr[0]; // maxim
  operations[2] = 0; // suma
  operations[3] = 1; // produs

  r = pthread_mutex_init(&mutex, NULL);
  if (r) {
    printf("Eroare la crearea mutex-ului\n");
    exit(-1);
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

  r = pthread_mutex_destroy(&mutex);
  if (r) {
    printf("Eroare la distrugerea mutex-ului\n");
    exit(-1);
  }

  r = pthread_barrier_destroy(&barrier);
  if (r) {
    printf("Eroare la distrugerea barierei\n");
    exit(-1);
  }

  return 0;
}