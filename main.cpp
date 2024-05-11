#include "camera.hpp"
#include "hittable.hpp"
#include "tiff.h"
#include "vec3.hpp"

#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <cmath>
#include <random>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

uint8_t float_to_uint8(float x) { return static_cast<uint8_t>(std::min(std::max(x * 255.9999f, 0.0f), 255.0f)); }

static std::mt19937 GEN(std::random_device{}());

void render(const Camera &cam, int n_rays, uint8_t *img_buf) {
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);

  Vec3 color1(1, 0, 0);
  Vec3 color2(0, 0, 1);
  Sphere sphere(Vec3(0, 0, -0.1), 0.01);

  for (int j = 0; j < cam.img_height; j++) {
    for (int i = 0; i < cam.img_width; i++) {
      Vec3 color = Vec3::zero();

      for (int n = 0; n < n_rays; n++) {
        // square sampling
        float u = static_cast<float>(i) + dist(GEN);
        float v = static_cast<float>(j) + dist(GEN);
        Ray ray = cam.pixel_to_ray(u, v);

        Vec3 local_color;
        if (!sphere.hit(ray, 0.0f, 1e9f, local_color)) {
          float y = (j + 0.5) / cam.img_height;
          local_color = color1 + y * (color2 - color1);
        }

        color = color + local_color;
      }

      color = color / static_cast<float>(n_rays);
      img_buf[(j * cam.img_width + i) * 3] = float_to_uint8(color.x);
      img_buf[(j * cam.img_width + i) * 3 + 1] = float_to_uint8(color.y);
      img_buf[(j * cam.img_width + i) * 3 + 2] = float_to_uint8(color.z);
    }
  }
}

int main() {
  int img_width = 1280;
  int img_height = 720;

  // setup OpenGL
  if (!glfwInit())
    return 1;

  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // Create window with graphics context
  GLFWwindow *window = glfwCreateWindow(img_width, img_height, "Ray tracing C++", nullptr, nullptr);
  if (window == nullptr)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  Camera cam(img_width / 2, img_height / 2);
  int n_rays = 4;

  std::vector<uint8_t> img_buf(cam.img_width * cam.img_height * 3);
  render(cam, n_rays, img_buf.data());

  // create texture to display results
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, cam.img_width, cam.img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  // copy data over to texture
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cam.img_width, cam.img_height, GL_RGB, GL_UNSIGNED_BYTE,
                  (void *)img_buf.data());

  GLuint readFboId = 0;
  glGenFramebuffers(1, &readFboId);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // {
    //   ImGui::Begin("Hello, world!");
    //   ImGui::End();
    // }

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
    glBlitFramebuffer(0, 0, cam.img_width, cam.img_height, 0, 0, display_w, display_h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    glfwSwapBuffers(window);
  }

  FILE *f = fopen("output.tiff", "w");
  write_tiff(f, cam.img_width, cam.img_height, 3, img_buf.data());

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
