// Copyright 2018
// Author: Ilia Fedotov, molotok93@gmx.de


#include "./Bridges.h"
#include "./Visualization.h"

// Main function.
int main(int argc, char** argv) {
  Bridges b;
  Bridges *t = &b;
  b.commands(argc, argv);
  b.play(t);
}