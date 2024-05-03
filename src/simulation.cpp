#include "simulation.hpp"
#include "object.hpp"
#include "environment.hpp"
#include "utils.h"

pp::pp(world* world, particle* particle, plane* plane) : simulation(world), m_particle(particle), m_plane(plane) {
  reset();
}

void pp::reset() {
  m_plane->move_to(m_world->position);
  // calculate start position
  glm::mat4 t(1.0f);
  t = glm::rotate(t, (m_plane->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
  glm::vec3 offset = glm::translate(t, glm::vec3(0.0f, m_particle->get_radius(), 0.0f))[3];
  t = glm::translate(t, glm::vec3(-1.0f, 0.0f, 0.0f));
  glm::vec3 start = m_world->distance*t[3];
  glm::vec3 position = m_world->position+start+offset; 
  m_particle->move_to(position);
}

void pp::end() {
  reset();
}

void pp::update() {
  // calculate start position
  glm::mat4 t(1.0f);
  t = glm::rotate(t, (m_plane->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
  glm::vec3 offset = glm::translate(t, glm::vec3(0.0f, m_particle->get_radius(), 0.0f))[3];
  t = glm::translate(t, glm::vec3(-1.0f, 0.0f, 0.0f));
  glm::vec3 start = t[3];
  // calculate displacement parallel to the plane
  float r = (
    ((m_particle->force - m_particle->mass*m_world->gravity
    *sin(m_plane->rotation)))
    /2*m_particle->mass)
    *get_time()*get_time() 
    + m_particle->u_velocity*get_time();
  m_particle->position = offset+m_world->position+start*m_world->distance+glm::normalize(start)*r*m_particle->get_radius();
}

void pp::start() {
    m_time_scale = m_world->time_scale;
    DEBUG_TEXT("now simulating particle and plane")
    // track particle
    environment::current_camera.track(&m_particle->position);
    // set timestamp 
    m_time.begin();
    // snap plane to starting position in case it was not already there
    m_plane->position = m_world->position;
}

spp::spp(world* world, particle* particle, plane* plane, spring* spring) : 
  simulation(world), 
  m_particle(particle), 
  m_plane(plane), 
  m_spring(spring) {
  reset();
}

void spp::reset() {
  m_plane->move_to(m_world->position);
  // calculate start position
  glm::mat4 t(1.0f);
  t = glm::rotate(t, (m_plane->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
  glm::vec3 offset = glm::translate(t, glm::vec3(0.0f, m_particle->get_radius(), 0.0f))[3];
  t = glm::translate(t, glm::vec3(-1.0f, 0.0f, 0.0f));
  glm::vec3 start = m_world->distance*t[3];
  glm::vec3 position = m_world->position+start+offset; 
  m_particle->move_to(glm::vec3((m_world->distance+(m_spring->length-m_spring->extension)*spring::coil_width*spring::coils*m_spring->get_scale())*t[3]) + m_world->position+offset);
  m_spring->rotation = m_plane->rotation;
  m_spring->move_to(position);
}
void spp::end() {
  m_spring->extension = extension;
  reset();
}

void spp::update() {
  // calculate start position
  glm::mat4 t(1.0f);
  t = glm::rotate(t, (m_plane->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
  glm::vec3 offset = glm::translate(t, glm::vec3(0.0f, m_particle->get_radius(), 0.0f))[3];
  t = glm::translate(t, glm::vec3(-1.0f, 0.0f, 0.0f));
  glm::vec3 start = t[3];
  float scalar = spring::coil_width*spring::coils*m_spring->get_scale();
  glm::vec3 position = m_world->position+start+offset; 
  glm::vec3 particle_start = (m_world->distance+(m_spring->length-m_spring->extension))*t[3];
  // calculate displacement parallel to the plane
  float n = (m_spring->length/m_spring->elasticity)*(m_spring->elasticity+m_particle->force-m_particle->mass*m_world->gravity*sin(m_plane->rotation));
  float p = m_spring->length-extension-n;
  float z = sqrt(m_spring->elasticity/(m_particle->mass*m_spring->length));
  float u = m_particle->u_velocity;

  float r = p*cos(z*get_time())+u*sin(z*get_time())+n; 
  float end_time = (asin((p+extension)/sqrt(p*p+u*u)) - atan(p/u))/z;

  m_spring->extension = m_spring->length-r;
  if (get_time() > end_time) {
    float v = -z*p*sin(z*end_time)+z*u*cos(z*end_time); 
    float a = (m_particle->force-m_particle->mass*m_world->gravity*sin(m_plane->rotation))/m_particle->mass;
    r = (a/2.0f)*get_time()*get_time() + (v-end_time*a)*get_time() + m_spring->length - end_time*end_time*a/2.0f - end_time*(v-end_time*a);
  }
  m_particle->position = position+(glm::normalize(start)*r)*scalar;
}

void spp::start() {
    m_time_scale = m_world->time_scale;
    extension = m_spring->extension;
    DEBUG_TEXT("now simulating spring, particle and plane")
    // track particle
    environment::current_camera.track(&m_particle->position);
    // set timestamp 
    m_time.begin();
    glm::mat4 t(1.0f);
    t = glm::rotate(t, (m_plane->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 offset = glm::translate(t, glm::vec3(0.0f, m_particle->get_radius(), 0.0f))[3];
    t = glm::translate(t, glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 start = m_world->distance*t[3];
    glm::vec3 position = m_world->position+start+offset; 
    m_particle->position = glm::vec3((m_world->distance+(m_spring->length-m_spring->extension)*spring::coil_width*spring::coils*m_spring->get_scale())*t[3]) + m_world->position+offset;
    m_spring->position = position;
    // snap plane to starting position in case it was not already there
    m_plane->position = m_world->position;
}

ppp::ppp(world* world, particle* particle1, particle* particle2, plane* plane) :
    simulation(world),
    m_particle1(particle1),
    m_particle2(particle2),
    m_plane(plane) {
    reset();
}

void ppp::reset() {
    m_plane->move_to(m_world->position);
    m_plane->rotation = 0.0f;
    // calculate start position
    glm::mat4 t(1.0f);
    t = glm::rotate(t, (m_plane->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 offset = glm::translate(t, glm::vec3(0.0f, m_particle1->get_radius(), 0.0f))[3];
    t = glm::translate(t, glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 start = abs(m_world->distance) * t[3];
    m_particle1->move_to(m_world->position - (start / 2.0f)*m_particle1->get_radius() + offset);
    m_particle2->move_to(m_world->position + (start / 2.0f)*m_particle2->get_radius() + offset);
}

void ppp::update() {
    // calculate start position
    glm::mat4 t(1.0f);
    t = glm::rotate(t, (m_plane->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 offset = glm::translate(t, glm::vec3(0.0f, m_particle1->get_radius(), 0.0f))[3];
    t = glm::translate(t, glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 start = t[3];
    float collision_time = ((m_world->distance*m_particle1->get_radius() - m_particle1->get_radius() - m_particle2->get_radius())/ m_particle1->get_radius()) / (m_particle1->u_velocity + m_particle2->u_velocity);
    float v1 = (m_particle1->u_velocity * m_particle1->mass - m_particle2->u_velocity * m_particle2->mass - m_world->restitution * (m_particle2->mass) * (m_particle1->u_velocity + m_particle2->u_velocity)) / (m_particle1->mass + m_particle2->mass);
    float v2 = (m_particle1->u_velocity*m_particle1->mass-m_particle2->u_velocity*m_particle2->mass+m_world->restitution*(m_particle1->mass)*(m_particle1->u_velocity+m_particle2->u_velocity))/(m_particle1->mass+m_particle2->mass);
    float r1 = m_particle1->u_velocity * get_time() - m_world->distance / 2.0f;
    float r2 = -m_particle2->u_velocity * get_time() + m_world->distance / 2.0f;

    if (get_time() > collision_time) {
        r1 = v1 * (get_time()-collision_time) + m_particle1->u_velocity*collision_time - m_world->distance / 2.0f;
        r2 = v2 * (get_time()-collision_time) + -m_particle2->u_velocity *collision_time + m_world->distance / 2.0f;
    }
    environment::current_camera.snap_to(offset + m_world->position + glm::normalize(start) * ((r1 + r2) / 2.0f) * m_particle1->get_radius());
    environment::current_camera.zoom = std::max(abs(r1 - r2) * 0.8f, 8.0f);
    // calculate displacement parallel to the plane
    m_particle1->position = offset + m_world->position + glm::normalize(start) * r1 * m_particle1->get_radius();
    m_particle2->position = offset + m_world->position + glm::normalize(start) * r2 * m_particle2->get_radius();
}

void ppp::start() {
    glm::mat4 t(1.0f);
    t = glm::rotate(t, (m_plane->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    t = glm::translate(t, glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 start = t[3];
    m_time_scale = m_world->time_scale;
    DEBUG_TEXT("now simulating particle and particle")
    // set timestamp 
    m_time.begin();
    // snap plane to starting position in case it was not already there
    m_plane->position = m_world->position;
}

void ppp::end() {
    environment::current_camera.zoom = 8.0f;
    environment::current_camera.focus(m_plane->position);
    reset();
}


