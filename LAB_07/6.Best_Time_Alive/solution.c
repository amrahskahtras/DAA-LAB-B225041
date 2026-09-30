#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int year;
    int type;
} Event;

int compareEvents(const void *a, const void *b) {
    Event *e1 = (Event *)a;
    Event *e2 = (Event *)b;
    if (e1->year != e2->year) return e1->year - e2->year;
    return e1->type - e2->type;
}

int findBestYear(int *birth, int *death, int n, int *bestYear) {
    Event *events = (Event *)malloc(2 * n * sizeof(Event));
    for (int i = 0; i < n; i++) {
        events[2 * i].year = birth[i];
        events[2 * i].type = 1;
        events[2 * i + 1].year = death[i];
        events[2 * i + 1].type = -1;
    }
    qsort(events, 2 * n, sizeof(Event), compareEvents);

    int alive = 0, maxAlive = 0;
    *bestYear = events[0].year;
    for (int i = 0; i < 2 * n; i++) {
        alive += events[i].type;
        if (alive > maxAlive) {
            maxAlive = alive;
            *bestYear = events[i].year;
        }
    }
    free(events);
    return maxAlive;
}

void generateRandomData(int *birth, int *death, int n, int seed) {
    srand(seed);
    for (int i = 0; i < n; i++) {
        int b = 1500 + rand() % 500;
        int lifespan = 20 + rand() % 60;
        birth[i] = b;
        death[i] = b + lifespan;
    }
}

int main(int argc, char *argv[]) {
    int sizes[] = {10, 50, 100, 500, 1000, 5000, 10000, 20000, 50000, 100000};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    FILE *fp = fopen("data.csv", "w");
    if (!fp) {
        printf("Error opening data.csv\n");
        return 1;
    }

    fprintf(fp, "n_scientists,best_year,max_alive,time_ms\n");
    printf("n_scientists\tbest_year\tmax_alive\ttime_ms\n");

    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];
        int *birth = (int *)malloc(n * sizeof(int));
        int *death = (int *)malloc(n * sizeof(int));
        generateRandomData(birth, death, n, 42);

        clock_t start = clock();
        int bestYear;
        int maxAlive = findBestYear(birth, death, n, &bestYear);
        clock_t end = clock();
        double timeMs = 1000.0 * (end - start) / CLOCKS_PER_SEC;

        fprintf(fp, "%d,%d,%d,%.4f\n", n, bestYear, maxAlive, timeMs);
        printf("%d\t\t%d\t\t%d\t\t%.4f\n", n, bestYear, maxAlive, timeMs);

        free(birth);
        free(death);
    }

    fclose(fp);
    return 0;
}
