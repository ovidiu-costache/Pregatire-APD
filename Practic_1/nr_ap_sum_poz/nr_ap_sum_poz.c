#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
    Subiectul 1 – Număr de apariții și suma pozițiilor
    Dificultate estimată: 5/10
    Să se implementeze, folosind Pthreads, un program paralel cu P thread-uri care determină:
    - Numărul de apariții ale unei valori X într-un vector cu N elemente.
    - Suma pozițiilor la care apare valoarea X.
    Cerințe obligatorii:
    1. Fiecare thread trebuie să parcurgă fragmentul de vector care îi este alocat.
    2. Thread-urile nu au voie să incrementeze un contor global partajat.
    3. Rezultatele vor fi afișate de un singur thread ales de tine, nu de main.
    4. Doar thread-ul care afișează are voie să citească rezultatele parțiale calculate de celelalte thread-uri.
    5. Toate thread-urile trebuie pornite într-o buclă comună, iar implementarea trebuie să maximizeze concurența.
    Date de intrare
    Programul se rulează astfel:
    ./testapd N P X


    Vectorul trebuie inițializat în main() după formula:
    v[i] = (i + 3) % 5;


    Restricții: 0 < N <= 10000, 0 < P <= 8, 0 <= X < 5.
    Exemple de rulare
    $ ./testapd 10 3 3
    2 5

    $ ./testapd 25 2 4
    5 55

    $ ./testapd 3 8 0
    1 2


    Output-ul trebuie să conțină exact două numere separate prin spațiu: numărul aparițiilor și suma indicilor acestora. Fără mesaje suplimentare.
*/

int *arr;
int array_size;
int num_threads;
int x;

int *local_app;
int *local_sum;

pthread_barrier_t barrier;

void *f(void *arg) {
  long id = *(long *)arg;
  
  int start = id * (double) array_size / num_threads;
  int end;

  if ((id + 1) * (double) array_size / num_threads < array_size) {
    end = (id + 1) * (double) array_size / num_threads;
  } else {
    end = array_size;
  }

  int i;

  // Ultimul thread face afisarile
  for (i = start; i < end; i++) {
    // Calculez nr de app si suma
    if (arr[i] == x) {
        local_app[id]++;
        local_sum[id] += i;
    }
  }

  // Am nevoie de bariera pentru a avea rezultatele calculate pana in acest punct
  int r = pthread_barrier_wait(&barrier);
  /*
  if (r) {
    printf("Eroare la asteptarea barierei\n");
    exit(-1);
  }
  */

  // Pot face afisarile
  if (id == num_threads - 1) {
    int app = 0;
    int sum = 0;

    for (int i = 0; i < num_threads; i++) {
        app += local_app[i];
        sum += local_sum[i];
    }

    printf("%d %d\n", app, sum);
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Specificati dimensiunea array-ului, numarul de thread-uri si valoarea lui X\n");
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

  local_app = calloc(num_threads, sizeof(int));
  local_sum = calloc(num_threads, sizeof(int));

  arr = malloc(array_size * sizeof(int));
  for (int i = 0; i < array_size; i++) {
    arr[i] = (i + 3) % 5;
  }

  printf("Vectorul este:\n");
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
  free(local_app);
  free(local_sum);

  return 0;
}