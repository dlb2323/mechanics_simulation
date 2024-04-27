#ifndef GUI_H
#define GUI_H

#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

class environment;
class shader;

class GUI {
public:
  enum STATE { EDIT,SIMULATE };
  static STATE state;
  GUI() {}
  static void show(environment& env);
  static void help();
  static void particle_options(std::string name);
};

class GUIitem {
const std::string m_name;
protected:
  bool selected = false;
public:
  GUIitem(std::string& name) : m_name(name) {}
  virtual void show() const {};
  std::string get_name() const { return m_name; }
};


#endif // !GUI_H
