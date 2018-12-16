#include "EditSizeDialog.h"
#include "ui_EditSizeDialog.h"

EditSizeDialog::EditSizeDialog(int w, int h, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EditSizeDialog)
{
	ui->setupUi(this);

	ui->spinBoxWidth->setValue(w);
	ui->spinBoxHeight->setValue(h);
}

EditSizeDialog::~EditSizeDialog()
{
	delete ui;
}
QSize EditSizeDialog::getSize() const
{
	return QSize(ui->spinBoxWidth->value(), ui->spinBoxHeight->value());
}
