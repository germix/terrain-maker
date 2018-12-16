#ifndef TOOLSELECT_H
#define TOOLSELECT_H
#include <QTimer>

#include "Tool.h"

class DocumentShape;

class ToolSelect : public Tool
{
	Q_OBJECT
	Document*		doc;
	QTimer			timer;
	Canvas*			canvas;
	int				dashOffset;
	QPoint			dragPoint;
	DocumentShape*	selectedShape;
public:
	ToolSelect();
	~ToolSelect();
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
	void slotTimer_timeout();
};

#endif // TOOLSELECT_H
