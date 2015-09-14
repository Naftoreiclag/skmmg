#include "Vec3f.hpp"

#include <cmath>

namespace skm {

Vec3f::Vec3f()
: x(0.f)
, y(0.f)
, z(0.f) {
}

Vec3f::Vec3f(float x, float y, float z)
: x(x)
, y(y)
, z(z) {
}

Vec3f::Vec3f(const Vec3f &v)
: x(v.x)
, y(v.y)
, z(v.z) {
}

Vec3f::~Vec3f() {
}

// Equals
bool Vec3f::operator==(const Vec3f& v1) {
    return v1.x == x && v1.y == y && v1.z == z;
}

// Scaling
Vec3f Vec3f::operator*(float s) {
    return Vec3f(x * s, y * s, z * s);
}
Vec3f& Vec3f::operator*=(float s) {
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

// Inverse scaling
Vec3f Vec3f::operator/(float s) {
    return Vec3f(x / s, y / s, z / s);
}
Vec3f& Vec3f::operator/=(float s) {
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

// Addition
Vec3f Vec3f::operator+(const Vec3f& v) {
    return Vec3f(x + v.x, y + v.y, z + v.z);
}
Vec3f& Vec3f::operator+=(const Vec3f& v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

// Subtraction
Vec3f Vec3f::operator-(const Vec3f& v) {
    return Vec3f(x - v.x, y - v.y, z - v.z);
}
Vec3f& Vec3f::operator-=(const Vec3f& v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

// Dot product
float Vec3f::dot(const Vec3f& v1, const Vec3f& v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

// Cross product
Vec3f Vec3f::cross(const Vec3f& v1, const Vec3f& v2) {
    return Vec3f((v1.y * v2.z) - (v2.y * v1.z), (v1.z * v2.x) - (v2.z * v1.x), (v1.x * v2.y) - (v2.x * v1.y));
}

// Distance to other vector
float Vec3f::distSq(const Vec3f& v) {
    float dx = v.x - x;
    float dy = v.y - y;
    float dz = v.z - z;
    return (dx * dx) + (dy * dy) + (dz * dz);
}
float Vec3f::dist(const Vec3f& v) {
    return std::sqrt(distSq(v));
}

// Length of vector
float Vec3f::magSq() {
    return (x * x) + (y * y) + (z * z);
}
float Vec3f::mag() {
    return std::sqrt(magSq());
}

void Vec3f::zero() {
    x = 0.f;
    y = 0.f;
    z = 0.f;
}

bool Vec3f::isZero() {
    return x == 0.f && y == 0.f && z == 0.f;
}

}
    
// For debugging
std::ostream& operator<<(std::ostream& os, const skm::Vec3f& v) {
    return os << "Vec3[" << v.x << ", " << v.y << ", " << v.z << "]";
}

sf::Packet& operator<<(sf::Packet& p, const skm::Vec3f& v) {
    return p << v.x << v.y << v.z;
}
sf::Packet& operator>>(sf::Packet& p, skm::Vec3f& v) {
    return p >> v.x >> v.y >> v.z;
}
