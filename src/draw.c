#include "draw.h"

#include <gd.h>
int NEED_SAVE = 0;
int NUMBER_NAME = 0;

void auto_scale(data_t *data) {
    int flag_y = 0;
    int flag_x = 0;
    if (aboba.extremes.y_min < 0.9 && aboba.extremes.y_min > -0.9) {
        aboba.extremes.y_min = aboba.extremes.y_max;
        flag_y = 1;
    }
    if (aboba.extremes.x_min < 0.9 && aboba.extremes.x_min > -0.9) {
        aboba.extremes.x_min = aboba.extremes.x_max;
        flag_x = 1;
    }
    double zoom_coef_x =
        400 / (fabs(aboba.extremes.x_min) + fabs(aboba.extremes.x_max));
    double zoom_coef_y =
        400 / (fabs(aboba.extremes.y_min) + fabs(aboba.extremes.y_max));
    zoom_coef_x = zoom_coef_x > zoom_coef_y ? zoom_coef_y : zoom_coef_x;
    zoom(&aboba.matrix_2d, zoom_coef_x);
    settings.scale = zoom_coef_x;
    if (flag_y) {
        move_y(&aboba.matrix_2d, (-aboba.extremes.y_max / 2) * zoom_coef_x);
    }
    if (flag_y) {
        move_x(&aboba.matrix_2d, (-aboba.extremes.x_max / 2) * zoom_coef_x);
    }
}

void draw_polygons(cairo_t *cr, data_t data) {
    cairo_set_source_rgb(cr, settings.line_settings.color[0],
                         settings.line_settings.color[1],
                         settings.line_settings.color[2]);
    cairo_set_line_width(cr, 0.05 * settings.line_settings.size);
    if (settings.line_settings.type == DASHED) {
        double dashes[3] = {5, 8, 5};
        cairo_set_dash(cr, dashes, 2, 0);
    }
    if (settings.type == CENTRAL) {
        data.matrix_2d = central_projection(data);
    } else if (settings.type == PARALLEL) {
        data.matrix_2d = parallel_projection(data);
    }
    for (unsigned long long i = 1; i < data.count_facets; i++) {
        cairo_move_to(cr, data.matrix_2d.matrix[data.facets[i].vertex[0]][0],
                      data.matrix_2d.matrix[data.facets[i].vertex[0]][1]);
        for (int j = 0; j < data.facets[i].numbers_of_facets; j++) {
            cairo_line_to(cr,
                          data.matrix_2d.matrix[data.facets[i].vertex[j]][0],
                          data.matrix_2d.matrix[data.facets[i].vertex[j]][1]);
        }
        cairo_line_to(cr, data.matrix_2d.matrix[data.facets[i].vertex[0]][0],
                      data.matrix_2d.matrix[data.facets[i].vertex[0]][1]);
        cairo_stroke(cr);
    }
}

void draw_vertex(cairo_t *cr, data_t data) {
    cairo_set_source_rgb(cr, settings.vertex_settings.color[0],
                         settings.vertex_settings.color[1],
                         settings.vertex_settings.color[2]);
    cairo_set_line_width(cr, 0.05 * settings.vertex_settings.size);
    double size = cairo_get_line_width(cr);
    if (settings.vertex_settings.type == CIRCLE) {
        for (int i = 1; i < data.count_vertexs; i++) {
            cairo_arc(cr, data.matrix_2d.matrix[i][0],
                      data.matrix_2d.matrix[i][1], size, 0, 2 * M_PI);
            cairo_fill(cr);
        }
    } else if (settings.vertex_settings.type == SQUAD) {
        for (int i = 1; i < data.count_vertexs; i++) {
            cairo_rectangle(cr, data.matrix_2d.matrix[i][0] - size / 2,
                            data.matrix_2d.matrix[i][1] - size / 2, size, size);
            cairo_fill(cr);
        }
    }
}

static gboolean on_draw(GtkWidget *widget, gpointer user_data) {
    GdkWindow *window = gtk_widget_get_window(widget);

    cairo_region_t *cairoRegion = cairo_region_create();
    GdkDrawingContext *drawingContext =
        gdk_window_begin_draw_frame(window, cairoRegion);
    cairo_t *cairo = gdk_drawing_context_get_cairo_context(drawingContext);

    if (NEED_SAVE == 1) {
        char str[50];
        sprintf(str, "images_for_gif/image%d.png", NUMBER_NAME);
        NUMBER_NAME++;
        save_screenshot(str);
        if (NUMBER_NAME == 50) {
            NEED_SAVE = 0;
            gdImagePtr im;
            FILE *temp = fopen("images_for_gif/image0.png", "rb");
            im = gdImageCreateFromPng(temp);
            fclose(temp);

            FILE *out1 = fopen("gifka.gif", "wb");
            gdImageGifAnimBegin(im, out1, -1, 0);
            gdImageGifAnimAdd(im, out1, 1, 1, 1, 10, 1, NULL);

            for (int i = 1; i < 50; i++) {
                char str[50];
                sprintf(str, "images_for_gif/image%d.png", i);
                FILE *file = fopen(str, "rb");
                im = gdImageCreateFromPng(file);
                gdImageGifAnimAdd(im, out1, 1, 1, 1, 10, 1,
                                  NULL);  //пишем кадр в гифку
                fclose(file);
            }
            putc(';', out1);     // пишем в гифку конец
            fclose(out1);        //закрываем файлик
            gdImageDestroy(im);  // убираем за собой
        }
    }
    cairo_set_source_rgb(cairo, settings.background_color[0],
                         settings.background_color[1],
                         settings.background_color[2]);
    cairo_paint(cairo);

    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    cairo_translate(cairo, width / 2, height / 2);
    cairo_scale(cairo, 1.1, -1.1);
    cairo_set_line_width(cairo, 0.05);

    draw_vertex(cairo, aboba);
    draw_polygons(cairo, aboba);

    gdk_window_end_draw_frame(window, drawingContext);
    cairo_region_destroy(cairoRegion);

    return FALSE;
}

