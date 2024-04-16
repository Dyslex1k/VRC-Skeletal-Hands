#include "Maths.h"

#include <cmath>

double length(vr::HmdVector4_t vector) {
	return sqrt(vector.v[0] * vector.v[0] +
				vector.v[1] * vector.v[1] +
				vector.v[2] * vector.v[2] +
				vector.v[3] * vector.v[3]);
}

double length(vr::HmdVector3_t vector) {
	return sqrt(vector.v[0] * vector.v[0] +
		vector.v[1] * vector.v[1] +
		vector.v[2] * vector.v[2]);
}

double length(vr::HmdVector3d_t vector) {
	return sqrt(vector.v[0] * vector.v[0] +
		vector.v[1] * vector.v[1] +
		vector.v[2] * vector.v[2]);
}

double lerp(double a, double b, double t) {
	return t * (b - a) + a;
}

double remap(double t, double fromMin, double fromMax, double toMin, double toMax) {
	//v = ((v-x)/y)(b-a) + a
	//return ((t - fromMin) / fromMax) * (toMax - toMin) + toMin;
	return lerp(toMin, toMax, ((t - fromMin) / (fromMax - fromMin)));
}


vr::HmdVector4_t normalize(vr::HmdVector4_t vector) {
	vr::HmdVector4_t result = {};
	double vectLength = length(vector);
	result.v[0] = vector.v[0] / vectLength;
	result.v[1] = vector.v[1] / vectLength;
	result.v[2] = vector.v[2] / vectLength;
	result.v[3] = vector.v[3] / vectLength;
	return result;
}

vr::HmdVector3_t normalize(vr::HmdVector3_t vector){
	vr::HmdVector3_t result = {};
	double vectLength = length(vector);
	result.v[0] = vector.v[0] / vectLength;
	result.v[1] = vector.v[1] / vectLength;
	result.v[2] = vector.v[2] / vectLength;
	return result;
}

vr::HmdVector3d_t normalize(vr::HmdVector3d_t vector){
	vr::HmdVector3d_t result = {};
	double vectLength = length(vector);
	result.v[0] = vector.v[0] / vectLength;
	result.v[1] = vector.v[1] / vectLength;
	result.v[2] = vector.v[2] / vectLength;
	return result;
}

vr::HmdVector4_t pow(vr::HmdVector4_t vector, double expo) {
	vr::HmdVector4_t result = {};
	result.v[0] = pow(vector.v[0], expo);
	result.v[1] = pow(vector.v[1], expo);
	result.v[2] = pow(vector.v[2], expo);
	result.v[3] = pow(vector.v[3], expo);
	return result;
}

vr::HmdVector3_t pow(vr::HmdVector3_t vector, double expo) {
	vr::HmdVector3_t result = {};
	result.v[0] = pow(vector.v[0], expo);
	result.v[1] = pow(vector.v[1], expo);
	result.v[2] = pow(vector.v[2], expo);
	return result;
}

vr::HmdVector3d_t pow(vr::HmdVector3d_t vector, double expo) {
	vr::HmdVector3d_t result = {};
	result.v[0] = pow(vector.v[0], expo);
	result.v[1] = pow(vector.v[1], expo);
	result.v[2] = pow(vector.v[2], expo);
	return result;
}

vr::HmdVector4_t operator+(const vr::HmdVector4_t& lhs, const vr::HmdVector4_t& rhs) {
	vr::HmdVector4_t result = {};
	result.v[0] = lhs.v[0] + rhs.v[0];
	result.v[1] = lhs.v[1] + rhs.v[1];
	result.v[2] = lhs.v[2] + rhs.v[2];
	result.v[3] = lhs.v[3] + rhs.v[3];
	return result;
}

vr::HmdVector3_t operator+(const vr::HmdVector3_t& lhs, const vr::HmdVector3_t& rhs){
	vr::HmdVector3_t result = {};
	result.v[0] = lhs.v[0] + rhs.v[0];
	result.v[1] = lhs.v[1] + rhs.v[1];
	result.v[2] = lhs.v[2] + rhs.v[2];
	return result;
}

vr::HmdVector3d_t operator+(const vr::HmdVector3d_t& lhs, const vr::HmdVector3d_t& rhs){
	vr::HmdVector3d_t result = {};
	result.v[0] = lhs.v[0] + rhs.v[0];
	result.v[1] = lhs.v[1] + rhs.v[1];
	result.v[2] = lhs.v[2] + rhs.v[2];
	return result;
}

