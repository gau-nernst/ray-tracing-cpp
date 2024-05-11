#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.hpp"

class Camera final {
public:
  // intrinsic
  float focal_length;
  float viewport_width;
  int img_width;
  int img_height;

  Camera(int img_width, int img_height)
      : focal_length(0.035f),   // 35mm
        viewport_width(0.036f), // 35mm film is 36mm wide
        img_width(img_width),   //
        img_height(img_height) {}

  Ray pixel_to_ray(float u, float v) const {
    // change pixel origin to image's center. flip vertical axis (from top-bottom to bottom-top).
    // scale to camera coordinates.
    float scale = viewport_width / img_width;
    u = (u - static_cast<float>(img_width) * 0.5f) * scale;
    v = (static_cast<float>(img_height) * 0.5f - v) * scale;
    return Ray(Vec3::zero(), Vec3(u, v, -focal_length));
  }
};

#endif // CAMERA_H
