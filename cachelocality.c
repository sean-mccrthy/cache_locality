/*
 * Cache Locality
 * Author: Sean McCarthy - mccaramv@bc.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define FALSE 0
#define MAX 20
#define TRUE 1

void init(const int dim, int * const m) {
    for (int i = 0; i < dim * dim; i++) {
        m[i] = rand() % MAX;
    }
}

void multiply(const int dim, const int * const a, int * const b, int * const c) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
	    c[i * dim + j] = 0;
	    for (int k = 0; k < dim; k++) {
	        c[i * dim + j] += a[i * dim + k] * b[k * dim + j];
	    }
	}
    }
}

void transpose(const int dim, int * const m) {
    for (int i = 0; i < dim; i++) {
        for (int j = i; j < dim; j++) {
	    int temp = m[j * dim + i];
	    m[j * dim + i] = m[i * dim + j];
	    m[i * dim + j] = temp;
	}
    }
}

void multiply_transpose(const int dim, const int * const a, const int * const b_t, int * const c) {
    for (int i = 0; i < dim; i++) {
	for (int j = 0; j < dim; j++) {
	    c[i * dim + j] = 0;
	    for (int k = 0; k < dim; k++) {
		c[i * dim + j] += a[i * dim + k] * b_t[j * dim + k];
	    }
	}
    }
}

void transpose_and_multiply(const int dim, const int * const a, int * const b, int * const c) {
    transpose(dim, b);
    multiply_transpose(dim, a, b, c);
}

struct timeval run_and_time(
    void (* mult_func)(const int, const int * const, int * const, int * const),
    const int dim,
    const int * const a,
    int * const b,
    int * const c
) {
    struct timeval start;
    struct timeval end;
    struct timeval result;
    gettimeofday(&start, NULL);
    mult_func(dim, a, b, c);
    gettimeofday(&end, NULL);
    result.tv_sec = end.tv_sec - start.tv_sec;
    result.tv_usec = end.tv_usec - start.tv_usec;
    if (result.tv_usec < 0) {
        result.tv_usec += 1000000;
	result.tv_sec -= 1;
    }
    return result;
}

int verify(const int dim, const int * const c1, const int * const c2) {
    for(int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
	    if (c1[i * dim + j] != c2[i * dim + j]) {
	        return FALSE;
	    }
	}
    }
    return TRUE;
}

void run_test(const int dim) {
    int * a = calloc(dim * dim, sizeof(int));
    int * b = calloc(dim * dim, sizeof(int));
    int * c1 = calloc(dim * dim, sizeof(int));
    int * c2 = calloc(dim * dim, sizeof(int));
    init(dim, a);
    init(dim, b);
    struct timeval time_mult = run_and_time(&multiply, dim, a, b, c1);
    struct timeval time_transpose = run_and_time(&transpose_and_multiply, dim, a, b, c2);
    printf("Testing on %d-by-%d square matrices.\n", dim, dim);
    if (verify(dim, c1, c2)) {
        puts("Results agree.");
	printf("Standard multiplication: %d seconds, %ld microseconds\n", (int)time_mult.tv_sec, (long)time_mult.tv_usec);
        printf("Multiplication with transpose: %d seconds, %ld microseconds\n", (int)time_transpose.tv_sec, (long)time_transpose.tv_usec);
    }
    free(a);
    free(b);
    free(c1);
    free(c2);
}

void print(const int dim, const int * const m) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
	    printf("%d ", m[i * dim + j]);
	}
	printf("\n");
    }
}

int main() {
    int dim;
    int max_power;
    puts("The dimensions of each square matrices are going to start at 8-by-8 to 2^(power you enter)-by-2(^power you enter)");
    printf("Enter the maximum power of two you want for the dimensions: ");
    scanf("%d", &max_power);
    for (int power = 3; power <= max_power; power++) {
        run_test(1 << power);
	puts("");
    }
    return EXIT_SUCCESS;
}
