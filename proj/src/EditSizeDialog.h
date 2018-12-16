#ifndef EDITSIZEDIALOG_H
#define EDITSIZEDIALOG_H

#include <QDialog>

namespace Ui {
class EditSizeDialog;
}

class EditSizeDialog : public QDialog
{
	Q_OBJECT
	Ui::EditSizeDialog *ui;
public:
	explicit EditSizeDialog(int w, int h, QWidget *parent = 0);
	~EditSizeDialog();
public:
	QSize getSize() const;
};

#endif // EDITSIZEDIALOG_H
