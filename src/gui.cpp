#include "gui.hpp"
#include "object.hpp"
#include "environment.hpp"
#include <string>

GUI::STATE GUI::state;

static object * create_particle(std::string& name) {
 return new particle(name, 3); 
}
static object * create_point(std::string& name) {
 return new point(name, 1); 
}
static object * create_plane(std::string& name) {
 return new plane(name, 30); 
}

void GUI::show(environment& env) {
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
  // We specify a default position/size in case there's no data in the .ini
  // file. We only do it to make the demo applications a little more welcoming,
  // but typically this isn't required.
  int width, height;
  glfwGetFramebufferSize(env.window, &width, &height);
  GLFWmonitor* monitor = glfwGetWindowMonitor(env.window);
  if (monitor) {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    height = mode->height;
  }
  const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(500, height), ImGuiCond_Always);

  // Main body of the Demo window starts here.
  if (!ImGui::Begin("Mechanics Simulation Window", NULL, window_flags)) {
    // Early out if the window is collapsed, as an optimization.
    ImGui::End();
    return;
  }
  
  if (ImGui::Button((GUI::state == GUI::EDIT ? "Start" : "Stop"), ImVec2(100, 30))) {
      GUI::state = GUI::state == GUI::EDIT ? GUI::SIMULATE : GUI::EDIT;
  }
  if (GUI::state == GUI::SIMULATE) {
    ImGui::SameLine(120.0f);
    ImGui::Text("Simulating, GUI locked");
  }

  // camera
  // implement scroll wheel zoom!
  ImGui::SliderFloat("zoom", &environment::current_camera.zoom,
                     0.0f, 20.0f, "%.4f");

  ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
  if (ImGui::BeginTabBar("Objects", tab_bar_flags))
  {
      auto tab_item = [](environment& env, std::string name,object* (*create_object)(std::string&)) {
        if (ImGui::BeginTabItem(name.c_str()))
        {
            ImGui::Text((std::string("create ") + name).c_str());
            {
              static char input[128] = "A";
              ImGui::InputText(" ", input, IM_ARRAYSIZE(input));
              std::string s_input(input);
              bool match = false;
              if (env.get_selection())
              {
                auto traverse = env.get_selection()->get_traversal_state(traversal_state<object*>::PREORDER);
                while(traverse.next()) {
                  if (traverse.get_item()->get_name() == s_input) {
                    match = true;
                    break;
                  }
                }
              } else {
                auto traverse = env.objects->get_traversal_state(traversal_state<object*>::PREORDER);
                while(traverse.next()) {
                  if (traverse.get_item()->get_name() == s_input) {
                    match = true;
                    break;
                  }
                }
              }
              if (match) {
                ImGui::SameLine(350.0f);
                ImGui::Text("name conflict");
              }
              static int count = 1;
              ImGui::InputInt("count", &count);
              if (ImGui::Button((std::string("create ") + name).c_str()) && GUI::state != GUI::SIMULATE) {
                for (int i = 0; i < count; i++) {
                  std::string s_alt = s_input;
                  if (i != 0)
                    s_alt += std::to_string(i);
                  if (true) { //!match) {
                    tree_node<object*>* p_object_node = env.create(create_object(s_alt));
                  }
                }
              }
              ImGui::SameLine(364.0f);
              if (ImGui::Button("remove object") && GUI::state != GUI::SIMULATE) {
                auto node = env.get_selection();
                if (node && node->get_data()->get_name() != "world") {
                  env.deselect(true);
                  tree_node<object*>::destroy(node);
                }
              }
            }
        ImGui::EndTabItem();
        }
    };

    tab_item(env, "particle", create_particle);
    tab_item(env, "point", create_point);
    tab_item(env, "plane", create_plane);
    ImGui::EndTabBar();
  }
  ImGui::Separator();

  // objects 
  ImGui::Spacing();
  if (GUI::state == GUI::EDIT){
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
      ImGui::BeginChild("Objects", ImVec2(ImGui::GetContentRegionAvail().x, 600), ImGuiChildFlags_None, window_flags);
      GUI::show_object_tree(env.objects, env);
      ImGui::EndChild();
  }

  ImGui::Spacing();
  ImGui::Separator();
  if (env.get_selection()) {
    env.get_selection()->get_data()->show();
  }

  // GUI::help();

  ImGui::End();
}

void GUI::show_object_tree(tree_node<object*>* object, environment& env) {
  ImGuiTreeNodeFlags base_flags =
           ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
           ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns;
  ImGuiTreeNodeFlags node_flags = base_flags;
  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  if (object == env.get_selection())
      node_flags |= ImGuiTreeNodeFlags_Selected;

  if (ImGui::TreeNodeEx((void*)(intptr_t)object, node_flags, object->get_data()->get_name().c_str()))
  {
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
      env.select(object);
      environment::current_camera.focus(object->get_data()->position);
    }
    // object->get_data()->show();
    // ImGui::Separator();
    for (int i = 0; i < object->get_child_count(); i++)
      GUI::show_object_tree(object->get_child(i), env);
    ImGui::TreePop();
  }
}

void GUI::help() {
  if (ImGui::CollapsingHeader("Help")) {
  }
}

void GUI::particle_options(std::string name) {
}
// GUIitem
void GUIitem::show() const {
}
