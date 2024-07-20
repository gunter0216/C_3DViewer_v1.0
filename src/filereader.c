#include "filereader.h"

int read_fasetsnumber(char *line) {
    char temp[500];
    line = line + 1;
    sscanf(line, "%499s", temp);
    int count = 0;
    for (int n = 1; line; n++) {
        line = strstr(line, temp);
        int len = strlen(temp);
        if (line) {
            line = line + len;
            sscanf(line, "%499s", temp);
            count++;
        }
    }
    return count;
}

void read_count(FILE *file, data_t *data) {
    size_t len;
    char line[128];
    data->count_vertexs = 0;
    data->count_facets = 0;
    while (fgets(line, 126, file) != NULL) {
        if (line[0] == 'v' && line[1] == ' ') {
            data->count_vertexs++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            data->count_facets++;
        }
    }
    data->count_vertexs++;
    data->count_facets++;
}

void read_vertex(char *line, double *matrix) {
    sscanf(line, "v %lf %lf %lf", &matrix[0], &matrix[1], &matrix[2]);
}

void read_facet(char *line, polygon_t *facets) {
    facets->numbers_of_facets = read_fasetsnumber(line);
    facets->vertex = calloc(facets->numbers_of_facets, sizeof(int));
    line = line + 1;
    sscanf(line, "%d", &facets->vertex[0]);
    line = line + 1;
    int i = 1;
    int count = 1;
    while (*(line) != '\0') {
        if (*(line) == ' ') {
            line = line + 1;
            sscanf(line, "%d", &facets->vertex[count]);
            count++;
        }
        line = line + 1;
    }
}

void read_data(FILE *file, data_t *data) {
    size_t len;
    char line[128];
    unsigned long long vertex = 1;
    unsigned long long facets = 1;
    while (fgets(line, 126, file) != NULL) {
        if (line[0] == 'v' && line[1] == ' ') {
            read_vertex(line, &data->matrix_2d.matrix[vertex][0]);
            if (vertex == 1) {
                data->extremes.x_max = data->matrix_2d.matrix[vertex][0];
                data->extremes.x_min = data->matrix_2d.matrix[vertex][0];
                data->extremes.x_max = data->matrix_2d.matrix[vertex][1];
                data->extremes.x_min = data->matrix_2d.matrix[vertex][1];
            } else {
                if (data->extremes.x_max < data->matrix_2d.matrix[vertex][0]) {
                    data->extremes.x_max = data->matrix_2d.matrix[vertex][0];
                }
                if (data->extremes.x_min > data->matrix_2d.matrix[vertex][0]) {
                    data->extremes.x_min = data->matrix_2d.matrix[vertex][0];
                }
                if (data->extremes.y_max < data->matrix_2d.matrix[vertex][1]) {
                    data->extremes.y_max = data->matrix_2d.matrix[vertex][1];
                }
                if (data->extremes.y_min > data->matrix_2d.matrix[vertex][1]) {
                    data->extremes.y_min = data->matrix_2d.matrix[vertex][1];
                }
            }
            vertex++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            read_facet(line, &data->facets[facets]);
            facets++;
        }
    }
}

data_t file_reader(char *_file_name) {
    data_t data;
    int error = 0;
    FILE *file = NULL;
    file = fopen(_file_name, "r");
    if (file) {
        read_count(file, &data);
        fclose(file);
        data.matrix_2d = s21_create_matrix(data.count_vertexs, 3);
        data.facets = calloc(data.count_facets, sizeof(polygon_t));
        file = fopen(_file_name, "r");
        if (file) {
            read_data(file, &data);
        }
    } else {
        error = 1;
    }
    return data;
}