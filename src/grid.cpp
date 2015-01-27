#include "grid.h"

void Grid::init()
{
  grid_root = 11;

  grid = new grid_cell*[grid_root];
  for(int i=0; i < grid_root; i++)
  {
    grid[i] = new grid_cell[grid_root];
  }
}

















