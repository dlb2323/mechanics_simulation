#include "gui.hpp"
#include "glm/common.hpp"
#include "object.hpp"
#include "environment.hpp"
#include <string>

#define DEBUG
#include "utils.h"

GUI::STATE GUI::state;

// object creation functions
// called by buttons
static object * create_particle(std::string& name) {
 return new particle(name, 5); 
}

static object * create_point(std::string& name) {
 return new point(name, 1); 
}

static object * create_plane(std::string& name) {
 return new plane(name, 80); 
}

static object * create_world(std::string& name) {
 return new world(name, 300); 
}

static object * create_spring(std::string& name) {
 return new spring(name, 5); 
}

// show the main gui tree
// requires environment data
void GUI::show(environment& env) {
  // window options
  static bool no_titlebar = false;
  static bool no_scrollbar = false;
  static bool no_menu = false;
  static bool no_move = true;
  static bool no_resize = true;
  static bool no_collapse = true;
  static bool no_close = false;
  static bool no_nav = false;
  static bool no_background = false;
  static bool no_bring_to_front = false;
  static bool unsaved_document = false;

  ImGuiWindowFlags window_flags = 0;
  if (no_titlebar)
    window_flags |= ImGuiWindowFlags_NoTitleBar;
  if (no_scrollbar)
    window_flags |= ImGuiWindowFlags_NoScrollbar;
  if (!no_menu)
    window_flags |= ImGuiWindowFlags_MenuBar;
  if (no_move)
    window_flags |= ImGuiWindowFlags_NoMove;
  if (no_resize)
    window_flags |= ImGuiWindowFlags_NoResize;
  if (no_collapse)
    window_flags |= ImGuiWindowFlags_NoCollapse;
  if (no_nav)
    window_flags |= ImGuiWindowFlags_NoNav;
  if (no_background)
    window_flags |= ImGuiWindowFlags_NoBackground;
  if (no_bring_to_front)
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
  if (unsaved_document)
    window_flags |= ImGuiWindowFlags_UnsavedDocument;

  // find window width and height
  int width, height;
  glfwGetFramebufferSize(env.window, &width, &height);
  GLFWmonitor* monitor = glfwGetWindowMonitor(env.window);
  if (monitor) {
    // find screen width and height if fullscreen
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    height = mode->height;
  }
  // set window position
  int window_width = 500;
  const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(window_width, height), ImGuiCond_Always);

  // Main body of the window starts here.
  if (!ImGui::Begin("Mechanics Simulation", NULL, window_flags)) {
    // Early out if the window is collapsed, as an optimization.
    ImGui::End();
    return;
  }
  
  // simulation start button
  // button text reflects gui state
  // activates only if there is a legal simulation
  if (ImGui::Button((GUI::state == GUI::EDIT ? "Start" : "Stop"), ImVec2(100, 30))) {
      DEBUG_TEXT("start simulation button clicked")
      // update gui state
      if (env.is_simulation_legal() && GUI::state == GUI::EDIT) {
         GUI::state = GUI::SIMULATE;
        // start simulation if in simulation state
        // deselect any selections
        env.simulation_start();
        env.deselect();
      } else if (GUI::state == GUI::SIMULATE) {
        env.simulation_end();
      GUI::state = GUI::EDIT;
    }
  }
  if (GUI::state == GUI::SIMULATE) {
    ImGui::SameLine(120.0f);
    ImGui::Text("Simulating, GUI locked");
  }

  auto io = ImGui::GetIO();
  static struct { float x; float y; } last_xy;
  // scroll to zoom
  if (io.MousePos.x > window_width) {
    env.current_camera.zoom += io.MouseWheel*0.7f;
    if (env.current_camera.zoom < 0)
      env.current_camera.zoom = 0;
    if (env.current_camera.zoom > 200.0f)
      env.current_camera.zoom = 200.0f;
  }

  // object spawn tabs
  ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
  if (ImGui::BeginTabBar("Objects", tab_bar_flags))
  {
      // lambda function to create a tab item
      auto tab_item = [](environment& env, std::string name,object* (*create_object)(std::string&)) {
        if (ImGui::BeginTabItem(name.c_str()))
        {
            ImGui::Text((std::string("press enter to create ") + name).c_str());
            {
              bool spawn = false;
              static char buf1[64] = "world";
              if (ImGui::InputText(" ", buf1, 64, ImGuiInputTextFlags_EnterReturnsTrue)) {
                spawn = true; 
              }
              // static char input[128] = "A";
              // ImGui::InputText(" ", input, IM_ARRAYSIZE(input));
              std::string s_input(buf1);
              // find nodes with duplicate names
              bool match = false;
              tree_node<object*>* node = env.objects;
              if (env.get_selection())
                // iterate selection if it exists
                node = env.get_selection(); 
              auto traverse = node->get_traversal_state(traversal_state<object*>::PREORDER);
              // iterate nodes
              while(traverse.next()) {
                if (traverse.get_item()->get_name() == s_input) {
                  // break if match is found 
                  match = true;
                  break;
                }
              }
              if (match) {
                ImGui::SameLine(350.0f);
                ImGui::Text("name conflict");
              }
              static int count = 1;
              ImGui::InputInt("count", &count);
              // create 'count' objects and add them to gui tree at 'node'
              // used to spawn multiple objects in one command
              // does not activate while simulating
              if (spawn && GUI::state != GUI::SIMULATE) {
                for (int i = 0; i < count; i++) {
                  std::string s_alt = s_input;
                  if (i != 0)
                    s_alt += std::to_string(i);
                  if (!match) {
                     env.create(create_object(s_alt));
                  }
                }
              }
              ImGui::SameLine(364.0f);
              // remove selected node and its children from gui tree
              // does not activate while simulating
              if (ImGui::Button("remove object") && GUI::state != GUI::SIMULATE) {
                auto node = env.get_selection();
                if (node && node->get_data()->get_name() != "root") {
                  // cannot delete root node world 
                  DEBUG_TEXT("removing node") 
                  env.deselect(true);
                  env.remove(node);
                }
              }
            }
        ImGui::EndTabItem();
        }
    };

    // dynamically generate tab items for each object class
    // links creation functions
    tab_item(env, "world", create_world);
    tab_item(env, "particle", create_particle);
    tab_item(env, "plane", create_plane);
    tab_item(env, "spring", create_spring);
    ImGui::EndTabBar();
  }
  ImGui::Separator();

  // objects 
  ImGui::Spacing();
  if (GUI::state == GUI::EDIT){
      // in gui state edit, create a child menu to display the object tree
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
      ImGui::BeginChild("Objects", ImVec2(ImGui::GetContentRegionAvail().x, 500), ImGuiChildFlags_None, window_flags);
      // recursively show object tree entries
      GUI::show_object_tree(env.objects, env);
      ImGui::EndChild();
  } else {
    // in gui state simulate, calculate and show simulation data
    // float a = (env.subjects.w->force-env.subjects.w->mass*env.subjects.w->gravity*sin(env.subjects.pl->rotation))/env.subjects.w->mass;
    // float r = (
    //   ((env.subjects.w->force - env.subjects.w->mass*env.subjects.w->gravity
    //   *sin(env.subjects.pl->rotation)))
    //   /2*env.subjects.w->mass)
    //   *env.subjects.time.get_elapsed_time()*env.subjects.time.get_elapsed_time() 
    //   + env.subjects.w->u_velocity*env.subjects.time.get_elapsed_time();
    // ImGui::Text((std::string("time elapsed: ") + std::to_string(env.subjects.time.get_elapsed_time())).c_str());
    // ImGui::Text((std::string("acceleration: ") + std::to_string(a)).c_str());
    // ImGui::Text((std::string("displacement: ") + std::to_string(r)).c_str());
  }

  ImGui::Spacing();
  ImGui::Separator();
  // subheading at the bottom shows selection options
  if (env.get_selection()) {
    env.get_selection()->get_data()->show();
  }

  ImGui::End();
}

// recursive function that implements the gui tree 
// post order traversal
void GUI::show_object_tree(tree_node<object*>* object, environment& env) {
  // tree node options
  ImGuiTreeNodeFlags base_flags =
           ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
           ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns;
  ImGuiTreeNodeFlags node_flags = base_flags;
  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  // highlight selected node
  if (object == env.get_selection())
      node_flags |= ImGuiTreeNodeFlags_Selected;

  // create tree node for object
  if (ImGui::TreeNodeEx((void*)(intptr_t)object, node_flags, object->get_data()->get_name().c_str()))
  {
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
      // selects object when clicked and not toggling the arrow
      env.select(object);
      environment::current_camera.focus(object->get_data()->position);
    }
    // recurse for each child
    for (int i = 0; i < object->get_child_count(); i++)
      GUI::show_object_tree(object->get_child(i), env);
    ImGui::TreePop();
  }
}

// GUIitem
void GUIitem::show() const {
}
