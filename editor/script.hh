#ifndef SCRIPT_HH
# define SCRIPT_HH

#include <QWidget>
#include <QMainWindow>

#include "node.hh"
#include "dropzone.hh"

class Script: public QMainWindow
{
  Q_OBJECT
public:
  Script(Node* owner);
  void capture();
  void release();
  void read();
  void write();
  void closeEvent(QCloseEvent *event);
private:
  DropZone* dropZone;
  Node* node;
public slots:
  void onClose();
};

#endif