void close_window(GtkWidget *button, gpointer data) {
    write_file_settings();
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    init_settings(&settings);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_SIZE_X,
                                WINDOW_SIZE_y);
    gtk_window_set_title(GTK_WINDOW(window), "Graph drawing");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(close_window),
                     NULL);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    drawing_area = gtk_drawing_area_new();
    gtk_fixed_put(GTK_FIXED(fixed), drawing_area, 0, (WINDOW_SIZE_y - 480) / 2);
    gtk_widget_set_size_request(drawing_area, 640, 480);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    // кнопка выбора файла
    GtkWidget *button = gtk_button_new_with_label("open");
    gtk_fixed_put(GTK_FIXED(fixed), button, WINDOW_SIZE_X - 100 - 10, 10);
    gtk_widget_set_size_request(button, 100, 50);
    g_signal_connect(GTK_BUTTON(button), "clicked", G_CALLBACK(select_file),
                     NULL);

    // кнопка сохранения картинки
    GtkWidget *button_save_image = gtk_button_new_with_label("save image");
    gtk_fixed_put(GTK_FIXED(fixed), button_save_image, WINDOW_SIZE_X - 200 - 40,
                  10);
    gtk_widget_set_size_request(button_save_image, 120, 50);
    g_signal_connect(GTK_BUTTON(button_save_image), "clicked",
                     G_CALLBACK(save_image), NULL);

    // кнопка сохранения gif
    GtkWidget *button_save_screencast =
        gtk_button_new_with_label("save screencast");
    gtk_fixed_put(GTK_FIXED(fixed), button_save_screencast,
                  WINDOW_SIZE_X - 300 - 85, 10);
    gtk_widget_set_size_request(button_save_screencast, 120, 50);
    g_signal_connect(GTK_BUTTON(button_save_screencast), "clicked",
                     G_CALLBACK(save_screencast), NULL);

    // метка имени файла
    label_file_name = gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed), label_file_name, 810 - 150, 10);
    gtk_widget_set_size_request(label_file_name, 270, 50);

    create_label_and_scale();

    read_file_settings();
    label_vertex_count_set_text();
    label_facets_count_set_text();

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void create_label_and_scale() {
    int x = -150;
    // метка "Facets"
    GtkWidget *label_facets = gtk_label_new("Facets:");
    gtk_fixed_put(GTK_FIXED(fixed), label_facets, 830 + x, 40);
    gtk_widget_set_size_request(label_facets, -1, 50);

    // метка "Facets_count"
    label_facets_count = gtk_label_new("0");
    gtk_fixed_put(GTK_FIXED(fixed), label_facets_count, 880 + x, 40);
    gtk_widget_set_size_request(label_facets_count, -1, 50);

    // метка "Vertex"
    GtkWidget *label_vertex = gtk_label_new("Vertex:");
    gtk_fixed_put(GTK_FIXED(fixed), label_vertex, 950 + x, 40);
    gtk_widget_set_size_request(label_vertex, -1, 50);

    // метка "Vertex_count"
    label_vertex_count = gtk_label_new("0");
    gtk_fixed_put(GTK_FIXED(fixed), label_vertex_count, 1000 + x, 40);
    gtk_widget_set_size_request(label_vertex_count, -1, 50);

    // метка "rotation"
    GtkWidget *label_rotation = gtk_label_new("Rotation");
    gtk_fixed_put(GTK_FIXED(fixed), label_rotation, 810 + x, 70);
    gtk_widget_set_size_request(label_rotation, 100, 50);

    // метка "x"
    GtkWidget *label_rotation_x = gtk_label_new("x");
    gtk_fixed_put(GTK_FIXED(fixed), label_rotation_x, 820 + x, 100);
    gtk_widget_set_size_request(label_rotation_x, 50, 50);

    // метка "y"
    GtkWidget *label_rotation_y = gtk_label_new("y");
    gtk_fixed_put(GTK_FIXED(fixed), label_rotation_y, 820 + x, 135);
    gtk_widget_set_size_request(label_rotation_y, 50, 50);

    // метка "z"
    GtkWidget *label_rotation_z = gtk_label_new("z");
    gtk_fixed_put(GTK_FIXED(fixed), label_rotation_z, 820 + x, 170);
    gtk_widget_set_size_request(label_rotation_z, 50, 50);

    // ползунок ох
    scale_rotation_ox =
        gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 360, 1);
    gtk_fixed_put(GTK_FIXED(fixed), scale_rotation_ox, 860 + x, 110);
    gtk_widget_set_size_request(scale_rotation_ox, 200, -1);
    gtk_scale_set_draw_value(GTK_SCALE(scale_rotation_ox), FALSE);

    // метка ох
    label_rotation_ox = gtk_label_new("0");
    gtk_fixed_put(GTK_FIXED(fixed), label_rotation_ox, 1060 + x, 100);
    gtk_widget_set_size_request(label_rotation_ox, 50, 50);
    g_signal_connect(scale_rotation_ox, "value-changed",
                     G_CALLBACK(foo_scale_rotation_ox), label_rotation_ox);

    // ползунок оy
    scale_rotation_oy =
        gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 360, 1);
    gtk_fixed_put(GTK_FIXED(fixed), scale_rotation_oy, 860 + x, 145);
    gtk_widget_set_size_request(scale_rotation_oy, 200, -1);
    gtk_scale_set_draw_value(GTK_SCALE(scale_rotation_oy), FALSE);

    // метка оy
    label_rotation_oy = gtk_label_new("0");
    gtk_fixed_put(GTK_FIXED(fixed), label_rotation_oy, 1060 + x, 135);
    gtk_widget_set_size_request(label_rotation_oy, 50, 50);
    g_signal_connect(scale_rotation_oy, "value-changed",
                     G_CALLBACK(foo_scale_rotation_oy), label_rotation_oy);

    // ползунок оz
    scale_rotation_oz =
        gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 360, 1);
    gtk_fixed_put(GTK_FIXED(fixed), scale_rotation_oz, 860 + x, 180);
    gtk_widget_set_size_request(scale_rotation_oz, 200, -1);
    gtk_scale_set_draw_value(GTK_SCALE(scale_rotation_oz), FALSE);

    // метка оz
    label_rotation_oz = gtk_label_new("0");
    gtk_fixed_put(GTK_FIXED(fixed), label_rotation_oz, 1060 + x, 170);
    gtk_widget_set_size_request(label_rotation_oz, 50, 50);
    g_signal_connect(scale_rotation_oz, "value-changed",
                     G_CALLBACK(foo_scale_rotation_oz), label_rotation_oz);

    // метка "position"
    GtkWidget *label_position = gtk_label_new("Position");
    gtk_fixed_put(GTK_FIXED(fixed), label_position, 810 + x, 210);
    gtk_widget_set_size_request(label_position, 100, 50);

    // метка "x"
    GtkWidget *label_position_x = gtk_label_new("x");
    gtk_fixed_put(GTK_FIXED(fixed), label_position_x, 820 + x, 240);
    gtk_widget_set_size_request(label_position_x, 50, 50);

    // метка "y"
    GtkWidget *label_position_y = gtk_label_new("y");
    gtk_fixed_put(GTK_FIXED(fixed), label_position_y, 820 + x, 275);
    gtk_widget_set_size_request(label_position_y, 50, 50);

    // метка "z"
    GtkWidget *label_position_z = gtk_label_new("z");
    gtk_fixed_put(GTK_FIXED(fixed), label_position_z, 820 + x, 310);
    gtk_widget_set_size_request(label_position_z, 50, 50);

    // поле ввода ох
    entry_x = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_x, 860 + x, 250);
    gtk_widget_set_size_request(entry_x, 20, -1);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_x), "0");
    g_signal_connect(entry_x, "activate", G_CALLBACK(foo_entry_position_ox),
                     NULL);

    // поле ввода оy
    entry_y = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_y, 860 + x, 285);
    gtk_widget_set_size_request(entry_y, 20, -1);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_y), "0");
    g_signal_connect(entry_y, "activate", G_CALLBACK(foo_entry_position_oy),
                     NULL);

    // поле ввода оz
    entry_z = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_z, 860 + x, 320);
    gtk_widget_set_size_request(entry_z, 20, -1);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_z), "0");
    g_signal_connect(entry_z, "activate", G_CALLBACK(foo_entry_position_oz),
                     NULL);

    // метка "scale"
    GtkWidget *label_scale = gtk_label_new("Scale");
    gtk_fixed_put(GTK_FIXED(fixed), label_scale, 810 + x, 350);
    gtk_widget_set_size_request(label_scale, 100, 50);

    // поле ввода
    entry_scale = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_scale, 860 + x, 390);
    gtk_widget_set_size_request(entry_scale, 200, -1);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_scale), "1.0");
    g_signal_connect(entry_scale, "activate", G_CALLBACK(foo_entry_scale),
                     NULL);

    // метка Projection type
    GtkWidget *label_projection_type = gtk_label_new("Projection type");
    gtk_fixed_put(GTK_FIXED(fixed), label_projection_type, 840 + x, 420);
    gtk_widget_set_size_request(label_projection_type, -1, 50);

    box_projection = gtk_combo_box_text_new_with_entry();
    gtk_fixed_put(GTK_FIXED(fixed), box_projection, 860 + x, 465);
    gtk_widget_set_size_request(box_projection, 150, -1);
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box_projection), "1",
                              "parallel");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box_projection), "2",
                              "central");
    g_signal_connect(G_OBJECT(box_projection), "changed",
                     G_CALLBACK(foo_box_projection), NULL);

    label_projection_type_text = gtk_label_new("parallel");
    gtk_fixed_put(GTK_FIXED(fixed), label_projection_type_text, 869 + x,
                  471);  // 860 465
    gtk_widget_set_size_request(label_projection_type, -1, 50);

    // метка Line
    GtkWidget *label_line = gtk_label_new("Line:");
    gtk_fixed_put(GTK_FIXED(fixed), label_line, 1150 + x, 70);  // 850 490
    gtk_widget_set_size_request(label_line, -1, 50);

    // метка Line type
    GtkWidget *label_line_type = gtk_label_new("Type");
    gtk_fixed_put(GTK_FIXED(fixed), label_line_type, 1140 + x, 105);
    gtk_widget_set_size_request(label_line_type, -1, 50);

    box_line_type = gtk_combo_box_text_new_with_entry();
    gtk_fixed_put(GTK_FIXED(fixed), box_line_type, 1200 + x, 115);
    gtk_widget_set_size_request(box_line_type, 200, -1);
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box_line_type), "1", "solid");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box_line_type), "2", "dashed");
    g_signal_connect(G_OBJECT(box_line_type), "changed",
                     G_CALLBACK(foo_box_line_type), NULL);

    label_line_type_text = gtk_label_new("solid");
    gtk_fixed_put(GTK_FIXED(fixed), label_line_type_text, 1209 + x, 107);
    gtk_widget_set_size_request(label_line_type_text, -1, 50);

    // метка Line depth
    GtkWidget *label_line_depth = gtk_label_new("Size");
    gtk_fixed_put(GTK_FIXED(fixed), label_line_depth, 1140 + x, 140);
    gtk_widget_set_size_request(label_line_depth, -1, 50);

    // поле ввода
    entry_line_depth = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_line_depth, 1200 + x, 150);
    gtk_widget_set_size_request(entry_line_depth, 205, -1);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_line_depth), "1");
    g_signal_connect(entry_line_depth, "activate",
                     G_CALLBACK(foo_entry_line_depth), NULL);

    // метка Line color
    GtkWidget *label_line_color = gtk_label_new("Color:");
    gtk_fixed_put(GTK_FIXED(fixed), label_line_color, 1140 + x, 175);
    gtk_widget_set_size_request(label_line_color, -1, 50);

    // метка r
    GtkWidget *label_line_color_r = gtk_label_new("R");
    gtk_fixed_put(GTK_FIXED(fixed), label_line_color_r, 1200 + x, 175);
    gtk_widget_set_size_request(label_line_color_r, -1, 50);

    // поле ввода
    entry_line_color_r = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_line_color_r, 1220 + x, 185);
    gtk_widget_set_size_request(entry_line_color_r, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_line_color_r), "0");
    g_signal_connect(entry_line_color_r, "activate",
                     G_CALLBACK(foo_entry_line_color_r), NULL);

    // метка g
    GtkWidget *label_line_color_g = gtk_label_new("G");
    gtk_fixed_put(GTK_FIXED(fixed), label_line_color_g, 1200 + x, 210);
    gtk_widget_set_size_request(label_line_color_g, -1, 50);

    // поле ввода
    entry_line_color_g = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_line_color_g, 1220 + x, 220);
    gtk_widget_set_size_request(entry_line_color_g, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_line_color_g), "0");
    g_signal_connect(entry_line_color_g, "activate",
                     G_CALLBACK(foo_entry_line_color_g), NULL);

    // метка b
    GtkWidget *label_line_color_b = gtk_label_new("B");
    gtk_fixed_put(GTK_FIXED(fixed), label_line_color_b, 1200 + x, 245);
    gtk_widget_set_size_request(label_line_color_b, -1, 50);

    // поле ввода
    entry_line_color_b = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_line_color_b, 1220 + x, 255);
    gtk_widget_set_size_request(entry_line_color_b, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_line_color_b), "0");
    g_signal_connect(entry_line_color_b, "activate",
                     G_CALLBACK(foo_entry_line_color_b), NULL);

    // метка facets
    GtkWidget *label_facets_settings = gtk_label_new("Vertex:");
    gtk_fixed_put(GTK_FIXED(fixed), label_facets_settings, 1150 + x, 290);
    gtk_widget_set_size_request(label_facets_settings, -1, 50);

    // метка facets size
    GtkWidget *label_facets_size = gtk_label_new("Size");
    gtk_fixed_put(GTK_FIXED(fixed), label_facets_size, 1140 + x, 325);
    gtk_widget_set_size_request(label_facets_size, -1, 50);

    // поле ввода
    entry_facets_size = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_facets_size, 1200 + x, 335);
    gtk_widget_set_size_request(entry_facets_size, 205, -1);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_facets_size), "1");
    g_signal_connect(entry_facets_size, "activate",
                     G_CALLBACK(foo_entry_facets_size), NULL);

    // метка Line display
    GtkWidget *label_line_display = gtk_label_new("Type");
    gtk_fixed_put(GTK_FIXED(fixed), label_line_display, 1140 + x, 360);
    gtk_widget_set_size_request(label_line_display, -1, 50);

    box_line_display = gtk_combo_box_text_new_with_entry();
    gtk_fixed_put(GTK_FIXED(fixed), box_line_display, 1200 + x, 370);
    gtk_widget_set_size_request(box_line_display, 200, -1);
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box_line_display), "1",
                              "default");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box_line_display), "2",
                              "circle");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(box_line_display), "3",
                              "square");
    g_signal_connect(G_OBJECT(box_line_display), "changed",
                     G_CALLBACK(foo_box_line_display), NULL);

    label_vertex_type_text = gtk_label_new("default");
    gtk_fixed_put(GTK_FIXED(fixed), label_vertex_type_text, 1209 + x, 362);
    gtk_widget_set_size_request(label_vertex_type_text, -1, 50);

    // метка facets color
    GtkWidget *label_facets_color = gtk_label_new("Color:");
    gtk_fixed_put(GTK_FIXED(fixed), label_facets_color, 1140 + x, 405);
    gtk_widget_set_size_request(label_facets_color, -1, 50);

    // метка r
    GtkWidget *label_facets_color_r = gtk_label_new("R");
    gtk_fixed_put(GTK_FIXED(fixed), label_facets_color_r, 1200 + x, 405);
    gtk_widget_set_size_request(label_facets_color_r, -1, 50);

    // поле ввода
    entry_facets_color_r = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_facets_color_r, 1220 + x, 415);
    gtk_widget_set_size_request(entry_facets_color_r, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_facets_color_r), "0");
    g_signal_connect(entry_facets_color_r, "activate",
                     G_CALLBACK(foo_entry_facets_color_r), NULL);

    // метка g
    GtkWidget *label_facets_color_g = gtk_label_new("G");
    gtk_fixed_put(GTK_FIXED(fixed), label_facets_color_g, 1200 + x, 440);
    gtk_widget_set_size_request(label_facets_color_g, -1, 50);

    // поле ввода
    entry_facets_color_g = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_facets_color_g, 1220 + x, 450);
    gtk_widget_set_size_request(entry_facets_color_g, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_facets_color_g), "0");
    g_signal_connect(entry_facets_color_g, "activate",
                     G_CALLBACK(foo_entry_facets_color_g), NULL);

    // метка b
    GtkWidget *label_facets_color_b = gtk_label_new("B");
    gtk_fixed_put(GTK_FIXED(fixed), label_facets_color_b, 1200 + x, 475);
    gtk_widget_set_size_request(label_facets_color_b, -1, 50);

    // поле ввода
    entry_facets_color_b = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_facets_color_b, 1220 + x, 485);
    gtk_widget_set_size_request(entry_facets_color_b, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_facets_color_b), "0");
    g_signal_connect(entry_facets_color_b, "activate",
                     G_CALLBACK(foo_entry_facets_color_b), NULL);

    // метка facets
    GtkWidget *label_background = gtk_label_new("Background:");
    gtk_fixed_put(GTK_FIXED(fixed), label_background, 860 + x, 500);
    gtk_widget_set_size_request(label_background, -1, 50);

    // метка facets color
    GtkWidget *label_background_color = gtk_label_new("Color:");
    gtk_fixed_put(GTK_FIXED(fixed), label_background_color, 840 + x, 535);
    gtk_widget_set_size_request(label_background_color, -1, 50);

    // метка r
    GtkWidget *label_background_color_r = gtk_label_new("R");
    gtk_fixed_put(GTK_FIXED(fixed), label_background_color_r, 900 + x, 535);
    gtk_widget_set_size_request(label_background_color_r, -1, 50);

    // поле ввода
    entry_background_color_r = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_background_color_r, 920 + x, 545);
    gtk_widget_set_size_request(entry_background_color_r, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_background_color_r), "1");
    g_signal_connect(entry_background_color_r, "activate",
                     G_CALLBACK(foo_entry_background_color_r), NULL);

    // метка g
    GtkWidget *label_background_color_g = gtk_label_new("G");
    gtk_fixed_put(GTK_FIXED(fixed), label_background_color_g, 900 + x, 570);
    gtk_widget_set_size_request(label_background_color_g, -1, 50);

    // поле ввода
    entry_background_color_g = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_background_color_g, 920 + x, 580);
    gtk_widget_set_size_request(entry_background_color_g, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_background_color_g), "1");
    g_signal_connect(entry_background_color_g, "activate",
                     G_CALLBACK(foo_entry_background_color_g), NULL);

    // метка b
    GtkWidget *label_background_color_b = gtk_label_new("B");
    gtk_fixed_put(GTK_FIXED(fixed), label_background_color_b, 900 + x, 605);
    gtk_widget_set_size_request(label_background_color_b, -1, 50);

    // поле ввода
    entry_background_color_b = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry_background_color_b, 920 + x, 615);
    gtk_widget_set_size_request(entry_background_color_b, 20, 20);
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_background_color_b), "1");
    g_signal_connect(entry_background_color_b, "activate",
                     G_CALLBACK(foo_entry_background_color_b), NULL);
}

