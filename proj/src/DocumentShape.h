#ifndef DOCUMENTSHAPE_H
#define DOCUMENTSHAPE_H
#include <QPolygonF>

#include "Vertex.h"
#include "Triangle.h"

class Document;

class DocumentShape
{
public:
	Document*			doc;
	QVector<Triangle>	triangles;

	VertexList			vertexList;

	QPolygonF			backgroundPolygon;
public:
	DocumentShape(Document* d);
	~DocumentShape();
public:
	QRect bound() const;
	void translate(const QPoint& p);
	void close();
	bool isClosed() const
	{
		return vertexList.isClosed();
	}
	void	paint(QPainter& p, DocumentShape* selectedShape, Vertex* selectedVertex, bool drawMesh, bool showLines, bool showVertexDots);
	Vertex*	addVertex(int mouseX, int mouseY);
	Vertex*	getVertexAt(int mouseX, int mouseY);
	void	moveVertex(Vertex* v, int mouseX, int mouseY);
	void	removeVertex(Vertex* v);

private:
	void rebuild();
	void generateGrassMesh(QVector<Vertex*>& grassVertexs);
};

#endif // DOCUMENTSHAPE_H
