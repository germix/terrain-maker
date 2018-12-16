#ifndef NEWDOCUMENTDIALOG_H
#define NEWDOCUMENTDIALOG_H

#include <QDialog>

namespace Ui {
class NewDocumentDialog;
}

class NewDocumentDialog : public QDialog
{
	Q_OBJECT
	Ui::NewDocumentDialog*	ui;
	QString					lastDirectory;
public:
	explicit NewDocumentDialog(QWidget *parent = 0);
	~NewDocumentDialog();
public:
	QSize getSize() const;
	QString getLeftPart() const;
	QString getRightPart() const;
	QString getMiddlePart() const;
	QString getTerrainPart() const;
private slots:
	void slotChooseFile();
};

#endif // NEWDOCUMENTDIALOG_H
