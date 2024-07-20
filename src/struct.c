#include "struct.h"



matrix_t s21_create_matrix(int rows, int columns) {
    matrix_t matrix;
    if (rows > 0 && columns > 0) {
        matrix.rows = rows;
        matrix.columns = columns;
        matrix.matrix = (double **)calloc(rows, sizeof(double));
        for (int i = 0; i < rows; i++) {
            matrix.matrix[i] = (double *)calloc(columns, sizeof(double));
        }
    }
    return matrix;
}

void s21_remove_matrix(matrix_t *A) {
    if (A->matrix != NULL) {
        for (int i = 0; i < A->rows; i++) {
            free(A->matrix[i]);
        }
        free(A->matrix);
        A->columns = 0;
        A->rows = 0;
    }
}

void remove_data(data_t *dt) {
    if (dt->facets) {
        for(int i = 0; i < dt->count_facets; i++){
            free(dt->facets[i].vertex);
        }
        free(dt->facets);
    }
    s21_remove_matrix(&dt->matrix_2d);
}

matrix_t parallel_projection(data_t data) {
    matrix_t result = s21_create_matrix(data.count_vertexs, 3);
    for (unsigned long long i = 0; i < data.count_vertexs; i++) {
        result.matrix[i][0] = data.matrix_2d.matrix[i][0];
        result.matrix[i][1] = data.matrix_2d.matrix[i][1];
        result.matrix[i][2] = data.matrix_2d.matrix[i][2];
    }
    return result;
}

matrix_t central_projection(data_t data) {
    matrix_t result = s21_create_matrix(data.count_vertexs, 3);
    for (unsigned long long i = 0; i < data.count_vertexs; i++) {
        for (int j = 0; j < 2; j++)
            result.matrix[i][j] = neconst * data.matrix_2d.matrix[i][j] /
                                  (neconst - data.matrix_2d.matrix[i][2]);
        result.matrix[i][2] = data.matrix_2d.matrix[i][2];
    }
    return result;
}

void rotation_by_oz(matrix_t *A, double angle) {
    for (int i = 0; i < A->rows; i++) {
        double temp_x = A->matrix[i][0];
        double temp_y = A->matrix[i][1];
        A->matrix[i][0] = cos(angle) * temp_x - sin(angle) * temp_y;
        A->matrix[i][1] = sin(angle) * temp_x + cos(angle) * temp_y;
    }
}

void rotation_by_oy(matrix_t *A, double angle) {
    for (int i = 0; i < A->rows; i++) {
        double temp_x = A->matrix[i][0];
        double temp_z = A->matrix[i][2];
        A->matrix[i][0] = cos(angle) * temp_x + sin(angle) * temp_z;
        A->matrix[i][2] = -sin(angle) * temp_x + cos(angle) * temp_z;
    }
}

void rotation_by_ox(matrix_t *A, double angle) {
    for (int i = 0; i < A->rows; i++) {
        double temp_y = A->matrix[i][1];
        double temp_z = A->matrix[i][2];
        A->matrix[i][1] = cos(angle) * temp_y - sin(angle) * temp_z;
        A->matrix[i][2] = sin(angle) * temp_y + cos(angle) * temp_z;
    }
}

void move_x(matrix_t *A, double a) {
    for (int i = 0; i < A->rows; i++) A->matrix[i][0] += a;
}

void move_y(matrix_t *A, double a) {
    for (int i = 0; i < A->rows; i++) A->matrix[i][1] += a;
}
void move_z(matrix_t *A, double a) {
    for (int i = 0; i < A->rows; i++) A->matrix[i][2] += a;
}

void zoom(matrix_t *A, double coefficient) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
            A->matrix[i][j] *= coefficient;
        }
    }
}



/* Для тестов */
matrix_t array_to_matrix(const double *array, int rows, int columns) {
    matrix_t matrix = s21_create_matrix(rows, columns);
    for (int i = 0, z = 0; i < rows; i++) {
        for (int k = 0; k < columns; k++, z++) {
            matrix.matrix[i][k] = array[z];
        }
    }
    return matrix;
}

int s21_eq_matrix(matrix_t *A, matrix_t *B) {
    int result = 1;
    for (int i = 0; i < A->rows; i++)
        for (int j = 0; j < A->columns; j++)
            if (fabs(A->matrix[i][j] - B->matrix[i][j]) > 1e-7) {
                result = 0;
                break;
            }
    return result;
}
