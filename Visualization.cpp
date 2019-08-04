// Copyright 2018
// Author: Ilia Fedotov, molotok93@gmx.de

#include <ncurses.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include "./Bridges.h"
#include "./Visualization.h"

// ____________________________________________________________________________
Visualization::Visualization(Bridges *bi) {
  // Scaling
  _xSize = 3;
  _ySize = 4;

  // Last click
  _lastClickedX = 0;
  _lastClickedY = 0;

  // Bridge size
  _oldxy = "";
  _xy = "";
  _xl = "";
  _xr = "";
  _yu = "";
  _yl = "";
  _bNum = "";
  _gotX = 0;
  _gotY = 0;
  _centerX = 0;
  _centerY = 0;
  _count = 0;
  _draws = 5;

  _highlight = false;
  _drawMinX = 3;
  _drawMinY = 10;
  // Bridges pointer
  _b = bi;
}

// ____________________________________________________________________________
void Visualization::Initialize() {
  // Initialize screen
  initscr();
  // clear();
  noecho();
  cbreak();
  curs_set(false);
  nodelay(stdscr, true);
  keypad(stdscr, true);
  // activate mouse buttons
  mousemask(ALL_MOUSE_EVENTS, NULL);
}

// ____________________________________________________________________________
void Visualization::printBridge(const vector<vector<string>> bridges,
    const size_t size_x, const size_t size_y) {
  start_color();
  // set colors for the initial print
  init_pair(2, COLOR_WHITE, COLOR_BLACK);
  // draw bridges
  for (size_t x = 0; x < size_x; x++) {
    for (size_t y = 0; y < size_y; y++) {
      if (bridges[x][y].length() > 2) {
        attron(COLOR_PAIR(2));
        attron(A_REVERSE);
      } else {
        // we dont want to print all fields, just the isles itself
        continue;
      }
      // find out the max ammount of bridges to the isle
      helperPrintBridge(bridges[x][y]);
      // Scale each visible bridge.
      for (size_t dy = 0; dy < _ySize; dy++) {
        for (size_t dx = 0; dx < _xSize; dx++) {
          mvprintw(_drawMinX + x * _xSize + dx,
              _drawMinY + y * _ySize + dy, " ");
        }
      }
      attroff(A_REVERSE);
      attroff(COLOR_PAIR(2));
      mvprintw(_drawMinX + x * _xSize + 1, _drawMinY + y * _ySize + 1, "%s",
          _bNum.c_str());
    }
  }
}

// ____________________________________________________________________________
string Visualization::helperPrintBridge(const string bridge) {
  // cut only the max number of bridges marked as "bc:"
  size_t pos1 = bridge.find("c");
  size_t pos2 = bridge.find(" ", pos1);
  _bNum = bridge.substr(pos1+2, pos2-pos1-2);
  return _bNum;
}

// ____________________________________________________________________________
int Visualization::processClick(int key) {
  MEVENT event;
  // different cases of input
  switch (key) {
    // first the mouse input
    case KEY_MOUSE:
      if (getmouse(&event) == OK) {
        if (event.bstate & BUTTON1_CLICKED) {
          // Store position of click and transform to coords
          if (static_cast<size_t>(event.y) < _drawMinX ||
              static_cast<size_t>(event.x) < _drawMinY) return 0;
          _lastClickedY = (event.y - _drawMinX) / _xSize;
          _lastClickedX = (event.x - _drawMinY) / _ySize;
          }
        }
      return 1;
    // exit the game by pressing c
    case 'c':
      endwin();
      exit(1);
      return -1;
    // undo the last printed bridge
    case 'u':
      return 2;
  }
  return 0;
}

