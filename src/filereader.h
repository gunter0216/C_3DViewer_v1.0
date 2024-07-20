#include "struct.h"

int read_fasetsnumber(char *line);
void read_count(FILE *file, data_t *data);
void read_vertex(char *line, double *matrix);
void read_facet(char *line, polygon_t *facets);
void read_data(FILE *file, data_t *data);
data_t file_reader();