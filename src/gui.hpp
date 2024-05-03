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
#include "utils.h"

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
protected:
  GUIitem* callback_node;
  void (*m_value_modified)(GUIitem*);
  static void default_modified_callback(GUIitem*) {};
// node name
const std::string m_name;
public:
  GUIitem(std::string& name) : m_name(name), callback_node(NULL) { m_value_modified = GUIitem::default_modified_callback; 
    DEBUG_TEXT((std::string("created node ")+name).c_str());
    (this->m_value_modified)(this);
  }
  GUIitem(const char * name) : m_name(name), callback_node(NULL) { m_value_modified = &GUIitem::default_modified_callback; }
  // individual node gui code
  virtual void show() const;
  void set_modified_callback(void (func)(GUIitem*)) { m_value_modified = func; }
  void set_callback_node(GUIitem* callback_node) { GUIitem::callback_node = callback_node; }
  // name getter
  std::string get_name() const { return m_name; }
};


#endif // !GUI_H