// ____________________________________________________________________________
int Visualization::playClick() {
  while (true) {
    string s;
    // get the input
    int key = getch();
    int run = processClick(key);
    if (run == 1) {
      // save isle in string
      s = _b->_map[_lastClickedY][_lastClickedX];
      // if string contains isle with neighbors it is larger then 6
      // xy:xx:yy or like xx:yy
      if (_b->_map[_lastClickedY][_lastClickedX].length() > 6) {
        // highlight is initialized as false, so we get the neighbors in
        // separate strings. If its true, we skip this turn and invert it
        if (!_highlight) saveNeighbors(s);
        _highlight = !_highlight;
        helperHighlight(_highlight);

        // If the isle we choosen in the last turn is a neighbour to our
        // current isle we connect it. _oldxy holds the old x:y as string.
        // If s contains _oldxy, we connect the isles
        if (s.find(_oldxy) != std::string::npos) {
          connectBridge(_lastClickedY, _lastClickedX, false);
          // add bridges for the undo option
          std::stringstream sso;
          sso << _lastClickedX << ":" << _lastClickedY << " " << _centerX <<
            ":" << _centerY;
           _undo.push_back(sso.str());
          _draws = 5;
        }
        // save old coords
        std::stringstream sso;
        sso << _lastClickedY << ":" << _lastClickedX;
        _oldxy = sso.str();
      }
    }

    // call undo, decrease _draws. _draws are reset if a new bridge has been
    // made
    if (run == 2) {
      undoConnection();
      _draws--;
    }
  }
}

// ____________________________________________________________________________
void Visualization::saveNeighbors(const string s) {
  // cut center coords
  size_t pos1 = s.find("xy:");
  size_t pos2 = s.find(" ", pos1);
  _xy = s.substr(pos1+3, pos2-pos1-3);
  saveInts(_xy);
  // save coords of the center Isle
  _centerX = _gotX;
  _centerY = _gotY;
  // cut left neighbor
  pos1 = s.find("xl:");
  pos2 = s.find(" ", pos1);
  _xl = s.substr(pos1+3, pos2-pos1-3);
  // cut right neighbor
  pos1 = s.find("xr:");
  pos2 = s.find(" ", pos1);
  _xr = s.substr(pos1+3, pos2-pos1-3);
  // cut upper neighbor
  pos1 = s.find("yu:");
  pos2 = s.find(" ", pos1);
  _yu = s.substr(pos1+3, pos2-pos1-3);
  // cut lower neighbor
  pos1 = s.find("yl");
  pos2 = s.find(" ", pos1);
  _yl = s.substr(pos1+3, pos2-pos1-3);
}

// ____________________________________________________________________________
void Visualization::helperHighlight(const bool t) {
  // see if there is a neighbor to draw and call function to draw
  // change the bool to an int, we later need more colors then just two :)
  int o = t;
  string nbr[4] = {_xl, _xr, _yl, _yu};
  for (size_t i = 0; i <= 3; i++) {
    if (nbr[i].length() < 6) {
      saveInts(nbr[i]);
      neighborsHighlight(_gotX, _gotY, o);
    }
  }
}

// ____________________________________________________________________________
void Visualization::saveInts(const string s) {
  // change type from string to size_t
  size_t pos1 = s.find(":");
  string d = s.substr(0, pos1);
  string f = s.substr(pos1+1);
  _gotX = std::stoi(d);
  _gotY = std::stoi(f);
}

// ____________________________________________________________________________
void Visualization::neighborsHighlight(const size_t x, const size_t y,
    const int colour) {
  // draw each neighbor one by one
  // red are to much connections
  // cyan are the isles, we can connect to each other
  // green are the ready ones
  start_color();
  init_pair(3, COLOR_BLACK, COLOR_GREEN);
  init_pair(1, COLOR_BLACK, COLOR_CYAN);
  init_pair(4, COLOR_BLACK, COLOR_RED);
  switch (colour) {
    case 0:
      attron(A_REVERSE);
      break;
    case 1:
      attron(COLOR_PAIR(1));
      break;
    case 2:
      attron(COLOR_PAIR(4));
      break;
    case 3:
      attron(COLOR_PAIR(3));
      break;
  }

  // iterate through size of isle, and leave out the middle for the ammount of
  // possible connections
  for (size_t dy = 0; dy < _ySize; dy++) {
    for (size_t dx = 0; dx < _xSize; dx++) {
      if (dx == 1 && dy == 1) continue;
      mvprintw(_drawMinX + x * _xSize + dx,
          _drawMinY + y * _ySize + dy, " ");
    }
  }

  // turn of all colors
  attroff(A_REVERSE);
  attroff(COLOR_PAIR(1));
  attroff(COLOR_PAIR(3));
  attroff(COLOR_PAIR(4));
}