void save_screenshot(char *filename) {
    cairo_surface_t *surface =
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 640, 480);
    cairo_t *cairo = cairo_create(surface);
    cairo_translate(cairo, 640 / 2, 480 / 2);
    cairo_scale(cairo, 1.1, -1.1);
    cairo_set_line_width(cairo, 0.05);
    cairo_set_source_rgb(cairo, settings.background_color[0],
                         settings.background_color[1],
                         settings.background_color[2]);
    cairo_paint(cairo);
    draw_vertex(cairo, aboba);
    draw_polygons(cairo, aboba);
    cairo_surface_write_to_png(surface, filename);
}

void save_image(GtkWidget *button, gpointer data) {
    save_screenshot("images/image.jpeg");
    save_screenshot("images/image.bmp");
}

void save_screencast(GtkWidget *button, gpointer data) { NEED_SAVE = 1; }

void read_file_settings() {
    FILE *file = fopen("settings.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d", &settings.type);
        fscanf(file, "%lf %lf %lf", &settings.background_color[0],
               &settings.background_color[1], &settings.background_color[2]);
        fscanf(file, "%d", &settings.line_settings.type);
        fscanf(file, "%d", &settings.line_settings.size);
        fscanf(file, "%lf %lf %lf", &settings.line_settings.color[0],
               &settings.line_settings.color[1],
               &settings.line_settings.color[2]);
        fscanf(file, "%d", &settings.vertex_settings.size);
        fscanf(file, "%d", &settings.vertex_settings.type);
        fscanf(file, "%lf %lf %lf", &settings.vertex_settings.color[0],
               &settings.vertex_settings.color[1],
               &settings.vertex_settings.color[2]);
        fclose(file);

        char str[128];
        sprintf(str, "%lf", settings.background_color[0]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_background_color_r),
                           str);
        sprintf(str, "%lf", settings.background_color[1]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_background_color_g),
                           str);
        sprintf(str, "%lf", settings.background_color[2]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_background_color_b),
                           str);
        sprintf(str, "%lf", settings.line_settings.color[0]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_line_color_r), str);
        sprintf(str, "%lf", settings.line_settings.color[1]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_line_color_g), str);
        sprintf(str, "%lf", settings.line_settings.color[2]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_line_color_b), str);
        sprintf(str, "%lf", settings.vertex_settings.color[0]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_facets_color_r), str);
        sprintf(str, "%lf", settings.vertex_settings.color[1]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_facets_color_g), str);
        sprintf(str, "%lf", settings.vertex_settings.color[2]);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_facets_color_b), str);
        sprintf(str, "%d", settings.line_settings.size);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_line_depth), str);
        sprintf(str, "%d", settings.vertex_settings.size);
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_facets_size), str);

        if (settings.type == PARALLEL)
            gtk_label_set_text(GTK_LABEL(label_projection_type_text),
                               "parallel");
        else if (settings.type == CENTRAL)
            gtk_label_set_text(GTK_LABEL(label_projection_type_text),
                               "central");

        if (settings.line_settings.type == SOLID)
            gtk_label_set_text(GTK_LABEL(label_line_type_text), "solid");
        else if (settings.line_settings.type == DASHED)
            gtk_label_set_text(GTK_LABEL(label_line_type_text), "dashed");

        if (settings.vertex_settings.type == NONE)
            gtk_label_set_text(GTK_LABEL(label_vertex_type_text), "default");
        else if (settings.vertex_settings.type == CIRCLE)
            gtk_label_set_text(GTK_LABEL(label_vertex_type_text), "circle");
        else if (settings.vertex_settings.type == SQUAD)
            gtk_label_set_text(GTK_LABEL(label_vertex_type_text), "square");
    }
}

