// Copyright 2018
// Author: Ilia Fedotov, molotok93@gmx.de

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <iostream>
#include "./Visualization.h"

using std::string;

// ____________________________________________________________________________
TEST(Visualization, Constructor) {
  Bridges b;
  Bridges * t = &b;
  Visualization a(t);
  ASSERT_EQ(3, a._xSize);
  ASSERT_EQ(4, a._ySize);
  ASSERT_EQ(0, a._lastClickedX);
  ASSERT_EQ(0, a._lastClickedY);
  ASSERT_EQ(0, a._gotX);
  ASSERT_EQ(0, a._gotY);
  ASSERT_EQ(5, a._draws);
  ASSERT_EQ(0, a._count);
  ASSERT_EQ(0, a._centerX);
  ASSERT_EQ(0, a._centerY);
  ASSERT_EQ(0, a._highlight);
  ASSERT_EQ("", a._bNum);
  ASSERT_EQ("", a._xl);
  ASSERT_EQ("", a._xy);
  ASSERT_EQ("", a._xr);
  ASSERT_EQ("", a._yu);
  ASSERT_EQ("", a._yl);
  ASSERT_EQ("", a._oldxy);
}

// ____________________________________________________________________________
TEST(Visualization, helperPrintBridge) {
  Bridges b;
  Bridges * t = &b;
  Visualization a(t);
  string s = "xy:00:0 bc:0 bp:0 ";
  a.helperPrintBridge(s);
  ASSERT_EQ("0", a._bNum);
  string d = "xy:0:00 bc:1 bp:0 ";
  a.helperPrintBridge(d);
  ASSERT_EQ("1", a._bNum);
  string f = "xy:00:00 bc:2 bp:0 ";
  a.helperPrintBridge(f);
  ASSERT_EQ("2", a._bNum);
}

// ____________________________________________________________________________
TEST(Visualization, saveNeighbors) {
  Bridges b;
  Bridges * t = &b;
  Visualization a(t);
  string s = "xy:00:0 bc:0 bp:0 xl:0:0 xr:12:1 yu:1:12 yl:23:23";
  a.saveNeighbors(s);
  ASSERT_EQ("00:0", a._xy);
  ASSERT_EQ("0:0", a._xl);
  ASSERT_EQ("12:1", a._xr);
  ASSERT_EQ("1:12", a._yu);
  ASSERT_EQ("23:23", a._yl);
  string d = "xy:0:00 bc:1 bp:0 xl:23:2 xr:2:32 yu:21:21 yl:1:1";
  a.saveNeighbors(d);
  ASSERT_EQ("0:00", a._xy);
  ASSERT_EQ("23:2", a._xl);
  ASSERT_EQ("2:32", a._xr);
  ASSERT_EQ("21:21", a._yu);
  ASSERT_EQ("1:1", a._yl);
  string f = "xy:00:00 bc:2 bp:0 xl:2:32 xr:24:52 yu:2:1 yl:23:2";
  a.saveNeighbors(f);
  ASSERT_EQ("00:00", a._xy);
  ASSERT_EQ("2:32", a._xl);
  ASSERT_EQ("24:52", a._xr);
  ASSERT_EQ("2:1", a._yu);
  ASSERT_EQ("23:2", a._yl);
  string g = "xy:00:0 bc:0 bp:0 xl:21:51 xr:2:6 yu:21:4 yl:2:43";
  a.saveNeighbors(g);
  ASSERT_EQ("00:0", a._xy);
  ASSERT_EQ("21:51", a._xl);
  ASSERT_EQ("2:6", a._xr);
  ASSERT_EQ("21:4", a._yu);
  ASSERT_EQ("2:43", a._yl);
}

// ____________________________________________________________________________
TEST(Visualization, saveInts) {
  Bridges b;
  Bridges * t = &b;
  Visualization a(t);
  string s = "0:0";
  a.saveInts(s);
  ASSERT_EQ(0, a._gotX);
  ASSERT_EQ(0, a._gotY);
  string d = "1:23";
  a.saveInts(d);
  ASSERT_EQ(1, a._gotX);
  ASSERT_EQ(23, a._gotY);
  string f = "12:3";
  a.saveInts(f);
  ASSERT_EQ(12, a._gotX);
  ASSERT_EQ(3, a._gotY);
  string g = "12:34";
  a.saveInts(g);
  ASSERT_EQ(12, a._gotX);
  ASSERT_EQ(34, a._gotY);
  string h = "01:10";
  a.saveInts(h);
  ASSERT_EQ(1, a._gotX);
  ASSERT_EQ(10, a._gotY);
}

