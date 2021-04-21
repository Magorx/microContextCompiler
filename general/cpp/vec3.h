#ifndef GENERAL_VEC3D
#define GENERAL_VEC3D

#include <cmath>
#include <iostream>

const double VEC2_EPS = 1e-6;

//=============================================================================
//<KCTF> Vec2_header ==========================================================

struct Vector3d {
    double x;
    double y;
    double z;

    Vector3d();
    Vector3d(double x_, double y_, double z_);

    double len()      const;
    Vector3d normal() const;

    double   dot  (const Vector3d &other) const;
    Vector3d cross(const Vector3d &other) const;

    Vector3d orient(const Vector3d &axis);
};

Vector3d operator+  (const Vector3d &first, const Vector3d &second);
Vector3d operator-  (const Vector3d &first, const Vector3d &second);
Vector3d operator*  (const Vector3d &first, const Vector3d &second);
Vector3d operator/  (const Vector3d &first, const Vector3d &second);

Vector3d operator*  (const Vector3d &first, const double k);
Vector3d operator/  (const Vector3d &first, const double k);

Vector3d operator+= (Vector3d &first, const Vector3d &second);
Vector3d operator*= (Vector3d &first, const Vector3d &second);
Vector3d operator*= (Vector3d &first, const double k);

bool operator==     (const Vector3d &first, const Vector3d &second);
bool operator<      (const Vector3d &first, const Vector3d &second);

std::ostream& operator<<(std::ostream& output_stream, const Vector3d &vector);
std::istream& operator>>(std::istream& input_stream, Vector3d &vector);

Vector3d oriented(Vector3d axis, Vector3d other);

Vector3d rotx(const Vector3d vec, double ang);
Vector3d roty(const Vector3d vec, double ang);
Vector3d rotz(const Vector3d vec, double ang);

Vector3d rotate(const Vector3d vec, double dx, double dy, double dz);
Vector3d rotate(const Vector3d vec, const Vector3d &rotation);

//=============================================================================
//<KCTF> Vec2 =================================================================

Vector3d::Vector3d() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

Vector3d::Vector3d(double x_, double y_, double z_) {
    x = x_;
    y = y_;
    z = z_;
}

double Vector3d::len() const {
    return sqrt(x * x + y * y + z * z);
}

Vector3d Vector3d::normal() const {
    double l = len();
    if (l < VEC2_EPS) {
        return {0, 0, 0};
    } else {
        return {x / l, y / l, z / l};
    }
}

double Vector3d::dot(const Vector3d &other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3d Vector3d::cross(const Vector3d &other) const {
    double res_x = y * other.z - z * other.y;
    double res_y = z * other.x - x * other.z;
    double res_z = x * other.y - y * other.x;
    return {res_x, res_y, res_z};
}

Vector3d operator+(const Vector3d &first, const Vector3d &second) {
    return {first.x + second.x, first.y + second.y, first.z + second.z};
}

Vector3d operator+=(Vector3d &first, const Vector3d &second) {
    first.x += second.x;
    first.y += second.y;
    first.z += second.z;
    return first;
}

Vector3d operator-(const Vector3d &first, const Vector3d &second) {
    return {first.x - second.x, first.y - second.y, first.z - second.z};
}

Vector3d operator*(const Vector3d &first, const Vector3d &second) {
    return {first.x * second.x, first.y * second.y, first.z * second.z};
}

Vector3d operator*=(Vector3d &first, const Vector3d &second) {
    first.x *= second.x;
    first.y *= second.y;
    first.z *= second.z;
    return first;
}

Vector3d operator*(const Vector3d &first, const double k) {
    return {first.x * k, first.y * k, first.z * k};
}

Vector3d operator*=(Vector3d &first, const double k) {
    first.x *= k;
    first.y *= k;
    first.z *= k;
    return first;
}

Vector3d operator/(const Vector3d &first, const Vector3d &second) {
    return {first.x / second.x, first.y / second.y, first.z / second.z};
}

Vector3d operator/(const Vector3d &first, const double k) {
    return {first.x / k, first.y / k, first.z / k};
}

bool operator==(const Vector3d &first, const Vector3d &second) {
    return (first.x - second.x) < VEC2_EPS  && (first.y - second.y) < VEC2_EPS && (first.z -second.z) < VEC2_EPS;
}

bool operator<(const Vector3d &first, const Vector3d &second) {
    return first.x < second.x  && first.y < second.y && first.z < second.z;
}

std::ostream& operator<<(std::ostream& output_stream, const Vector3d &vector) {
    output_stream << vector.x << ' ' << vector.y << ' ' << vector.z;
    return output_stream;
}

std::istream& operator>>(std::istream& input_stream, Vector3d &vector) {
    char coma, bracket;
    input_stream >> bracket >> vector.x >> coma >> vector.y >> coma >> vector.z >> bracket;

    return input_stream;
}

Vector3d oriented(Vector3d axis, Vector3d other) {
    if (axis.dot(other) < 0) {
        other = other * (-1.0);
    }
    return other;
}

Vector3d Vector3d::orient(const Vector3d &axis) {
    if (axis.dot(*this) < 0) {
        *this *= -1.0;
    }
    return *this;
}

Vector3d rotx(const Vector3d vec, double ang) {
    double x = vec.x;
    double y = vec.y * cos(ang) - vec.z * sin(ang);
    double z = vec.z * cos(ang) + vec.y * sin(ang);
    return {x, y, z};
}

Vector3d roty(const Vector3d vec, double ang) {
    double x = vec.x * cos(ang) + vec.z * sin(ang);
    double y = vec.y;
    double z = vec.z * cos(ang) - vec.x * sin(ang);
    return {x, y, z};
}

Vector3d rotz(const Vector3d vec, double ang) {
    double x = vec.x * cos(ang) - vec.y * sin(ang);
    double y = vec.y * cos(ang) - vec.x * sin(ang);
    double z = vec.z;
    return {x, y, z};
}

Vector3d rotate(const Vector3d vec, double dx, double dy, double dz) {
    return rotz(roty(rotx(vec, dx), dy), dz);
}

Vector3d rotate(const Vector3d vec, const Vector3d rotation) {
    return rotz(roty(rotx(vec, rotation.x), rotation.y), rotation.z);
}

#endif // GENERAL_VEC3D