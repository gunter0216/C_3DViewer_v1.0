#include <gtk/gtk.h>

#include "filereader.h"

#define WINDOW_SIZE_X 1320
#define WINDOW_SIZE_y 700

typedef enum {
    PARALLEL = 0,
    CENTRAL = 1
} projection_type_t;

typedef enum {
    SOLID = 0,
    DASHED = 1
} line_type_t;

typedef struct line {
    int size;
    double color[3];
    line_type_t type;
} line_t;

typedef enum {
    NONE = 0,
    SQUAD = 1,
    CIRCLE = 2
} vertex_type_t;

typedef struct vertex {
    int size;
    double color[3];
    vertex_type_t type;
} vertex_t;

typedef struct settings {
    line_t line_settings;
    vertex_t vertex_settings;
    projection_type_t type;
    double background_color[3];
    double scale;
} setting_t;


setting_t settings;
data_t aboba;

char *file_name = NULL;
GtkWidget *window;
GtkWidget *drawing_area;
GtkWidget *label_file_name = NULL;
GtkWidget *fixed;

GtkWidget *entry_x;
GtkWidget *entry_y;
GtkWidget *entry_z;
GtkWidget *entry_scale;
GtkWidget *label_facets_count;
GtkWidget *label_vertex_count;
GtkWidget *label_projection_type_text;
GtkWidget *label_line_type_text;
GtkWidget *label_vertex_type_text;
GtkWidget *scale_rotation_ox;
GtkWidget *scale_rotation_oy;
GtkWidget *scale_rotation_oz;
GtkWidget *entry_line_depth;
GtkWidget *entry_facets_size;
GtkWidget *entry_line_color_r;
GtkWidget *entry_line_color_g;
GtkWidget *entry_line_color_b;
GtkWidget *entry_facets_color_r;
GtkWidget *entry_facets_color_g;
GtkWidget *entry_facets_color_b;
GtkWidget *entry_background_color_r;
GtkWidget *entry_background_color_g;
GtkWidget *entry_background_color_b;

GtkWidget *label_rotation_ox;
GtkWidget *label_rotation_oy;
GtkWidget *label_rotation_oz;

GtkWidget *box_projection;
GtkWidget *box_line_type;
GtkWidget *box_line_display;

int old_value_rotation_ox = 0;
int old_value_rotation_oy = 0;
int old_value_rotation_oz = 0;

int old_value_position_ox = 0;
int old_value_position_oy = 0;
int old_value_position_oz = 0;

void auto_scale(data_t *data);
void draw_polygons(cairo_t *cr, data_t data);
void draw_vertex(cairo_t *cr, data_t data);

char *get_file_name();
void select_file(GtkWidget *button, gpointer data);
char *get_string_from_entry(GtkWidget *widget);

void foo_scale_rotation_ox(GtkRange *range, gpointer win);
void foo_scale_rotation_oy(GtkRange *range, gpointer win);
void foo_scale_rotation_oz(GtkRange *range, gpointer win);

void foo_entry_position_ox(GtkRange *range, gpointer win);
void foo_entry_position_oy(GtkRange *range, gpointer win);
void foo_entry_position_oz(GtkRange *range, gpointer win);

void foo_entry_scale(GtkRange *range, gpointer win);

void foo_entry_line_depth(GtkRange *range, gpointer win);
void foo_entry_line_color_r(GtkRange *range, gpointer win);
void foo_entry_line_color_g(GtkRange *range, gpointer win);
void foo_entry_line_color_b(GtkRange *range, gpointer win);

void foo_entry_facets_size(GtkRange *range, gpointer win);
void foo_entry_facets_color_r(GtkRange *range, gpointer win);
void foo_entry_facets_color_g(GtkRange *range, gpointer win);
void foo_entry_facets_color_b(GtkRange *range, gpointer win);

void foo_entry_background_color_r(GtkRange *range, gpointer win);
void foo_entry_background_color_g(GtkRange *range, gpointer win);
void foo_entry_background_color_b(GtkRange *range, gpointer win);

void create_label_and_scale();

void label_facets_count_set_text();
void label_vertex_count_set_text();

void reset_label_count();

void read_file_settings();
void write_file_settings();
void close_window(GtkWidget *button, gpointer data);

void save_image(GtkWidget *button, gpointer data);
void save_screencast(GtkWidget *button, gpointer data);
void save_screenshot(char *filename);

void foo_box_projection(GtkWidget *widget, gpointer window);
void foo_box_line_type(GtkWidget *widget, gpointer window);
void foo_box_line_display(GtkWidget *widget, gpointer window);

void init_settings(setting_t *config);