void write_file_settings() {
    FILE *file = fopen("settings.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d\n", settings.type);
        fprintf(file, "%lf %lf %lf\n", settings.background_color[0],
                settings.background_color[1], settings.background_color[2]);
        fprintf(file, "%d\n", settings.line_settings.type);
        fprintf(file, "%d\n", settings.line_settings.size);
        fprintf(file, "%lf %lf %lf\n", settings.line_settings.color[0],
                settings.line_settings.color[1],
                settings.line_settings.color[2]);
        fprintf(file, "%d\n", settings.vertex_settings.size);
        fprintf(file, "%d\n", settings.vertex_settings.type);
        fprintf(file, "%lf %lf %lf\n", settings.vertex_settings.color[0],
                settings.vertex_settings.color[1],
                settings.vertex_settings.color[2]);
        fclose(file);
    }
}

char *get_string_from_entry(GtkWidget *widget) {
    char *temp = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
    char *str = g_locale_from_utf8(temp, strlen(temp), NULL, NULL, NULL);
#ifdef WIN32
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') str[i] = ',';
    }
#endif
    if (temp != NULL) g_free(temp);
    return str;
}

void foo_entry_line_depth(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_line_depth);
    settings.line_settings.size = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_background_color_r(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_background_color_r);
    settings.background_color[0] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_background_color_g(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_background_color_g);
    settings.background_color[1] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_background_color_b(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_background_color_b);
    settings.background_color[2] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_line_color_r(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_line_color_r);
    settings.line_settings.color[0] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_line_color_g(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_line_color_g);
    settings.line_settings.color[1] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_line_color_b(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_line_color_b);
    settings.line_settings.color[2] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_facets_size(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_facets_size);
    settings.vertex_settings.size = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_facets_color_r(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_facets_color_r);
    settings.vertex_settings.color[0] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_facets_color_g(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_facets_color_g);
    settings.vertex_settings.color[1] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_facets_color_b(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_facets_color_b);
    settings.vertex_settings.color[2] = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_box_projection(GtkWidget *widget, gpointer window) {
    gint count = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    if (count == 0) {  // parallel
        settings.type = PARALLEL;
    } else if (count == 1) {  // central
        settings.type = CENTRAL;
    }
    on_draw(drawing_area, NULL);
    gtk_label_set_text(GTK_LABEL(label_projection_type_text), "");
}

