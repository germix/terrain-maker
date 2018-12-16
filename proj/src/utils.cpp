#include <QPainter>
#include "Vector2d.h"

double toDegrees(double radians)
{
	return (radians * 180.0 / M_PI);
}
double toRadians(double degress)
{
	return (degress * M_PI / 180.0);
}
double distanceOfLine(const Vector2d& v1, const Vector2d& v2, const Vector2d& p)
{
	double dx = (v2.x - v1.x);
	double dy = (v2.y - v1.y);
	return qAbs((v2.y - v1.y)*p.x - (v2.x - v1.x)*p.y + v2.x*v1.y - v2.y*v1.x) / sqrt((dy*dy)+(dx*dx));
}
bool lineIntersection(const Vector2d& lineStart1, const Vector2d& lineDirection1, const Vector2d& lineStart2, const Vector2d& lineDirection2, Vector2d* result)
{
	double Ax = lineStart1.x;
	double Ay = lineStart1.y;
	double Bx = lineStart1.x+lineDirection1.x;
	double By = lineStart1.y+lineDirection1.y;

	double Cx = lineStart2.x;
	double Cy = lineStart2.y;
	double Dx = lineStart2.x+lineDirection2.x;
	double Dy = lineStart2.y+lineDirection2.y;

	double  distAB, theCos, theSin, newX, ABpos ;

	//  Fail if either line is undefined.
	if (Ax==Bx && Ay==By || Cx==Dx && Cy==Dy)
	{
		return false;
	}

	//  (1) Translate the system so that point A is on the origin.
	Bx-=Ax; By-=Ay;
	Cx-=Ax; Cy-=Ay;
	Dx-=Ax; Dy-=Ay;

	//  Discover the length of segment A-B.
	distAB=sqrt(Bx*Bx+By*By);

	//  (2) Rotate the system so that point B is on the positive X axis.
	theCos=Bx/distAB;
	theSin=By/distAB;
	newX=Cx*theCos+Cy*theSin;
	Cy  =Cy*theCos-Cx*theSin; Cx=newX;
	newX=Dx*theCos+Dy*theSin;
	Dy  =Dy*theCos-Dx*theSin; Dx=newX;

	//  Fail if the lines are parallel.
	if (Cy==Dy)
	{
		return false;
	}

	//  (3) Discover the position of the intersection point along line A-B.
	ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);

	//  (4) Apply the discovered position to line A-B in the original coordinate system.
	double iX = Ax+ABpos*theCos;
	double iY = Ay+ABpos*theSin;

	//  Success.
	*result = Vector2d(iX, iY);
	return true;
}
static void setPixel(QPainter& p, int w, int h, int pitch, int x, int y, int dwColour)
{
	int alpha = (dwColour >> 24) & 0xFF;
	if(alpha != 0)
	{
		int red = (dwColour >> 16) & 0xFF;
		int green = (dwColour >> 8) & 0xFF;
		int blue = dwColour & 0xFF;

		p.fillRect(x, y, 2, 2, QColor(red, green, blue, alpha));
	}
}
void drawTexturedTriangle(
			QPainter& gr,
			int w, int h,
			//int pitch,
			 double x0, double y0, double u0, double v0,
			 double x1, double y1, double u1, double v1,
			 double x2, double y2, double u2, double v2,
			 const QImage& image
			 )
{
	int pitch = -1;
//	int[] pixels = ((DataBufferInt) image.getRaster().getDataBuffer()).getData();
//	QRgb* pixels = (QRgb)image.bits();
	if(image.isNull())
		return;

	//
	// MultiSwap
	//
	// Sort our y values into order:
	// y0 < y2 < y1
	//
	{
		int itmp;
		float ftmp;

		// Sort our points into order of y
		// 0 top
		// 2 middle
		// 1 bottom
		if( y1 < y0 )
		{
			itmp = y1; y1 = y0; y0 = itmp;
			itmp = x1; x1 = x0; x0 = itmp;
			ftmp = u1; u1 = u0; u0 = ftmp;
			ftmp = v1; v1 = v0; v0 = ftmp;
		}
		if( y2 < y0 )
		{
			itmp = y2; y2 = y0; y0 = itmp;
			itmp = x2; x2 = x0; x0 = itmp;
			ftmp = u2; u2 = u0; u0 = ftmp;
			ftmp = v2; v2 = v0; v0 = ftmp;
		}
		if( y1 < y2 )
		{
			itmp = y2; y2 = y1; y1 = itmp;
			itmp = x2; x2 = x1; x1 = itmp;
			ftmp = u2; u2 = u1; u1 = ftmp;
			ftmp = v2; v2 = v1; v1 = ftmp;
		}
	}
	// Declare some variables that we'll use and where starting from y0 at the
	// top of the triangle
	float dxdy1    = (float)(x2-x0);
	float dxdu1    = (u2-u0);
	float dxdv1    = (v2-v0);

	float dxdy2    = (float)(x1-x0);
	float dxdu2    = (u1-u0);
	float dxdv2    = (v1-v0);

	float sdx,sdu,sdv;
	float edx,edu,edv;
	float pu, pv;
	int x, y;

	float dy1 = (float)(y2-y0);
	float dy2 = (float)(y1-y0);

	// Check for divide by zero's
	if( y2-y0 != 0 )
	{
		dxdy1 /= dy1;
		dxdu1 /= dy1;
		dxdv1 /= dy1;
	}

	if( y1-y0 != 0 )
	{
		dxdy2 /= dy2;
		dxdu2 /= dy2;
		dxdv2 /= dy2;
	}

	float dxldy;  float dxrdy;
	float dxldu;  float dxrdu;
	float dxldv;  float dxrdv;

	// Determine our left and right points for our x value gradient..
	// e.g. the starting and ending line for our x inner loop
	if( dxdy1 < dxdy2 )
	{
		dxldy = dxdy1;  dxrdy = dxdy2;
		dxldu = dxdu1;  dxrdu = dxdu2;
		dxldv = dxdv1;  dxrdv = dxdv2;
	}
	else
	{
		dxldy = dxdy2;  dxrdy = dxdy1;
		dxldu = dxdu2;  dxrdu = dxdu1;
		dxldv = dxdv2;  dxrdv = dxdv1;
	}

	// Initial starting x and ending x is sdx and edx which are x0,y0...the
	// top of our triangle
	sdx = (float)x0;  sdu=u0;  sdv=v0;
	edx = (float)x0;  edu=u0;  edv=v0;
	pu = u0; pv = v0;


	float p_delta_u;
	float p_delta_v;

	for( y=(int)y0; y<= (int)y2; y++ )
	{
		p_delta_u = (edu - sdu);
		p_delta_v = (edv - sdv);
		if( edx - sdx != 0 )
		{
			p_delta_u /= (float)(edx - sdx);
			p_delta_v /= (float)(edx - sdx);
		}
		pu=sdu;  pv=sdv;

		for( x=(int)sdx; x<=(int)edx; x++ )
		{
			int iwidth  = image.width();
			int iheight = image.height();

			int ww = (int)(pu*(iwidth-1));
			int hh = (int)(pv*(iheight-1));

			/*
			*/
#if 0
			if(ww >= 0 && ww < image.width() && hh >= 0 && hh < image.height())
			{
				setPixel(gr, w, h, pitch, x, y, image.pixel(ww, hh));
			}
#else
			int pos = ww + hh*(iwidth);
			if( pos > (iwidth-1)*(iheight-1)) pos=(iwidth-1)*(iheight-1);
			if( pos < 0 ) pos=0;
			setPixel(gr, w, h, pitch, x, y, ((QRgb*)image.bits())[pos]);
#endif
			pu += p_delta_u;
			pv += p_delta_v;

		}// End for loop x

		sdx += dxldy;  sdu+=dxldu;  sdv+=dxldv;
		edx += dxrdy;  edu+=dxrdu;  edv+=dxrdv;

	}// End for loop y

	///////////////Now for the bottom of the triangle////////////////////

	if( dxdy1 < dxdy2 )
	{
		dxldy = (float)(x1-x2);
		dxldu = (u1-u2);
		dxldv = (v1-v2);
		if(y1-y2 != 0)
		{
			dxldy /= (float)(y1-y2);
			dxldu /= (float)(y1-y2);
			dxldv /= (float)(y1-y2);
		}// End inner if
		sdx=(float)x2;  sdu=u2;   sdv=v2;
	}
	else
	{
		dxrdy = (float)(x1-x2);
		dxrdu = (u1-u2);
		dxrdv = (v1-v2);
		if(y1-y2 != 0)
		{
			dxrdy /= (float)(y1-y2);
			dxrdu /= (float)(y1-y2);
			dxrdv /= (float)(y1-y2);
		}// End inner if
		edx=(float)x2;  edu=u2;   edv=v2;
	}// End of else

	pu=u2;  pv=v2;


	for( y=(int)y2; y<= (int)y1; y++ )
	{
		p_delta_u = (edu - sdu);
		p_delta_v = (edv - sdv);
		if( edx - sdx != 0 )
		{
			p_delta_u /= (float)(edx - sdx);
			p_delta_v /= (float)(edx - sdx);
		}
		pu=sdu;  pv=sdv;

		for( x=(int)sdx; x<=(int)edx; x++ )
		{
			int iwidth  = image.width();
			int iheight = image.height();

			int ww = (int)(pu*(iwidth-1));
			int hh = (int)(pv*(iheight-1));

#if 0
			if(ww >= 0 && ww < image.width() && hh >= 0 && hh < image.height())
			{
				setPixel(gr, w, h, pitch, x, y, image.pixel(ww, hh));
			}
#else
			int pos = ww + hh*(iwidth);
			if( pos > (iwidth-1)*(iheight-1)) pos=(iwidth-1)*(iheight-1);
			if( pos < 0 ) pos=0;
			setPixel(gr, w, h, pitch, x, y, ((QRgb*)image.bits())[pos]);

#endif
			pu += p_delta_u;
			pv += p_delta_v;

		}// End for loop x

		sdx += dxldy;  sdu+=dxldu;  sdv+=dxldv;
		edx += dxrdy;  edu+=dxrdu;  edv+=dxrdv;

	}// End for loop y
}