// ____________________________________________________________________________
void Visualization::connectBridge(const size_t x, const size_t y,
    const bool g) {
  // called with lastclicked coords it will call the output for the connection
  // from centerY/X to lastclickedX/Y
  size_t dist = 0;

  if (x == _centerX) {
    // check which is larger, so it is easyer to draw it from small to big
    // also the called function needs less adjustment
    // so we determine wich one will be the stable value and wich is gonna
    // change. dist will give the amount of cells we are going to draw in our
    // vector.
    if (y < _centerY) {
      dist = _centerY -(1+y);
      setConnect(y, dist, x, 1);
    }
    if (y > _centerY) {
      dist = y - (1+_centerY);
      setConnect(_centerY, dist, x, 1);
    }
  }
  if (y == _centerY) {
    if (x < _centerX) {
      dist = _centerX -(1+x);
      setConnect(x, dist, y, 2);
    }
    if (x > _centerX) {
      dist = x - (1+_centerX);
      setConnect(_centerX, dist, y, 2);
    }
  }
}

// ____________________________________________________________________________
void Visualization::setConnect(const size_t from, const size_t count,
    const size_t stable, const int choose ) {
  // 2 is for y coord, 1 for x coord
  // if there already is a bridge, it is then drawn as a
  // double bridge and so on
  // map[x][y] = xy:xx:yy bc:x bp:x ...
  // map[stable][y+1] = -
  // map[stable][y+2]
  bool d = false;
  bool k = true;

  if (choose == 2) {
    // take the neighbor cell in the map and save it in variable
    // t now is one of these signs: " ", "", -, =. |, H
    string t = _b->_map[from+1][stable].c_str();
    // if t already is in {-,=}, it should not change to horizontal connection
    string s = t;

    // now check if all the way we dont collide with other bridges
    for (size_t e = 1; e <= count; e++) {
      string b = _b->_map[from+e][stable].c_str();
      // if k is true the connection will not be printed at all
      if ("-" == b || "=" == b) {
      k = false;
      break;
      }
    }

    // iterate the sign wich would be needed to print, if we would print it
    if (t == " " || t == "") {
      t = '|';
    } else if (t == "|") {
      t = 'H';
    } else if (t == "H") {
      t = " ";
      // d is important to revert the bridges count of 2 connected islands
      // explained in connect count, only called if 2 connections turn to 0
      d = true;
    }
    if (k) {
      for (size_t i = 1; i <= count; i++) {
        _b->_map[from + i][stable] = t;
        printConnection(t, from+i, stable, choose);
      }
      updateConnectCount(_lastClickedX, _lastClickedY, d);
      updateConnectCount(_centerY, _centerX, d);
    }
  }
  if (choose == 1) {
    string t = _b->_map[stable][from+1].c_str();
    string s = t;
    for (size_t e = 1; e <= count; e++) {
      string b = _b->_map[stable][from+e].c_str();
      if ("|" == b || "H" == b) {
        k = false;
        break;
      }
    }
    if (t == " " || t == "") {
      t = '-';
    } else if (t == "-") {
      t = '=';
    } else if (t == "=") {
      t = " ";
      d = true;
    }
    if (k) {
      for (size_t i = 1; i <= count; i++) {
        _b->_map[stable][from + i] = t;
        printConnection(t, stable, from+i, choose);
      }
      updateConnectCount(_lastClickedX, _lastClickedY, d);
      updateConnectCount(_centerY, _centerX, d);
    }
  }
}

