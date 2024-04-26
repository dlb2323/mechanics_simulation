#include "glm/ext/matrix_transform.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#define GLFW_INCLUDE_NONE
#define DRAW_WIREFRAME
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include "src/shader.hpp"
#include "src/sphere.hpp"

// gen sphere
// gl stuff
// abstraction
void error_callback(int error, const char *description) {
  std::cerr << "Error: %s\n" << description;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << std::endl;

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << std::endl;

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: notification";
    break;
  }
  std::cout << std::endl;
  std::cout << std::endl;
  std::cin.get();
}

glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
  return x * (1.f - t) + y * t;
}

class timestamp {
  std::chrono::time_point<std::chrono::system_clock> start;

public:
  timestamp() {}
  void begin() { start = std::chrono::system_clock::now(); }
  double get_elapsed_time() {
    return ((std::chrono::duration<double>)(std::chrono::system_clock::now() -
                                            start))
        .count();
  }
};

class camera {
  enum MODE { STILL, FOCUS, TRACK };
  glm::mat4 m_offset;
  glm::vec3 m_position;
  glm::vec3 m_start;
  glm::vec3 m_focus_point;
  glm::vec3 *m_p_target;
  timestamp m_timestamp;
  const double m_total_time = 1.0;
  camera::MODE m_mode;
  double smooth(double x);

public:
  float zoom = 1.0f;
  float rotation = M_PI / 4;
  camera() {
    const float of = 1.0f;
    m_offset = glm::mat4(1.0f);
    m_offset = glm::translate(m_offset, glm::vec3(of, 0.0f, 0.0f));
  }
  void focus(const glm::vec3 &point);
  void track(glm::vec3 *const target);

  void update();

  glm::mat4 get_view_matrix() const {
    glm::mat4 view(1.0f);
    glm::vec3 position = m_position + glm::vec3(0, 5, 10) * zoom;
    view = glm::lookAt(position, m_position, glm::vec3(0.0f, 1.0f, 0.0f));
    return view * m_offset;
  }
};

void camera::focus(const glm::vec3 &point) {
  m_timestamp.begin();
  m_focus_point = point;
  m_start = m_position;
  m_mode = camera::MODE::FOCUS;
}

void camera::track(glm::vec3 *target) {
  m_timestamp.begin();
  m_p_target = target;
  m_start = m_position;
  m_mode = camera::MODE::TRACK;
}

double camera::smooth(double x) { return tanh(sqrt(x) * 6 - M_PI) / 2 + 0.503; }

void camera::update() {
  switch (m_mode) {
  case camera::MODE::FOCUS: {
    if (m_timestamp.get_elapsed_time() > m_total_time) {
      m_position = m_focus_point;
      m_mode = camera::MODE::STILL;
    }
    m_position = lerp(m_start, m_focus_point,
                      smooth(m_timestamp.get_elapsed_time() / m_total_time));
    break;
  }
  case camera::MODE::TRACK: {
    if (!m_p_target) {
      m_mode = camera::MODE::STILL;
    } else {
      if (m_timestamp.get_elapsed_time() > m_total_time) {
        m_position = *m_p_target;
      } else {
        m_position =
            lerp(m_start, *m_p_target,
                 smooth(m_timestamp.get_elapsed_time() / m_total_time));
      }
    }
    break;
  }
  case camera::MODE::STILL:
  default:
    break;
  }
}

// world rendering
class environment {
  glm::mat4 proj;
  shader main_shader;
  mesh sphere_mesh;
  GLFWwindow * const window;
public:
  std::vector<object*> objects;
  static camera current_camera;
  environment(GLFWwindow *window)
      : window(window),
        main_shader("vertex_shader.glsl", "fragment_shader.glsl"),
        sphere_mesh(&main_shader) {
    sphere::gen_vertex_data(120, sphere_mesh);
  }
  ~environment() {
    auto clean = [](object* p_object) { delete p_object; };
    std::for_each(objects.begin(), objects.end(), clean);
  };
  void update(float delta);
  void draw();
  sphere *create(unsigned int radius);
};
camera environment::current_camera;

