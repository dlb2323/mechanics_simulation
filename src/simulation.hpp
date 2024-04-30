#ifndef SIMULATION_H
#define SIMULATION_H

#include "object.hpp"

  // define struct to hold simulation data
  // declare simulation struct
class simulation {
  world* w;
  timestamp time;
public:
  simulation(world* w) : w(w) {}
  virtual void update() = 0;
  virtual void start() = 0;
};

class no_simulation : simulation {
public:
  void update() override {};
  void start() override {};
};

class particle_and_plane : simulation {
};



#endif // !SIMULATION_H