// ____________________________________________________________________________
void Visualization::updateConnectCount(const size_t x, const size_t y,
    const bool reset) {
  string s = _b->_map[y][x];
  std::stringstream sso;

  // max count of bridges, read from file
  size_t pos1 = s.find("bc:");
  size_t pos2 = s.find(" ", pos1);
  string d = s.substr(pos1+3, pos2-pos1-3);
  int maxBridge = std::stoi(d);

  // connections made to this isle overall
  pos1 = s.find("bp:");
  pos2 = s.find(" ", pos1);
  d = s.substr(pos1+3, pos2-pos1-3);
  int currentBridge =  std::stoi(d);

  // if d got passed as true, we delete the connections from the isle string
  // the count of isles is important for painting the isles in different colors
  if (reset) currentBridge -= 3;
  pos1 = s.find("bp:");
  // cut the string in begin - bridges set new bridge ammount - end
  d = s.substr(0, pos1+3);
  sso << d << currentBridge+1;
  pos2 = s.find(" ", pos1);
  d = s.substr(pos2, s.size()-1);
  sso << d;

  // update the map, keep the isles colored up
  _b->_map[y][x] = sso.str();
  // > turns isle red, to many bridges
  if (currentBridge+1 > maxBridge) neighborsHighlight(y, x, 2);
  // == turns green, all bridges set
  if (currentBridge+1 == maxBridge) neighborsHighlight(y, x, 3);
  // < actually the basic case, but is needed for restoring white color after
  // deleting to bridges/reset == true
  if (currentBridge+1 < maxBridge) neighborsHighlight(y, x, 0);
}

// ____________________________________________________________________________
void Visualization::printConnection(const string s, const size_t x,
    const size_t y, const int i) {
  // print in the style brought up in the requirements
  //     | |, --
  //     | |, --
  if (i == 1) {
    for (size_t dy = 0; dy < _ySize; dy++) {
      for (size_t dx = 0; dx < _xSize; dx++) {
        if (dx == 1) continue;
        mvprintw(_drawMinX + x * _xSize + dx,
            _drawMinY + y * _ySize + dy, "%s", s.c_str());
      }
    }
  }
  if (i == 2) {
    for (size_t dy = 0; dy < _ySize; dy++) {
      for (size_t dx = 0; dx < _xSize; dx++) {
        if (dy == 1 || dy == 2) continue;
        mvprintw(_drawMinX + x * _xSize + dx,
            _drawMinY + y * _ySize + dy, "%s", s.c_str());
      }
    }
  }
}

// ____________________________________________________________________________
void Visualization::undoConnection() {
  string s;
  // find last bridge which was clicked
  // format: lastclickedx:lastclickedy oldx:oldy
  if (_undo.size() > 0) {
    s = _undo.back();
    // delete element
    _undo.pop_back();
    // get coords
    size_t pos1 = s.find(" ");
    string d = s.substr(0, pos1);
    // convert to size_t
    saveInts(d);
    _centerX = _gotX;
    _centerY = _gotY;
    string f = s.substr(pos1+1);
    saveInts(f);
    // call the connection two times reverts the state of last connected bridge
    connectBridge(_gotX, _gotY, true);
    connectBridge(_gotX, _gotY, true);
  }
}

// ____________________________________________________________________________
void Visualization::solverBridge() {
  for (auto& e : _b->_solver) {
    size_t pos1 = e.find(" ");
    _xy = e.substr(0, pos1);
    // cut right neighbor
    _xr = e.substr(pos1+2);
    saveInts(_xy);
    _centerX = _gotX;
    _centerY = _gotY;
    saveInts(_xr);
    connectBridge(_centerX, _centerY, true);
  }
}