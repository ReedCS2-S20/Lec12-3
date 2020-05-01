#ifndef _TRAIL_HH
#define _TRAIL_HH

#include <mutex>
#include <string>
#include <queue>
#include <unordered_map>

enum Direction {North, East, South, West, NumDirections};

struct Position {
  int row;
  int column;
  Position N() {
    return Position {row-1,column};
  }
  Position E() {
    return Position {row,column+1};
  }
  Position S() {
    return Position {row+1,column};
  }
  Position W() {
    return Position {row,column-1};
  }
  Position offset(Direction d) {
    if (d == North) {
      return N();
    } else if (d == East) {
      return E();
    } else if (d == South) {
      return S();
    } else if (d == West) {
      return W();
    } else {
      return *this;
    }
  }
  Position operator+(Direction d) {
    return this->offset(d);
  }
  std::string to_string(void) {
    return "(" + std::to_string(row) + "," + std::to_string(column) + ")";
  }
};

class Grid;

class Turtle {
private:
  bool trailing;
  Position position;
  Direction direction;
  int prizes;
  Grid* grid;
  std::string name;
  std::queue<std::string> messages;
public:
  Turtle(Grid* g, std::string nm, Position p, Direction d);
  bool forward(void);   
  void left(void);      
  void right(void);     
  void trail(bool on);  
  bool canBuild(void);
  bool build(void);
  bool canClear(void);
  bool clear(void);
  bool canStep(void);
  bool isAt(Position p);
  void send(std::string msg);
  bool recv(std::string& msg);

  std::string to_string(void);
  std::string getName(void);
};


class Grid {
private:
  char* positions;
  char* southEdges;
  char* eastEdges;
  int rows;
  int columns;

  std::mutex mutex;
  std::unordered_map<std::string,Turtle*> turtles;

  void clear(void);

  std::string displayPoint(Position p);
  std::string displaySouthEdge(Position p);
  std::string displayEastEdge(Position p);
  std::string displayRow(int r);
  std::string displayEdgeRow(int r);

  int getIndex(Position p);
  int getEastEdgeIndex(Position p);
  int getSouthEdgeIndex(Position p);

  char get(Position p);
  void set(Position p, char c);
  
public:
  Grid(int rows, int columns);
  Position trim(Position p);

  void setWall(Position p);
  void setEmpty(Position p);
  void setPrize(Position p);
  void getPrize(Position p);
  void setTrail(Position p, char c);
  void setTrail(Position p, Direction d, char c);

  bool hasWall(Position p);
  bool hasPrize(Position p);
  bool hasTrail(Position p, char& c);
  bool hasTrail(Position p, Direction d,char& c);
  bool hasTurtle(Position p, char& c);

  bool onGrid(Position p);

  int getWidth(void);
  int getHeight(void);

  void add(Turtle* t);
  std::string enter(std::string name, Position p, Direction d);
  void leave(std::string name);
  std::string exec(std::string name, std::string command);
  std::string display(void);
};

#endif
