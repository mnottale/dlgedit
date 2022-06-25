#ifndef DIALOG_HH
# define DIALOG_HH

#include <QWidget>
#include <QTimer>
#include <QtNetwork/QTcpSocket>
#include "ui_MainWindow.h"
#include "node.hh"

class QScrollArea;
class DialogFind;
class Dialog: public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
public:
  Dialog();
  ~Dialog();
  void save(QString path="", bool json=true, bool backup = false);
  void load(QString path);
  static QString lastUsedPath;
protected:
  void closeEvent(QCloseEvent* event);
  bool maybeSave();
public slots:
  void onActionBuilder();
  void onActionIcons();
  void onActionNew();
  void onActionOpen();
  void onActionSave();
  void onActionSaveAs();
  void onActionHelp();
  void onBackupTimer();
  void onDirtyTimer();
  void onActionFont();
  void onActionIcon32();
  void onActionIcon24();
  void onActionIcon16();
  void onActionFind();
  void onActionQuit();
  void onActionFixedTextLayout();
  void onActionFlowTextLayout();
  void onActionAttachToolbox();
  void onActionConnectDDB();
  void onDropzoneMaxWidth();
  void onDropzoneShortSize();
  void collapseAll();
  void expandAll();
  void socketConnected();
  void socketError();
  void socketReadyRead();
  void socketRequestPending();
  void disconnect();
private:
  Node* root;
  QScrollArea* scroll;
  QString path;
  QTimer backupTimer;
  QTimer dirtyTimer;
  QMap<QString, QVariant> savedPtrs;
  QVariant savedData;
  DialogFind* find;
  QTcpSocket* socket;
  static QList<Dialog*> dialogs;
  friend class DialogFind;
};

class DialogFind: public QMainWindow
{
public slots:
  void onFindClicked();
  void onFindNextClicked();
public:
  Q_OBJECT
public:
  DialogFind(Dialog* owner);
  void findone(Node* current, bool skip = false);
  Dialog* owner;
  QLineEdit* text;
  QPushButton* find;
  QPushButton* findNext;
  QList<int> stack;
  Node* lastMatch;
};

#endif