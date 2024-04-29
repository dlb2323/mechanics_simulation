#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <vector>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "tree.hpp"

// forward declarations
class object;
class environment;
class shader;
class GUIitem;


// static gui class to contain gui code
class GUI {
public:
  enum STATE { EDIT,SIMULATE };
private:
  // store gui state
  static STATE state;
public:
  GUI() {}
  static void show(environment& env);
  static void show_object_tree(tree_node<object*>* object, environment& env);
  static STATE get_state() { return state; };
};

// base class for a GUI tree item
class GUIitem {
// node name
const std::string m_name;
public:
  GUIitem(std::string& name) : m_name(name) {}
  GUIitem(const char * name) : m_name(name) {}
  // individual node gui code
  virtual void show() const;
  // name getter
  std::string get_name() const { return m_name; }
};


#endif // !GUI_H
