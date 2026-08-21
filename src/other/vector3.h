/**
 *  ============================================================================
 *  MIT License
 *
 *  Copyright (c) 2016 Eric Phillips
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *  ============================================================================
 *
 *
 *  This file implements a series of math functions for manipulating a
 *  3D vector.
 *
 *  Created by Eric Phillips on October 8, 2016.
 */

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <vector>

struct Vector4 {
    float x, y, z, w;
    inline Vector4() : x(0), y(0), z(0), w(0) {}
    inline Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct Quaternion;
struct Vector3 {
    union {
        struct { float x, y, z; };
        float data[3];
    };
    inline Vector3() : x(0), y(0), z(0) {};
    inline Vector3(float x, float y, float z) : x(x), y(y), z(z) {};
    [[maybe_unused]] inline static Vector3 Back() { return Vector3(0, 0, -1); };
    [[maybe_unused]] inline static Vector3 Down() { return Vector3(0, -1, 0); };
    [[maybe_unused]] inline static Vector3 Forward() { return Vector3(0, 0, 1); };
    [[maybe_unused]] inline static Vector3 Left() { return Vector3(-1, 0, 0); };
    [[maybe_unused]] inline static Vector3 Right() { return Vector3(1, 0, 0); };
    [[maybe_unused]] inline static Vector3 Up() { return Vector3(0, 1, 0); }
    [[maybe_unused]] inline static Vector3 Zero() { return Vector3(0, 0, 0); };
    [[maybe_unused]] inline static Vector3 One() { return Vector3(1, 1, 1); };
    inline static float Angle(Vector3 from, Vector3 to);
    inline static float Dot(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 ClampMagnitude(Vector3, float);
    [[maybe_unused]] inline static float Component(Vector3, Vector3);
    inline static Vector3 Cross(Vector3, Vector3);
    [[maybe_unused]] inline static float Distance(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 FromSpherical(float, float, float);
    [[maybe_unused]] inline static Vector3 Lerp(Vector3, Vector3, float);
    inline static Vector3 LerpUnclamped(Vector3, Vector3, float);
    [[maybe_unused]] inline static Vector3 Max(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 Min(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 MoveTowards(Vector3, Vector3, float);
    inline static Vector3 Orthogonal(Vector3);
    [[maybe_unused]] inline static Vector3 RotateTowards(Vector3, Vector3, float, float);
    [[maybe_unused]] inline static Vector3 Scale(Vector3, Vector3);
    [[maybe_unused]] inline static Vector3 Slerp(Vector3, Vector3, float);
    inline static Vector3 SlerpUnclamped(Vector3, Vector3, float);
    [[maybe_unused]] inline static void ToSpherical(Vector3 vector, float &, float &, float &);
    inline static float Magnitude(Vector3);
    inline static float SqrMagnitude(Vector3);
    inline static Vector3 Normalize(Vector3);
    inline static Vector3 NormalizeEuler(Vector3);
    inline static float NormalizeAngle(float f);
    [[maybe_unused]] inline static Vector3 FromString(std::string);
    inline Vector3 orthogonal() { return Orthogonal(*this); }
    inline float magnitude() { return Magnitude(*this); }
    inline float sqrMagnitude() { return SqrMagnitude(*this); }
    inline Vector3 normalized() { return Normalize(*this); }
    [[maybe_unused]] inline Vector3 normalizedEuler() { return NormalizeEuler(*this); }
    inline std::string str() { return std::to_string(x) + (", ") + std::to_string(y) + (", ") + std::to_string(z); }
    inline const Vector3& operator+=(float v) { x+=v; y+=v; z+=v; return *this; };
    inline const Vector3& operator-=(float v) { x-=v; y-=v; y-=v; return *this; };
    inline const Vector3& operator*=(float v) { x*=v; y*=v; z*=v; return *this; };
    inline const Vector3& operator/=(float v) { x/=v; y/=v; z/=v; return *this; };
    inline const Vector3& operator+=(Vector3 v) { x+=v.x; y+=v.y; z+=v.z; return *this; };
    inline const Vector3& operator-=(Vector3 v) { x-=v.x; y-=v.y; z-=v.z; return *this; };
    inline const Vector3& operator*=(Vector3 v) { x*=v.x; y*=v.y; z*=v.z; return *this; };
    inline Vector3& operator/=(Vector3 v) { x/=v.x; y/=v.y; z/=v.z; return *this; };
};
inline Vector3 operator+(Vector3 lhs, const float rhs) { return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs); }
inline Vector3 operator-(Vector3 lhs, const float rhs) { return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs); }
inline Vector3 operator*(Vector3 lhs, const float rhs) { return Vector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs); }
inline Vector3 operator/(Vector3 lhs, const float rhs) { return Vector3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs); }
inline Vector3 operator+(const float lhs, Vector3 rhs) { return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z); }
inline Vector3 operator-(const float lhs, Vector3 rhs) { return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z); }
inline Vector3 operator*(const float lhs, Vector3 rhs) { return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z); }
inline Vector3 operator/(const float lhs, Vector3 rhs) { return Vector3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z); }
inline Vector3 operator+(Vector3 lhs, const Vector3 rhs) { return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }
inline Vector3 operator-(Vector3 lhs, const Vector3 rhs) { return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }
inline Vector3 operator*(Vector3 lhs, const Vector3 rhs) { return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); }
inline Vector3 operator/(Vector3 lhs, const Vector3 rhs) { return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); }
inline bool operator==(const Vector3 lhs, const Vector3 rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z; }
inline bool operator!=(const Vector3 lhs, const Vector3 rhs) { return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z; }
inline Vector3 operator-(Vector3 v) {return v * -1;}

