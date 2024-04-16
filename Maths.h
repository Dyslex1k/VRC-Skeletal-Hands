#pragma once

#include <openvr.h>

#define MATHS_PI       3.14159265358979323846

inline double degToRad(const double degree) {
	return degree * MATHS_PI / 180.0;
}
inline float degToRad(const float degree) {
	return degree * MATHS_PI / 180.0;
}
inline double radToDeg(const double radian) {
	return radian * 180.0 / MATHS_PI;
}
inline float radToDeg(const float radian) {
	return radian * 180.0 / MATHS_PI;
}

double lerp(double a, double b, double t);
double remap(double t, double fromMin, double fromMax, double toMin, double toMax);

double length(vr::HmdVector4_t vector);
double length(vr::HmdVector3_t vector);
double length(vr::HmdVector3d_t vector);

//normalize means making length of a vector 1
vr::HmdVector4_t normalize(vr::HmdVector4_t vector);
vr::HmdVector3_t normalize(vr::HmdVector3_t vector);
vr::HmdVector3d_t normalize(vr::HmdVector3d_t vector);

vr::HmdVector4_t pow(vr::HmdVector4_t vector, double expo);
vr::HmdVector3_t pow(vr::HmdVector3_t vector, double expo);
vr::HmdVector3d_t pow(vr::HmdVector3d_t vector, double expo);

vr::HmdVector4_t operator+(const vr::HmdVector4_t& lhs, const vr::HmdVector4_t& rhs);
vr::HmdVector3_t operator+(const vr::HmdVector3_t& lhs, const vr::HmdVector3_t& rhs);
vr::HmdVector3d_t operator+(const vr::HmdVector3d_t& lhs, const vr::HmdVector3d_t& rhs);

vr::HmdVector4_t operator-(const vr::HmdVector4_t& lhs);
vr::HmdVector3_t operator-(const vr::HmdVector3_t& lhs);
vr::HmdVector3d_t operator-(const vr::HmdVector3d_t& lhs);

vr::HmdVector4_t operator-(const vr::HmdVector4_t& lhs, const vr::HmdVector4_t& rhs);
vr::HmdVector3_t operator-(const vr::HmdVector3_t& lhs, const vr::HmdVector3_t& rhs);
vr::HmdVector3d_t operator-(const vr::HmdVector3d_t& lhs, const vr::HmdVector3d_t& rhs);

vr::HmdVector4_t operator*(const vr::HmdVector4_t& lhs, const vr::HmdVector4_t& rhs);
vr::HmdVector3_t operator*(const vr::HmdVector3_t& lhs, const vr::HmdVector3_t& rhs);
vr::HmdVector3d_t operator*(const vr::HmdVector3d_t& lhs, const vr::HmdVector3d_t& rhs);

//Dot product =  |a||b|cos0
double dot(vr::HmdVector4_t lhs, vr::HmdVector4_t rhs);
double dot(vr::HmdVector3_t lhs, vr::HmdVector3_t rhs);
double dot(vr::HmdVector3d_t lhs, vr::HmdVector3d_t rhs);

vr::HmdVector3_t operator*(const vr::HmdVector3_t& lhs, const vr::HmdQuaternion_t& rhs);
vr::HmdVector3d_t operator*(const vr::HmdVector3d_t& lhs, const vr::HmdQuaternion_t& rhs);
vr::HmdVector3_t operator*(const vr::HmdVector3_t& lhs, const vr::HmdQuaternionf_t& rhs);
vr::HmdVector3d_t operator*(const vr::HmdVector3d_t& lhs, const vr::HmdQuaternionf_t& rhs);

vr::HmdQuaternion_t operator-(const vr::HmdQuaternion_t& q);
vr::HmdQuaternionf_t operator-(const vr::HmdQuaternionf_t& q);

vr::HmdQuaternion_t operator*(const vr::HmdQuaternion_t& q, const vr::HmdQuaternion_t& r);
vr::HmdQuaternionf_t operator*(const vr::HmdQuaternionf_t& q, const vr::HmdQuaternion_t& r);
vr::HmdQuaternionf_t operator*(const vr::HmdQuaternion_t& q, const vr::HmdQuaternionf_t& r);
vr::HmdQuaternionf_t operator*(const vr::HmdQuaternionf_t& q, const vr::HmdQuaternionf_t& r);