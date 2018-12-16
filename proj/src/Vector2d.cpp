#include "Vector2d.h"
#include <math.h>

double Vector2d::distanceOf(const Vector2d& o)
{
	double dx = o.x-x;
	double dy = o.y-y;
	double dist = ::sqrt(dx*dx + dy*dy);

	return dist;
}
