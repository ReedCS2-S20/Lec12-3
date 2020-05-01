#include "trail.hh"
#include <cassert>
#include <queue>
#include <algorithm>

Turtle::Turtle(Grid* g, std::string nm, Position p, Direction d) :
  grid      {g},
  position  {g->trim(p)},
  direction {d},
  trailing  {false},
  name      {nm},
  prizes    {0},
  messages  {}
{
  std::for_each(name.begin(), name.end(),[](char& c) { c = std::tolower(c); });
  std::for_each(name.begin(), name.begin()+1,[](char& c) { c = std::toupper(c); });
  if (grid->hasWall(position)) {
    grid->setPrize(position);
  }
  grid->add(this);
  if (grid->hasPrize(position)) {
    prizes += 4;
    grid->getPrize(position);
  }
}

bool Turtle::recv(std::string& msg) {
  if (messages.empty()) {
    msg = "";
    return false;
  } else {
    msg = messages.front();
    messages.pop();
    return true;
  }
}

void Turtle::send(std::string msg) {
  messages.push(msg);
}

bool Turtle::canStep(void) {
  return !grid->hasWall(position+direction);
}

bool Turtle::canBuild(void) {
  char c;
  return 
    !grid->hasWall(position+direction) &&
    grid->onGrid(position+direction) &&
    !grid->hasPrize(position+direction) &&
    prizes > 1 &&
    !grid->hasTurtle(position+direction,c);
}

bool Turtle::canClear(void) {
  char c;
  return 
    grid->hasWall(position+direction) &&
    grid->onGrid(position+direction) &&
    prizes > 0;
}

bool Turtle::build(void) {
  if (canBuild()) {
    grid->setWall(position+direction);
    prizes -= 2;
    return true;
  } else {
    return false;
  }
}

bool Turtle::clear(void) {
  if (canClear()) {
    grid->setPrize(position+direction);
    prizes -= 1;
    return true;
  } else {
    return false;
  }
}

bool Turtle::forward(void) {
  if (canStep()) {
    if (trailing) {
      grid->setTrail(position,direction,std::tolower(name[0]));
      if (grid->hasPrize(position+direction)) {
	prizes += 4;
	grid->getPrize(position+direction);
      }
      grid->setTrail(position+direction,std::tolower(name[0]));
    } else {
      if (grid->hasPrize(position+direction)) {
	prizes += 4;
	grid->getPrize(position+direction);
      }
    }
    position = position + direction;
    return true;
  } else {
    return false;
  }
}

bool Turtle::isAt(Position p) {
  return p.row == position.row && p.column == position.column;
}

void Turtle::left(void) {
  direction = static_cast<Direction>((direction - 1 + NumDirections)%NumDirections);
}

void Turtle::right(void) {
  direction = static_cast<Direction>((direction + 1)%NumDirections);
}

void Turtle::trail(bool on) {
  trailing = on;
  if (trailing) {
    grid->setTrail(position,std::tolower(name[0]));
  }
}

std::string dir_string[] = {"north", "east", "south", "west"};

std::string Turtle::to_string(void) {
  std::string s = name + " at ";
  s += position.to_string();
  s += " facing ";
  s += dir_string[direction];
  s += " with $";
  s += std::to_string(prizes);
  return s;
}

std::string Turtle::getName(void) {
  return name;
}

Grid::Grid(int width, int height) :
  rows {height}, columns {width},
  positions {new char[(height+1)*(width+1)]},
  eastEdges {new char[(height+1)*width]},
  southEdges {new char[height*(width+1)]}
{ 
  clear();
}

Position Grid::trim(Position p) {
  if (p.row > getHeight()) p.row = getHeight();
  if (p.row < 0) p.row = 0;
  if (p.column > getWidth()) p.column = getWidth();
  if (p.column < 0) p.column = 0;
  return p;
}

void Grid::clear(void) {
  int h = getHeight();
  int w = getWidth();
  for (int i = 0; i <= getIndex(Position {h,w}); i++) {
    positions[i] = '$';
  }
  for (int i = 0; i <= getSouthEdgeIndex(Position {h-1,w}); i++) {
    southEdges[i] = ' ';
  }
  for (int i = 0; i <= getEastEdgeIndex(Position {h,w-1}); i++) {
    eastEdges[i] = ' ';
  }
}
bool Grid::onGrid(Position p) {
  if (p.row < 0) return false;  
  if (p.column < 0) return false;
  if (p.row > rows) return false;
  if (p.column > columns) return false;
  return true;
}

int Grid::getWidth(void) {
  return columns;
}

int Grid::getHeight(void) {
  return rows;
}

int Grid::getIndex(Position p) {
  assert(onGrid(p));
  return p.row * (columns+1) + p.column;
}

int Grid::getSouthEdgeIndex(Position p) {
  assert(onGrid(p) && onGrid(p.S()));
  return p.row * (columns+1) + p.column;
}

int Grid::getEastEdgeIndex(Position p) {
  assert(onGrid(p) && onGrid(p.E()));
  return p.row * columns + p.column;
}

void Grid::set(Position p, char c) {
  positions[getIndex(p)] = c;
}

void Grid::setWall(Position p) {
  set(p,'X');
}

void Grid::setTrail(Position p, char c) {
  set(p,c);
}

void Grid::setPrize(Position p) {
  set(p,'$');
}

void Grid::getPrize(Position p) {
  set(p,' ');
}

char Grid::get(Position p) {
  if (onGrid(p)) {
    return positions[getIndex(p)];
  } else {
    return 'X';
  }
}

bool Grid::hasWall(Position p) {
  return (get(p) == 'X');
}

