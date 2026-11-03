#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
    Scrie un program cu P thread-uri care numără aparițiile unei valori X într-un 
    vector de N elemente. Fiecare thread procesează propriul interval, iar main adună 
    rezultatele după pthread_join().
*/

int *arr;
int array_size;
int num_threads;
int x;
int *partial_apps;

void *f(void *arg) {
  long id = *(long *)arg;
  
  int start = id * (double) array_size / num_threads;
  int end;

  if ((id + 1) * (double) array_size / num_threads < array_size) {
    end = (id + 1) * (double) array_size / num_threads;
  } else {
    end = array_size;
  }

  for (int i = start; i < end; i++) {
    if (arr[i] == x) {
        partial_apps[id]++;
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Specificati dimensiunea array-ului, numarul de thread-uri si valoarea X\n");
    exit(-1);
  }

  array_size = atoi(argv[1]);
  num_threads = atoi(argv[2]);
  x = atoi(argv[3]);

  pthread_t threads[num_threads];
  int r;
  long id;
  void *status;
  long ids[num_threads];
  int sum_apps = 0;

  partial_apps = calloc(num_threads, sizeof(int));

  arr = malloc(array_size * sizeof(int));
  for (int i = 0; i < array_size; i++) {
    if (i % 9 == 0) {
        arr[i] = x;
    } else {
        arr[i] = i;
    }
  }

  for (int i = 0; i < array_size; i++) {
    printf("%d", arr[i]);
    if (i != array_size - 1) {
      printf(" ");
    } else {
      printf("\n");
    }
  }

  // TODO: aceasta operatie va fi paralelizata cu num_threads fire de executie
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

  for (int i = 0; i < array_size; i++) {
    printf("%d", arr[i]);
    if (i != array_size - 1) {
      printf(" ");
    } else {
      printf("\n");
    }
  }

  for (int i = 0; i < num_threads; i++) {
    sum_apps += partial_apps[i];
  }

  printf("Nr. de aparitii ale lui X = %d este %d\n", x, sum_apps);

  free(arr);
  free(partial_apps);

  return 0;
}