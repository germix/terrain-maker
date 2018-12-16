#include "Vertex.h"


extern double toDegrees(double radians);

Vertex::Vertex(const Vector2d &p) : pos(p), prev(0), next(0)
{
	color = Qt::black;
}
Vertex::~Vertex()
{
}
void Vertex::recomputeType()
{
	type = TYPE_UNDEFINED;
	double deg = toDegrees(angle);
	/*
	if(!normalAxisY)
	{
		if(deg <= 60 && -60 <= deg)
		{
			type = TYPE_GRASS;
		}
		else if(deg >= 60 && deg <= 120)
		{
			type = TYPE_LEFT;
		}
		else if(deg <= -60 && deg >= -120)
		{
			type = TYPE_RIGHT;
		}
	}
	else
	*/
	{
		if(deg <= 60 && -60 <= deg)
		{
			type = TYPE_GRASS;
		}
		else if(deg >= 60 && deg <= 120)
		{
			type = TYPE_RIGHT;
		}
		else if(deg <= -60 && deg >= -120)
		{
			type = TYPE_LEFT;
		}
	}
}
