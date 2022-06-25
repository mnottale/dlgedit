#include "previewer.hh"
#include "main.hh"

#include <QStatusBar>

Previewer* previewer = 0;

Previewer::Previewer(int w, int h)
: w(w)
, h(h)
{
  this->statusBar()->setSizeGripEnabled(false);
  setWindowTitle("Preview");
  label = new QLabel(this);
  setCentralWidget(label);
  QFont qf(imported_font_family, 12);
  QFontMetrics fm(qf);
  int width = fm.horizontalAdvance(QString(w, 'x'));
  int height = fm.height()*h;
  qDebug() << "zone size: " << width <<' ' << height;
  label->setStyleSheet(
    "QLabel {"
    "border: 1px solid black;"
    "}");
  label->setFont(qf);
  label->resize(width, height);
  label->setMaximumSize(width, height);
  label->setMinimumSize(width, height);
  label->setWordWrap(true);
  label->setText("0123456789012345678901234567890123456789X 0123456789012345678901234567890123456789X 0123456789012345678901234567890123456789X 0123456789012345678901234567890123456789X");
}

void Previewer::preview(QString str)
{
  str = str.replace("\\n", "\n");
  label->setText(str);
}
