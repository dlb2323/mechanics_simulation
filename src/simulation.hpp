#ifndef SIMULATION_H
#define SIMULATION_H

#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class world;
class particle;
class plane;
class spring;
// holds a point in time for reference
class timestamp {
  std::chrono::time_point<std::chrono::system_clock> start;
public:
  timestamp() {}
  // update time reference point 
  void begin() { start = std::chrono::system_clock::now(); }
  // get time in seconds since last reference point 
  double get_elapsed_time() {
    return ((std::chrono::duration<double>)(std::chrono::system_clock::now() - start)).count();
  }
};

// define struct to hold simulation data
// declare simulation struct
// blank simulation interface to inherit
class simulation {
protected:
  world* m_world;
  timestamp m_time;
  float m_time_scale;
  float get_time() { return m_time.get_elapsed_time()*m_time_scale; };
public:
  simulation(world* world) : m_world(world), m_time_scale(1.0f) {}
  virtual ~simulation() {};
  virtual void reset() = 0;
  virtual void update() = 0;
  virtual void start() = 0;
  virtual void end() = 0;
};

// simulation of a particle and a plane
class pp : public simulation {
  particle* m_particle;
  plane* m_plane;
public:
  pp (world* world, particle* particle, plane* plane);
  void reset() override;
  void update() override;
  void start() override;
  void end() override;
};

class spp : public simulation {
  float extension;
  spring* m_spring;
  particle* m_particle;
  plane* m_plane;
public:
  spp(world* world, particle* particle, plane* plane, spring* spring);
  void reset() override;
  void update() override;
  void start() override;
  void end() override;
};

class ppp : public simulation {
    float extension;
    spring* m_spring;
    particle* m_particle1;
    particle* m_particle2;
    plane* m_plane;
public:
    ppp(world* world, particle* particle1, particle* particle2, plane* plane);
    void reset() override;
    void update() override;
    void start() override;
    void end() override;
};



#endif // !SIMULATION_H
