#include "canvas.h"
#include "lib/minunit.h"
#include "util.h"

static int cols = 2;
static int rows = 3;
static char load_str[] = "012345";

/*
 * c1 is normally loaded as a 3x2:
 *   01
 *  +---
 * 0|01
 * 1|23
 * 2|45
 *  |
 */

static Canvas *c1, *c2;

// Begin canvas_bootstrapping tests
MU_TEST(test_canvas_new) {
  c1 = canvas_new(rows, cols);
  mu_assert(c1->num_cols == cols, "cols should be 2");
  mu_assert(c1->num_rows == rows, "rows should be 3");
  canvas_free(c1);
}

MU_TEST(test_canvas_load_str) {
  c1 = canvas_new(rows, cols);
  int numset = canvas_load_str(c1, load_str);
  mu_assert(numset == sizeof(load_str) - 1, "numset should be 6");
  mu_check(c1->rows[0][0] == '0');
  mu_check(c1->rows[0][1] == '1');
  mu_check(c1->rows[1][0] == '2');
  mu_check(c1->rows[1][1] == '3');
  mu_check(c1->rows[2][0] == '4');
  mu_check(c1->rows[2][1] == '5');

  canvas_free(c1);
}

MU_TEST(test_canvas_eq) {
  // canvas with same content
  c1 = canvas_new(rows, cols);
  c2 = canvas_new(rows, cols);

  // canvases of different sizes
  Canvas *c3 = canvas_new(cols - 1, rows);
  Canvas *c4 = canvas_new(cols, rows - 1);

  // init all canvases with same str
  canvas_load_str(c1, load_str);
  canvas_load_str(c2, load_str);
  canvas_load_str(c3, load_str);
  canvas_load_str(c4, load_str);

  // same canvases should be the same
  mu_check(canvas_eq(c1, c2));
  mu_check(canvas_eq(c2, c1));

  // different canvases should be different
  mu_check(!canvas_eq(c1, c3));
  mu_check(!canvas_eq(c1, c4));
  mu_check(!canvas_eq(c3, c4));

  // change one of the same canvases
  c1->rows[2][1] = 'A';
  mu_check(!canvas_eq(c1, c2));

  // change the other to match
  c2->rows[2][1] = 'A';
  mu_check(canvas_eq(c1, c2));

  canvas_free(c1);
  canvas_free(c2);
  canvas_free(c3);
  canvas_free(c4);
}

MU_TEST(test_canvas_isin) {
  c1 = canvas_new(rows, cols);
  // points inside canvas
  mu_assert(canvas_isin_yx(c1, 0, 0), "Top left corner");
  mu_assert(canvas_isin_yx(c1, 2, 1), "Bottom right corner");
  mu_assert(canvas_isin_i(c1, 0), "First index");
  mu_assert(canvas_isin_i(c1, 5), "Last index");

  // points outside of canvas
  mu_assert(!canvas_isin_y(c1, -1), "negative y");
  mu_assert(!canvas_isin_x(c1, -1), "negative x");

  mu_assert(!canvas_isin_y(c1, c1->num_rows), "num_rows");
  mu_assert(!canvas_isin_x(c1, c1->num_cols), "num_cols");

  mu_assert(!canvas_isin_i(c1, -1), "Negative index");
  mu_assert(!canvas_isin_i(c1, 6), "Larger index");
}

// Tests to make sure the setup/teardown and eq functions will work
MU_TEST_SUITE(canvas_bootstrapping) {
  MU_RUN_TEST(test_canvas_new);
  MU_RUN_TEST(test_canvas_load_str);
  MU_RUN_TEST(test_canvas_eq);
  MU_RUN_TEST(test_canvas_isin);
}

// Begin canvas_main tests
void test_setup(void) {
  c1 = canvas_new(rows, cols);
  canvas_load_str(c1, load_str);
}

void test_teardown(void) {
  canvas_free(c1);
}

MU_TEST(test_canvas_gcharyx) {
  mu_check(c1->rows[2][1] == canvas_gcharyx(c1, 2, 1));
  mu_check(c1->rows[2][1] != canvas_gcharyx(c1, 2, 0));
}

MU_TEST(test_canvas_gchari) {
  mu_check(c1->rows[2][1] == canvas_gchari(c1, 5));
  mu_check(c1->rows[2][1] != canvas_gchari(c1, 4));
}

MU_TEST(test_canvas_scharyx) {
  mu_check(c1->rows[0][0] != 'X');
  canvas_scharyx(c1, 0, 0, 'X');
  mu_check(c1->rows[0][0] == 'X');

  mu_check(c1->rows[2][1] != 'X');
  canvas_scharyx(c1, 2, 1, 'X');
  mu_check(c1->rows[2][1] == 'X');
}

MU_TEST(test_canvas_schari) {
  mu_check(canvas_gchari(c1, 0) != 'X');
  canvas_schari(c1, 0, 'X');
  mu_check(canvas_gchari(c1, 0) == 'X');

  mu_check(canvas_gchari(c1, 4) != 'X');
  canvas_schari(c1, 4, 'X');
  mu_check(canvas_gchari(c1, 4) == 'X');
}

MU_TEST(test_canvas_cpy) {
  c2 = canvas_cpy(c1);

  mu_assert(c1 != c2, "Canvas pointers should not be the same");
  mu_assert(canvas_eq(c1, c2), "Canvases should be equal");
  canvas_schari(c2, 5, 'X');
  mu_assert(!canvas_eq(c1, c2), "Canvases should no longer be equal");

  canvas_free(c2);
}

