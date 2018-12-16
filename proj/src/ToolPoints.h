#ifndef TOOLPOINTS_H
#define TOOLPOINTS_H
#include "Tool.h"

class Vertex;
class DocumentShape;

class ToolPoints : public Tool
{
	Q_OBJECT
	Document*		doc;
	Canvas*			canvas;

	DocumentShape*	selectedShape;
	Vertex*			draggedVertex;
	Vertex*			selectedVertex;

	QPoint			oldPosition;
public:
	ToolPoints();
	~ToolPoints();
public:
	virtual QIcon getIcon() const;
	virtual QString getName() const;

	virtual void onActivate(Canvas* canvas, Document* doc);
	virtual void onDeactivate(Canvas* canvas, Document* doc);

	virtual void onPaint(QPainter& p);
	virtual void onKeyPress(QKeyEvent* e);
	virtual void onMouseMove(QMouseEvent* e);
	virtual void onMousePress(QMouseEvent* e);
	virtual void onMouseRelease(QMouseEvent* e);
private slots:
};

#endif // TOOLPOINTS_H