void foo_box_line_type(GtkWidget *widget, gpointer window) {
    gint count = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    if (count == 0) {  // continuous
        settings.line_settings.type = SOLID;
    } else if (count == 1) {  // dotted
        settings.line_settings.type = DASHED;
    }
    on_draw(drawing_area, NULL);
    gtk_label_set_text(GTK_LABEL(label_line_type_text), "");
}

void foo_box_line_display(GtkWidget *widget, gpointer window) {
    gint count = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    if (count == 0) {  // default
        settings.vertex_settings.type = NONE;
    } else if (count == 1) {  // circle
        settings.vertex_settings.type = CIRCLE;
    } else if (count == 2) {  // square
        settings.vertex_settings.type = SQUAD;
    }
    on_draw(drawing_area, NULL);
    gtk_label_set_text(GTK_LABEL(label_vertex_type_text), "");
}

void foo_entry_scale(GtkRange *range, gpointer win) {
    static int value = 0;
    char *temp = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_scale)));
    char *str = g_locale_from_utf8(temp, strlen(temp), NULL, NULL, NULL);
#ifdef WIN32
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') str[i] = ',';
    }
#endif
    if (atof(str) == 0) {
        gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_scale), "n/a");
    } else {
        if (value == 0) {
            zoom(&aboba.matrix_2d, atof(str));
            settings.scale = atof(str);
            value = 1;
        }
        if (value == 1) {
            zoom(&aboba.matrix_2d, 1 / settings.scale);
            zoom(&aboba.matrix_2d, atof(str));
            settings.scale = atof(str);
        }
    }
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
    if (temp != NULL) g_free(temp);
}

