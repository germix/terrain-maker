#include "ToolPoints.h"
#include "Canvas.h"
#include "Document.h"

#include <QMouseEvent>
#include <QMessageBox>

ToolPoints::ToolPoints()
{

}
ToolPoints::~ToolPoints()
{
}
QIcon ToolPoints::getIcon() const
{
	return QIcon(":/images/tool-point.png");
}
QString ToolPoints::getName() const
{
	return tr("Points");
}
void ToolPoints::onActivate(Canvas* c, Document* d)
{
	doc = d;
	canvas = c;
	selectedShape = canvas->getSelectedShape();
}
void ToolPoints::onDeactivate(Canvas* canvas, Document* doc)
{
	Q_UNUSED(doc);
	Q_UNUSED(canvas);
	selectedShape = nullptr;
	draggedVertex = nullptr;
	selectedVertex = nullptr;
}
void ToolPoints::onPaint(QPainter& p)
{
	Q_UNUSED(p);
	Q_UNUSED(doc);
	Q_UNUSED(canvas);
}
void ToolPoints::onKeyPress(QKeyEvent* e)
{
	switch(e->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			selectedShape->close();
			canvas->viewport()->update();
			break;
		case Qt::Key_Delete:
			if(selectedVertex != nullptr)
			{
				selectedShape->removeVertex(selectedVertex);
				selectedVertex = nullptr;
				canvas->setSelectedVertex(nullptr);
			}
			break;
		case Qt::Key_Escape:
			if(selectedShape != nullptr)
			{
				if(!selectedShape->isClosed())
				{
					doc->shapes.removeOne(selectedShape);
					delete selectedShape;
				}
				selectedShape = nullptr;
				draggedVertex = nullptr;
				selectedVertex = nullptr;
				canvas->setSelectedShape(nullptr);
			}
			break;
	}
}
void ToolPoints::onMouseMove(QMouseEvent* e)
{
	QPoint pos = canvas->toCanvas(e->pos());
	if(draggedVertex != nullptr)
	{
		if(e->modifiers() & Qt::ControlModifier)
		{
			int dx = qAbs(pos.x() - oldPosition.x());
			int dy = qAbs(pos.y() - oldPosition.y());

			if(dx < dy)
			{
				pos.setY(oldPosition.y());
			}
			else
			{
				pos.setX(oldPosition.x());
			}
		}
		oldPosition = pos;
		selectedShape->moveVertex(draggedVertex, pos.x(), pos.y());

		canvas->viewport()->update();
	}
}
void ToolPoints::onMousePress(QMouseEvent* e)
{
	QPoint pos = canvas->toCanvas(e->pos());
	// ...
	oldPosition = pos;
	// ...
	if(selectedShape == NULL)
	{
/*
		if(QMessageBox::question(canvas,
							  tr("New terrain"),
							  tr("Do you want to create a new terrain?"),
							  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
*/
		{
			selectedShape = doc->addShape();
			canvas->setSelectedShape(selectedShape);
			if(NULL != (draggedVertex = selectedShape->addVertex(pos.x(), pos.y())))
			{
				selectedVertex = draggedVertex;
				canvas->setSelectedVertex(selectedVertex);
			}
		}
	}
	else
	{
		if(e->button() == Qt::LeftButton)
		{
			if(NULL != (draggedVertex = selectedShape->getVertexAt(pos.x(), pos.y())))
			{
				selectedVertex = draggedVertex;
				canvas->setSelectedVertex(selectedVertex);
			}
			else if(NULL != (draggedVertex = selectedShape->addVertex(pos.x(), pos.y())))
			{
				selectedVertex = draggedVertex;
				canvas->setSelectedVertex(selectedVertex);
			}
		}
		else if(e->button() == Qt::RightButton)
		{
			selectedShape->close();
			canvas->viewport()->update();
		}
	}
}
void ToolPoints::onMouseRelease(QMouseEvent* e)
{
	Q_UNUSED(e);
	Q_UNUSED(doc);
	Q_UNUSED(canvas);
	draggedVertex = nullptr;
}

