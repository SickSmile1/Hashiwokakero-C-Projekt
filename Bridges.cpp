// Copyright 2018
// Author: Ilia Fedotov, molotok93@gmx.de


#include "./Bridges.h"
#include <ncurses.h>
#include <getopt.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "./Visualization.h"


using std::string;
using std::vector;
using std::to_string;

// ____________________________________________________________________________
Bridges::Bridges() {
  _playname = "";
  _result = "";
  _size_x = 0;
  _size_y = 0;
  _xo = 60;
  _yo = 60;
  _draw = 0;
}

// ____________________________________________________________________________
Bridges::~Bridges() {
  for (auto& e : _map) for (auto& b : e) b = "";
  _map.shrink_to_fit();
}

void Bridges::commands(const int argc, char** argv) {
  struct option options[] = {
    {"undo-moves", 1, NULL, 'u'},
    { NULL, 0, NULL, 0}
  };
  optind = 1;
  while (true) {
    char c = getopt_long(argc, argv, "u:", options, NULL);
    if (c == -1) { break; }
    switch (c) {
      case 'u':
        _draw = size_t(atoi(optarg));
        break;
    }
  }
  if (optind + 1 != argc) {
    fprintf(stderr,
        "Usage: ./BridgesMain [options] <filename>\n"
        "-u, --undo-moves : set quantity of undo moves\n");
    exit(1);
  }
  _playname = argv[optind];
}
// ____________________________________________________________________________
vector<vector<string>> Bridges::createGame(string fN) {
  std::ifstream file(fN.c_str());
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << fN << std::endl;
    exit(1);
  }
  string x, y, z, endls, line;

  // bitwise get the first line, get the coords x, y = _x, _y
  file >> x >> y >> z;

  // clear vextor of entities
  for (auto& e : _map) for (auto& b : e) b = "";
  _map.shrink_to_fit();

  saveCoords(y);
  _size_x = _x;
  _size_y = _y;

  vector<string> help;
  for (size_t i = 0; i < _size_x; i++) {
    for (size_t j = 0; j < _size_y; j++) help.push_back("");
    _map.push_back(help);
    help.clear();
  }

  int chooseFile;
  size_t i = fN.length()-1;
  if (fN[i] == 'n' && fN[i-1] == 'i') chooseFile = 1;
  else if (fN[i] == 'y' && fN[i-1] == 'x') chooseFile = 2;
  else if (fN[i] == 'n' && fN[i-1] == 'o') chooseFile = 3;
  if (chooseFile == 1) {
    // .plain File
    string line;
    int count = -1;
    while (true) {
      if (count == 0) size_t count = size_t(count);
      std::getline(file, line);
      if (file.eof()) { break; }
      // line by line, position for position
      for (size_t y = 0; y < line.length(); y++) {
        // true if y is 2digit, like 10, 20, 13
        if (isdigit(line[y]) && isdigit(line[y+1])) {
          addBridgePlain(y, count, line, true);
          y++;
        }
        if (isdigit(line[y])) addBridgePlain(y, count, line, false);
      }
      count++;
    }
  }
  // .xy File
  _yo = 60;
  if (chooseFile == 2) while (file >> x) {addBridgeXY(x);}
  if (chooseFile == 3) {
    int i = 0;
    while (file >> x) {
      if (x == "#" || x == "Single" || x == "Double" || x == "bridge" ||
          x == "x1,y1,x2,y2") continue;
      saveSolution(x, i);
    }
  }
  return _map;
}

// ____________________________________________________________________________
void Bridges::saveSolution(string x, int i) {
  size_t pos1 = x.find(",");
  size_t pos2 = x.find(",", pos1+1);
  size_t pos3 = x.find(",", pos2+1);
  string xs = x.substr(0, pos1);
  string ys = x.substr(pos1+1, pos2-pos1-1);
  string cs = x.substr(pos2+1, pos3-pos2-1);
  string vs = x.substr(pos3+1);
  std::stringstream sso;
  sso << xs << ":" << ys << " " << cs << ":" << vs;
  _solver.push_back(sso.str());
}

void Bridges::addBridgeXY(const string x) {
  // find separators position
  size_t pos1 = x.find(",");
  size_t pos2 = x.find(",", pos1+1);
  string xs = x.substr(0, pos1);
  size_t xi = std::atoi(xs.c_str());
  // get y if 2digit
  string s = (x.substr(pos1+1, pos2 - pos1-1));
  if (isdigit(x[pos1+2])) s = (x.substr(pos1+1, 2));
  string bridge = x.substr(pos2 + 1);
  std::stringstream sso;
  sso << "xy:" << xs << ":" << s << " bc:" << bridge << " bp:0";
  _result.append(sso.str());
  // call to add horizontal bridges
  size_t y = std::atoi(s.c_str());
  if (_xo == xi && _yo < 50) addyNeighbor(xi, y);
  _xo = xi;
  _yo = y;
  // add Bridge to map
  _map[xi][y] = _result;
  _result.clear();
}

void Bridges::addBridgePlain(const size_t y, const size_t count,
    const string line, const bool _t) {
  // add x & y
  string s = to_string(y);
  // add if y is 2digit
  if (_t) s.append(to_string(y+1));
  std::stringstream sso;
  sso << "xy:" << count << ":" << s << " bc:" << line[y] << " bp:0";
  _result.append(sso.str());
  // check if bridge has y-axis neighbor
  size_t yi = std::atoi(s.c_str());
  if (_xo == count) addyNeighbor(count, yi);
  // set old coords
  _xo = count;
  _yo = yi;
  _map[count][yi] = _result;
  _result.clear();
}

// ____________________________________________________________________________
void Bridges::addyNeighbor(const int x, const int y) {
  std::stringstream cco;
  cco << " xl:" << _xo << ":" << _yo;
  _result.append(cco.str());
  std::stringstream sso;
  sso << " xr:" << x << ":" << y;
  _map[_xo][_yo] += sso.str();
}

// ____________________________________________________________________________
void Bridges::saveCoords(const string xy) {
  int pos0 = -1;
  if (xy.find("xy:") != std::string::npos) pos0 = xy.find(":");
  size_t pos1 = xy.find(":", pos0+1);
  size_t pos2 = xy.find(" ", pos1+1);
  string x = xy.substr(pos0+1, pos1);
  string y = xy.substr(pos1+1, pos2 -pos1-1);
  _x = std::atoi(x.c_str());
  _y = std::atoi(y.c_str());
}

// ____________________________________________________________________________
void Bridges::findNeighbourRow(vector<vector<string>> bridges) {
  // go through the _map and find all bridges with same x
  // if 0:0 has neighbor at 0:3
  // 0:0 should add "yl 0:3"
  // 0:3 should add "yu 0:0"
  _yyo = 50, _xyo = 50;
  for (size_t i = 0; i < _size_y; i++) {
    for (size_t j = 0; j < _size_x; j++) {
      if (bridges[j][i].length() > 2) {
        saveCoords(bridges[j][i]);
        // std::cout << bridges[j][i] << std::endl;
        if (_yyo == _y) {
          std::stringstream sso, cco;
          // add coords of upper bridge as neighbor to the string
          sso << " yu:" << _xyo << ":" << _yyo;
          string yu = sso.str();
          _map[_x][_y] = _map[_x][_y] + yu;
          cco << " yl:" << _x << ":" << _y;
          string yl = cco.str();
          _map[_xyo][_yyo] = _map[_xyo][_yyo] + yl;
        }
        _yyo = _y, _xyo = _x;
      }
    }