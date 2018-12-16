#include "Canvas.h"
#include "Document.h"
#include "DocumentShape.h"

#include <QPainter>
#include <QScrollBar>
#include <QMouseEvent>

#include <float.h>
#include <limits.h>

#include "Tool.h"
#include "ToolPoints.h"
#include "ToolSelect.h"

#define BORDER 50

static void updateScrollBar(QScrollBar* bar, int max, int page)
{
	bar->setMaximum(qMax(0, max-page+1));
	bar->setPageStep(page);
	bar->setSingleStep(4);
}

//--------------------------------------------------------------------------------------------------
Canvas::Canvas(QWidget* parent)
	: QAbstractScrollArea(parent)
	, doc(nullptr)
	, tool(nullptr)
	, showMesh(false)
	, showLines(false)
	, showVertexDots(false)
	, selectedShape(nullptr)
	, selectedVertex(nullptr)
{
	dragging = false;
	backgroundColor = Qt::white;
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slotScrollBar_valueChanged(int)));
	connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(slotScrollBar_valueChanged(int)));
}
Canvas::~Canvas()
{
}
void Canvas::setTool(Tool *t)
{
	if(tool != t)
	{
		if(tool && doc)
			tool->onDeactivate(this, doc);
		tool = t;
		if(tool && doc)
			tool->onActivate(this, doc);

		viewport()->update();
	}
}
void Canvas::setDocument(Document* d)
{
	if(doc)
	{
		disconnect(doc, SIGNAL(onSizeChanged(int,int)), this, SLOT(slotDocument_sizeChanged(int,int)));
		if(tool)
			tool->onDeactivate(this, NULL);
	}
	doc = d;
	selectedShape = nullptr;
	selectedVertex = nullptr;
	if(doc)
	{
		connect(doc, SIGNAL(onSizeChanged(int,int)), this, SLOT(slotDocument_sizeChanged(int,int)));
		if(tool)
			tool->onActivate(this, doc);
	}
	reset();

	emit onShapeSelected(selectedShape);
}
QPoint Canvas::toCanvas(const QPoint& pt)
{
	int x = pt.x() - paperRect.x();
	int y = pt.y() - paperRect.y();
	return QPoint(x, y);
}
void Canvas::terrainUpLevel()
{
	if(selectedShape)
	{
		int index = doc->shapes.indexOf(selectedShape);
		if(index != -1 && index < doc->shapes.length()-1)
		{
			DocumentShape* s = doc->shapes[index+1];
			doc->shapes[index] = s;
			doc->shapes[index+1] = selectedShape;
			viewport()->update();
		}
	}
}
void Canvas::terrainDownLevel()
{
	if(selectedShape)
	{
		int index = doc->shapes.indexOf(selectedShape);
		if(index != -1 && index > 0)
		{
			DocumentShape* s = doc->shapes[index-1];
			doc->shapes[index] = s;
			doc->shapes[index-1] = selectedShape;
			viewport()->update();
		}
	}
}
void Canvas::terrainSendToBack()
{
	if(selectedShape)
	{
		int index = doc->shapes.indexOf(selectedShape);
		if(index != -1 && index != 0)
		{
			DocumentShape* s = doc->shapes[0];
			doc->shapes[index] = s;
			doc->shapes[0] = selectedShape;
			viewport()->update();
		}
	}
}
void Canvas::terrainBringToFront()
{
	if(selectedShape)
	{
		int index = doc->shapes.indexOf(selectedShape);
		if(index != -1 && index != doc->shapes.length()-1)
		{
			DocumentShape* s = doc->shapes[doc->shapes.length()-1];
			doc->shapes[index] = s;
			doc->shapes[doc->shapes.length()-1] = selectedShape;
			viewport()->update();
		}
	}
}
void Canvas::reset()
{
	int w = 0;
	int h = 0;
	QScrollBar* vbar = verticalScrollBar();
	QScrollBar* hbar = horizontalScrollBar();

	if(doc)
	{
		w = doc->width + BORDER*2;
		h = doc->height + BORDER*2;
	}
	updateScrollBar(hbar, w, viewport()->width());
	updateScrollBar(vbar, h, viewport()->height());

	calcPaperRect();
	viewport()->update();
}
void Canvas::calcPaperRect()
{
	if(!doc)
	{
		paperRect = QRect();
	}
	else
	{
		int x;
		int y;
		QScrollBar* vbar = verticalScrollBar();
		QScrollBar* hbar = horizontalScrollBar();
		int docWidth = doc->width;
		int docHeight = doc->height;
		int frameWidth = docWidth + BORDER*2;
		int frameHeight = docHeight + BORDER*2;

		if(!(frameWidth < viewport()->width()))
			x = -hbar->value();
		else
			x = (viewport()->width() - frameWidth)/2;

		if(!(frameHeight < viewport()->height()))
			y = -vbar->value();
		else
			y = (viewport()->height() - frameHeight)/2;

		paperRect = QRect(x+BORDER, y+BORDER, docWidth, docHeight);
	}
}
void Canvas::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);
	QPainter p(viewport());
	if(!doc)
	{
		p.fillRect(rect(), Qt::lightGray);
		return;
	}