vr::HmdVector4_t operator-(const vr::HmdVector4_t& lhs){
	vr::HmdVector4_t result = {};
	result.v[0] = -lhs.v[0];
	result.v[1] = -lhs.v[1];
	result.v[2] = -lhs.v[2];
	result.v[3] = -lhs.v[3];
	return result;
}

vr::HmdVector3_t operator-(const vr::HmdVector3_t& lhs){
	vr::HmdVector3_t result = {};
	result.v[0] = -lhs.v[0];
	result.v[1] = -lhs.v[1];
	result.v[2] = -lhs.v[2];
	return result;
}

vr::HmdVector3d_t operator-(const vr::HmdVector3d_t& lhs) {
	vr::HmdVector3d_t result = {};
	result.v[0] = -lhs.v[0];
	result.v[1] = -lhs.v[1];
	result.v[2] = -lhs.v[2];
	return result;
}

vr::HmdVector4_t operator-(const vr::HmdVector4_t& lhs, const vr::HmdVector4_t& rhs) {
	vr::HmdVector4_t result = {};
	result.v[0] = lhs.v[0] - rhs.v[0];
	result.v[1] = lhs.v[1] - rhs.v[1];
	result.v[2] = lhs.v[2] - rhs.v[2];
	result.v[3] = lhs.v[3] - rhs.v[3];
	return result;
}

vr::HmdVector3_t operator-(const vr::HmdVector3_t& lhs, const vr::HmdVector3_t& rhs) {
	vr::HmdVector3_t result = {};
	result.v[0] = lhs.v[0] - rhs.v[0];
	result.v[1] = lhs.v[1] - rhs.v[1];
	result.v[2] = lhs.v[2] - rhs.v[2];
	return result;
}

vr::HmdVector3d_t operator-(const vr::HmdVector3d_t& lhs, const vr::HmdVector3d_t& rhs) {
	vr::HmdVector3d_t result = {};
	result.v[0] = lhs.v[0] - rhs.v[0];
	result.v[1] = lhs.v[1] - rhs.v[1];
	result.v[2] = lhs.v[2] - rhs.v[2];
	return result;
}

vr::HmdVector4_t operator*(const vr::HmdVector4_t& lhs, const vr::HmdVector4_t& rhs) {
	vr::HmdVector4_t result = {};
	result.v[0] = lhs.v[0] * rhs.v[0];
	result.v[1] = lhs.v[1] * rhs.v[1];
	result.v[2] = lhs.v[2] * rhs.v[2];
	result.v[3] = lhs.v[3] * rhs.v[3];
	return result;
}

vr::HmdVector3_t operator*(const vr::HmdVector3_t& lhs, const vr::HmdVector3_t& rhs) {
	vr::HmdVector3_t result = {};
	result.v[0] = lhs.v[0] * rhs.v[0];
	result.v[1] = lhs.v[1] * rhs.v[1];
	result.v[2] = lhs.v[2] * rhs.v[2];
	return result;
}

vr::HmdVector3d_t operator*(const vr::HmdVector3d_t& lhs, const vr::HmdVector3d_t& rhs) {
	vr::HmdVector3d_t result = {};
	result.v[0] = lhs.v[0] * rhs.v[0];
	result.v[1] = lhs.v[1] * rhs.v[1];
	result.v[2] = lhs.v[2] * rhs.v[2];
	return result;
}

double dot(vr::HmdVector4_t lhs, vr::HmdVector4_t rhs) {
	return 
		lhs.v[0] * rhs.v[0] +
		lhs.v[1] * rhs.v[1] +
		lhs.v[2] * rhs.v[2] +
		lhs.v[3] * rhs.v[3];
}

double dot(vr::HmdVector3_t lhs, vr::HmdVector3_t rhs){
	return
		lhs.v[0] * rhs.v[0] +
		lhs.v[1] * rhs.v[1] +
		lhs.v[2] * rhs.v[2];
}

double dot(vr::HmdVector3d_t lhs, vr::HmdVector3d_t rhs){
	return
		lhs.v[0] * rhs.v[0] +
		lhs.v[1] * rhs.v[1] +
		lhs.v[2] * rhs.v[2];
}

