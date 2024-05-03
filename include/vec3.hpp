#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

class Vec3 final {
public:
  float x;
  float y;
  float z;

  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

  Vec3 operator-() const { return Vec3(-x, -y, -z); }
  Vec3 inv() const { return Vec3(1.0f / x, 1.0f / y, 1.0f / z); }

  float dot(const Vec3 &other) const;
  Vec3 cross(const Vec3 &other) const;

  float length2() const { return this->dot(*this); }
  float length() const { return std::sqrt(this->length2()); }
  Vec3 normalize() const;
};

Vec3 operator+(const Vec3 &u, const Vec3 &v) { return Vec3(u.x + v.x, u.y + v.y, u.z + v.z); }
Vec3 operator*(const Vec3 &u, const Vec3 &v) { return Vec3(u.x * v.x, u.y * v.y, u.z * v.z); }
Vec3 operator-(const Vec3 &u, const Vec3 &v) { return u + (-v); }
Vec3 operator/(const Vec3 &u, const Vec3 &v) { return u * v.inv(); }

Vec3 operator+(const Vec3 &u, float v) { return Vec3(u.x + v, u.y + v, u.z + v); }
Vec3 operator*(const Vec3 &u, float v) { return Vec3(u.x * v, u.y * v, u.z * v); }
Vec3 operator-(const Vec3 &u, float v) { return u + (-v); }
Vec3 operator/(const Vec3 &u, float v) { return u * (1.0f / v); }

Vec3 operator+(float u, const Vec3 &v) { return v + u; }
Vec3 operator*(float u, const Vec3 &v) { return v * u; }
Vec3 operator-(float u, const Vec3 &v) { return u + (-v); }
Vec3 operator/(float u, const Vec3 &v) { return u * v.inv(); }

float dot(const Vec3 &u, const Vec3 &v) { return u.x * v.x + u.y * v.y + u.z * v.z; }
Vec3 cross(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.y * v.z - v.y * u.z, //
              u.z * v.x - v.z * u.x, //
              u.x * v.y - v.x * u.y);
}

float Vec3::dot(const Vec3 &other) const { return ::dot(*this, other); }
Vec3 Vec3::cross(const Vec3 &other) const { return ::cross(*this, other); }
Vec3 Vec3::normalize() const { return (*this) / this->length(); }

std::ostream &operator<<(std::ostream &out, const Vec3 &v) {
  return out << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ')';
}

class Ray final {
public:
  Vec3 origin;
  Vec3 direction;

  Ray(const Vec3 &origin, const Vec3 &direction) : origin(origin), direction(direction) {}

  Vec3 at(float t) const { return origin + direction * t; }
};

#endif // VEC3_H