void environment::update(float delta) {}

sphere *environment::create(unsigned int radius) {
  sphere* n = new sphere(&sphere_mesh, radius);
  objects.push_back(n);
  // messy
  return n;
}

void environment::draw() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    proj = glm::perspective((float)(M_PI / 4), (float)width / height, 0.1f,
                            300.0f);
    // draw
    glm::mat4 view = environment::current_camera.get_view_matrix();
    glm::mat4 vp_matrix = proj * view;
    for (int i = 0; i < objects.size(); i++) {
      objects[i]->draw(vp_matrix);
    }
}

class GUI {
  static int help_count;

public:
  GUI() {}
  static void show(environment& env);
  static void help();
  static void particle_options(std::string name);
};

void GUI::show(environment& env) {
  // Demonstrate the various window flags. Typically you would just use the
  // default!
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
  const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_Once);

  // Main body of the Demo window starts here.
  if (!ImGui::Begin("Mechanics Simulation Window", NULL, window_flags)) {
    // Early out if the window is collapsed, as an optimization.
    ImGui::End();
    return;
  }

  // camera
  ImGui::SliderFloat("slider float (log)", &environment::current_camera.zoom,
                     0.0f, 10.0f, "%.4f");

  // particles
  const unsigned int input_text_charlen = 128;
  const unsigned int particles_array_len = 50;
  static std::string particles[particles_array_len];
  ImGui::Text("list of particles: size %d", 1);
  ImGui::Spacing();
  {
    static int code = 65;
    char input[input_text_charlen] = {(char)code};
    ImGui::InputText("particle name", input, IM_ARRAYSIZE(input));
    if (ImGui::Button("add particle")) {
      std::string input_string(input);
      int location = -1;
      bool match = false;
      for (int i = 0; i < particles_array_len; i++) {
        if (particles[i].length() == 0 && input_string.length() > 0 &&
            location == -1)
          location = i;
        if (particles[i] == input_string) {
          match = true;
          break;
        }
      }
      if (location >= 0 && !match) {
        particles[location] = input;
        sphere *p_sphere = env.create(5);
        p_sphere->position =
            glm::vec3(std::rand() % 100 - 50, std::rand() % 100 - 50,
                      std::rand() % 100 - 50);
        environment::current_camera.track(&p_sphere->position);
        code++;
      }
    }
  }
  ImGui::Spacing();

  // for (int i = 0; i < particles_array_len; i++) {
  //   if (particles[i].length() > 0)
  //     GUI::particle_options(particles[i]);
  // }
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
    for (int i = 0; i < particles_array_len; i++) {
      if (particles[i].length() > 0) {
        // Disable the default "open on single-click behavior" + set Selected
        // flag according to our selection. To alter selection we use
        // IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow
        // doesn't alter selection.
        ImGuiTreeNodeFlags node_flags = base_flags;
        const bool is_selected = (selection_mask & (1 << i)) != 0;
        if (is_selected)
          node_flags |= ImGuiTreeNodeFlags_Selected;
        if (i < particles_array_len) {
          // Items 0..2 are Tree Node
          ImGui::SetNextItemOpen(true, ImGuiCond_Once);
          bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)i, node_flags,
                                             particles[i].c_str());
          if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            node_clicked = i;
          if (test_drag_and_drop && ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
            ImGui::Text("This is a drag and drop source");
            ImGui::EndDragDropSource();
          }
          if (node_open) {
            ImGui::Text("properties");
            ImGui::TreePop();
          }
        }
        // else
        // {
        //     // Items 3..5 are Tree Leaves
        //     // The only reason we use TreeNode at all is to allow selection
        //     of the leaf. Otherwise we can
        //     // use BulletText() or advance the cursor by
        //     GetTreeNodeToLabelSpacing() and call Text(). node_flags |=
        //     ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; //
        //     ImGuiTreeNodeFlags_Bullet ImGui::TreeNodeEx((void*)(intptr_t)i,
        //     node_flags, particles[i].c_str()); if (ImGui::IsItemClicked() &&
        //     !ImGui::IsItemToggledOpen())
        //         node_clicked = i;
        //     if (test_drag_and_drop && ImGui::BeginDragDropSource())
        //     {
        //         ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
        //         ImGui::Text("This is a drag and drop source");
        //         ImGui::EndDragDropSource();
        //     }
        // }
      }
    }
    if (node_clicked != -1) {
      // Update selection state
      // (process outside of tree loop to avoid visual inconsistencies during
      // the clicking frame)
      if (ImGui::GetIO().KeyCtrl)
        selection_mask ^= (1 << node_clicked); // CTRL+click to toggle
      else // if (!(selection_mask & (1 << node_clicked))) // Depending on
           // selection behavior you want, may want to preserve selection when
           // clicking on item that is part of the selection
        selection_mask = (1 << node_clicked); // Click to single-select

      environment::current_camera.track(
          &env.objects[node_clicked]->position);
    }
    if (align_label_with_current_x_position)
      ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
  }

  ImGui::Spacing();

  GUI::help();

  ImGui::End();
}

