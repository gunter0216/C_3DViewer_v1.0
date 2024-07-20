#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "filereader.h"

START_TEST(fileread_test1) {
    data_t temp;
    temp = file_reader("obj/cube.obj");
    ck_assert_int_eq(temp.count_vertexs, 9);
    ck_assert_int_eq(temp.count_facets, 13);
    remove_data(&temp);
}
END_TEST

START_TEST(fileread_test2) {
    data_t temp;
    temp = file_reader("obj/cube.obj");
    ck_assert_int_eq(temp.extremes.x_max, 1);
    ck_assert_int_eq(temp.extremes.y_max, 1);
    ck_assert_int_eq(temp.extremes.x_min, 0);
    ck_assert_int_eq(temp.extremes.y_min, 0);
    remove_data(&temp);
}
END_TEST

START_TEST(struct_test1) {
    double test_shift_x[] = {
        1, 0.0, 0.0, 
        1.0, 0.0, 0.0, 
        1.0, 0.0, 0.0, 
        1.0, 00.0, 0.0,
    };
    matrix_t test_shift_x_matr = array_to_matrix(test_shift_x, 4, 3);
    move_x(&test_shift_x_matr, 2.5);

    double result_shift_x[] = {
        1.0+2.5, 0.0, 0.0, 
        1.0+2.5, 0.0, 0.0, 
        1.0+2.5, 0.0, 0.0, 
        1.0+2.5, 00.0, 0.0,
    };
    matrix_t result_shift_x_matrix = array_to_matrix(result_shift_x, 4, 3);
    int result_shift_x_int =
        s21_eq_matrix(&test_shift_x_matr, &result_shift_x_matrix);
    ck_assert_int_eq(result_shift_x_int, 1);
    s21_remove_matrix(&test_shift_x_matr);
    s21_remove_matrix(&result_shift_x_matrix);
}
END_TEST

START_TEST(struct_test2) {
    double test_shift_y[] = {
        1.0, 0.0, 0.0, 
        1.0, 0.0, 0.0, 
        1.0, 0.0, 0.0, 
        1.0, 0.0, 0.0,
    };
    matrix_t test_shift_y_matr = array_to_matrix(test_shift_y, 4, 3);
    move_y(&test_shift_y_matr, 2.5);

    double result_shift_y[] = {
        1.0, 0.0+2.5, 0.0, 
        1.0, 0.0+2.5, 0.0, 
        1.0, 0.0+2.5, 0.0, 
        1.0, 0.0+2.5, 0.0, 
    };
    matrix_t result_shift_y_matrix = array_to_matrix(result_shift_y, 4, 3);
    int result_shift_y_int =
        s21_eq_matrix(&test_shift_y_matr, &result_shift_y_matrix);
    ck_assert_int_eq(result_shift_y_int, 1);
    s21_remove_matrix(&test_shift_y_matr);
    s21_remove_matrix(&result_shift_y_matrix);
}
END_TEST

START_TEST(struct_test3) {
    double test_shift_z[] = {
        1.0, 0.0, 0.0, 
        1.0, 0.0, 0.0, 
        1.0, 0.0, 0.0, 
        1.0, 0.0, 0.0,
    };
    matrix_t test_shift_z_matr = array_to_matrix(test_shift_z, 4, 3);
    move_z(&test_shift_z_matr, 2.5);

    double result_shift_z[] = {
        1.0, 0.0, 0.0+2.5, 
        1.0, 0.0, 0.0+2.5, 
        1.0, 0.0, 0.0+2.5, 
        1.0, 0.0, 0.0+2.5, 
    };
    matrix_t result_shift_z_matrix = array_to_matrix(result_shift_z, 4, 3);
    int result_shift_z_int =
        s21_eq_matrix(&test_shift_z_matr, &result_shift_z_matrix);
    ck_assert_int_eq(result_shift_z_int, 1);
        s21_remove_matrix(&test_shift_z_matr);
    s21_remove_matrix(&result_shift_z_matrix);
}
END_TEST


START_TEST(struct_test4) {
    double test_zoom[] = {
        1.0, 2.0, 3.0, 
        1.0, 2.0, 3.0, 
        1.0, 2.0, 3.0, 
        1.0, 2.0, 3.0, 
    };
    matrix_t test_zoom_matr = array_to_matrix(test_zoom, 4, 3);
    zoom(&test_zoom_matr, 2.5);

    double result_zoom[] = {
        1.0*2.5, 2.0*2.5, 3.0*2.5,  
        1.0*2.5, 2.0*2.5, 3.0*2.5, 
        1.0*2.5, 2.0*2.5, 3.0*2.5, 
        1.0*2.5, 2.0*2.5, 3.0*2.5,
    };
    matrix_t result_zoom_matrix = array_to_matrix(result_zoom, 4, 3);
    int result_zoom_int =
        s21_eq_matrix(&test_zoom_matr, &result_zoom_matrix);
    ck_assert_int_eq(result_zoom_int, 1);
    s21_remove_matrix(&test_zoom_matr);
    s21_remove_matrix(&result_zoom_matrix);
}
END_TEST

START_TEST(struct_test5) {
    double test_rotation_z[] = {
        10, 0.4, 0.0,
        20.0, 0.0, 0.45,
        0.0, 1.0, 1.0,
        20.0, 20.0, 20.0,
    };
    matrix_t test_rotation_z_matr = array_to_matrix(test_rotation_z, 4, 3);
    rotation_by_oz(&test_rotation_z_matr, 45.8);

    double result_rotation_z[] = {
        cos(45.8)*10 - sin(45.8)* 0.4, sin(45.8)*10 + cos(45.8)*0.4, 0.0,
        cos(45.8)*20, sin(45.8)*20, 0.45,
         - sin(45.8)*1.0, cos(45.8), 1.0,
        cos(45.8)*20 - sin(45.8)*20.0, sin(45.8)*20 + cos(45.8)*20, 20.0,
    };
    matrix_t result_rotation_z_matrix = array_to_matrix(result_rotation_z, 4, 3);
    int result_rotation_z_int =
        s21_eq_matrix(&test_rotation_z_matr, &result_rotation_z_matrix);
    ck_assert_int_eq(result_rotation_z_int, 1);
    s21_remove_matrix(&test_rotation_z_matr);
    s21_remove_matrix(&result_rotation_z_matrix);
}
END_TEST

