#include "vector.h"
#include <iostream>
#include <exception>

Vector3 operator*(double a, const Vector3& vec)
{
	return Vector3{a*vec.x, a*vec.y, a*vec.z};
}

Vector3 operator*(const Vector3& vec, double a)
{
	return a*vec;
}

Vector3 operator/(const Vector3& vec, double a)
{
	if (a == 0.0)
		throw std::runtime_error{"division by zero"};
	return (1/a)*vec;
}

Vector3 operator+(const Vector3& lhs, const Vector3& rhs)
{
	Vector3 vec(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	return vec;
}

Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

double operator*(const Vector3& lhs, const Vector3& rhs) // dot product
{	
	return (lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z);
}

std::ostream& operator<<(std::ostream& os, const Vector3& vec)
{
	os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
	return os;
}

// int main()
// {
// 	Vector3 vec1 = {1.0, 2.0, 3.f};
// 	Vector3 vec2{1, 0, 10};
// 	Vector3 vec3{vec2};
// 	Vector3 vec4{vec1 + vec2};
// 	Vector3 vec5{vec3 - vec4};

// 	std::cout << "vec4 - vec3 == " << vec4 << " - " << vec3 << " : " << vec4 - vec3 << std::endl;
// 	std::cout << "vec4 + vec5 == " << vec4 << " + " << vec5 << " : " << vec4 + vec5 << std::endl;
// 	std::cout << "vec1*vec2 == " << vec1 << "*" << vec2 << " : " << vec1*vec2 << std::endl;
// 	std::cout << "10*vec1 == 10*" << vec1 << " : " << 10*vec1 << " == " << vec1*10 << std::endl;
// 	std::cout << "vec3/10 == " << vec3 << "/10 : " << vec3/10 << std::endl;
// }