// ____________________________________________________________________________
TEST(Visualization, setConnect_connectBridge) {
  Bridges b;
  Bridges* t = &b;
  Visualization s(t);
  s.Initialize;
  // b.play();
  vector<string> help;
  for (size_t i = 0; i < 25; i++) {
    for (size_t j = 0; j < 25; j++) help.push_back("");
    t->_map.push_back(help);
    help.clear();
  }
  s._centerX = 2;
  s._centerY = 7;
  t->_map[2][3] = "xy:2:3 bp:5 bc:0 x";
  t->_map[2][7] = "xy:2:7 bp:5 bc:0 x";
  t->_map[0][3] = "xy:0:3 bp:5 bc:0 x";
  t->_map[0][0] = "xy:0:0 bp:5 bc:0 x";
  s.connectBridge(2, 3, true);
  ASSERT_EQ("-", t->_map[2][4]);
  ASSERT_EQ("-", t->_map[2][5]);
  ASSERT_EQ("-", t->_map[2][6]);
  s.setConnect(0, 2, 0, 1);
  ASSERT_EQ("-", t->_map[0][1]);
  ASSERT_EQ("-", t->_map[0][2]);
  s.connectBridge(2, 3, true);
  ASSERT_EQ("=", t->_map[2][4]);
  ASSERT_EQ("=", t->_map[2][5]);
  ASSERT_EQ("=", t->_map[2][6]);
  s.setConnect(0, 2, 0, 1);
  ASSERT_EQ("=", t->_map[0][1]);
  ASSERT_EQ("=", t->_map[0][2]);
  s.connectBridge(2, 3, true);
  ASSERT_EQ(" ", t->_map[2][4]);
  ASSERT_EQ(" ", t->_map[2][5]);
  ASSERT_EQ(" ", t->_map[2][6]);
  s.setConnect(0, 2, 0, 1);
  ASSERT_EQ(" ", t->_map[0][1]);
  ASSERT_EQ(" ", t->_map[0][2]);
  t->_map[6][7] = "xy:6:7 bp:5 bc:0 x";
  s.connectBridge(6, 7, true);
  ASSERT_EQ("|", t->_map[3][7]);
  ASSERT_EQ("|", t->_map[4][7]);
  ASSERT_EQ("|", t->_map[5][7]);
  s.connectBridge(6, 7, true);
  ASSERT_EQ("H", t->_map[3][7]);
  ASSERT_EQ("H", t->_map[4][7]);
  ASSERT_EQ("H", t->_map[5][7]);
  s.connectBridge(6, 7, true);
  ASSERT_EQ(" ", t->_map[3][7]);
  ASSERT_EQ(" ", t->_map[4][7]);
  ASSERT_EQ(" ", t->_map[5][7]);
  endwin();
}

// ____________________________________________________________________________
TEST(Visualization, updateConnectCount) {
  Bridges b;
  Bridges* t = &b;
  Visualization g(t);
  g.Initialize;
  vector<string> help;
  for (size_t i = 0; i < 25; i++) {
    for (size_t j = 0; j < 25; j++) help.push_back("");
    t->_map.push_back(help);
    help.clear();
  }

  // only 2 cases possible:
  //   1 connection added(called by connectBridge)
  //   2 connections decreased
  //
  //   isles get painted depending on the bridge amount
  // test adding single bridge
  t->_map[2][2] = "xy:2:2 bc:5 bp:0 x";
  string s = t->_map[2][2];
  size_t pos1 = s.find("bp:");
  size_t pos2 = s.find(" ", pos1);
  string d = s.substr(pos1+3, pos2-pos1-3);
  int old = std::stoi(d);
  g.updateConnectCount(2, 2, false);
  s = t->_map[2][2];
  pos1 = s.find("bp:");
  pos2 = s.find(" ", pos1);
  d = s.substr(pos1+3, pos2-pos1-3);
  int bnew = std::stoi(d);
  ASSERT_EQ(old+1, bnew);

  // test = -> " "/ H -> " "
  s = t->_map[2][2];
  pos1 = s.find("bp:");
  pos2 = s.find(" ", pos1);
  d = s.substr(pos1+3, pos2-pos1-3);
  old = std::stoi(d);
  g.updateConnectCount(2, 2, true);
  s = t->_map[2][2];
  pos1 = s.find("bp:");
  pos2 = s.find(" ", pos1);
  d = s.substr(pos1+3, pos2-pos1-3);
  bnew = std::stoi(d);
  ASSERT_EQ(old-2, bnew);
}