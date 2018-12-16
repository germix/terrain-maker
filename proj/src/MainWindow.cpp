#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMimeData>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>

#include "Canvas.h"
#include "Document.h"

#include "ToolSelect.h"
#include "ToolPoints.h"

#include "AboutDialog.h"
#include "EditSizeDialog.h"
#include "NewDocumentDialog.h"

#define TITLE "Terrain Maker"
#define SETTINGS_APPLICATION "TerrainEditor"
#define SETTINGS_ORGANIZATION "Germix"

extern void ShowInContainerFolder(const QString& fileName);

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QSettings s(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);

	initRecentFilesMenu(s.value("RecentFiles").toByteArray());

	languages.init(ui->menuLanguajes, "translations", "terrainmaker", s.value("Language").toString());

	doc = nullptr;
	setCentralWidget(canvas = new Canvas());
	connect(canvas, SIGNAL(onShapeSelected(DocumentShape*)), this, SLOT(slotCanvas_shapeSelected(DocumentShape*)));

	connect(ui->actionFileNew, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionFileOpen, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionFileSave, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionFileClose, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionFileExport, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionFileExit, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionFileOpenContainerFolder, SIGNAL(triggered()), this, SLOT(slotActions()));

	connect(ui->actionEditSize, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionEditUpLevel, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionEditBringToFront, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionEditDownLevel, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionEditSendToBack, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionHelpAbout, SIGNAL(triggered()), this, SLOT(slotActions()));

	ui->menuBars->addAction(ui->mainToolBar->toggleViewAction());
	ui->menuBars->addAction(ui->toolsToolBar->toggleViewAction());

	initTools();

	restoreGeometry(s.value("WindowGeometry").toByteArray());
	restoreState(s.value("WindowState").toByteArray());
#ifdef DEBUG
	openFile("ej1.xml");
#else
	QStringList args = qApp->arguments();
	if(args.size() > 1)
	{
		openFile(args.at(1));
	}
	else
	{
		updateActions();
	}
