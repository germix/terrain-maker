#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <QImage>
#include "Vector2d.h"
#include "TriangleVertex.h"

class Triangle
{
public:
	TriangleVertex	v1;
	TriangleVertex	v2;
	TriangleVertex	v3;
	QColor			color;
	QImage			image;
public:
	Triangle();
	Triangle(const Triangle& t)
	{
		v1 = t.v1;
		v2 = t.v2;
		v3 = t.v3;
		color = t.color;
		image = t.image;
	}

	Triangle(const Vector2d& v1, const Vector2d& v2, const Vector2d& v3, const QColor& color)
	{
		this->v1 = TriangleVertex(v1);
		this->v2 = TriangleVertex(v2);
		this->v3 = TriangleVertex(v3);
		this->color = color;
	}
	Triangle(const TriangleVertex& v1, const TriangleVertex& v2, const TriangleVertex& v3, const QColor& color = Qt::black)
	{
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		this->color = color;
	}
	Triangle(const TriangleVertex& v1, const TriangleVertex& v2, const TriangleVertex& v3, const QImage& image, const QColor& color = Qt::black);
	~Triangle();
public:
	Triangle& operator = (const Triangle& t)
	{
		v1 = t.v1;
		v2 = t.v2;
		v3 = t.v3;
		color = t.color;
		image = t.image;
		return *this;
	}
};

#endif // TRIANGLE_H
