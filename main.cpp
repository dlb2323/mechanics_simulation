#include <iostream>
#define GLFW_INCLUDE_NONE
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

class environment {
    glm::mat4 proj;  
  environment(int width, int height) : proj(glm::perspective((float)(M_PI / 4), (float)width / height,
                                      0.1f, 100.0f)) {
}
  void draw();
};

void environment::draw() {
}

class GUI {
  static int help_count;
public:
  GUI() {}
  static void show();
  static void help();
};

void GUI::show() {
    help_count = 0;
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

    ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    ImGui::Spacing();
    {
        static int clicked = 0;
        if (ImGui::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            ImGui::SameLine();
            ImGui::Text("Thanks for clicking me!");
        }
    }
    ImGui::Spacing();
    {
        static int clicked = 0;
        if (ImGui::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            ImGui::SameLine();
            ImGui::Text("Thanks for clicking me!");
        }
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

        ImGui::SeparatorText("USER GUIDE:");
        ImGui::ShowUserGuide();
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

  int nodes = 120;
  shader sphere_shader("vertex_shader.glsl", "fragment_shader.glsl");
  const unsigned int mvp_location = sphere_shader.get_uniform_location("u_mvp");
  const unsigned int time_location =
      sphere_shader.get_uniform_location("u_time");
  vertexObject sphere_object(&sphere_shader);
  sphere::gen_vertex_data(nodes, sphere_object);
  sphere test(&sphere_object, 5);

  while (!mainWindow.shouldClose()) {
    if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(mainWindow.getWindow(), GLFW_TRUE);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)
    GUI::show();
    
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width, height;
    glfwGetFramebufferSize(mainWindow.getWindow(), &width, &height);

    // draw
    glm::mat4 proj = glm::perspective((float)(M_PI / 4), (float)width / height,
                                      0.1f, 100.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-20.0f, -20.0f, -100.0f));
    sphere_object.bind();
    for (int i = 0; i < 30; i++) {
      for (int j = 0; j < 30; j++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i*5*2, j*5*2, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() / 20,
                            glm::vec3(1.0f, 0.0f, 1.0f));
        model = glm::scale(
            model, glm::vec3(test.m_radius, test.m_radius, test.m_radius));
        glm::mat4 mvp = proj * view * model;
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1f(time_location, (float)glfwGetTime());
        sphere_object.draw();
      }
    }
    sphere_object.unbind();


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
