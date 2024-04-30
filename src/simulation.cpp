#include "simulation.hpp"


void particle_and_plane::update() {
  // calculate start position
  glm::mat4 t(1.0f);
  t = glm::rotate(t, (subjects.pl->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
  t = glm::translate(t, glm::vec3(subjects.w->distance, 0.0f, 0.0f));
  glm::vec3 offset = t[3];
  glm::vec3 position = offset+glm::vec3(0.0f, 10.0f, 0.0f); 
  // calculate displacement parallel to the plane
  float r = (
    ((subjects.w->force - subjects.w->mass*subjects.w->gravity
    *sin(subjects.pl->rotation)))
    /2*subjects.w->mass)
    *subjects.time.get_elapsed_time()*subjects.time.get_elapsed_time() 
    + subjects.w->u_velocity*subjects.time.get_elapsed_time();
  // since 'offset' is alreadly parallel to the plane, it normalised and scaled by r
  // to obtain the displacement as a vector.
  // this displacement is scaled by the particle's radius to give a more intuitive
  // 'unit' vector
  if (subjects.w->distance < 0) 
    // if distance is negative, the offset vector will face the wrong direction
    // reverse r to correct this
    r*=-1;
  // return position - glm::normalize(offset)*r*subjects.pa->get_radius();
}

void particle_and_plane::start() {
    // deselect any selections
    deselect();
    // track particle
    current_camera.track(&subjects.pa->position);
    // set timestamp 
    subjects.time.begin();
    // snap plane to starting position in case it was not already there
    subjects.pl->position = glm::vec3(0.0f, 0.0f, 0.0f);
}
