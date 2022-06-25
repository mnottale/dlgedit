#ifndef TOOLSET_HH
# define TOOLSET_HH

#include <QWidget>
#include <QTimer>
#include "ui_MainWindow.h"

class DropZone;
class ToolSet: public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
public:
  ToolSet();
  void load(QString where);
  QWidget* makeGroup(QString name, QWidget* parent);
  QWidget* loadContent();
public slots:
  void onActionNew();
  void onActionOpen();
  void onActionSave();
  void onActionSaveAs();
  void onActionNewGroup();
  void onBackupTimer();
private:
  void doSave(QString path);
  QWidget* root;
  QString path;
  QTimer backupTimer;
};


#endif