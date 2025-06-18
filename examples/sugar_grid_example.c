#include <stdio.h>
#include <sugar-ext.h>

static void print_grid_state(SugarGrid *grid) {
  printf("Grid dimensions: %dx%d\n", grid->width, grid->height);

  for (gint y = 0; y < grid->height; y++) {
    for (gint x = 0; x < grid->width; x++) {
      GdkRectangle cell = {x, y, 1, 1};
      guint weight = sugar_grid_compute_weight(grid, &cell);
      printf("%2d ", weight);
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  printf("Sugar Grid Example\n");
  printf("==================\n\n");

  //  8x6 grid
  SugarGrid *grid = g_object_new(SUGAR_TYPE_GRID, NULL);
  sugar_grid_setup(grid, 8, 6);

  printf("1. Initial empty grid:\n");
  print_grid_state(grid);

  // obstacles
  GdkRectangle obstacle1 = {1, 1, 2, 2};
  GdkRectangle obstacle2 = {5, 2, 2, 3};

  sugar_grid_add_weight(grid, &obstacle1);
  sugar_grid_add_weight(grid, &obstacle2);

  printf("2. Grid with obstacles:\n");
  print_grid_state(grid);

  // removal
  printf("3. Removing first obstacle:\n");
  sugar_grid_remove_weight(grid, &obstacle1);
  print_grid_state(grid);

  // resize grid
  printf("4. Resizing grid to 12x8:\n");
  sugar_grid_setup(grid, 12, 8);
  print_grid_state(grid);

  g_object_unref(grid);

  printf("Example completed successfully!\n");
  return 0;
}
