#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
Să se implementeze, folosind Pthreads, un program paralel pentru sortarea unui vector cu N elemente
prin intermediul Rank Sort, folosind P thread-uri. Rezolvarea se va face maximizând concurența
thread-urilor.
Rezultatul se va printa de către un thread ales de către programator (poate fi thread-ul cu un anumit
ID), dar NU de către thread-ul main. Mai mult, doar acestui thread îi va fi permis să citească
fragmentele parțiale de vector sortate de celelalte thread-uri.
În urma compilării, va rezulta un executabil numit testapd, care va primi ca parametri valorile lui N și
P (în această ordine). Vectorul de sortat va fi inițializat în funcția main ca v[i] = N - i + 5, înainte
de pornirea thread-urilor. Aveți mai jos trei exemple de rulare:
$ ./testapd 10 3
6 7 8 9 10 11 12 13 14 15
$ ./testapd 25 4
6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
$ ./testapd 3 8
6 7 8
Atenție! Toate thread-urile trebuie pornite în același timp (într-o buclă comună). Programul submis nu
trebuie să printeze altceva decât vectorul sortat. Afișarea vectorului sortat se va face dintr-unul din
thread-urile pornite de către voi.
Se presupune că parametrii de intrare sunt corecți (atât ca număr și ordine, cât și ca valori). Valorile
acceptate pentru N și P sunt: 0 < N <= 10000, 0 < P <= 8. Programul trebuie să funcționeze corect
pentru orice combinație de N și P din aceste intervale.
Trebuie să încărcați pe Moodle o arhivă care să conțină în rădăcină fișierele sursă, un Makefile cu
target-ul build și eventual un fișier README.
Atenție! Fișierul README este necesar doar în cazul în care considerați că este nevoie de explicații
adiționale în legătura cu implementarea realizată.
*/

int *arr;
int array_size;
int num_threads;
pthread_barrier_t barrier;
int *sorted;

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
    int rank = 0;

    for (int j = 0; j < array_size; j++) {
        if (arr[j] < arr[i]) {
            rank++;
        }
    }

    sorted[rank] = arr[i];
  }

  int r = pthread_barrier_wait(&barrier);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD) {
    printf("Eroare la asteptarea barierei\n");
    exit(-1);
  }

  if (id == 0) {
    for (int i = 0; i < array_size; i++) {
      printf("%d ", sorted[i]);
    }
    printf("\n");
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Specificati dimensiunea array-ului si numarul de thread-uri\n");
    exit(-1);
  }

  array_size = atoi(argv[1]);
  num_threads = atoi(argv[2]);

  pthread_t threads[num_threads];
  int r;
  long id;
  void *status;
  long ids[num_threads];

  sorted = malloc(array_size * sizeof(int));
  arr = malloc(array_size * sizeof(int));

  for (int i = 0; i < array_size; i++) {
    arr[i] = array_size - i + 5;
    sorted[i] = 0;
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
  free(sorted);

  return 0;
}