#endif
}
MainWindow::~MainWindow()
{
	QSettings s(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);

	s.clear();
	// ...
	s.setValue("WindowGeometry", saveGeometry());
	s.setValue("WindowState", saveState());
	s.setValue("Language", languages.language());
	s.setValue("RecentFiles", recentFiles->saveState());

	delete ui;
	delete doc;
}
void MainWindow::initTools()
{
	tools.append(new ToolSelect());
	tools.append(new ToolPoints());

	toolsGroup = new QActionGroup(this);
	toolsGroup->setExclusive(true);

	for(int i = 0; i < tools.size(); i++)
	{
		Tool* tool = tools.at(i);
		QAction* action = new QAction(tool->getIcon(), tool->getName(), tool);
		connect(action, SIGNAL(triggered()), this, SLOT(slotChangeTool()));

		action->setCheckable(true);
		toolsGroup->addAction(action);
		ui->menuTools->addAction(action);
		toolsActions.append(action);

		toolsHash.insert(tool, action);
	}
	ui->toolsToolBar->addActions(toolsActions);

	toolsActions.at(0)->trigger();
}
void MainWindow::initRecentFilesMenu(const QByteArray& state)
{
	recentFiles = new RecentFilesMenu(20, 10, tr("Recent files"), QIcon(":/images/action-clear.png"));

	ui->menuFile->insertMenu(ui->actionFileExit, recentFiles);
	ui->menuFile->insertSeparator(ui->actionFileExit);

	connect(recentFiles, SIGNAL(onFileTriggered(QString)), this, SLOT(slotRecentFiles_fileTriggered(QString)));

	recentFiles->restoreState(state);
}
void MainWindow::updateActions()
{
	bool enabled;
	if(doc)
		enabled = true;
	else
		enabled = false;
	// ...
	ui->actionFileSave->setEnabled(enabled);
	ui->actionFileClose->setEnabled(enabled);
	ui->actionFileExport->setEnabled(enabled);
	ui->actionEditSize->setEnabled(enabled);
	ui->actionFileOpenContainerFolder->setEnabled(enabled);

	for(QHash<Tool*, QAction*>::Iterator it = toolsHash.begin(); it != toolsHash.end(); it++)
	{
		it.value()->setEnabled(enabled);
	}
	// ...
	ui->actionFileOpenContainerFolder->setEnabled(!currentFileName.isEmpty());
}
void MainWindow::newFile()
{
	NewDocumentDialog dlg;
	if(QDialog::Accepted == dlg.exec())
	{
		Document* d = new Document();

		if(d->newDocument(
						dlg.getSize(),
						dlg.getLeftPart(),
						dlg.getRightPart(),
						dlg.getMiddlePart(),
						dlg.getTerrainPart()))
		{
			if(closeFile())
			{
				doc = d;
				canvas->setDocument(doc);
				connect(doc, SIGNAL(onModified()), this, SLOT(slotDocument_modified()));

				updateTitle();
				updateActions();
			}
		}
		else
		{
			delete d;
			QMessageBox::information(this, tr("Error"), tr("Can't create the document!"));
		}
	}
}
void MainWindow::openFile(const QString& fileName)
{
	if(closeFile())
	{
		Document* d = new Document();

		if(d->loadDocument(fileName))
		{
			QFileInfo fi(fileName);

			canvas->setDocument(doc = d);
			recentFiles->removeFile(currentFileName = fi.absoluteFilePath());
			connect(doc, SIGNAL(onModified()), this, SLOT(slotDocument_modified()));

			updateTitle();
			updateActions();
		}
	}
}
bool MainWindow::saveFile(const QString& fileName)
{
	if(doc->saveDocument(fileName))
	{
		QFileInfo fi(fileName);

		if(currentFileName.isEmpty())
		{
			recentFiles->addFile(currentFileName = fi.absoluteFilePath());
			updateTitle();
			updateActions();
		}
		return true;
	}
	return false;
}
bool MainWindow::closeFile()
{
	if(doc)
	{
		if(doc->isModified())
		{
			QString s;
			if(currentFileName.isEmpty())
				s = tr("Untitled");
			else
				s = QFileInfo(currentFileName).fileName();

			switch(QMessageBox::question(this,
										 tr("Save"),
										 tr("Save file \"%1\"?").arg(s),
										 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
			{
				case QMessageBox::Yes:
					if(!saveFile(currentFileName))
					{
						return false;
					}
					break;
				case QMessageBox::Cancel:
					return false;
				default:
				case QMessageBox::No:
					break;
			}
		}
		canvas->setDocument(nullptr);
		delete doc;
		doc = nullptr;
		if(!currentFileName.isEmpty())
		{
			recentFiles->addFile(currentFileName);
		}
		currentFileName.clear();

		updateTitle();
		updateActions();
	}
	return true;
}
void MainWindow::updateTitle()
{
	QString s = TITLE;
	if(doc != NULL)
	{
		QString fname = currentFileName;
		if(fname.isEmpty())
		{
			s += " - [" + tr("Untitled") + "]";
		}
		else
		{
			s += " - " + QFileInfo(fname).fileName();
		}
		if(doc->isModified())
		{
			s += " *";
		}
	}
	setWindowTitle(s);
}
void MainWindow::closeEvent(QCloseEvent* e)
{
	if(closeFile())
		e->accept();
	else
		e->ignore();
}
void MainWindow::changeEvent(QEvent* e)
{
	if(e != NULL)
	{
		switch(e->type())
		{
#if 1
			case QEvent::LocaleChange:
				{
					QString locale = QLocale::system().name();
					locale.truncate(locale.lastIndexOf('_'));
					languages.load(locale);
				}
				break;
#endif
			case QEvent::LanguageChange:
				ui->retranslateUi(this);
				recentFiles->rebuild(tr("Recent files"));

				for(QHash<Tool*, QAction*>::Iterator it = toolsHash.begin(); it != toolsHash.end(); it++)
				{
					it.value()->setText(it.key()->getName());
				}
				updateTitle();
				break;
			default:
				break;
		}
	}
	QMainWindow::changeEvent(e);
}
void MainWindow::dropEvent(QDropEvent* e)
{
	QString s;
	const QList<QUrl> urls = e->mimeData()->urls();

	e->acceptProposedAction();
	// ...
	for(int i = 0; i < qMin(1, urls.size()); i++)
	{
		s = urls.at(i).toLocalFile();
		if(!s.isEmpty())
		{
			openFile(s);
		}
	}
}
void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
	if(e->mimeData()->hasFormat("text/uri-list"))
	{
		e->acceptProposedAction();
	}
}
void MainWindow::slotActions()
{
	QAction* action = qobject_cast<QAction*>(sender());

	if(action == ui->actionFileNew)
	{
		newFile();
	}
	else if(action == ui->actionFileOpen)
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), QString(), tr("2D terrain file") + " (*.xml)");

		if(!fileName.isEmpty())
		{
			if(closeFile())
			{
				Document* d = new Document();
				if(d->loadDocument(fileName))
				{
					QFileInfo fi(fileName);

					canvas->setDocument(doc = d);
					recentFiles->removeFile(currentFileName = fi.absoluteFilePath());
					connect(doc, SIGNAL(onModified()), this, SLOT(slotDocument_modified()));

					updateTitle();
					updateActions();
				}
				else
				{
					QMessageBox::information(this, tr("Error"), tr("Can't open the file"), QMessageBox::Ok);
				}
			}
		}
	}
	else if(action == ui->actionFileSave)
	{
		QString fname = currentFileName;
		if(fname.isEmpty())
		{
			fname = QFileDialog::getSaveFileName(this, tr("Save file"), QString(), tr("2D terrain file") + " (*.xml)");
		}
		if(fname.isEmpty())
		{
			return;
		}
		if(!doc->saveDocument(fname))
		{
			QMessageBox::information(this, tr("Error"), tr("Can't save the file"), QMessageBox::Ok);
		}
		else
		{
			QFileInfo fi(fname);

			recentFiles->addFile(currentFileName = fi.absoluteFilePath());
		}
	}
	else if(action == ui->actionFileClose)
	{
		closeFile();
	}
	else if(action == ui->actionFileExport)
	{
		QString fileName = QFileDialog::getSaveFileName(this, tr("Export document"), QString(), tr("PNG files") + " (*.png)");

		if(!fileName.isEmpty())
		{
			doc->exportDocument(fileName);
		}
	}
	else if(action == ui->actionFileOpenContainerFolder)
	{
		ShowInContainerFolder(currentFileName);
	}
	else if(action == ui->actionFileExit)
	{
		close();
	}
	else if(action == ui->actionEditSize)
	{
		QSize s;
		EditSizeDialog dlg(doc->width, doc->height);
		if(QDialog::Accepted == dlg.exec())
		{
			s = dlg.getSize();
			doc->setSize(s.width(), s.height());
		}
	}
	else if(action == ui->actionEditUpLevel)
	{
		canvas->terrainUpLevel();
	}
	else if(action == ui->actionEditDownLevel)
	{
		canvas->terrainDownLevel();
	}
	else if(action == ui->actionEditSendToBack)
	{
		canvas->terrainSendToBack();
	}
	else if(action == ui->actionEditBringToFront)
	{
		canvas->terrainBringToFront();
	}
	else if(action == ui->actionHelpAbout)
	{
		AboutDialog(this).exec();
	}
}
void MainWindow::slotChangeTool()
{
	QAction* action = qobject_cast<QAction*>(sender());
	Tool* tool = qobject_cast<Tool*>(action->parent());

	if(tool)
	{
		canvas->setTool(tool);
	}
}
void MainWindow::slotCanvas_shapeSelected(DocumentShape* s)
{
	ui->actionEditUpLevel->setEnabled(s != nullptr);
	ui->actionEditDownLevel->setEnabled(s != nullptr);
	ui->actionEditSendToBack->setEnabled(s != nullptr);
	ui->actionEditBringToFront->setEnabled(s != nullptr);
}
void MainWindow::slotDocument_modified()
{
	updateTitle();
}
void MainWindow::slotRecentFiles_fileTriggered(const QString& fileName)
{
	openFile(fileName);
}
