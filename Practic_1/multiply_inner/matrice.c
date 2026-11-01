#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
    Plecând de la implementarea naivă a algoritmului de înmulțire a două matrici (C = A x B):
    for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
    c[i][j] = 0;
    for(int k = 0; k < n; k++) {
    c[i][j] += a[i][k] * b[k][j];
    }
    }
    }
    a) (7p) Implementați un program paralel (folosind PThreads) pentru înmulțirea a două
    matrici pătratice de dimensiuni N x N, folosind P thread-uri.
    Atenție! Descopunerea problemei se va face obligatoriu prin împărțirea for-ului interior
    (cel cu k);
    Atenție! Programul se va rula de forma ./matrice N P și va trebui să funcționeze pentru
    orice valori ale lui N și P, afișând matricea C.
    Atenție! Implementarea va maximiza paralelismul. O soluție care serializează părți
    importante din execuție va fi notată cu 0.
    b) (3p) Pe baza rulării soluției voastre cu 1, 2, 3 și 4 thread-uri, comentați (în scris ->
    README.txt) scalabilitatea soluției. Pentru fiecare configurație rulată veți nota: timpii
    relevanți, accelerația obținută și eficiența determinată.
    Atenție! Alegeți o dimensiune a problemei astfel încât timpii de execuție să fie relevanți.
    Matricile A și B se vor inițializa după formulele:
    a[i][j] = 7 + (i + j) % 100;
    b[i][j] = 50 + (i + j) % 100;
    Programul trebuie să funcționeze pentru orice valori ale lui N și P.
    Restricție practică: N <= 1500
    Punctajul pentru cerința B este condiționat de o soluție corectă la cerință A.
*/

int N;
int P;
int **a;
int **b;
int **c;
pthread_mutex_t mutex;

void *thread_function(void *arg)
{
	int thread_id = *(int *)arg;

    int start = thread_id * (double) N / P;
    int end;

    if ((thread_id + 1) * (double) N / P < N) {
        end = (thread_id + 1) * (double) N / P;
    } else {
        end = N;
    }

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
            int sum = 0;
			for (int k = start; k < end; k++) {
				sum += a[i][k] * b[k][j];
			}

            int r = pthread_mutex_lock(&mutex);
            if (r) {
                printf("Eroare la blocarea mutex-ului\n");
                exit(-1);
            }

            c[i][j] += sum;

            r = pthread_mutex_unlock(&mutex);
            if (r) {
                printf("Eroare la deblocarea mutex-ului\n");
                exit(-1);
            }
		}
	}

	pthread_exit(NULL);
}

void get_args(int argc, char **argv)
{
	if(argc < 3) {
		printf("Numar insuficient de parametri: ./program N P\n");
		exit(1);
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
}

void init()
{
	a = malloc(sizeof(int *) * N);
	b = malloc(sizeof(int *) * N);
	c = malloc(sizeof(int *) * N);

	if (a == NULL || b == NULL || c == NULL) {
		printf("Eroare la malloc!");
		exit(1);
	}

	int i, j;
	for (i = 0; i < N; i++) {
		a[i] = malloc(sizeof(int) * N);
		b[i] = malloc(sizeof(int) * N);
		c[i] = malloc(sizeof(int) * N);

		if(a[i] == NULL || b[i] == NULL || c[i] == NULL) {
			printf("Eroare la malloc!");
			exit(1);
		}
	}

	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			c[i][j] = 0;

			a[i][j] =  7 + (i + j) % 100;
            b[i][j] = 50 + (i + j) % 100;
		}
	}
}

void print(int **mat)
{
	int i, j;

	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			printf("%i\t", mat[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int i;

	get_args(argc, argv);
	init();

	pthread_t tid[P];
	int thread_id[P];

    int r = pthread_mutex_init(&mutex, NULL);
    if (r) {
        printf("Eroare la crearea mutex-ului\n");
        exit(-1);
    }

	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

    r = pthread_mutex_destroy(&mutex);
    if (r) {
        printf("Eroare la distrugerea mutex-ului\n");
        exit(-1);
    }

    // print(a);
    // print(b);
	// print(c);

	return 0;
}