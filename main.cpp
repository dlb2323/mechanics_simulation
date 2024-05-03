#include "glm/ext/matrix_transform.hpp"
#include <algorithm>
#include <iostream>
#include <chrono>
#define GLFW_INCLUDE_NONE
#define DRAW_WIREFRAME
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "src/shader.hpp"
#include "src/sphere.hpp"
#include "src/window.hpp"

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
  timestamp() {
  }
  void begin() {
    start = std::chrono::system_clock::now();
  }
  double get_elapsed_time() {
    return ((std::chrono::duration<double>)(std::chrono::system_clock::now()-start)).count();
  }
};

class camera {
  enum MODE {
    STILL,
    FOCUS,
    TRACK
  };
  glm::vec3 m_position;
  glm::vec3 m_start;
  glm::vec3 m_focus_point;
  glm::vec3* m_p_target;
  timestamp m_timestamp;
  const double m_total_time = 1.0;
  camera::MODE m_mode;
  double smooth(double x);
public:
  float zoom = 1.0f;
  float rotation = M_PI/4;
  camera() {
  }
  void focus(const glm::vec3 & point);
  void track(glm::vec3 * const target);

  void update();

  glm::mat4 get_view() const {
    glm::mat4 view(1.0f);
    glm::vec3 position = m_position + glm::vec3(0, 5, 10)*zoom; 
    view = glm::lookAt(position, m_position, glm::vec3(0.0f, 1.0f, 0.0f));
    return view;
  }
};

void camera::focus(const glm::vec3 & point) {
  m_timestamp.begin();
  m_focus_point = point;
  m_start = m_position;
  m_mode = camera::MODE::FOCUS;
}

void camera::track(glm::vec3 * const target) {
  m_timestamp.begin();
  m_p_target = target; 
  m_start = m_position;
  m_mode = camera::MODE::TRACK;
}

double camera::smooth(double x) {
  return tanh(sqrt(x)*6-M_PI)/2+0.503;
}

void camera::update() {
  switch (m_mode) {
    case camera::MODE::FOCUS: {
      if (m_timestamp.get_elapsed_time() > m_total_time) {
        m_position = m_focus_point;
        m_mode = camera::MODE::STILL;
      }
      m_position = lerp(m_start, m_focus_point, smooth(m_timestamp.get_elapsed_time()/m_total_time));
      break;
    }
    case camera::MODE::TRACK: {
      if (m_p_target == NULL) {
        m_mode = camera::MODE::STILL;
      } else {
        if (m_timestamp.get_elapsed_time() > m_total_time) {
          m_position = *m_p_target;
        } else {
          m_position = lerp(m_start, *m_p_target, smooth(m_timestamp.get_elapsed_time()/m_total_time));
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
    static glm::mat4 proj; 
public:
    static std::vector<sphere> spheres;
    static camera current_camera;
    static void init(GLFWwindow* window);
    static void deinit();
    static void update();
    static void draw();
    static sphere* create(unsigned int radius);
};
camera environment::current_camera;
std::vector<sphere> environment::spheres;


void environment::init(GLFWwindow* window) {
}
void environment::deinit() {
}

void environment::update() {
}

sphere* environment::create(unsigned int radius) {
  spheres.push_back(sphere(radius));
  return &spheres[spheres.size()-1];
}

void environment::draw() {
}

class GUI {
  static int help_count;
public:
  GUI() {}
  static void show();
  static void help();
  static void particle_options(std::string name); 
};

void GUI::show() {
    // Demonstrate the various window flags. Typically you would just use the default!
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
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_Once);

    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Mechanics Simulation Window", NULL, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // camera
    ImGui::SliderFloat("slider float (log)", &environment::current_camera.zoom, 0.0f, 10.0f, "%.4f");

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
          int  location = -1;
          bool match = false;
          for (int i = 0; i < particles_array_len; i++) {
            if (particles[i].length() == 0 && input_string.length() > 0)
              location = i;
            if (particles[i] == input_string) {
              match = true;
              break;
            }
          }
          if (location >= 0 && !match) {
            particles[location] = input;
            sphere* p_sphere = environment::create(5);
            p_sphere->position = glm::vec3(std::rand()%100-50, std::rand()%100-50, std::rand()%100-50);
            environment::current_camera.focus(p_sphere->position);
            code++;
          }
        }
    }
    ImGui::Spacing();

    auto display = [](std::string& name) { GUI::particle_options(name); };
    for (int i = 0; i < particles_array_len; i++) {
      if (particles[i].length() > 0)
        GUI::particle_options(particles[i]);
  }

    ImGui::Spacing();
    
    GUI::help();

    ImGui::End();
}

void GUI::help() {
    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::SeparatorText("ABOUT THIS DEMO:");
        ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
        ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
        ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
                          "and Metrics/Debugger (general purpose Dear ImGui debugging tool).");

        ImGui::SeparatorText("PROGRAMMER GUIDE:");
        ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
        ImGui::BulletText("See comments in imgui.cpp.");
        ImGui::BulletText("See example applications in the examples/ folder.");
        ImGui::BulletText("Read the FAQ at https://www.dearimgui.com/faq/");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
    }
}

