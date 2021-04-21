#ifndef GENERAL_VEC2D
#define GENERAL_VEC2D

#include <cmath>
#include <iostream>

const double VEC2_EPS = 1e-6;

//=============================================================================
//<KCTF> Vec2_header ==========================================================

struct Vector3d {
    double x;
    double y;

    Vector3d();
    Vector3d(double x_, double y_);

    double len()      const;
    Vector3d normal() const;

    double   dot  (const Vector3d &other) const;
    double cross(const Vector3d &other) const;

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

Vector3d rot_clockwise        (const Vector3d vec, double ang);
Vector3d rot_counterclockwise (const Vector3d vec, double ang);

Vector3d rotate(const Vector3d vec, double dx, double dy, double dz);
Vector3d rotate(const Vector3d vec, const Vector3d &rotation);

//=============================================================================
//<KCTF> Vec2 =================================================================

Vector3d::Vector3d() {
    x = 0.0;
    y = 0.0;
}

Vector3d::Vector3d(double x_, double y_) {
    x = x_;
    y = y_;
}

double Vector3d::len() const {
    return sqrt(x * x + y * y);
}

Vector3d Vector3d::normal() const {
    double l = len();
    if (l < VEC2_EPS) {
        return {0, 0};
    } else {
        return {x / l, y / l};
    }
}

double Vector3d::dot(const Vector3d &other) const {
    return x * other.x + y * other.y;
}

double Vector3d::cross(const Vector3d &other) const {
    return x * other.y - y * other.x;
}

Vector3d operator+(const Vector3d &first, const Vector3d &second) {
    return {first.x + second.x, first.y + second.y};
}

Vector3d operator+=(Vector3d &first, const Vector3d &second) {
    first.x += second.x;
    first.y += second.y;
    return first;
}

Vector3d operator-(const Vector3d &first, const Vector3d &second) {
    return {first.x - second.x, first.y - second.y};
}

Vector3d operator*(const Vector3d &first, const Vector3d &second) {
    return {first.x * second.x, first.y * second.y};
}

Vector3d operator*=(Vector3d &first, const Vector3d &second) {
    first.x *= second.x;
    first.y *= second.y;
    return first;
}

Vector3d operator*(const Vector3d &first, const double k) {
    return {first.x * k, first.y * k};
}

Vector3d operator*=(Vector3d &first, const double k) {
    first.x *= k;
    first.y *= k;
    return first;
}

Vector3d operator/(const Vector3d &first, const Vector3d &second) {
    return {first.x / second.x, first.y / second.y};
}

Vector3d operator/(const Vector3d &first, const double k) {
    return {first.x / k, first.y / k};
}

bool operator==(const Vector3d &first, const Vector3d &second) {
    return (first.x - second.x) < VEC2_EPS  && (first.y - second.y) < VEC2_EPS;
}

bool operator<(const Vector3d &first, const Vector3d &second) {
    return first.x < second.x  && first.y < second.y;
}

std::ostream& operator<<(std::ostream& output_stream, const Vector3d &vector) {
    output_stream << vector.x << ' ' << vector.y;
    return output_stream;
}

std::istream& operator>>(std::istream& input_stream, Vector3d &vector) {
    char coma, bracket;
    input_stream >> bracket >> vector.x >> coma >> vector.y >> bracket;

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

Vector3d rot_counterclockwise(const Vector3d vec, double ang) {
    double x = vec.x * cos(ang) - vec.y * sin(ang);
    double y = vec.x * sin(ang) + vec.y * cos(ang);
    return {x, y};
}

Vector3d rot_clockwise(const Vector3d vec, double ang) {
    double x = vec.x * cos(ang) + vec.y * sin(ang);
    double y = -vec.x * sin(ang) + vec.y * cos(ang);
    return {x, y};
}

#endif // GENERAL_VEC2D