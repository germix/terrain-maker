#include "Canvas.h"
#include "Document.h"
#include "ToolSelect.h"
#include "DocumentShape.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>

ToolSelect::ToolSelect()
{
	dashOffset = 0;
	selectedShape = 0;

	timer.setInterval(50);
	connect(&timer, SIGNAL(timeout()), this, SLOT(slotTimer_timeout()));
}
ToolSelect::~ToolSelect()
{
}
QIcon ToolSelect::getIcon() const
{
	return QIcon(":/images/tool-arrow.png");
}
QString ToolSelect::getName() const
{
	return tr("Selection");
}
void ToolSelect::onActivate(Canvas* c, Document* d)
{
	doc = d;
	canvas = c;
	selectedShape = canvas->getSelectedShape();
	if(selectedShape)
		timer.start(100);
}
void ToolSelect::onDeactivate(Canvas* c, Document* d)
{
	Q_UNUSED(d);
	Q_UNUSED(c);

	doc = nullptr;
	canvas = nullptr;
	if(selectedShape)
	{
		timer.stop();
		selectedShape = NULL;
	}
}
void ToolSelect::onPaint(QPainter& p)
{
#if 0
	qDebug("ToolSelect::onPaint");
#endif
	DocumentShape* shape = selectedShape;
	if(NULL != shape)
	{
		QPen pen;
		QBrush brush;
		QVector<qreal> dashes;
		QRect rect = shape->bound().adjusted(0, 0, -1, -1);

		dashes << 4 << 4;
		pen.setDashPattern(dashes);
		p.setBrush(Qt::NoBrush);

		for(int i = 0; i < 2; i++)
		{
			if(i == 0)
			{
				pen.setColor(Qt::black);
				pen.setDashOffset(dashOffset);
			}
			else
			{
				pen.setColor(Qt::white);
				pen.setDashOffset(dashOffset+4);
			}
			p.setPen(pen);

			p.drawRect(rect);
		}
	}
}
void ToolSelect::onKeyPress(QKeyEvent* e)
{
	switch(e->key())
	{
		case Qt::Key_Delete:
			if(selectedShape)
			{
				if(QMessageBox::Yes == QMessageBox::question(
							nullptr,
							tr("Remove terrain"),
							tr("Are you sure to remove the selected terrain?"),
							QMessageBox::Yes | QMessageBox::No))
				{
					timer.stop();
					doc->shapes.removeOne(selectedShape);
					delete selectedShape;
					doc->setModified(true);
					canvas->setSelectedShape(selectedShape = nullptr);
				}
			}
			break;
	}
}
void ToolSelect::onMouseMove(QMouseEvent* e)
{
	QPoint pos = canvas->toCanvas(e->pos());
	if(selectedShape != NULL)
	{
		// Trasladar
		selectedShape->translate(pos - dragPoint);

		// Actualizar
		canvas->viewport()->update();
		dragPoint = pos;
	}
}
void ToolSelect::onMousePress(QMouseEvent* e)
{
	QPoint pos = canvas->toCanvas(e->pos());
	DocumentShape* shape;
	DocumentShape* oldSelectedShape = selectedShape;

	selectedShape = NULL;
	for(int i = doc->shapes.size()-1; i >= 0; i--)
	{
		shape = doc->shapes[i];
#if 0
		QRect bound = shape->bound();
		if(bound.contains(pos))
#else
		QPolygonF poly = shape->backgroundPolygon;
		if(poly.containsPoint(pos, Qt::OddEvenFill))
#endif
		{
			selectedShape = shape;

			dragPoint = pos;
			break;
		}
	}
	canvas->setSelectedShape(selectedShape);
	if(selectedShape != NULL)
	{
		timer.start();
	}
	else
	{
		timer.stop();
		if(oldSelectedShape)
			canvas->viewport()->update(oldSelectedShape->backgroundPolygon.boundingRect().toRect());
	}
}
void ToolSelect::onMouseRelease(QMouseEvent* e)
{
	Q_UNUSED(e);
	Q_UNUSED(doc);
	Q_UNUSED(canvas);
	dragPoint = QPoint();
}
void ToolSelect::slotTimer_timeout()
{
	dashOffset++;
	if(dashOffset == 8)
		dashOffset = 0;

	canvas->viewport()->update();
}

