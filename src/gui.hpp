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
protected:
  bool selected = false;
public:
  static shader* selected_shader;
  GUIitem() {}
  virtual void show() const {};
};


#endif // !GUI_H
