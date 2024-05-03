#include "gui.hpp"
#include "environment.hpp"

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

  // camera
  ImGui::SliderFloat("zoom", &environment::current_camera.zoom,
                     0.0f, 10.0f, "%.4f");

  ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
  if (ImGui::BeginTabBar("Objects", tab_bar_flags))
  {
      if (ImGui::BeginTabItem("Sphere"))
      {
          ImGui::Text("Add Sphere");
          const unsigned int input_text_charlen = 128;
          ImGui::Text("list of particles: size %d", 1);
          {
            static int code = 65;
            char input[input_text_charlen] = {(char)code};
            ImGui::InputText("particle name", input, IM_ARRAYSIZE(input));
            if (ImGui::Button("add particle")) {
              std::string input_string(input);
              bool match = false;
              // auto itr = env.
              //   if (env.object_at(i)->get_name() == input_string) {
              //     match = true;
              //     break;
              if (!match) {
                std::string s_input(input);
                sphere *p_sphere = env.create(s_input, 5);
                p_sphere->position =
                    glm::vec3(std::rand() % 100 - 50, std::rand() % 100 - 50,
                              std::rand() % 100 - 50);
                environment::current_camera.track(&p_sphere->position);
                code++;
              }
            }
          }
          ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Broccoli"))
      {
          ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
          ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Cucumber"))
      {
          ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
          ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
  }
  ImGui::Separator();

  // particles
  ImGui::Spacing();

  {
    static ImGuiTreeNodeFlags base_flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAllColumns;
    static bool align_label_with_current_x_position = false;
    static bool test_drag_and_drop = false;
    if (align_label_with_current_x_position)
      ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

    // 'selection_mask' is dumb representation of what may be user-side
    // selection state.
    //  You may retain selection state inside or outside your objects in
    //  whatever format you see fit.
    // 'node_clicked' is temporary storage of what node we have clicked to
    // process selection at the end
    /// of the loop. May be a pointer to your own node type, etc.
    static int selection_mask = (1 << 2);
    int node_clicked = -1;
  //   for (int i = 0; i < env.object_count(); i++) {
  //     // Disable the default "open on single-click behavior" + set Selected
  //     // flag according to our selection. To alter selection we use
  //     // IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow
  //     // doesn't alter selection.
  //     ImGuiTreeNodeFlags node_flags = base_flags;
  //     const bool is_selected = (selection_mask & (1 << i)) != 0;
  //     if (is_selected)
  //       node_flags |= ImGuiTreeNodeFlags_Selected;
  //     if (i < env.object_count()) {
  //       ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  //       bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)i, node_flags,
  //                                          env.object_at(i)->get_name().c_str());
  //       if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
  //         node_clicked = i;
  //       if (test_drag_and_drop && ImGui::BeginDragDropSource()) {
  //         ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
  //         ImGui::Text("This is a drag and drop source");
  //         ImGui::EndDragDropSource();
  //       }
  //       if (node_open) {
  //         ImGui::Text("properties");
  //         ImGui::TreePop();
  //       }
  //     }
  //   }
  //   if (node_clicked != -1) {
  //     // Update selection state
  //     // (process outside of tree loop to avoid visual inconsistencies during
  //     // the clicking frame)
  //     if (ImGui::GetIO().KeyCtrl)
  //       selection_mask ^= (1 << node_clicked); // CTRL+click to toggle
  //     else // if (!(selection_mask & (1 << node_clicked))) // Depending on
  //          // selection behavior you want, may want to preserve selection when
  //          // clicking on item that is part of the selection
  //       selection_mask = (1 << node_clicked); // Click to single-select
  //
  //     if (env.object_at(node_clicked)) {
  //       environment::current_camera.track(&env.object_at(node_clicked)->position);
  //       env.select(env.object_at(node_clicked));
  //     }
  //     else
  //       std::cout << "ERROR SELECTING NON-EXISTING OBJECT\n";
  //   }
  //   if (align_label_with_current_x_position)
  //     ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
  // }
}

  ImGui::Spacing();

  GUI::help();

  ImGui::End();
}

void GUI::help() {
  if (ImGui::CollapsingHeader("Help")) {
  }
}

void GUI::particle_options(std::string name) {
}
