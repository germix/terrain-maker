#include "Triangle.h"

Triangle::Triangle()
{

}
Triangle::Triangle(const TriangleVertex& v1, const TriangleVertex& v2, const TriangleVertex& v3, const QImage& image, const QColor& color)
{
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
	this->image = image;
	this->color = color;
}
Triangle::~Triangle()
{
}

