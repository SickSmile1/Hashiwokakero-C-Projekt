// Copyright 2018
// Author: Ilia Fedotov, molotok93@gmx.de

#ifndef BRIDGES_H_
#define BRIDGES_H_

#include <gtest/gtest.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Visualization;

class Bridges {
 public:
  Bridges();
  ~Bridges();
  vector<vector<string>> createGame(string fN);
  // map holding the bridges and the islands
  vector<vector<string>> _map;
  void play(Bridges *b);

  size_t *ptr;
  vector<string> _solver;
  void saveSolution(string fN, int i);

  void commands(const int argc, char** argv);

 private:
  friend class Visualization;
  // read all the islands in the map, plain and x// y
  string _playname;
  size_t _draw;
  string _result;
  FRIEND_TEST(Bridges, solverRead);
  FRIEND_TEST(Bridges, createXY);
  FRIEND_TEST(Bridges, createXY1);
  FRIEND_TEST(Bridges, createPlain);
  FRIEND_TEST(Bridges, createPlain1);
  FRIEND_TEST(Bridges, createPlain2);
  FRIEND_TEST(Bridges, constructor);

  // variable for the field size
  size_t _size_x, _size_y;

  // Adding bridges for different formats
  void addBridgePlain(const size_t y, const size_t count,
      const string line, const bool _t);
  void addBridgeXY(const string x);
  FRIEND_TEST(Bridges, addBridgePlain);
  FRIEND_TEST(Bridges, addBridgeXY);

  // Coords
  void saveCoords(const string xy);
  size_t _x, _y;
  FRIEND_TEST(Bridges, coords);

  // neighbor xy
  void addyNeighbor(const int x, const int y);
  size_t _xo, _yo;
  FRIEND_TEST(Bridges, addyNeighbor);

  // neighbors vertical
  void findNeighbourRow(vector<vector<string>> bridges);
  size_t _xyo, _yyo;
  FRIEND_TEST(Bridges, createXY);
};

#endif  // BRIDGES_H_