float Vector3::Angle(Vector3 from, Vector3 to) {
    float v = Dot(from, to) / (from.magnitude() * to.magnitude());
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    return acos(v);
}

float Vector3::Dot(Vector3 lhs, Vector3 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
[[maybe_unused]] Vector3 Vector3::ClampMagnitude(Vector3 vector, float maxLength) {
    float length = vector.magnitude();
    if (length > maxLength) vector *= maxLength / length;
    return vector;
}

[[maybe_unused]] float Vector3::Component(Vector3 a, Vector3 b) {
    return Dot(a, b) / b.magnitude();
}

Vector3 Vector3::Cross(Vector3 lhs, Vector3 rhs) {
    float x = lhs.y * rhs.z - lhs.z * rhs.y;
    float y = lhs.z * rhs.x - lhs.x * rhs.z;
    float z = lhs.x * rhs.y - lhs.y * rhs.x;
    return Vector3(x, y, z);
}

[[maybe_unused]] float Vector3::Distance(Vector3 a, Vector3 b) {
    return (a - b).magnitude();
}

[[maybe_unused]] Vector3 Vector3::FromSpherical(float rad, float theta, float phi) {
    Vector3 v;
    v.x = rad * sin(theta) * cos(phi);
    v.y = rad * sin(theta) * sin(phi);
    v.z = rad * cos(theta);
    return v;
}

[[maybe_unused]] Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t) {
    if (t < 0) return a;
    else if (t > 1) return b;
    return LerpUnclamped(a, b, t);
}

Vector3 Vector3::LerpUnclamped(Vector3 a, Vector3 b, float t) {
    return (b - a) * t + a;
}

[[maybe_unused]] Vector3 Vector3::Max(Vector3 a, Vector3 b) {
    float x = a.x > b.x ? a.x : b.x;
    float y = a.y > b.y ? a.y : b.y;
    float z = a.z > b.z ? a.z : b.z;
    return Vector3(x, y, z);
}

[[maybe_unused]] Vector3 Vector3::Min(Vector3 a, Vector3 b) {
    float x = a.x > b.x ? b.x : a.x;
    float y = a.y > b.y ? b.y : a.y;
    float z = a.z > b.z ? b.z : a.z;
    return Vector3(x, y, z);
}

[[maybe_unused]] Vector3 Vector3::MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta) {
    Vector3 d = target - current;
    float m = d.magnitude();
    if (m < maxDistanceDelta || m == 0)
        return target;
    return current + (d * maxDistanceDelta / m);
}

Vector3 Vector3::Orthogonal(Vector3 v) {
    return v.z < v.x ? Vector3(v.y, -v.x, 0) : Vector3(0, -v.z, v.y);
}

