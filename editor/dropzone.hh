#ifndef DROPZONE_HH
# define DROPZONE_HH
#include <QWidget>
#include <QDropEvent>
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QTimer>
#include "expr.hh"
#include "main.hh"
#include "flowlayout.h"

class QPlainTextEdit;
class DropZone: public QWidget
{
  Q_OBJECT
public:
  DropZone(QWidget* parent)
  : QWidget(parent)
  , enableFixedSize(false)
  , rubberBand(0)
  , timer(new QTimer(this))
  , multiline(false)
  {
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    allowedActions = Qt::CopyAction| Qt::MoveAction;
  }
  ~DropZone();
  void dragEnterEvent(QDragEnterEvent *event)
 {
     event->acceptProposedAction();
 }
 bool enableFixedSize;
 QFlags<Qt::DropAction> allowedActions;
 void dropEvent(QDropEvent *event);
 void mousePressEvent(QMouseEvent *event);
 void mouseMoveEvent(QMouseEvent *event);
 void mouseReleaseEvent(QMouseEvent *event);
 void focusOutEvent(QFocusEvent * event);
 void focusInEvent(QFocusEvent * event);
 void keyPressEvent(QKeyEvent* event);

 void paintEvent(QPaintEvent *)
 {
     QStyleOption opt;
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }
 void emitOnDragged(Qt::DropAction action)
 {
   onDragged(action);
 }
 QPlainTextEdit* newSequence();
 void sequenceContentChanged(QPlainTextEdit* sequence);
 void setMultiLine();
 bool isMultiLine() { return multiline;}
 void addWidget(QWidget* w);
 void removeWidget(QWidget* w);
 void addLine();
signals:
   void onDragged(Qt::DropAction action);
   void onDrop();
 public slots:
   void onTimer();
   void sequenceContentChanged();
   void forceSize();
 private:
   QRubberBand* rubberBand;
   QPoint origin;
   QList<QWidget*> selection;
   QTimer* timer;
   bool multiline;
 public:
   static int maxWidth;
   static int shortSize;
 private:
   static QList<QWidget*> clipboard;
   friend class Expr;
};


#endif
