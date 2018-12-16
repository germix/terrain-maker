#include "NewDocumentDialog.h"
#include "ui_NewDocumentDialog.h"

#include <QFileDialog>

NewDocumentDialog::NewDocumentDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NewDocumentDialog)
{
	ui->setupUi(this);
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
}

NewDocumentDialog::~NewDocumentDialog()
{
	delete ui;
}
QSize NewDocumentDialog::getSize() const
{
	return QSize(ui->spinBoxWidth->value(), ui->spinBoxHeight->value());
}
QString NewDocumentDialog::getLeftPart() const
{
	return ui->txtLeftPart->text();
}
QString NewDocumentDialog::getRightPart() const
{
	return ui->txtRightPart->text();
}
QString NewDocumentDialog::getMiddlePart() const
{
	return ui->txtMiddlePart->text();
}
QString NewDocumentDialog::getTerrainPart() const
{
	return ui->txtTerrainPart->text();
}
void NewDocumentDialog::slotChooseFile()
{
	qDebug("slotChooseFile()");
	QToolButton* button = qobject_cast<QToolButton*>(sender());
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), lastDirectory, tr("PNG files") + " (*.png)");
	if(!fileName.isEmpty())
	{
		QFileInfo fi(fileName);
		if(button == ui->btnLeftPart)			ui->txtLeftPart->setText(fi.absoluteFilePath());
		else if(button == ui->btnRightPart)		ui->txtRightPart->setText(fi.absoluteFilePath());
		else if(button == ui->btnMiddlePart)	ui->txtMiddlePart->setText(fi.absoluteFilePath());
		else if(button == ui->btnTerrainPart)	ui->txtTerrainPart->setText(fi.absoluteFilePath());

		lastDirectory = fi.absolutePath();
	}
}
