#ifndef MAIN_HH
#define MAIN_HH

#include <QApplication>
#include <QFileSystemWatcher>

#include "expr.hh"

extern QString imported_font_family;

class ToolSet;

class QMyApplication: public QApplication
{
  Q_OBJECT
public:
  QMyApplication(int& argc, char** &argv)
  : QApplication(argc, argv)
  {
    watcher.addPath("quests.txt");
    connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(onChange(QString)));
  }
  virtual ~QMyApplication() {}
public slots:
  void onChange(QString const&)
  {
    qDebug() << "Reloading quests";
    Expr::loadQuests("quests.txt");
    watcher.addPath("quests.txt");
  }
private:
  QFileSystemWatcher watcher;
public:
  static int elemHeight;
  static int textCharacterWidth;
  static int textLines;
  static int textPixelWidth;
  static void computeTextSize();
  static ToolSet* baseToolset;
};

#endif