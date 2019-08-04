// Copyright 2018
// Author: Ilia Fedotov, molotok93@gmx.de

#ifndef VISUALIZATION_H_
#define VISUALIZATION_H_

#include <ncurses.h>
#include <string>
#include <vector>
#include "./Bridges.h"

class Bridges;

class Visualization {
 public:
  Bridges *_b;
  // Constructor
  explicit Visualization(Bridges *bi);

  // Initialization of the screen
  static void Initialize();

 private:
  friend class Bridges;
  FRIEND_TEST(Visualization, Constructor);
  // spot last clicked
  int _lastClickedX, _lastClickedY;
  // borders of the map
  size_t _drawMaxY, _drawMaxX, _drawMinY, _drawMinX;
  // size of isles
  size_t _xSize, _ySize;

  // bridges amount: actual and just the possible
  string helperPrintBridge(const string bridge);
  string _bNum;
  FRIEND_TEST(Visualization, helperPrintBridge);

  // highlight the bridge
  void neighborsHighlight(const size_t x, const size_t y, int colour);
  void helperHighlight(const bool t);
  void saveInts(string s);
  FRIEND_TEST(Visualization, saveInts);
  size_t _gotX, _gotY;
  void printBridge(const vector<vector<string>> bridges, const size_t size_x,
      const size_t size_y);
  FRIEND_TEST(Visualization, saveNeighbors);

  // connect bridge in the map
  void connectBridge(const size_t x, const size_t y, const bool g);
  void setConnect(const size_t from, const size_t count,
      const size_t stable, const int choose);
  size_t _centerX, _centerY;
  FRIEND_TEST(Visualization, setConnect_connectBridge);

  // draw the isle
  void printConnection(const string s, const size_t x, const size_t y,
      const int i);
  void updateConnectCount(const size_t x, const size_t y, const bool reset);
  size_t _count;
  vector<string> _undo;
  FRIEND_TEST(Visualization, updateConnectCount);

  // Neighbors in string
  void saveNeighbors(const string s);
  string _xy, _xr, _xl, _yu, _yl;
  // play function to support play in class Bridges
  int playClick();
  int processClick(int key);
  void undoConnection();
  size_t _draws;
  string _oldxy;
  bool _highlight;

  // solver
  void solverBridge();
};

#endif  // VISUALIZATION_H_