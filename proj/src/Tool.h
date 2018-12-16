#ifndef TOOL_H
#define TOOL_H
#include <QObject>
#include <QIcon>
#include <QString>

class Canvas;
class Document;

class QPainter;
class QKeyEvent;
class QMouseEvent;

class Tool : public QObject
{
	Q_OBJECT
public:
	Tool();
	virtual ~Tool();
public:
	virtual QIcon getIcon() const = 0;
	virtual QString getName() const = 0;
public:
	virtual void onActivate(Canvas* canvas, Document* doc) { Q_UNUSED(canvas); Q_UNUSED(doc); }
	virtual void onDeactivate(Canvas* canvas, Document* doc) { Q_UNUSED(canvas); Q_UNUSED(doc); }
public:
	virtual void onPaint(QPainter& p) { Q_UNUSED(p); }

	virtual void onKeyPress(QKeyEvent* e) { Q_UNUSED(e); }
	virtual void onKeyRelease(QKeyEvent* e) { Q_UNUSED(e); }

	virtual void onMouseMove(QMouseEvent* e) { Q_UNUSED(e); }
	virtual void onMousePress(QMouseEvent* e) { Q_UNUSED(e); }
	virtual void onMouseRelease(QMouseEvent* e) { Q_UNUSED(e); }
};

#endif // TOOL_H
