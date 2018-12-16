#include "Document.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QImage>
#include <QPainter>
#include <QImageWriter>

#include "Vertex.h"

Document::Document()
{
	width = 0;
	height = 0;
	modified = false;
}
Document::~Document()
{
}
bool Document::newDocument(const QSize& size, const QString& leftPart, const QString& rightPart, const QString& middlePart, const QString& terrainPart)
{
	width = size.width();
	height = size.height();
	modified = false;
	// ...
	images[0] = QImage(leftPart);
	imageNames[0] = leftPart;
	images[1] = QImage(middlePart);
	imageNames[1] = middlePart;
	images[2] = QImage(rightPart);
	imageNames[2] = rightPart;
	images[3] = QImage(terrainPart);
	imageNames[3] = terrainPart;

	return !(images[0].isNull() || images[1].isNull() || images[2].isNull() || images[3].isNull());
}
bool Document::loadDocument(const QString& fname)
{
	QFile f(fname);
	if(f.open(QIODevice::ReadOnly))
	{
		QXmlStreamReader xml(&f);
		QString path = QFileInfo(f).absoluteDir().absolutePath();

		while(xml.readNextStartElement())
		{
			loadXmlDocument(path, xml);
		}
		modified = false;
		emit onModified();
		return true;
	}
	return false;
}
bool Document::saveDocument(const QString& fname)
{
	QFile f(fname);
	if(f.open(QIODevice::WriteOnly))
	{
		QXmlStreamWriter xml(&f);
		QString path = QFileInfo(f).absoluteDir().absolutePath();


		xml.setAutoFormatting(true);
		xml.writeStartDocument();

		xml.writeStartElement("Document");

			xml.writeAttribute("version", "1.0");
			xml.writeAttribute("width", QString::number(width));
			xml.writeAttribute("height", QString::number(height));
			xml.writeAttribute("left", imageNames[0]);
			xml.writeAttribute("middle", imageNames[1]);
			xml.writeAttribute("right", imageNames[2]);
			xml.writeAttribute("body", imageNames[3]);

			xml.writeStartElement("Shapes");
			for(int i= 0; i < shapes.size(); i++)
			{
				int j;
				Vertex* v;
				DocumentShape* shape = shapes[i];
				// ...
				xml.writeStartElement("Shape");
				xml.writeAttribute("closed", shape->isClosed() ? "true" : "false");

				for(j = 0, v = shape->vertexList.head; j < shape->vertexList.size; j++, v = v->next)
				{
					xml.writeStartElement("Vertex");
					xml.writeAttribute("x", QString::number(v->pos.x));
					xml.writeAttribute("y", QString::number(v->pos.y));
					xml.writeEndElement();
				}
				xml.writeEndElement();
			}
			xml.writeEndElement();
		xml.writeEndElement();

		xml.writeEndDocument();

		modified = false;
		emit onModified();
		return true;
	}
	return false;
}
bool Document::exportDocument(const QString& fname)
{
	QImage image(width, height, QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);

	for(int i = 0; i < shapes.size(); i++)
	{
		shapes[i]->paint(p, nullptr, nullptr, false, false, false);
	}
	QFile file(fname);
	if(file.open(QFile::WriteOnly))
	{
		QImageWriter iw(&file, "PNG");

		iw.write(image);
		return true;
	}
	return false;
}
void Document::setModified(bool m)
{
	if(modified != m)
	{
		modified = m;
		emit onModified();
	}
}
DocumentShape* Document::addShape()
{
	DocumentShape* s = new DocumentShape(this);
	shapes.append(s);
	return s;
}
void Document::setSize(int w, int h)
{
	if(width != w || height != h)
	{
		width = w;
		height = h;
		modified = true;
		onModified();
		emit onSizeChanged(width, height);
	}
}
/*
void Document::checkSize()
{
	// Calcular tamaño maximo
	int minWidth = 0;
	int minHeight = 0;
	int maxWidth = 0;
	int maxHeight = 0;
	for(int i = 0; i < shapes.length(); i++)
	{
		QRect rc = shapes[i]->bound();
#if 0
		minWidth = qMin(minWidth, rc.left());
		minHeight = qMin(minHeight, rc.bottom());
#endif
		maxWidth = qMax(maxWidth, rc.right());
		maxHeight = qMax(maxHeight, rc.bottom());
	}
#if 0
	maxWidth += qAbs(minWidth);
	maxHeight += qAbs(minHeight);
#endif
	if(width != maxWidth || height != maxHeight)
	{
		width = maxWidth;
		height = maxHeight;
		emit onSizeChanged(width, height);
	}
}
*/
void Document::loadXmlDocument(const QString& path, QXmlStreamReader& xml)
{
	images[0] = QImage(xml.attributes().value("left").toString());
	images[1] = QImage(xml.attributes().value("middle").toString());
	images[2] = QImage(xml.attributes().value("right").toString());
	images[3] = QImage(xml.attributes().value("body").toString());

	if(images[0].isNull()) qDebug("image invalid 0");
	if(images[1].isNull()) qDebug("image invalid 1");
	if(images[2].isNull()) qDebug("image invalid 2");
	if(images[3].isNull()) qDebug("image invalid 3");

	imageNames[0] = xml.attributes().value("left").toString();
	imageNames[1] = xml.attributes().value("middle").toString();
	imageNames[2] = xml.attributes().value("right").toString();
	imageNames[3] = xml.attributes().value("body").toString();

	width = xml.attributes().value("width").toInt();
	height = xml.attributes().value("height").toInt();

	while(xml.readNextStartElement())
	{
		if(xml.name() == "Shapes")
		{
			loadXmlDocumentShapes(path, xml);
		}
	}
	xml.skipCurrentElement();
}
void Document::loadXmlDocumentShapes(const QString& path, QXmlStreamReader& xml)
{
	Q_UNUSED(path);
	while(xml.readNextStartElement())
	{
		if(xml.name() == "Shape")
		{
			bool closed = (xml.attributes().value("closed").toString() == "true");
			DocumentShape* shape = addShape();

			while(xml.readNextStartElement())
			{
				if(xml.name() == "Vertex")
				{
					shape->addVertex(
								xml.attributes().value("x").toDouble(),
								xml.attributes().value("y").toDouble());
				}
				xml.skipCurrentElement();
			}
			if(closed)
			{
				shape->close();
			}
		}
	}
}
