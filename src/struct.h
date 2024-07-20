#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define neconst 1000



typedef struct max_min_struct {
    double x_max, x_min, y_max, y_min;
} max_min_t;

typedef struct polygon_struct {
    int *vertex;
    int numbers_of_facets;
} polygon_t;

typedef struct matrix_struct {
    double **matrix;
    int rows;
    int columns;
} matrix_t;

typedef struct data_struct {
    unsigned long long count_vertexs;
    unsigned long long count_facets;
    max_min_t extremes;
    polygon_t *facets;
    matrix_t matrix_2d;
} data_t;



matrix_t s21_create_matrix(int rows, int columns);
void s21_remove_matrix(matrix_t *A);
void remove_data(data_t *dt);

/* Проекции: параллельные и центральная */
matrix_t parallel_projection(data_t data);
matrix_t central_projection(data_t data);

/* Типы вершин: квадртаные и круглые */

/* Повороты по oz ox oy */
void rotation_by_oz(matrix_t *A, double angle);
void rotation_by_ox(matrix_t *A, double angle);
void rotation_by_oy(matrix_t *A, double angle);

/* сдвиг по oz ox oy */
void move_x(matrix_t *A, double step);
void move_y(matrix_t *A, double step);
void move_z(matrix_t *A, double step);

/* увеличение */
void zoom(matrix_t *A, double coefficient);

/* Для тестов */
matrix_t array_to_matrix(const double *array, int rows, int columns);
int s21_eq_matrix(matrix_t *A, matrix_t *B);
