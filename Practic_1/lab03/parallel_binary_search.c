
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define INSIDE -1
#define OUTSIDE -2

struct my_arg {
    int id;
    int N;
    int P;
    int number;
    int *left;
    int *right;
    int *keep_running;
    int *v;
    int *found;
};

pthread_barrier_t barrier;

void *f(void *arg)
{
    struct my_arg *data = (struct my_arg *)arg;

    int id = data->id;
    int P = data->P;
    int number = data->number;

    // Cazul P = 1: cautare binara clasica
    if (P == 1) {
        int left = 0;
        int right = data->N - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;

            if (data->v[mid] == number) {
                printf("Number found at position %d\n", mid);
                return NULL;
            }

            if (data->v[mid] < number) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        printf("Number not found\n");
        return NULL;
    }

    while (1) {
        int left = *data->left;
        int right = *data->right;
        int size = right - left;

        // Impartim intervalul curent intre thread-uri
        int start = left + (long long)id * size / P;
        int end = left + (long long)(id + 1) * size / P;

        data->found[id] = OUTSIDE;

        if (start < end) {
            if (data->v[start] == number) {
                data->found[id] = start;
            } else if (data->v[end - 1] == number) {
                data->found[id] = end - 1;
            } else if (data->v[start] < number &&
                       number < data->v[end - 1]) {
                data->found[id] = INSIDE;
            }
        }

        // Asteptam rezultatele tuturor thread-urilor
        pthread_barrier_wait(&barrier);

        if (id == 0) {
            int position = -1;
            int selected = -1;

            for (int i = 0; i < P; i++) {
                if (data->found[i] >= 0) {
                    position = data->found[i];
                    break;
                }

                if (data->found[i] == INSIDE) {
                    selected = i;
                }
            }

            if (position != -1) {
                printf("Number found at position %d\n", position);
                *data->keep_running = 0;
            } else if (selected == -1) {
                printf("Number not found\n");
                *data->keep_running = 0;
            } else {
                // Restrangem intervalul global
                *data->left =
                    left + (long long)selected * size / P;

                *data->right =
                    left + (long long)(selected + 1) * size / P;
            }
        }

        // Asteptam actualizarea limitelor
        pthread_barrier_wait(&barrier);

        if (!*data->keep_running) {
            break;
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("Usage: ./ex N P number\n");
        return 1;
    }

    int N = atoi(argv[1]);
    int P = atoi(argv[2]);
    int number = atoi(argv[3]);

    int keep_running = 1;
    int left = 0;
    int right = N;

    int *v = malloc(N * sizeof(int));
    int *found = malloc(P * sizeof(int));

    pthread_t *threads = malloc(P * sizeof(pthread_t));
    struct my_arg *arguments =
        malloc(P * sizeof(struct my_arg));

    if (!v || !found || !threads || !arguments) {
        printf("Eroare la alocare\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        v[i] = i * 2;
    }

    pthread_barrier_init(&barrier, NULL, P);

    for (int i = 0; i < P; i++) {
        arguments[i].id = i;
        arguments[i].N = N;
        arguments[i].P = P;
        arguments[i].number = number;
        arguments[i].left = &left;
        arguments[i].right = &right;
        arguments[i].keep_running = &keep_running;
        arguments[i].v = v;
        arguments[i].found = found;

        pthread_create(
            &threads[i], NULL, f, &arguments[i]
        );
    }

    for (int i = 0; i < P; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    free(v);
    free(found);
    free(threads);
    free(arguments);

    return 0;
}
