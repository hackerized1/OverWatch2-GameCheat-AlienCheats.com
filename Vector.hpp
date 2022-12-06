#pragma once
#include "includes.hpp"

namespace JIN {
	class Vector2 {
	public:
		float X, Y;

		Vector2() { X = 0; Y = 0; }

		Vector2(float _X, float _Y)
		{
			X = _X;
			Y = _Y;
		}

		inline float Distance(Vector2 v)
		{
			return float(sqrtf(powf(v.X - X, 2.0) + powf(v.Y - Y, 2.0)));
		}

		inline float get_length()
		{
			return float(sqrtf(powf(X, 2.0) + powf(Y, 2.0)));
		}

		inline Vector2 operator+(Vector2 v)
		{
			return { X + v.X, Y + v.Y };
		}

		inline Vector2 operator-(Vector2 v)
		{
			return { X - v.X, Y - v.Y };
		}
	};
#pragma pack(push, 1)
	struct Vector3
	{
	public:
		float X;
		float Y;
		float Z;

		inline Vector3()
			: X(0), Y(0), Z(0) {
		}

		inline Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

		__forceinline Vector3 operator-(const Vector3& V) {
			return Vector3(X - V.X, Y - V.Y, Z - V.Z);
		}

		__forceinline Vector3 operator+(const Vector3& V) {
			return Vector3(X + V.X, Y + V.Y, Z + V.Z);
		}

		__forceinline Vector3 operator*(float Scale) const {
			return Vector3(X * Scale, Y * Scale, Z * Scale);
		}

		__forceinline Vector3 operator/(float Scale) const {
			const float RScale = 1.f / Scale;
			return Vector3(X * RScale, Y * RScale, Z * RScale);
		}

		__forceinline Vector3 operator+(float A) const {
			return Vector3(X + A, Y + A, Z + A);
		}

		__forceinline Vector3 operator-(float A) const {
			return Vector3(X - A, Y - A, Z - A);
		}

		__forceinline Vector3 operator*(const Vector3& V) const {
			return Vector3(X * V.X, Y * V.Y, Z * V.Z);
		}

		__forceinline Vector3 operator/(const Vector3& V) const {
			return Vector3(X / V.X, Y / V.Y, Z / V.Z);
		}

		__forceinline float operator|(const Vector3& V) const {
			return X * V.X + Y * V.Y + Z * V.Z;
		}

		__forceinline float operator^(const Vector3& V) const {
			return X * V.Y - Y * V.X - Z * V.Z;
		}

		__forceinline Vector3& operator+=(const Vector3& v) {
			X += v.X;
			Y += v.Y;
			Z += v.Z;
			return *this;
		}

		__forceinline Vector3& operator-=(const Vector3& v) {
			X -= v.X;
			Y -= v.Y;
			Z -= v.Z;
			return *this;
		}

		__forceinline Vector3& operator*=(const Vector3& v) {
			X *= v.X;
			Y *= v.Y;
			Z *= v.Z;
			return *this;
		}

		__forceinline Vector3& operator/=(const Vector3& v) {
			X /= v.X;
			Y /= v.Y;
			Z /= v.Z;
			return *this;
		}

		__forceinline bool operator==(const Vector3& src) const {
			return (src.X == X) && (src.Y == Y) && (src.Z == Z);
		}

		__forceinline bool operator!=(const Vector3& src) const {
			return (src.X != X) || (src.Y != Y) || (src.Z != Z);
		}

		__forceinline Vector3 Rotate(float angle)
		{
			return Vector3(X * cos(-angle) - Z * sin(-angle), Y, X * sin(-angle) + Z * cos(-angle));
		}

		__forceinline float Size() const {
			return sqrt(X * X + Y * Y + Z * Z);
		}

		__forceinline float DistTo(Vector3 targetTo) const {
			return (targetTo - *this).Size();
		}

		__forceinline Vector3 toRotator(Vector3 targetTo)
		{
			Vector3 Normalized = (targetTo - *this);
			return Normalized * (1 / Normalized.Size());
		}
	};
#pragma pack(pop)
	struct espBone {
		bool boneerror = false;
		Vector2 upL, upR, downL, downR;

		Vector2 head, neck, body_1, body_2, l_1, l_2, r_1, r_2, l_d_1, l_d_2, r_d_1, r_d_2, l_a_1, l_a_2, r_a_1, r_a_2;
	};
	struct Matrix
	{
		float m11, m12, m13, m14,
			m21, m22, m23, m24,
			m31, m32, m33, m34,
			m41, m42, m43, m44;

		Vector2 WorldToScreen(Vector3 worldPos, int width, int height)
		{
			float screenX = (m11 * worldPos.X) + (m21 * worldPos.Y) + (m31 * worldPos.Z) + m41;
			float screenY = (m12 * worldPos.X) + (m22 * worldPos.Y) + (m32 * worldPos.Z) + m42;
			float screenW = (m14 * worldPos.X) + (m24 * worldPos.Y) + (m34 * worldPos.Z) + m44;

			float camX = width / 2.0f;
			float camY = height / 2.0f;

			float x = camX + (camX * screenX / screenW);
			float y = camY - (camY * screenY / screenW);

			return (screenW > 0.001f) ? Vector2(x, y) : Vector2(-1, -1);
		}

		bool isWorldToScreen(Vector3 worldPos, int width, int height, Vector2& OutPos) {
			float screenX = (m11 * worldPos.X) + (m21 * worldPos.Y) + (m31 * worldPos.Z) + m41;
			float screenY = (m12 * worldPos.X) + (m22 * worldPos.Y) + (m32 * worldPos.Z) + m42;
			float screenW = (m14 * worldPos.X) + (m24 * worldPos.Y) + (m34 * worldPos.Z) + m44;

			float camX = width / 2.0f;
			float camY = height / 2.0f;

			float x = camX + (camX * screenX / screenW);
			float y = camY - (camY * screenY / screenW);

			if (x <= 0 || y <= 0 || x >= width || y >= height) { return false; }

			OutPos = { x, y };

			return (screenW > 0.001f) ? true : false;
		}

		Vector3 GetCameraVec()
		{
			float	A = m22 * m33 - m32 * m23,
				B = m32 * m13 - m12 * m33,
				C = m12 * m23 - m22 * m13,
				Z = m11 * A + m21 * B + m31 * C;

			if (abs(Z) < 0.0001) return Vector3();

			float	D = m31 * m23 - m21 * m33,
				E = m11 * m33 - m31 * m13,
				F = m21 * m13 - m11 * m23,
				G = m21 * m32 - m31 * m22,
				H = m31 * m12 - m11 * m32,
				K = m11 * m22 - m21 * m12;

			return Vector3(-(A * m41 + D * m42 + G * m43) / Z, -(B * m41 + E * m42 + H * m43) / Z, -(C * m41 + F * m42 + K * m43) / Z);
		}
	};
}