void GUI::help() {
  if (ImGui::CollapsingHeader("Help")) {
    ImGui::SeparatorText("ABOUT THIS DEMO:");
    ImGui::BulletText(
        "Sections below are demonstrating many aspects of the library.");
    ImGui::BulletText(
        "The \"Examples\" menu above leads to more demo contents.");
    ImGui::BulletText(
        "The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
        "and Metrics/Debugger (general purpose Dear ImGui debugging tool).");

    ImGui::SeparatorText("PROGRAMMER GUIDE:");
    ImGui::BulletText(
        "See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
    ImGui::BulletText("See comments in imgui.cpp.");
    ImGui::BulletText("See example applications in the examples/ folder.");
    ImGui::BulletText("Read the FAQ at https://www.dearimgui.com/faq/");
    ImGui::BulletText(
        "Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
    ImGui::BulletText(
        "Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
  }
}

void GUI::particle_options(std::string name) {
  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  if (ImGui::CollapsingHeader(name.c_str())) {
    ImGui::SeparatorText("TEST");
    ImGui::BulletText(
        "Sections below are demonstrating many aspects of the library.");
    // ImGui::SeparatorText("ABOUT THIS DEMO:");
    // ImGui::BulletText("Sections below are demonstrating many aspects of the
    // library."); ImGui::BulletText("The \"Examples\" menu above leads to more
    // demo contents."); ImGui::BulletText("The \"Tools\" menu above gives
    // access to: About Box, Style Editor,\n"
    //                   "and Metrics/Debugger (general purpose Dear ImGui
    //                   debugging tool).");
    //
    // ImGui::SeparatorText("PROGRAMMER GUIDE:");
    // ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <-
    // you are here!"); ImGui::BulletText("See comments in imgui.cpp.");
    // ImGui::BulletText("See example applications in the examples/ folder.");
    // ImGui::BulletText("Read the FAQ at https://www.dearimgui.com/faq/");
    // ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard
    // controls."); ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad'
    // for gamepad controls.");
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  if (!glfwInit())
    exit(EXIT_FAILURE);

  GLFWwindow *window;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  window = glfwCreateWindow(1200, 800, "mechanics_simulation", NULL, NULL);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwMakeContextCurrent(window);
  glfwSetErrorCallback(error_callback);

  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  // opengl setup
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glEnable(GL_DEPTH_TEST);
#ifdef DRAW_WIREFRAME
  /* set drawing to wireframe mode */
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

  /* debugging */
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR,
                          GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
  }
  // end

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
#ifdef IMGUI_DOCKING_BRANCH
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch
#endif                                              // !IMGUI_DOCKING_BRANCH

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      window, true); // Second param install_callback=true will install GLFW
                     // callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsClassic();
  io.Fonts->AddFontFromFileTTF("res/IBMPlexMono-Light.otf", 20.0f);
  // end
  environment env(window);

  timestamp delta;
  delta.begin();

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)
    GUI::show(env);

    env.update(delta.get_elapsed_time());
    environment::current_camera.update();

    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    env.draw();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
    delta.begin();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
