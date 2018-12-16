#ifndef VECTOR2D_H
#define VECTOR2D_H
#include <math.h>
#include <QPoint>

class Vector2d
{
public:
	double x;
	double y;
public:
	Vector2d() : x(0), y(0)
	{
	}
	Vector2d(double px, double py) : x(px), y(py)
	{
	}
	Vector2d(const Vector2d& other) : x(other.x), y(other.y)
	{
	}
public:
	Vector2d& operator = (const Vector2d& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}
	double distanceOf(const Vector2d& o);

	static Vector2d subtract(const Vector2d& a, const Vector2d& b)
	{
		return Vector2d(a.x-b.x, a.y-b.y);
	}
	double angle()
	{
		return atan2(y, x);
	}
	double length()
	{
		return sqrt(x*x + y*y);
	}
	Vector2d getScaled(double f)
	{
		return Vector2d(x*f, y*f);
	}
	Vector2d getNormalized()
	{
		double n = length();
		return Vector2d(x/n, y/n);
	}
	Vector2d getPerpendicular()
	{
		return Vector2d(-y, x);
	}
	Vector2d getAddition(const Vector2d& v)
	{
		return Vector2d(x+v.x, y+v.y);
	}
	Vector2d getSubtraction(const Vector2d& v)
	{
		return Vector2d(x-v.x, y-v.y);
	}
	QPoint toPoint()
	{
		return QPoint(x, y);
	}
};

#endif // VECTOR2D_H