MU_TEST(test_canvas_cpy_p1p2) {
  // copy the left column of c1
  c2 = canvas_cpy_p1p2(c1, 0, 0, 2, 0);

  mu_assert(c1 != c2, "Canvas pointers should not be the same");
  mu_assert(!canvas_eq(c1, c2), "Canvases should not be equal");

  mu_assert_int_eq(3, c2->num_rows);
  mu_assert_int_eq(1, c2->num_cols);

  mu_assert_double_eq('0', canvas_gcharyx(c2, 0, 0));
  mu_assert_double_eq('2', canvas_gcharyx(c2, 1, 0));
  mu_assert_double_eq('4', canvas_gcharyx(c2, 2, 0));

  canvas_free(c2);

  char *ldstr = "123456789";
  Canvas *c3 = canvas_new(3, 3);
  canvas_ldstr(c3, ldstr);
  Canvas *c4 = canvas_cpy_p1p2(c3, 0, 0, c3->num_rows - 1, c3->num_cols - 1);
  mu_assert(canvas_eq(c3, c4), "Canvases should be equal");
  canvas_free(c4);

  c4 = canvas_cpy_p1p2(c3, 1, 1, c3->num_rows - 1, c3->num_cols - 1);

  mu_assert_int_eq(2, c4->num_cols);
  mu_assert_int_eq(2, c4->num_rows);
  mu_assert_int_eq(canvas_gcharyx(c3, 1, 1), canvas_gcharyx(c4, 0, 0));
}

MU_TEST(test_canvas_ldcanvasyx) {
  c2 = canvas_cpy(c1);
  // copy the left column of c1 into the right column of c2, shifted down 1
  int res = canvas_ldcanvasyx(c2, c1, 1, 1);
  mu_assert_int_eq(1, res);

  mu_assert(!canvas_eq(c1, c2), "Canvases should not be equal");

  // check left column
  mu_assert_double_eq('0', canvas_gcharyx(c2, 0, 0));
  mu_assert_double_eq('2', canvas_gcharyx(c2, 1, 0));
  mu_assert_double_eq('4', canvas_gcharyx(c2, 2, 0));

  // check right column
  mu_assert_double_eq('1', canvas_gcharyx(c2, 0, 1));  // same
  mu_assert_double_eq('0', canvas_gcharyx(c2, 1, 1));
  mu_assert_double_eq('2', canvas_gcharyx(c2, 2, 1));

  canvas_free(c2);
}

MU_TEST(test_canvas_ldcanvasyxc) {
  c2 = canvas_cpy(c1);
  // copy the left column of c1 into the right column of c2, shifted down 1, and
  // skipping the second value
  int res = canvas_ldcanvasyxc(c2, c1, 1, 1, '2');
  mu_assert_int_eq(1, res);

  mu_assert(!canvas_eq(c1, c2), "Canvases should not be equal");

  // check left column
  mu_assert_double_eq('0', canvas_gcharyx(c2, 0, 0));
  mu_assert_double_eq('2', canvas_gcharyx(c2, 1, 0));
  mu_assert_double_eq('4', canvas_gcharyx(c2, 2, 0));

  // check right column
  mu_assert_double_eq('1', canvas_gcharyx(c2, 0, 1));  // same
  mu_assert_double_eq('0', canvas_gcharyx(c2, 1, 1));
  mu_assert_double_eq('5', canvas_gcharyx(c2, 2, 1));  // same

  canvas_free(c2);
}

MU_TEST(test_canvas_trimc) {
  c2 = canvas_new(c1->num_rows, c1->num_cols);
  canvas_fill(c2, ' ');
  canvas_ldcanvasyx(c2, c1, 1, 1);

  Canvas *c3 = canvas_trimc(c2, ' ', true, true, true, true);
  mu_assert_int_eq(1, c3->num_cols);
  mu_assert_int_eq(2, c3->num_rows);

  Canvas *c4 = canvas_cpy_p1p2(c2, 1, 1, c3->num_cols - 1, c3->num_rows - 1);

  canvas_free(c2);
  canvas_free(c3);
  canvas_free(c4);
}

MU_TEST(test_canvas_serialize_deserialize) {
  char buf[c1->num_rows * c1->num_cols];
  int numwritten = canvas_serialize(c1, buf);
  mu_assert(numwritten == sizeof(buf),
            "Serialized form should be size cols*rows");

  c2 = canvas_new(c1->num_rows, c1->num_cols);
  canvas_deserialize(buf, c2);
  mu_assert(canvas_eq(c1, c2), "Canvases should be equivalent");

  canvas_free(c2);
}

// test the rest of canvas functions
MU_TEST_SUITE(canvas_main) {
  MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

  MU_RUN_TEST(test_canvas_gcharyx);
  MU_RUN_TEST(test_canvas_gchari);
  MU_RUN_TEST(test_canvas_scharyx);
  MU_RUN_TEST(test_canvas_schari);

  MU_RUN_TEST(test_canvas_ldcanvasyx);
  MU_RUN_TEST(test_canvas_ldcanvasyxc);

  MU_RUN_TEST(test_canvas_cpy);
  MU_RUN_TEST(test_canvas_cpy_p1p2);

  MU_RUN_TEST(test_canvas_trimc);

  MU_RUN_TEST(test_canvas_serialize_deserialize);
}

int main(int argc, char const *argv[]) {
  MU_RUN_SUITE(canvas_bootstrapping);
  MU_RUN_SUITE(canvas_main);
  MU_REPORT();
  return minunit_status;
}
