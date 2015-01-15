#include "grid.h"

void Grid::init()
{
  grid_root = 11;
  
  grid = new Grid*[grid_root];
  for(int i=0; i < grid_root; i++)
  {
    grid[i] = new Grid[grid_root];
  }
}

