void foo_entry_position_ox(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_x);
    move_x(&aboba.matrix_2d, atof(str) - old_value_position_ox);
    old_value_position_ox = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_position_oy(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_y);
    move_y(&aboba.matrix_2d, atof(str) - old_value_position_oy);
    old_value_position_oy = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void foo_entry_position_oz(GtkRange *range, gpointer win) {
    char *str = get_string_from_entry(entry_z);
    move_z(&aboba.matrix_2d, atof(str) - old_value_position_oz);
    old_value_position_oz = atof(str);
    on_draw(drawing_area, NULL);
    if (str != NULL) g_free(str);
}

void label_vertex_count_set_text() {
    char str[128];
    if (aboba.count_vertexs > 0) {
        sprintf(str, "%d", aboba.count_vertexs - 1);
    } else {
        sprintf(str, "%d", aboba.count_vertexs);
    }
    gtk_label_set_text(GTK_LABEL(label_vertex_count), str);
}

void label_facets_count_set_text() {
    char str[128];
    if (aboba.count_facets > 0) {
        sprintf(str, "%d", aboba.count_facets - 1);
    } else {
        sprintf(str, "%d", aboba.count_facets);
    }
    gtk_label_set_text(GTK_LABEL(label_facets_count), str);
}

void reset_label_count() {
    
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_x), "0");
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_y), "0");
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_z), "0");
    gtk_entry_set_text(GTK_ENTRY((GtkWidget *)entry_scale), "1.0");
    gtk_label_set_text(GTK_LABEL(label_rotation_ox), "0");
    gtk_label_set_text(GTK_LABEL(label_rotation_oy), "0");
    gtk_label_set_text(GTK_LABEL(label_rotation_oz), "0");

    old_value_rotation_ox = 0;
    old_value_rotation_oy = 0;
    old_value_rotation_oz = 0;

    old_value_position_ox = 0;
    old_value_position_oy = 0;
    old_value_position_oz = 0;

    settings.scale = 1.0;

    gtk_widget_destroy(scale_rotation_ox);
    scale_rotation_ox =
        gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 360, 1);
    gtk_fixed_put(GTK_FIXED(fixed), scale_rotation_ox, 860, 110);
    gtk_widget_set_size_request(scale_rotation_ox, 200, -1);
    gtk_scale_set_draw_value(GTK_SCALE(scale_rotation_ox), FALSE);
    g_signal_connect(scale_rotation_ox, "value-changed",
                     G_CALLBACK(foo_scale_rotation_ox), label_rotation_ox);

    gtk_widget_destroy(scale_rotation_oy);
    scale_rotation_oy =
        gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 360, 1);
    gtk_fixed_put(GTK_FIXED(fixed), scale_rotation_oy, 860, 145);
    gtk_widget_set_size_request(scale_rotation_oy, 200, -1);
    gtk_scale_set_draw_value(GTK_SCALE(scale_rotation_oy), FALSE);
    g_signal_connect(scale_rotation_oy, "value-changed",
                     G_CALLBACK(foo_scale_rotation_oy), label_rotation_oy);

    gtk_widget_destroy(scale_rotation_oz);
    scale_rotation_oz =
        gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 360, 1);
    gtk_fixed_put(GTK_FIXED(fixed), scale_rotation_oz, 860, 180);
    gtk_widget_set_size_request(scale_rotation_oz, 200, -1);
    gtk_scale_set_draw_value(GTK_SCALE(scale_rotation_oz), FALSE);
    g_signal_connect(scale_rotation_oz, "value-changed",
                     G_CALLBACK(foo_scale_rotation_oz), label_rotation_oz);

    gtk_widget_show(scale_rotation_ox);
    gtk_widget_show(scale_rotation_oy);
    gtk_widget_show(scale_rotation_oz);
}

