#ifndef DOCUMENT_H
#define DOCUMENT_H
#include <QImage>
#include <QVector>

#include "Triangle.h"

#include "DocumentShape.h"

#define IMAGE_TARRAIN 3

class QXmlStreamReader;

class Document : public QObject
{
	Q_OBJECT
public:
	int							width;
	int							height;
	bool						modified;
	QImage						images[4];
	QString						imageNames[4];

	QVector<DocumentShape*>		shapes;
public:
	Document();
	~Document();
public:
	bool newDocument(const QSize& size, const QString& leftPart, const QString& rightPart, const QString& middlePart, const QString& terrainPart);
	bool loadDocument(const QString& fname);
	bool saveDocument(const QString& fname);
	bool exportDocument(const QString& fname);
	bool isModified() const { return modified; }
	void setModified(bool m);

	void setSize(int w, int h);
	DocumentShape* addShape();
signals:
	void onModified();
	void onSizeChanged(int w, int h);
private:
	void loadXmlDocument(const QString& path, QXmlStreamReader& xml);
	void loadXmlDocumentShapes(const QString& path, QXmlStreamReader& xml);
};

#endif // DOCUMENT_H
