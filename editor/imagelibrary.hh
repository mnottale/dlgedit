#include <QMainWindow>
#include <QVBoxLayout>

class ImageLibrary: public QMainWindow
{
public:
  Q_OBJECT
public:
  ImageLibrary(QString path);
  QList<QIcon*> content;
  QVBoxLayout* layout;
  QWidget* mainWidget;
public slots:
  void onImage();
};