vr::HmdVector3_t operator*(const vr::HmdVector3_t& lhs, const vr::HmdQuaternion_t& rhs) {
	const vr::HmdQuaternion_t qVec = { 1.0, lhs.v[0], lhs.v[1], lhs.v[2] };
	const vr::HmdQuaternion_t qResult = rhs * qVec * -rhs;

	return { (float)qResult.x, (float)qResult.y, (float)qResult.z };
}

vr::HmdVector3d_t operator*(const vr::HmdVector3d_t& lhs, const vr::HmdQuaternion_t& rhs) {
	const vr::HmdQuaternion_t qVec = { 1.0, lhs.v[0], lhs.v[1], lhs.v[2] };
	const vr::HmdQuaternion_t qResult = rhs * qVec * -rhs;

	return { qResult.x, qResult.y, qResult.z };
}

vr::HmdVector3_t operator*(const vr::HmdVector3_t& lhs, const vr::HmdQuaternionf_t& rhs) {
	const vr::HmdQuaternion_t qVec = { 1.0, lhs.v[0], lhs.v[1], lhs.v[2] };
	const vr::HmdQuaternionf_t qResult = rhs * qVec * -rhs;

	return { (float)qResult.x, (float)qResult.y, (float)qResult.z };
}
vr::HmdVector3d_t operator*(const vr::HmdVector3d_t& lhs, const vr::HmdQuaternionf_t& rhs) {
	const vr::HmdQuaternion_t qVec = { 1.0, lhs.v[0], lhs.v[1], lhs.v[2] };
	const vr::HmdQuaternionf_t qResult = rhs * qVec * -rhs;

	return { qResult.x, qResult.y, qResult.z };
}

vr::HmdQuaternion_t operator-(const vr::HmdQuaternion_t& q) {
	return { q.w, -q.x, -q.y, -q.z };
}

vr::HmdQuaternionf_t operator-(const vr::HmdQuaternionf_t& q) {
	return { q.w, -q.x, -q.y, -q.z };
}

vr::HmdQuaternion_t operator*(const vr::HmdQuaternion_t& q, const vr::HmdQuaternion_t& r) {
	vr::HmdQuaternion_t result{};

	result.w = r.w * q.w - r.x * q.x - r.y * q.y - r.z * q.z;
	result.x = r.w * q.x + r.x * q.w - r.y * q.z + r.z * q.y;
	result.y = r.w * q.y + r.x * q.z + r.y * q.w - r.z * q.x;
	result.z = r.w * q.z - r.x * q.y + r.y * q.x + r.z * q.w;

	return result;
}

vr::HmdQuaternionf_t operator*(const vr::HmdQuaternionf_t& q, const vr::HmdQuaternion_t& r) {
	vr::HmdQuaternionf_t result{};

	result.w = r.w * q.w - r.x * q.x - r.y * q.y - r.z * q.z;
	result.x = r.w * q.x + r.x * q.w - r.y * q.z + r.z * q.y;
	result.y = r.w * q.y + r.x * q.z + r.y * q.w - r.z * q.x;
	result.z = r.w * q.z - r.x * q.y + r.y * q.x + r.z * q.w;

	return result;
}

vr::HmdQuaternionf_t operator*(const vr::HmdQuaternion_t& q, const vr::HmdQuaternionf_t& r) {
	vr::HmdQuaternionf_t result{};

	result.w = r.w * q.w - r.x * q.x - r.y * q.y - r.z * q.z;
	result.x = r.w * q.x + r.x * q.w - r.y * q.z + r.z * q.y;
	result.y = r.w * q.y + r.x * q.z + r.y * q.w - r.z * q.x;
	result.z = r.w * q.z - r.x * q.y + r.y * q.x + r.z * q.w;

	return result;
}

vr::HmdQuaternionf_t operator*(const vr::HmdQuaternionf_t& q, const vr::HmdQuaternionf_t& r) {
	vr::HmdQuaternionf_t result{};

	result.w = r.w * q.w - r.x * q.x - r.y * q.y - r.z * q.z;
	result.x = r.w * q.x + r.x * q.w - r.y * q.z + r.z * q.y;
	result.y = r.w * q.y + r.x * q.z + r.y * q.w - r.z * q.x;
	result.z = r.w * q.z - r.x * q.y + r.y * q.x + r.z * q.w;

	return result;
}


