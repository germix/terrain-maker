#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QVector>
#include <QPolygon>
#include <QAbstractScrollArea>

#include "Vertex.h"

class Tool;
class Triangle;
class Document;
class DocumentShape;

class Canvas : public QAbstractScrollArea
{
	Q_OBJECT
	Document*			doc;

	Tool*				tool;
	bool				showMesh;
	bool				showLines;
	bool				showVertexDots;
	DocumentShape*		selectedShape;
	Vertex*				selectedVertex;

	QRect				paperRect;

	bool				dragging;
	QPoint				deltaSize;

	QColor				backgroundColor;
public:
	explicit Canvas(QWidget* parent = nullptr);
	~Canvas();
signals:
	void onShapeSelected(DocumentShape* s);
public:
	void setTool(Tool *t);
	void setDocument(Document* d);
	DocumentShape* getSelectedShape() const
	{
		return selectedShape;
	}
	void setSelectedShape(DocumentShape* shape)
	{
		selectedShape = shape;
		selectedVertex = nullptr;
		viewport()->update();
		emit onShapeSelected(selectedShape);
	}
	void setSelectedVertex(Vertex* vertex)
	{
		selectedVertex = vertex;
		viewport()->update();
	}
	QPoint toCanvas(const QPoint& pt);
	void terrainUpLevel();
	void terrainDownLevel();
	void terrainSendToBack();
	void terrainBringToFront();
private:
	void reset();
	void calcPaperRect();
	void paintEvent(QPaintEvent* e);
	void resizeEvent(QResizeEvent* e);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
private slots:
	void slotDocument_sizeChanged(int w, int h);
	void slotScrollBar_valueChanged(int value);
};

#endif // CANVAS_H
