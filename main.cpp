#include "glm/ext/matrix_transform.hpp"
#include <algorithm>
#include <chrono>
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

#include "src/environment.hpp"
#include "src/gui.hpp"
#include "src/shader.hpp"
#include "src/sphere.hpp"

// gen sphere
// gl stuff
// abstraction
template <class T> class tree_node;

template <class T> class traversal_state {
  typedef struct {
    tree_node<T> *node_pointer;
    int leaf_pointer;
  } state;
  state m_stack[128];
  int m_stack_pointer;
  T *m_item;

public:
  enum MODE { PREORDER, POSTORDER, INORDER };
  const MODE mode;
  traversal_state(MODE mode, tree_node<T> *root) : mode(mode) {
    m_stack[0] = state{root, 0};
  };
  T *get_item() const { return m_item; };
  bool next();
};

template <class T> class tree_node {
  T *data;
  bool lock = false;
  std::vector<tree_node<T> *> children;

  tree_node<T>(T *data) { tree_node<T>::data = data; }
  ~tree_node<T>() {
    if (data)
      delete data;
    for (int i = 0; i < children.size(); i++)
      destroy(children[i]);
  }

public:
  static tree_node<T> *create_new(T *data);
  static int size(tree_node<T> *node);
  static void destroy(tree_node<T> *node);
  traversal_state<T>
  get_traversal_state(typename traversal_state<T>::MODE mode);
  void insert_node(tree_node<T> *node, int idx = -1);

  T *get_data() const { return data; }
  tree_node<T> *get_child(int idx) const { return children[idx]; }
  unsigned int get_child_count() const { return children.size(); }
};

template <class T> bool traversal_state<T>::next() {
  // preorder
  T *node;
  while (!node) {
    if (m_stack[m_stack_pointer].node_pointer->get_child_count() == 0 ||
        m_stack[m_stack_pointer].leaf_pointer >=
            m_stack[m_stack_pointer].node_pointer->get_child_count()) {
      if (m_stack_pointer == 0)
        return false;
      node = m_stack[m_stack_pointer--].node_pointer->get_data();
    } else {
      m_stack[m_stack_pointer + 1] =
          state{m_stack[m_stack_pointer].node_pointer->get_child(
                    m_stack[m_stack_pointer++].leaf_pointer++),
                0};
    }
  }
  return true;
}

template <class T>
traversal_state<T>
tree_node<T>::get_traversal_state(typename traversal_state<T>::MODE mode) {
  return traversal_state(mode, this);
};

template <class T> void tree_node<T>::insert_node(tree_node<T> *node, int idx) {
  if (idx >= 0 && idx <= children.size()) {
    children.insert(children.begin() + idx, node);
    return;
  }
  children.push_back(node);
  return;
}

template <class T> tree_node<T> *tree_node<T>::create_new(T *data) {
  return new tree_node<T>(data);
}

template <class T> int tree_node<T>::size(tree_node<T> *node) {
  int size = 0;
  if (node) {
    size += 1;
    for (int i = 0; i < node->children.size(); i++)
      size += tree_node<T>::size(node->children[i]);
  }
  return size;
}

template <class T> void tree_node<T>::destroy(tree_node<T> *node) {
  if (node->data)
    delete node->data;
  for (int i = 0; i < node->children.size(); i++)
    tree_node<T>::destroy(node->children[i]);
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam);

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
    int width = 1200, height = 800;
    GLFWmonitor *monitor = glfwGetWindowMonitor(window);
    if (!monitor) {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      width = mode->width;
      height = mode->height;
    }
    glfwSetWindowMonitor(window, monitor ? NULL : glfwGetPrimaryMonitor(), 50,
                         50, width, height, 60);
  }
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void error_callback(int error, const char *description) {
  std::cerr << "Error: %s\n" << description;
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

  int width, height;
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  if (monitor) {
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
  }
  window = glfwCreateWindow(width, height, "mechanics_simulation",
                            glfwGetPrimaryMonitor(), NULL);
  glfwSetKeyCallback(window, key_callback);
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
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
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

  std::string name = "test";
  tree_node<GUIitem> *root = tree_node<GUIitem>::create_new(new GUIitem(name));
  for (int i = 0; i < 12; i++)
    root->insert_node(tree_node<GUIitem>::create_new(new GUIitem(name)));
  std::cout << tree_node<GUIitem>::size(root) << std::endl;

  int count = 0; 
  tree_node<int>* rootl = tree_node<int>::create_new(new int(count++));
  for (int i = 0; i < 4; i++)
    rootl->insert_node(tree_node<int>::create_new(new int(count++)));
  auto c1 = rootl->get_child(0);
  for (int i = 0; i < 4; i++)
    c1->insert_node(tree_node<int>::create_new(new int(count++)));
  auto c2 = c1->get_child(0);
  for (int i = 0; i < 4; i++)
    c2->insert_node(tree_node<int>::create_new(new int(count++)));
  auto t = rootl->get_traversal_state(traversal_state<int>::MODE::INORDER);
  while(t.next())
    std::cout << *t.get_item() << std::endl;
  std::cin.get();


  environment env(window);

  timestamp delta;
  delta.begin();

  while (!glfwWindowShouldClose(window)) {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)
    GUI::show(env);

    // update
    env.update(delta.get_elapsed_time());
    environment::current_camera.update();

    // draw
    glClearColor(0.529, 0.808, 0.98, 0.7);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    env.draw();
    glStencilFunc(GL_ALWAYS, 0, 0xFF);

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