void foo_scale_rotation_ox(GtkRange *range, gpointer win) {
    gdouble val = gtk_range_get_value(range);
    gchar *str = g_strdup_printf("%.f", val);
    gtk_label_set_text(GTK_LABEL(win), str);
    rotation_by_ox(&aboba.matrix_2d,
                   0.0174533 * (atof(str) - old_value_rotation_ox));
    old_value_rotation_ox = atof(str);
    on_draw(drawing_area, NULL);
    g_free(str);
}

void foo_scale_rotation_oy(GtkRange *range, gpointer win) {
    gdouble val = gtk_range_get_value(range);
    gchar *str = g_strdup_printf("%.f", val);
    gtk_label_set_text(GTK_LABEL(win), str);
    rotation_by_oy(&aboba.matrix_2d,
                   0.0174533 * (atof(str) - old_value_rotation_oy));
    old_value_rotation_oy = atof(str);
    on_draw(drawing_area, NULL);
    g_free(str);
}

void foo_scale_rotation_oz(GtkRange *range, gpointer win) {
    gdouble val = gtk_range_get_value(range);
    gchar *str = g_strdup_printf("%.f", val);
    gtk_label_set_text(GTK_LABEL(win), str);
    rotation_by_oz(&aboba.matrix_2d,
                   0.0174533 * (atof(str) - old_value_rotation_oz));
    old_value_rotation_oz = atof(str);
    on_draw(drawing_area, NULL);
    g_free(str);
}

