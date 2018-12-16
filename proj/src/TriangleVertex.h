#ifndef TRIANGLEVERTEX_H
#define TRIANGLEVERTEX_H
#include "Vector2d.h"

class TriangleVertex
{
public:
	double x;
	double y;
	double u;
	double v;
public:
	TriangleVertex();
	/*
	TriangleVertex(const Vector2d&  p)
	{
		this->x = p.x;
		this->y = p.y;
		this->u = 0;
		this->v = 0;
	}
	*/
	TriangleVertex(double x, double y, double u = 0, double v = 0);

	TriangleVertex(const Vector2d& p, double u = 0, double v = 0)
	{
		x = p.x;
		y = p.y;
		this->u = u;
		this->v = v;
	}
	~TriangleVertex();

};

#endif // TRIANGLEVERTEX_H
