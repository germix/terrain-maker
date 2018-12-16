#include "Document.h"
#include "DocumentShape.h"
#include <float.h>
#include <QPainter>

extern double distanceOfLine(const Vector2d& v1, const Vector2d& v2, const Vector2d& p);
extern bool lineIntersection(const Vector2d& lineStart1, const Vector2d& lineDirection1, const Vector2d& lineStart2, const Vector2d& lineDirection2, Vector2d* result);
extern void drawTexturedTriangle(
			QPainter& gr,
			int w, int h,
			//int pitch,
			 double x0, double y0, double u0, double v0,
			 double x1, double y1, double u1, double v1,
			 double x2, double y2, double u2, double v2,
			 const QImage& image
			 );

DocumentShape::DocumentShape(Document* d) : doc(d)
{
}
DocumentShape::~DocumentShape()
{
}

QRect DocumentShape::bound() const
{
	return backgroundPolygon.boundingRect().toRect();
}
void DocumentShape::translate(const QPoint& p)
{
	int i;
	Vertex* v;
	for(i = 0, v = vertexList.head; i < vertexList.size; i++, v = v->next)
	{
		v->pos.x += p.x();
		v->pos.y += p.y();
	}
	rebuild();
	doc->setModified(true);
}
void DocumentShape::close()
{
	vertexList.close();
	rebuild();
}
void	DocumentShape::paint(QPainter& p, DocumentShape* selectedShape, Vertex* selectedVertex, bool drawMesh, bool showLines, bool showVertexDots)
{
	//
	// Dibujar terreno
	//
	if(vertexList.isClosed())
	{
		p.setPen(Qt::NoPen);
//		p.setBrush(QBrush(doc->images[IMAGE_TARRAIN]));

		QBrush br(doc->images[IMAGE_TARRAIN]);
		br.setTransform(QTransform().translate(backgroundPolygon.boundingRect().x(), backgroundPolygon.boundingRect().y()));
		p.setBrush(br);
		p.drawPolygon(backgroundPolygon);
	}

	//
	// Dibujar triangulos
	//
	for(int i = 0; i < triangles.size(); i++)
	{
		Triangle& t = triangles[i];

		if(!t.image.isNull())
		{
			drawTexturedTriangle(
				p,
				640,
				480,
				t.v1.x, t.v1.y, t.v1.u, t.v1.v,
				t.v2.x, t.v2.y, t.v2.u, t.v2.v,
				t.v3.x, t.v3.y, t.v3.u, t.v3.v,
				t.image);
		}
		else
		{
			p.setPen(t.color);
			p.setBrush(QBrush());
			QPolygonF poly;
			poly.append(QPoint(t.v1.x, t.v1.y));
			poly.append(QPoint(t.v2.x, t.v2.y));
			poly.append(QPoint(t.v3.x, t.v3.y));
			p.drawPolygon(poly);
		}
		if(drawMesh)
		{
			p.setPen(t.color);
			p.setBrush(QBrush());
			QPolygonF poly;
			poly.append(QPoint(t.v1.x, t.v1.y));
			poly.append(QPoint(t.v2.x, t.v2.y));
			poly.append(QPoint(t.v3.x, t.v3.y));
			p.drawPolygon(poly);
		}
	}
	if(!vertexList.isEmpty())
	{
		Vertex* v;
		Vertex* next;

		//
		// Dibujar lineas
		//
		if(showLines && vertexList.size > 1)
		{
			p.setPen(Qt::black);
			v = vertexList.head;
			next = vertexList.head->next;
			while(next != NULL && next != vertexList.head)
			{
				p.drawLine(QPointF(v->pos.x, v->pos.y), QPointF(next->pos.x, next->pos.y));
				v = next;
				next = v->next;
			}
			if(vertexList.isClosed())
			{
				p.drawLine(QPointF(vertexList.head->pos.x, vertexList.head->pos.y), QPointF(vertexList.tail->pos.x, vertexList.tail->pos.y));
			}
		}
		//
		// Dibujar puntos
		//
		if(selectedShape == this)
		{
			p.setBrush(Qt::black);
			v = vertexList.head;

			do
			{
				//if(v == selectedVertex)
				{
					if(selectedVertex == v)
						p.setBrush(Qt::red);
					else
						p.setBrush(Qt::black);
					p.drawEllipse(QPoint(v->pos.x, v->pos.y), VERTEX_RADIUS, VERTEX_RADIUS);
					if(showVertexDots)
					{
						p.setBrush(Qt::magenta);
						p.drawEllipse(v->top.toPoint(), 4, 4);
						p.setBrush(Qt::green);
						p.drawEllipse(v->bottom.toPoint(), 4, 4);
					}
				}
				v = v->next;
			}
			while(v != nullptr && v != vertexList.head);
		}
	}
}
Vertex* DocumentShape::addVertex(int mouseX, int mouseY)
{
	if(!vertexList.isClosed())
	{
		return vertexList.append(mouseX, mouseY);
	}
	Vertex* v = vertexList.head;
	Vertex* next = vertexList.head->next;
	Vector2d click(mouseX, mouseY);
	double dist;
	double minDistance = DBL_MAX;
	Vertex* selectedVertex = nullptr;

	while(next != NULL && next != vertexList.head)
	{
		dist = distanceOfLine(v->pos, next->pos, click);
		if(dist < minDistance)
		{
			minDistance = dist;
			selectedVertex = v;
		}
		v = next;
		next = v->next;
	}
	// Última línea, entre el head y el tail
	dist = distanceOfLine(vertexList.head->pos, vertexList.tail->pos, click);
	if(dist < minDistance)
	{
		minDistance = dist;
		selectedVertex = vertexList.tail;
	}
	if(selectedVertex != NULL)
	{
		doc->setModified(true);
		return vertexList.append(mouseX, mouseY, selectedVertex);
	}
	return NULL;
}
Vertex* DocumentShape::getVertexAt(int mouseX, int mouseY)
{
	return vertexList.getVertexAt(mouseX, mouseY);
}
void DocumentShape::moveVertex(Vertex* v, int mouseX, int mouseY)
{
	v->pos.x = mouseX;
	v->pos.y = mouseY;
	rebuild();
	doc->setModified(true);
}
void DocumentShape::removeVertex(Vertex* v)
{
	vertexList.remove(v);
	rebuild();
	doc->setModified(true);
}
void DocumentShape::rebuild()
{
	if(vertexList.isClosed())
	{
		//
		// Calcular background
		//
		{
			int i;
			Vertex* v;
			backgroundPolygon.clear();
			for(i = 0, v = vertexList.head; i < vertexList.size; i++, v = v->next)
			{
				backgroundPolygon.append(QPointF(v->pos.x, v->pos.y));
			}
		}
		//
		// Recorrer todos los puntos
		//
		{
			QVector<Vertex*> list = vertexList.getClosedList();

			int i;
			Vertex* cp = list.at(0);
			Vertex* pp = cp->prev;
			Vector2d pVec = Vector2d::subtract(cp->pos, pp->pos);
			Vector2d pTop = pVec.getPerpendicular().getNormalized().getScaled(-15);
			Vector2d pBottom = pVec.getPerpendicular().getNormalized().getScaled(+25);

			pp->angle = pVec.angle();
			pp->recomputeType();
			pp = cp;
			for(i = 1; i < list.size(); i++)
			{
				cp = list.at(i);
				Vector2d cVec = Vector2d::subtract(cp->pos, pp->pos);			// Vector desde el anterior al actual
				Vector2d cTop = cVec.getPerpendicular().getNormalized().getScaled(-15);
				Vector2d cBottom = cVec.getPerpendicular().getNormalized().getScaled(+25);

				// ...
				{
					Vector2d iTop;
					Vector2d iBottom;

					if(!lineIntersection(pTop, pVec, cTop, cVec, &iTop))
						iTop = cTop;
					if(!lineIntersection(pBottom, pVec, cBottom, cVec, &iBottom))
						iBottom = cBottom;

					pp->angle = cVec.angle();
					pp->recomputeType();
					pp->top = pp->pos.getAddition(iTop);
					pp->bottom = pp->pos.getAddition(iBottom);
				}
				// ...
				pp = cp;
				pVec = cVec;
				pTop = cTop;
				pBottom = cBottom;
			}
		}
		triangles.clear();
		//
		// Construir malla de triángulos generica
		//
		if(0)
		{
			if(vertexList.isClosed())
			{
				QVector<Vertex*> list = vertexList.getOpenList();

				for(int i = 0; i < list.size(); i++)
				{
					Vertex* v = list.at(i);

					QVector<Vector2d> points;
					if(v->next != nullptr)
					{
						Vector2d vTopLeft = v->top;
						Vector2d vBottomLeft = v->bottom;
						Vector2d vTopRight = v->next->top;
						Vector2d vBottomRight = v->next->bottom;

						Vector2d top = (Vector2d::subtract(vTopRight, vTopLeft));
						Vector2d bottom = (Vector2d::subtract(vBottomRight, vBottomLeft));

						points.append(Vector2d(vTopLeft.x, vTopLeft.y));
						points.append(Vector2d(vBottomLeft.x, vBottomLeft.y));
						{
							int subdivisions = 4;
							double topSubdivisionsSize = top.length()/(double)subdivisions;
							double bottomSubdivisionsSize = bottom.length()/(double)subdivisions;

							for(int a = 0; a < subdivisions; a++)
							{
								Vector2d topDir = top.getNormalized().getScaled(topSubdivisionsSize*a);
								Vector2d bottomDir = bottom.getNormalized().getScaled(bottomSubdivisionsSize*a);

								points.append(Vector2d(vTopLeft.x+topDir.x, vTopLeft.y+topDir.y));
								points.append(Vector2d(vBottomLeft.x+bottomDir.x, vBottomLeft.y+bottomDir.y));
							}
						}
						points.append(Vector2d(vTopRight.x, vTopRight.y));
						points.append(Vector2d(vBottomRight.x, vBottomRight.y));
						QColor color = Qt::black;
						switch(v->type)
						{
							case Vertex::TYPE_GRASS:	color = Qt::green;		break;
							case Vertex::TYPE_LEFT:		color = Qt::blue;		break;
							case Vertex::TYPE_RIGHT:	color = Qt::cyan;		break;
						}
						//
						// Generar triángulos
						//
						Vector2d v1 = points.at(0);
						Vector2d v2 = points.at(1);
						for(int i = 2; i < points.size(); i++)
						{
							Vector2d v3 = points.at(i);

							Triangle tri(v1, v2, v3, color);
							triangles.append(tri);
							v1 = v2;
							v2 = v3;
						}
					}
				}
			}
		}
		//
		// Construir mallas para 'GRASS'
		//
		{
			int i = 0;
			QVector<Vertex*> list = vertexList.getOpenList();

			while(i < list.size())
			{
				Vertex* v = list.at(i);

				if(v->type == Vertex::TYPE_GRASS)
				{
					QVector<Vertex*> grassVertexs;

					//
					// Agregar GRASS
					//
					grassVertexs.append(v);

					//
					// Buscar cantidad de nodos GRASS contiguos
					//
					for(int j = i+1; j < list.size(); j++)
					{
						Vertex* n = list.at(j);
						grassVertexs.append(n);
						if(n->type != Vertex::TYPE_GRASS)
							break;
					}
					//
					// Generar malla
					//
					generateGrassMesh(grassVertexs);

					//
					// Saltar los vértices procesados
					//
					i += grassVertexs.size();
				}
				else
				{
					i++;
				}
			}
		}
	}
	else
	{
		//
		// Calcular ángulos
		//
		{
			Vertex* prev = vertexList.head;
			Vertex* curr = vertexList.head->next;
			while(curr != nullptr)
			{
				double dx = curr->pos.x-prev->pos.x;
				double dy = curr->pos.y-prev->pos.y;
				prev->angle = atan2(dy, dx);

				prev = curr;
				curr = curr->next;
				if(prev == vertexList.head)
					break;
			}
		}
	}
}
void DocumentShape::generateGrassMesh(QVector<Vertex*>& grassVertexs)
{
	double fullLength = 0;
	QImage& grassLeft = doc->images[0];
	QImage& grassMiddle = doc->images[1];
	QImage& grassRight = doc->images[2];

	//
	// Calcular longitud
	//
	{
		Vertex* cv = nullptr;
		Vertex* pv = grassVertexs.at(0);
		for(int i = 1; i < grassVertexs.size(); i++, pv = cv)
		{
			cv = grassVertexs.at(i);
			fullLength += cv->pos.getSubtraction(pv->pos).length();
		}
	}
	//
	// ...
	//
	double quadLength = 0;
	{
		double a = qMax(1.0, floor(fullLength/grassMiddle.width()));
		double b = fullLength/a;

		quadLength = b;
	}

	double lastX = 0;
	Vertex* cv = nullptr;
	Vertex* pv = grassVertexs.at(0);

	if(vertexList.isClosed())
	{
		TriangleVertex v1;
		TriangleVertex v2;
		TriangleVertex v3;

		double leftCap = grassLeft.width()/(double)grassMiddle.width();
		double rightCap = grassRight.width()/(double)grassMiddle.width();
		//
		// Agregar secciones intermedias
		//
		for(int i = 1; i < grassVertexs.size(); i++, pv = cv)
		{
			cv = grassVertexs.at(i);

			double d = 0;
			double length = cv->pos.getSubtraction(pv->pos).length();

			Vector2d vTopLeft = pv->top;
			Vector2d vBottomLeft = pv->bottom;
			Vector2d vTopRight = cv->top;
			Vector2d vBottomRight = cv->bottom;

			if(i == 1 || i == grassVertexs.size()-1)
			{
				Vector2d cVec = Vector2d::subtract(cv->pos, pv->pos);			// Vector desde el anterior al actual
				Vector2d cTop = cVec.getPerpendicular().getNormalized().getScaled(-25);
				Vector2d cBottom = cVec.getPerpendicular().getNormalized().getScaled(+15);
				if(i == 1)
				{
					vTopLeft = pv->pos.getAddition(cTop);
					vBottomLeft = pv->pos.getAddition(cBottom);

					//
					// Agregar sección inicial
					//
					Vector2d left = cVec.getNormalized().getScaled(-grassLeft.width());
					Vector2d p1 = vTopLeft.getAddition(left);
					Vector2d p2 = vBottomLeft.getAddition(left);

					v1 = TriangleVertex(p1, 0, 0);
					v2 = TriangleVertex(p2, 0, 1);
					v3 = TriangleVertex(vTopLeft, 1, 0);

					triangles.append(Triangle(v1, v2, v3, grassLeft, Qt::cyan));

					v1 = TriangleVertex(p2, 0, 1);
					v2 = TriangleVertex(vTopLeft, 1, 0);
					v3 = TriangleVertex(vBottomLeft, 1, 1);
					triangles.append(Triangle(v1, v2, v3, grassLeft, Qt::cyan));
				}
				if(i == grassVertexs.size()-1)
				{
					vTopRight = cv->pos.getAddition(cTop);
					vBottomRight = cv->pos.getAddition(cBottom);

					//
					// Agregar sección final
					//
					Vector2d right = cVec.getNormalized().getScaled(+grassRight.width());
					Vector2d p1 = vTopRight.getAddition(right);
					Vector2d p2 = vBottomRight.getAddition(right);

					v1 = TriangleVertex(vTopRight);
					v2 = TriangleVertex(vBottomRight, 0, 1);
					v3 = TriangleVertex(p1, 1, 0);
					triangles.append(Triangle(v1, v2, v3, grassRight, Qt::cyan));
					v1 = TriangleVertex(vBottomRight, 0, 1);
					v2 = TriangleVertex(p1, 1, 0);
					v3 = TriangleVertex(p2, 1, 1);
					triangles.append(Triangle(v1, v2, v3, grassRight, Qt::cyan));
				}
			}

			Vector2d top = vTopLeft;
			Vector2d bottom = vBottomLeft;

			Vector2d topVec = (Vector2d::subtract(vTopRight, vTopLeft));
			Vector2d bottomVec = (Vector2d::subtract(vBottomRight, vBottomLeft));
			Vector2d topDir = (Vector2d::subtract(vTopRight, vTopLeft)).getNormalized();
			Vector2d bottomDir = (Vector2d::subtract(vBottomRight, vBottomLeft)).getNormalized();

			//if(v1 == null)
			{
				v1 = TriangleVertex(vTopLeft, lastX/quadLength, 0);
				v2 = TriangleVertex(vBottomLeft, lastX/quadLength, 1);
			}
			while(d < length)
			{
				if(lastX >= quadLength)
					lastX = 0;
				double l = qMin(length-d, quadLength-lastX);
				if(l<=0)
				{
					break;
				}
				top = top.getAddition(topDir.getScaled(l/length*topVec.length()));
				v1.u = leftCap + lastX/quadLength;
				v1.v = 0;
				v2.u = leftCap + lastX/quadLength;
				v2.v = 1;
				v3 = TriangleVertex(top, leftCap + (lastX+l)/quadLength, 0);
				triangles.append(Triangle(v1, v2, v3, grassMiddle, Qt::magenta));
				v1 = v2;
				v2 = v3;

				bottom = bottom.getAddition(bottomDir.getScaled(l/length*bottomVec.length()));
				v1.u = leftCap + lastX/quadLength;
				v2.v = 1;
				v2.u = leftCap + (lastX+l)/quadLength;
				v2.v = 0;
				v3 = TriangleVertex(bottom, leftCap + (lastX+l)/quadLength, 1);
				triangles.append(Triangle(v1, v2, v3, grassMiddle, Qt::magenta));
				v1 = v2;
				v2 = v3;

				d += l;
				lastX += l;
			}
		}
	}
}