bool Grid::hasTrail(Position p, char& c) {
  if (get(p) != 'x' &&
      get(p) >= 'a' && get(p) <= 'z') {
    c = get(p);
    return true;
  } else {
    return false;
  }
}

bool Grid::hasTrail(Position p, Direction d, char& c) {
  if (d == East) {
    c = eastEdges[getEastEdgeIndex(p)];
    return (c != ' ');
  }
  if (d == South) {
    c = southEdges[getSouthEdgeIndex(p)];
    return (c != ' ');
  }
  if (d == West) {
    c = eastEdges[getEastEdgeIndex(p.W())];
    return (c != ' ');
  }
  if (d == North) {
    c = southEdges[getSouthEdgeIndex(p.N())];
    return (c != ' ');
  }
  assert(false);
  return false;
}

bool Grid::hasPrize(Position p) {
  return (get(p) == '$');
}

bool Grid::hasTurtle(Position p, char& c) {
  for (std::pair<std::string,Turtle*> nm_t: turtles) {
    if (nm_t.second->isAt(p)) {
      c = nm_t.second->getName()[0];
      return true;
    }
  }
  return false;
}

void Grid::setTrail(Position p, Direction d, char c) {
  if (d == North) {
    southEdges[getSouthEdgeIndex(p.N())] = c;
  } else if (d == East) {
    eastEdges[getEastEdgeIndex(p)] = c;
  } else if (d == South) {
    southEdges[getSouthEdgeIndex(p)] = c;
  } else if (d == West) {
    eastEdges[getEastEdgeIndex(p.W())] = c;
  } else {
    assert(false);
  }
}

std::string Grid::displayPoint(Position p) {
  char c;
  if (hasWall(p)) {
    return "X";
  } else if (hasTurtle(p,c)) { 
    return std::string (1,c);
  } else if (hasTrail(p,c)) { 
    return std::string (1,c);
  } else if (hasPrize(p)) { 
    return "$";
  } else {
    return "+";
  }
}

std::string Grid::displaySouthEdge(Position p) {
  char c;
  if (hasWall(p) && hasWall(p.S())) {
    return "x";
  } else if (hasWall(p) || hasWall(p.S())) {
    return " ";
  } else if (hasTrail(p,South,c)) {
    return std::string(1,c);
  } else {
    return "|";
  }
}

std::string Grid::displayEastEdge(Position p) {
  char c;
  if (hasWall(p) && hasWall(p.E())) {
    return "xxx";
  } else if (hasWall(p) || hasWall(p.E())) {
    return "   ";
  } else if (hasTrail(p,East,c)) {
    return std::string(3,c);
  } else {
    return "---";
  }
}

std::string Grid::displayRow(int r) {
  std::string s = " ";
  for (int c=-1; c <= getWidth(); c++) {
    s += displayPoint(Position {r,c});
    s += displayEastEdge(Position {r,c});
  }
  s += displayPoint(Position {r,getWidth()+1});
  return s + "\n";
}

std::string Grid::displayEdgeRow(int r) {
  std::string s = " ";
  for (int c=-1; c <= getWidth(); c++) {
    s += displaySouthEdge(Position {r,c});
    s += "   ";
  }
  return s + "x\n"; 
}

std::string Grid::display(void) {
  std::string s = "\n";
  for (int r=-1; r <= getHeight(); r++){
    s += displayRow(r);
    s += displayEdgeRow(r);
  }
  s += displayRow(getHeight()+1);
  return s + "\n";
}

void Grid::add(Turtle* t) {
  turtles[t->getName()] = t;
}

std::string Grid::exec(std::string name, std::string command) {
  std::string gt = "";
  mutex.lock();
  if (turtles.find(name) != turtles.end()) {
    Turtle* t = turtles[name];
    if (command == "forward\n") {
      t->forward();
    } else if (command == "left\n") {
      t->left();
    } else if (command == "right\n") {
      t->right();
    } else if (command == "on\n") {
      t->trail(true);
    } else if (command == "off\n") {
      t->trail(false);
    } else if (command == "build\n") {
      if (!t->build()) {
	gt = "Can't build. ";
      }
    } else if (command == "clear\n") {
      if (!t->clear()) {
	gt = "Can't clear. ";
      }
    } else if (command == "who\n") {
      std::vector<std::string> names {};
      for (auto e: turtles) {
	names.push_back(e.first);
      }
      bool comma = false;
      for (std::string nm : names) {
	if (comma) gt += ",";
	gt += nm;
	comma = true;
      }
      gt += "\n";
    } else if (command.substr(0,4) == "text") {
      gt = "Message send failed.\n";
      std::string theRest = command.substr(command.find(" ")+1);
      std::string who  = theRest.substr(0,theRest.find(" "));
      std::string what = theRest.substr(theRest.find(" ")+1);
      if (turtles.find(who) != turtles.end()) {
	Turtle *r = turtles[who];
	r->send(name+": "+what);
	gt = "Message sent.\n";
      } 
    } else {
      gt = "Command misunderstood. ";
    }
    std::string inbox;
    if (t->recv(inbox)) {
      gt += inbox;
    }
    gt += t->to_string() + "\n";
  } else {
    gt = "No turtle named '" + name + "' found on grid.\n";
  }
  std::string gs = display();
  mutex.unlock();
  return gs+gt;
}

std::string Grid::enter(std::string name, Position p, Direction d) {
  mutex.lock();
  Turtle* t = new Turtle{this,name,p,d};
  std::string gs = display();
  std::string gt = t->to_string() + "\n";
  mutex.unlock();
  return gs+gt;
}

void Grid::leave(std::string name) {
  mutex.lock();
  if (turtles.find(name) != turtles.end()) {
    Turtle* t = turtles[name];
    turtles.erase(name);
    delete t;
  }
  mutex.unlock();
}
  
