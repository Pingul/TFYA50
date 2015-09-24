#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <ostream>

class Vector3
{
	public:
		Vector3() = default;
		Vector3(double xp, double yp, double zp) 
			: x{xp}, y{yp}, z{zp} {}
		~Vector3() = default;
		
		double x{0.0};
		double y{0.0};
		double z{0.0};
};

Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
Vector3 operator*(double a, const Vector3&);
Vector3 operator*(const Vector3&, double a);
Vector3 operator/(const Vector3&, double a);
double operator*(const Vector3& lhs, const Vector3& rhs); // dot product

std::ostream& operator<<(std::ostream&, const Vector3&);

#endif