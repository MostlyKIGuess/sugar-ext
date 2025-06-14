#include <glib.h>
#include <sugar-ext.h>

static void test_sugar_grid_creation(void) {
  SugarGrid *grid = g_object_new(SUGAR_TYPE_GRID, NULL);
  g_assert_nonnull(grid);

  sugar_grid_setup(grid, 10, 10);
  g_assert_cmpint(grid->width, ==, 10);
  g_assert_cmpint(grid->height, ==, 10);

  g_object_unref(grid);
}

static void test_sugar_grid_setup(void) {
  SugarGrid *grid = g_object_new(SUGAR_TYPE_GRID, NULL);

  sugar_grid_setup(grid, 20, 15);
  g_assert_cmpint(grid->width, ==, 20);
  g_assert_cmpint(grid->height, ==, 15);

  g_object_unref(grid);
}

static void test_sugar_grid_add_weight(void) {
  SugarGrid *grid = g_object_new(SUGAR_TYPE_GRID, NULL);
  sugar_grid_setup(grid, 10, 10);

  GdkRectangle rect = {2, 2, 3, 3};
  sugar_grid_add_weight(grid, &rect);

  guint weight = sugar_grid_compute_weight(grid, &rect);
  g_assert_cmpuint(weight, ==, 9); // 3x3 = 9 cells, each with weight 1

  // Test single cell
  GdkRectangle single_cell = {2, 2, 1, 1};
  guint single_weight = sugar_grid_compute_weight(grid, &single_cell);
  g_assert_cmpuint(single_weight, ==, 1);

  g_object_unref(grid);
}

static void test_sugar_grid_remove_weight(void) {
  SugarGrid *grid = g_object_new(SUGAR_TYPE_GRID, NULL);
  sugar_grid_setup(grid, 10, 10);

  GdkRectangle rect = {2, 2, 3, 3};

  // double weights here
  sugar_grid_add_weight(grid, &rect);
  sugar_grid_add_weight(grid, &rect);

  guint weight = sugar_grid_compute_weight(grid, &rect);
  g_assert_cmpuint(weight, ==, 18); // 3x3 = 9 cells, each with weight 2

  // weight removed, so now 2->1
  sugar_grid_remove_weight(grid, &rect);

  weight = sugar_grid_compute_weight(grid, &rect);
  g_assert_cmpuint(weight, ==, 9); // 3x3 = 9 cells, each with weight 1

  sugar_grid_remove_weight(grid, &rect);

  weight = sugar_grid_compute_weight(grid, &rect);
  g_assert_cmpuint(weight, ==, 0); // All weights should be 0

  g_object_unref(grid);
}

static void test_sugar_grid_bounds_checking(void) {
  SugarGrid *grid = g_object_new(SUGAR_TYPE_GRID, NULL);
  sugar_grid_setup(grid, 5, 5);

  // Test rectangle outside bounds
  GdkRectangle out_of_bounds = {3, 3, 4, 4}; // Extends beyond 5x5 grid

  // should not crash and should produce warnings
  g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                        "*outside the grid bounds*");
  sugar_grid_add_weight(grid, &out_of_bounds);
  g_test_assert_expected_messages();

  g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                        "*outside the grid bounds*");
  sugar_grid_remove_weight(grid, &out_of_bounds);
  g_test_assert_expected_messages();

  g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                        "*outside the grid bounds*");
  guint weight = sugar_grid_compute_weight(grid, &out_of_bounds);
  g_test_assert_expected_messages();
  g_assert_cmpuint(weight, ==, 0);

  g_object_unref(grid);
}

int main(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);

  g_test_add_func("/sugar/grid/creation", test_sugar_grid_creation);
  g_test_add_func("/sugar/grid/setup", test_sugar_grid_setup);
  g_test_add_func("/sugar/grid/add-weight", test_sugar_grid_add_weight);
  g_test_add_func("/sugar/grid/remove-weight", test_sugar_grid_remove_weight);
  g_test_add_func("/sugar/grid/bounds-checking",
                  test_sugar_grid_bounds_checking);

  return g_test_run();
}
