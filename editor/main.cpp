#include <QApplication>
#include <QWidget>
#include <QDir>
#include <QToolBar>
#include <QDebug>
#include <QSettings>
#include <QSplitter>

#include <signal.h>

#include "node.hh"
#include "dialog.hh"
#include "imagelibrary.hh"
#include "exprbuilder.hh"
#include "main.hh"
#include "toolset.hh"
#include "previewer.hh"


int QMyApplication::elemHeight = 32;
int QMyApplication::textCharacterWidth = 0;
int QMyApplication::textPixelWidth = 0;
int QMyApplication::textLines = 0;
ToolSet* QMyApplication::baseToolset = 0;

extern Previewer* previewer;

void QMyApplication::computeTextSize()
{
  QFont qf(font());
  QFontMetrics fm(qf);
  textPixelWidth = fm.horizontalAdvance(QString(textCharacterWidth, 'x'));
  delete previewer;
  previewer = new Previewer(textCharacterWidth, textLines);
  previewer->show();
}

void ImageLibrary::onImage()
{
  QAction* source = dynamic_cast<QAction*>(sender());
  ExprBuilder::builder->setIcon(source->toolTip());
}


ImageLibrary::ImageLibrary(QString path)
{
  mainWidget = new QWidget();
  setCentralWidget(mainWidget);
  layout = new QVBoxLayout(mainWidget);
  QStringList nameFilter("*.png");
  QDir directory(path);
  QStringList files = directory.entryList(nameFilter);
  QToolBar* current = 0;
  for (int i=0; i<files.count(); ++i)
  {
    if (!(i%10))
    {
      if (current)
        layout->addWidget(current);
      current = new QToolBar();
    }
    QAction* action = current->addAction(QIcon(path + "/" + files.at(i)), files.at(i));
    action->setToolTip(path + "/" + files.at(i));
    connect(action, SIGNAL(triggered()), this, SLOT(onImage()));
  }
  layout->addWidget(current);
}


QString imported_font_family;

void onsignal(int)
{
  QApplication::quit();
}

int main(int argc, char** argv)
{
  signal(SIGTERM, onsignal);
  signal(SIGINT, onsignal);
  //signal(SIGQUIT, onsignal);
  QMyApplication app(argc, argv);
  QCoreApplication::setOrganizationName("NoiseSoft");
  QCoreApplication::setApplicationName("Noise");
  Expr::loadQuests("quests.txt");
  ImageLibrary qil("assets/");
  qil.show();
  ExprBuilder builder;
  builder.show();
  ToolSet ts;
  ts.load("base.tbx");
  ts.show();
  QMyApplication::baseToolset = &ts;
  int res = QFontDatabase::addApplicationFont("font.ttf");
  qDebug() << "font load " << ((res!=-1) ? "ok": "fail");
  qDebug() << QFontDatabase::applicationFontFamilies(res);
  if (res != -1)
  	  imported_font_family = QFontDatabase::applicationFontFamilies(res)[0];
  {
    QSettings settings;
    QMyApplication::elemHeight = settings.value("elemHeight", 32).toInt();
    QMyApplication::textCharacterWidth = settings.value("textCharacterWidth", 0).toInt();
    QMyApplication::textLines = settings.value("textLines", 0).toInt();
    DropZone::maxWidth = settings.value("dropZone/maxWidth", 300).toInt();
    DropZone::shortSize = settings.value("dropZone/shortSize", 2).toInt();
    if (settings.contains("font"))
    {
      QMyApplication::setFont(settings.value("font", QFont()).value<QFont>());
    }
    QMyApplication::computeTextSize();
    previewer->move(settings.value("preview/geometry", QPoint(10, 10)).value<QPoint>());
    ts.setGeometry(settings.value("tools/geometry", QRect(20, 20, 500, 500)).value<QRect>());
    builder.setGeometry(settings.value("builder/geometry", QRect(20, 20, 200, 400)).value<QRect>());
    qil.setGeometry(settings.value("images/geometry", QRect(20, 20, 200, 200)).value<QRect>());
    Dialog::lastUsedPath = settings.value("dialog/lastUsedPath", "").toString();
    // now restore dialog windows
    int size = settings.beginReadArray("dialogs");
    qDebug() << "Restoring " << size << " dialogs";
    for (int i=0; i<size; ++i)
    {
      settings.setArrayIndex(i);
      Dialog* dlg = new Dialog();
      dlg->load(settings.value("file").toString());
      dlg->setGeometry(settings.value("geometry").value<QRect>());
      dlg->show();
      bool hasToolbox = settings.value("hasToolbox").toBool();
      if (hasToolbox)
      {
        dlg->onActionAttachToolbox();
        QSplitter* splitter = dynamic_cast<QSplitter*>(dlg->centralWidget());
        splitter->restoreState(settings.value("splitterSizes").toByteArray());
      }
    }
    settings.endArray();
    if (size == 0)
    {
      Dialog* dlg = new Dialog;
      dlg->show();
    }
  }

  int ares = app.exec();
  qDebug() << "exiting exec";
  {
    QSettings settings;
    settings.setValue("preview/geometry", previewer->pos());
    settings.setValue("tools/geometry", ts.geometry());
    settings.setValue("builder/geometry", builder.geometry());
    settings.setValue("images/geometry", qil.geometry());
    settings.setValue("font", QApplication::font());
    settings.setValue("elemHeight", QMyApplication::elemHeight);
    settings.setValue("textCharacterWidth", QMyApplication::textCharacterWidth);
    settings.setValue("textLines", QMyApplication::textLines);
    settings.setValue("dropZone/maxWidth", DropZone::maxWidth);
    settings.setValue("dropZone/shortSize", DropZone::shortSize);
    settings.setValue("dialog/lastUsedPath", Dialog::lastUsedPath);
  }
  return ares;
}
