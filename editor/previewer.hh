#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>


class Previewer: public QMainWindow
{
public:
  Q_OBJECT
public:
  Previewer(int w = 42, int h = 4);
  void preview(QString v);
  QLabel* label;
  int w,h;
};



extern Previewer* previewer;