#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Languages.h"
#include "RecentFilesMenu.h"

namespace Ui {
class MainWindow;
}

class Tool;
class QActionGroup;

class DocumentShape;

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Ui::MainWindow*				ui;
	class Document*				doc;
	class Canvas*				canvas;

	QVector<Tool*>				tools;
	QHash<Tool*, QAction*>		toolsHash;
	QActionGroup*				toolsGroup;

	QList<QAction*>				toolsActions;

	Languages					languages;
	RecentFilesMenu*			recentFiles;

	QString						currentFileName;
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	void initTools();
	void initRecentFilesMenu(const QByteArray& state);
	void updateActions();
	void newFile();
	void openFile(const QString& fileName);
	bool saveFile(const QString& fileName);
	bool closeFile();
	void updateTitle();
private:
	void closeEvent(QCloseEvent* e);
	void changeEvent(QEvent* e);
	void dropEvent(QDropEvent* e);
	void dragEnterEvent(QDragEnterEvent* e);
private slots:
	void slotActions();
	void slotChangeTool();
	void slotCanvas_shapeSelected(DocumentShape* s);
	void slotDocument_modified();
	void slotRecentFiles_fileTriggered(const QString& fileName);
};

#endif // MAINWINDOW_H