void GUI::particle_options(std::string name) {
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader(name.c_str()))
    {
        ImGui::SeparatorText("TEST");
        ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
        // ImGui::SeparatorText("ABOUT THIS DEMO:");
        // ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
        // ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
        // ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
        //                   "and Metrics/Debugger (general purpose Dear ImGui debugging tool).");
        //
        // ImGui::SeparatorText("PROGRAMMER GUIDE:");
        // ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
        // ImGui::BulletText("See comments in imgui.cpp.");
        // ImGui::BulletText("See example applications in the examples/ folder.");
        // ImGui::BulletText("Read the FAQ at https://www.dearimgui.com/faq/");
        // ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        // ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
    }
}




int main() {
  if (!glfwInit())
    exit(EXIT_FAILURE);

  window mainWindow(std::string("mechanics_simulation"), 1200, 800);
  mainWindow.makeCurrent();
  glfwSetErrorCallback(error_callback);

  if (!mainWindow.exists()) {
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
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_DOCKING_BRANCH
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
#endif // !IMGUI_DOCKING_BRANCH

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(mainWindow.getWindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
  // end
  environment::init(mainWindow.getWindow());
  int nodes = 120;
  shader sphere_shader("vertex_shader.glsl", "fragment_shader.glsl");
  const unsigned int mvp_location = sphere_shader.get_uniform_location("u_mvp");
  const unsigned int time_location = sphere_shader.get_uniform_location("u_time");
  mesh sphere_mesh = mesh(&sphere_shader);
  sphere::gen_vertex_data(nodes, sphere_mesh);
  environment::spheres.push_back(sphere(5));


  while (!mainWindow.shouldClose()) {
    if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(mainWindow.getWindow(), GLFW_TRUE);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow(); // Show demo window! :)
    GUI::show();
    
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    environment::current_camera.update();
    int width, height;
    glfwGetFramebufferSize(mainWindow.getWindow(), &width, &height);
    glm::mat4 proj = glm::perspective((float)(M_PI / 4), (float)width / height,
                                      0.1f, 300.0f);
    // draw
    glm::mat4 view = environment::current_camera.get_view();
    // glm::mat4 view = glm::mat4(1.0);
    sphere_mesh.bind();
    for (int i = 0; i < environment::spheres.size(); i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, environment::spheres[i].position);
        model = glm::rotate(model, (float)glfwGetTime() / 20,
                            glm::vec3(1.0f, 0.0f, 1.0f));
        model = glm::scale(
            model, glm::vec3(environment::spheres[i].m_radius, environment::spheres[i].m_radius, environment::spheres[i].m_radius));
        glm::mat4 mvp = proj * view * model;
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1f(time_location, (float)glfwGetTime());
        sphere_mesh.draw();
  }
  sphere_mesh.unbind();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    mainWindow.swapBuffers();
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