void select_file(GtkWidget *widget, gpointer gFilepath) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "gtk-close",
        GTK_RESPONSE_CANCEL, "document-open", GTK_RESPONSE_ACCEPT, NULL);

    const guint MY_SELECTED = 0;
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Select", MY_SELECTED);

    guint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT || response == MY_SELECTED) {
        // if (file_name != NULL)
        //    g_free(file_name);
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        aboba = file_reader(file_name);
        label_vertex_count_set_text();
        label_facets_count_set_text();
        reset_label_count();
        // auto_scale(&aboba);
        char *new_file_name = get_file_name();
        gtk_label_set_text(GTK_LABEL((GtkWidget *)label_file_name),
                           new_file_name);
        g_free(new_file_name);
    }
    gtk_widget_destroy(dialog);
}

char *get_file_name() {
    int point = 0;
    for (int i = 0; file_name[i] != '\0'; i++) {
#ifdef WIN32
        if (file_name[i] == '\\')
#else
        if (file_name[i] == '/')
#endif
            point = i;
    }
    char *new_file_name = (char *)malloc(strlen(file_name) * sizeof(char));
    int i = 0;
    point++;
    for (i = point; file_name[i] != '\0'; i++)
        new_file_name[i - point] = file_name[i];
    new_file_name[i - point] = '\0';
    return new_file_name;
}

void init_settings(setting_t *config) {
    config->type = PARALLEL;
    for (int i = 0; i < 3; i++) {
        config->background_color[i] = 255.0;
    }
    config->line_settings.type = SOLID;
    config->line_settings.size = 1;
    for (int i = 0; i < 3; i++) {
        config->line_settings.color[i] = 0.0;
    }
    config->vertex_settings.type = NONE;
    config->vertex_settings.size = 1;
    for (int i = 0; i < 3; i++) {
        config->line_settings.color[i] = 0.0;
    }
}