START_TEST(struct_test6) {
    double test_rotation_y[] = {
        10, 0.4, 0.0,
        20.0, 0.0, 0.45,
        0.0, 1.0, 1.0,
        20.0, 20.0, 20.0,
    };
    matrix_t test_rotation_y_matr = array_to_matrix(test_rotation_y, 4, 3);
    rotation_by_oy(&test_rotation_y_matr, -0.8);

    double result_rotation_y[] = {
        cos(-0.8)*10,  0.4, -sin(-0.8)*10,
        cos(-0.8)*20 + sin(-0.8)*0.45, 0, -sin(-0.8)*20 + cos(-0.8)*0.45,
        +sin(-0.8), 1.0, cos(-0.8)*1,
        cos(-0.8)*20 + sin(-0.8)*20, 20, -sin(-0.8)*20 + cos(-0.8)*20
    };
    matrix_t result_rotation_y_matrix = array_to_matrix(result_rotation_y, 4, 3);
    int result_rotation_y_int =
        s21_eq_matrix(&test_rotation_y_matr, &result_rotation_y_matrix);
    ck_assert_int_eq(result_rotation_y_int, 1);
        s21_remove_matrix(&test_rotation_y_matr);
    s21_remove_matrix(&result_rotation_y_matrix);
}
END_TEST

START_TEST(struct_test7) {
    double test_rotation_x[] = {
        10, 0.4, 0.0,
        20.0, 0.0, 0.45,
        0.0, 1.0, 1.0,
        20.0, 20.0, 20.0,
    };
    matrix_t test_rotation_x_matr = array_to_matrix(test_rotation_x, 4, 3);
    rotation_by_ox(&test_rotation_x_matr, -23);

    double result_rotation_x[] = {
        10, cos(-23)*0.4,  sin(-23)*0.4,
        20.0, -sin(-23)*0.45, cos(-23)*0.45,
        0.0, cos(-23)*1 - sin(-23)*1,  sin(-23)*1 + cos(-23)*1,
        20.0, cos(-23)*20 - sin(-23)*20, sin(-23)*20 + cos(-23)*20,
    };
    matrix_t result_rotation_x_matrix = array_to_matrix(result_rotation_x, 4, 3);
    int result_rotation_x_int =
        s21_eq_matrix(&test_rotation_x_matr, &result_rotation_x_matrix);
    ck_assert_int_eq(result_rotation_x_int, 1);
        s21_remove_matrix(&test_rotation_x_matr);
    s21_remove_matrix(&result_rotation_x_matrix);
}
END_TEST

START_TEST(struct_test8) {
    data_t temp;
    temp = file_reader("obj/cube.obj");
    matrix_t test_parralel_proj_matr;
    test_parralel_proj_matr = parallel_projection(temp);
    double result_parralel_proj[] = {
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 1.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 1.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 1.0,
        1.0, 1.0, 0.0,
        1.0, 1.0, 1.0,
    };
    matrix_t result_parralel_proj_matrix = array_to_matrix(result_parralel_proj, 9, 3);
    int result_parralel_proj_int =
        s21_eq_matrix(&test_parralel_proj_matr, &result_parralel_proj_matrix);
    ck_assert_int_eq(result_parralel_proj_int, 1);
    remove_data(&temp);
    s21_remove_matrix(&test_parralel_proj_matr);
    s21_remove_matrix(&result_parralel_proj_matrix);
}
END_TEST

START_TEST(struct_test9) {
    data_t temp;
    temp = file_reader("obj/cube.obj");
    matrix_t test_central_proj_matr;
    test_central_proj_matr = central_projection(temp);
    double result_central_proj[] = {
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 1.0,
        0.0, 1.0, 0.0,
        0.0, neconst*1.0/(neconst-1.0), 1.0,
        1.0, 0.0, 0.0,
        neconst*1.0/(neconst-1.0), 0.0, 1.0,
        1.0, 1.0, 0.0,
        neconst*1.0/(neconst-1.0), neconst*1.0/(neconst-1.0), 1.0,
    };
    matrix_t result_central_proj_matrix = array_to_matrix(result_central_proj, 9, 3);
    int result_central_proj_int =
        s21_eq_matrix(&test_central_proj_matr, &result_central_proj_matrix);
    ck_assert_int_eq(result_central_proj_int, 1);
        remove_data(&temp);
    s21_remove_matrix(&test_central_proj_matr);
    s21_remove_matrix(&result_central_proj_matrix);
}
END_TEST

int main(void) {
    Suite *s1 = suite_create("Core");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, fileread_test1);
    tcase_add_test(tc1_1, fileread_test2);

    tcase_add_test(tc1_1, struct_test1);
    tcase_add_test(tc1_1, struct_test2);
    tcase_add_test(tc1_1, struct_test3);
    tcase_add_test(tc1_1, struct_test4);
    tcase_add_test(tc1_1, struct_test5);
    tcase_add_test(tc1_1, struct_test6);
    tcase_add_test(tc1_1, struct_test7);
    tcase_add_test(tc1_1, struct_test8);
    tcase_add_test(tc1_1, struct_test9);








    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}
