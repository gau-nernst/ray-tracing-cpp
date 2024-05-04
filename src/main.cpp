#include "camera.hpp"
#include "hittable.hpp"
#include "vec3.hpp"
extern "C" {
#include "tiff.h"
}
#include <cmath>
#include <random>

uint8_t float_to_uint8(float x) { return static_cast<uint8_t>(std::min(std::max(x * 255.9999f, 0.0f), 255.0f)); }

int main() {
  Camera cam(400, 300);
  int n_rays = 4;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);

  std::vector<uint8_t> img_buf(cam.img_width * cam.img_height * 3);

  Vec3 color1(1, 0, 0);
  Vec3 color2(0, 0, 1);
  Sphere sphere(Vec3(0, 0, -0.1), 0.01);

  for (int j = 0; j < cam.img_height; j++) {
    for (int i = 0; i < cam.img_width; i++) {
      Vec3 color = Vec3::zero();

      for (int n = 0; n < n_rays; n++) {
        // square sampling
        float u = static_cast<float>(i) + dist(gen);
        float v = static_cast<float>(j) + dist(gen);
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

  FILE *f = fopen("output.tiff", "w");
  write_tiff(f, cam.img_width, cam.img_height, 3, img_buf.data());
}