[[maybe_unused]] Vector3 Vector3::RotateTowards(Vector3 current, Vector3 target, float maxRadiansDelta, float maxMagnitudeDelta) {
    float magCur = current.magnitude();
    float magTar = target.magnitude();
    float newMag = magCur + maxMagnitudeDelta * ((magTar > magCur) - (magCur > magTar));
    newMag = fmin(newMag, fmax(magCur, magTar));
    newMag = fmax(newMag, fmin(magCur, magTar));
    float totalAngle = Angle(current, target) - maxRadiansDelta;
    if (totalAngle <= 0) return Normalize(target) * newMag;
    else if (totalAngle >= M_PI) return -target.normalized() * newMag;
    Vector3 axis = Cross(current, target);
    float magAxis = axis.magnitude();
    if (magAxis == 0) axis = Cross(current, current + Vector3(3.95, 5.32, -4.24)).normalized();
    else axis /= magAxis;
    current = current.normalized();
    Vector3 newVector = current * cos(maxRadiansDelta) + Cross(axis, current) * sin(maxRadiansDelta);
    return newVector * newMag;
}

[[maybe_unused]] Vector3 Vector3::Scale(Vector3 a, Vector3 b) {
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

[[maybe_unused]] Vector3 Vector3::Slerp(Vector3 a, Vector3 b, float t) {
    if (t < 0) return a;
    else if (t > 1) return b;
    return SlerpUnclamped(a, b, t);
}

Vector3 Vector3::SlerpUnclamped(Vector3 a, Vector3 b, float t) {
    float magA = a.magnitude();
    float magB = b.magnitude();
    a /= magA;
    b /= magB;
    float dot = Dot(a, b);
    dot = fmax(dot, -1.0);
    dot = fmin(dot, 1.0);
    float theta = acos(dot) * t;
    Vector3 relativeVec = Normalize(b - a * dot);
    Vector3 newVec = a * cos(theta) + relativeVec * sin(theta);
    return newVec * (magA + (magB - magA) * t);
}

[[maybe_unused]] void Vector3::ToSpherical(Vector3 vector, float &rad, float &theta, float &phi) {
    rad = vector.magnitude();
    float v = vector.z / rad;
    v = fmax(v, -1.0);
    v = fmin(v, 1.0);
    theta = acos(v);
    phi = atan2(vector.y, vector.x);
}

float Vector3::Magnitude(Vector3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float Vector3::SqrMagnitude(Vector3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

Vector3 Vector3::Normalize(Vector3 v) {
    float mag = v.magnitude();
    if (mag == 0) return Vector3::Zero();
    return v / mag;
}

float Vector3::NormalizeAngle(float f) {
    while (f > 360) f -= 360;
    while (f < 0) f += 360;
    return f;
}

Vector3 Vector3::NormalizeEuler(Vector3 vec) {
    vec.x = NormalizeAngle(vec.x);
    vec.y = NormalizeAngle(vec.y);
    vec.z = NormalizeAngle(vec.z);
    return vec;
}

[[maybe_unused]] Vector3 Vector3::FromString(std::string str) {
    std::vector<std::string> commands;
    std::string buffer = "";
    for (int i = 0; i < str.size(); i++) {
        bool no = true;
        if (str[i] == 'f') no = false;
        if (str[i] != ',') buffer += str[i];
        else if (no) {
            commands.push_back(buffer);
            buffer = "";
        }
    }
    if (buffer != "") commands.push_back(buffer);
    return Vector3(std::atof(commands[0].c_str()), std::atof(commands[1].c_str()), std::atof(commands[2].c_str()));
}

#define SMALL_FLOAT 0.0000000001f
#define Deg2Rad (3.14159265358979323846f / 180.f)
#define Rad2Deg (180.f / 3.14159265358979323846f)

struct Quaternion {
    float x, y, z, w;

    inline Quaternion() : x(0), y(0), z(0), w(1) {}
    inline Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    inline Quaternion(Vector3 v, float s) : x(v.x), y(v.y), z(v.z), w(s) {}

    static inline Quaternion Identity() { return Quaternion(0, 0, 0, 1); }

    static inline float Norm(Quaternion q) {
        return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    }

    static inline Quaternion Normalize(Quaternion q) {
        float n = Norm(q);
        if (n < SMALL_FLOAT) return Identity();
        return Quaternion(q.x / n, q.y / n, q.z / n, q.w / n);
    }

    static inline Quaternion FromToRotation(Vector3 from, Vector3 to) {
        float dot = Vector3::Dot(from, to);
        float k = sqrtf(from.sqrMagnitude() * to.sqrMagnitude());
        if (fabsf(dot / k + 1) < 0.00001f) {
            Vector3 ortho = from.orthogonal();
            return Quaternion(ortho.normalized(), 0);
        }
        Vector3 cross = Vector3::Cross(from, to);
        return Normalize(Quaternion(cross, dot + k));
    }

    static inline Quaternion LookRotation(Vector3 forward, Vector3 upwards = Vector3::Up()) {
        forward = forward.normalized();
        upwards = upwards.normalized();
        if (forward.sqrMagnitude() < SMALL_FLOAT || upwards.sqrMagnitude() < SMALL_FLOAT)
            return Identity();
        if (1 - fabsf(Vector3::Dot(forward, upwards)) < SMALL_FLOAT)
            return FromToRotation(Vector3::Forward(), forward);
        Vector3 right = Vector3::Cross(upwards, forward).normalized();
        upwards = Vector3::Cross(forward, right);
        Quaternion q;
        float radicand = right.x + upwards.y + forward.z;
        if (radicand > 0) {
            q.w = sqrtf(1.0f + radicand) * 0.5f;
            float recip = 1.0f / (4.0f * q.w);
            q.x = (upwards.z - forward.y) * recip;
            q.y = (forward.x - right.z) * recip;
            q.z = (right.y - upwards.x) * recip;
        } else if (right.x >= upwards.y && right.x >= forward.z) {
            q.x = sqrtf(1.0f + right.x - upwards.y - forward.z) * 0.5f;
            float recip = 1.0f / (4.0f * q.x);
            q.w = (upwards.z - forward.y) * recip;
            q.z = (forward.x + right.z) * recip;
            q.y = (right.y + upwards.x) * recip;
        } else if (upwards.y > forward.z) {
            q.y = sqrtf(1.0f - right.x + upwards.y - forward.z) * 0.5f;
            float recip = 1.0f / (4.0f * q.y);
            q.z = (upwards.z + forward.y) * recip;
            q.w = (forward.x - right.z) * recip;
            q.x = (right.y + upwards.x) * recip;
        } else {
            q.z = sqrtf(1.0f - right.x - upwards.y + forward.z) * 0.5f;
            float recip = 1.0f / (4.0f * q.z);
            q.y = (upwards.z + forward.y) * recip;
            q.x = (forward.x + right.z) * recip;
            q.w = (right.y - upwards.x) * recip;
        }
        return q;
    }

    static inline Vector3 ToEuler(Quaternion q) {
        float sqw = q.w * q.w;
        float sqx = q.x * q.x;
        float sqy = q.y * q.y;
        float sqz = q.z * q.z;
        float unit = sqx + sqy + sqz + sqw;
        float test = q.x * q.w - q.y * q.z;
        Vector3 v;
        if (test > 0.4995f * unit) {
            v.y = 2.f * atan2f(q.y, q.x);
            v.x = 1.5707963f;
            v.z = 0;
            return v * Rad2Deg;
        }
        if (test < -0.4995f * unit) {
            v.y = -2.f * atan2f(q.y, q.x);
            v.x = -1.5707963f;
            v.z = 0;
            return v * Rad2Deg;
        }
        v.y = atan2f(2.f * q.w * q.y + 2.f * q.z * q.x, 1.f - 2.f * (sqx + sqy));
        v.x = asinf(2.f * (q.w * q.x - q.y * q.z));
        v.z = atan2f(2.f * q.w * q.z + 2.f * q.x * q.y, 1.f - 2.f * (sqz + sqx));
        return (v * Rad2Deg) + 180.f;
    }

    inline Vector3 euler() const { return ToEuler(*this); }
    inline Quaternion normalized() const { return Normalize(*this); }
};
