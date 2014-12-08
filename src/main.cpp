#include "main.h"

int main(int argc, char* args[])
{
  Master master;

  if(master.init())
  { master.run(); }
  else
  { fprintf(stderr, "Failed to initialize, exiting.\n"); }

	return 0;
}

