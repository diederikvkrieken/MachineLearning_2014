#ifndef GRID_H
#define GRID_H

class Grid
{
  public:
    void init();
    void addHuman(human *h);
  private:
    int grid_root;  // Amount of cells is [grid_root]^2
    grid_cell **grid;
};

#endif
