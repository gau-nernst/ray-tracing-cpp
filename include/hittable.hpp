#ifndef HITTABLE_H
#define HITTABLE_H
#include "vec3.hpp"
#include <cmath>

class Sphere final {
public:
  Sphere(const Vec3 &origin, float radius) : origin(origin), radius(radius) {}

  bool hit(const Ray &ray, float t_min, float t_max, Vec3 &color) {
    Vec3 oc = ray.origin - origin;
    float a = ray.direction.length2();
    float b = oc.dot(ray.direction);
    float c = oc.length2() - radius * radius;
    float disc = b * b - a * c;

    if (disc < 0)
      return false;

    float disc_sqrt = std::sqrt(disc);
    float t = (-b - disc_sqrt) / a;
    if (t < t_min || t_max < t) {
      t = (-b + disc_sqrt) / a;
      if (t < t_min || t_max < t)
        return false;
    }

    Vec3 incidence = ray.at(t);
    Vec3 normal = incidence - origin;
    color = (normal.normalize() + 1.0f) * 0.5f;

    return true;
  }

private:
  Vec3 origin;
  float radius;
};

#endif // HITTABLE_H