/*
	QScrollBar* vbar = verticalScrollBar();
	QScrollBar* hbar = horizontalScrollBar();
	p.fillRect(rect(), Qt::white);
	p.fillRect(QRect(-hbar->value(), -vbar->value(), BORDER, vbar->maximum() + vbar->pageStep()), Qt::lightGray);
	p.fillRect(QRect(-hbar->value()+BORDER, -vbar->value(), hbar->maximum() + hbar->pageStep() - BORDER, BORDER), Qt::lightGray);
	p.fillRect(QRect(-hbar->value() + hbar->maximum() + hbar->pageStep() - BORDER, -vbar->value() + BORDER, BORDER, vbar->maximum() + vbar->pageStep() - BORDER), Qt::lightGray);
	p.fillRect(QRect(-hbar->value()+BORDER, -vbar->value() + vbar->maximum() + vbar->pageStep() - BORDER, hbar->maximum() + hbar->pageStep() - BORDER*2, BORDER), Qt::lightGray);
*/
	p.fillRect(rect(), Qt::lightGray);
	p.fillRect(paperRect, backgroundColor);
	p.translate(paperRect.x(), paperRect.y());

	for(int i = 0; i < doc->shapes.size(); i++)
	{
		DocumentShape* s = doc->shapes[i];

		if(s != selectedShape)
			s->paint(p, selectedShape, selectedVertex, false, false, false);
		else
			s->paint(p, selectedShape, selectedVertex, false, showLines, showVertexDots);
	}
//	qDebug("selectedVertex: %p", selectedVertex);
	if(doc && tool)
		tool->onPaint(p);
}
void Canvas::resizeEvent(QResizeEvent* e)
{
	Q_UNUSED(e);
	if(doc)
	{
		QScrollBar* vbar = verticalScrollBar();
		QScrollBar* hbar = horizontalScrollBar();

		updateScrollBar(hbar, doc->width + BORDER*2, viewport()->width());
		updateScrollBar(vbar, doc->height + BORDER*2, viewport()->height());

		calcPaperRect();
	}
}
void Canvas::keyPressEvent(QKeyEvent* e)
{
	if(dragging && e->key() == Qt::Key_Escape)
	{
		dragging = false;
		viewport()->releaseMouse();
		return;
	}
	if(doc && tool)
		tool->onKeyPress(e);
}
void Canvas::keyReleaseEvent(QKeyEvent* e)
{
	if(doc && tool)
		tool->onKeyRelease(e);
}
void Canvas::mouseMoveEvent(QMouseEvent* e)
{
	if(dragging)
	{
		verticalScrollBar()->setValue(-(e->globalPos() - deltaSize).y());
		horizontalScrollBar()->setValue(-(e->globalPos() - deltaSize).x());
	}
	else
	{
		if(doc && tool)
			tool->onMouseMove(e);
	}
}
void Canvas::mousePressEvent(QMouseEvent* e)
{
	setFocus();
	if(e->button() == Qt::RightButton)
	{
		viewport()->grabMouse(QCursor(Qt::SizeAllCursor));

		dragging = true;
		deltaSize = e->globalPos() - QPoint(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
	}
	else
	{
		if(doc && tool)
			tool->onMousePress(e);
	}
}
void Canvas::mouseReleaseEvent(QMouseEvent* e)
{
	if(dragging)
	{
		dragging = false;
		viewport()->releaseMouse();
	}
	else
	{
		if(doc && tool)
			tool->onMouseRelease(e);
	}
}
void Canvas::slotDocument_sizeChanged(int w, int h)
{
	QScrollBar* vbar = verticalScrollBar();
	QScrollBar* hbar = horizontalScrollBar();


	updateScrollBar(hbar, w + BORDER*2, viewport()->width());
	updateScrollBar(vbar, h + BORDER*2, viewport()->height());

	calcPaperRect();
}
void Canvas::slotScrollBar_valueChanged(int value)
{
	Q_UNUSED(value);
	calcPaperRect();
}


