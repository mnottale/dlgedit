#include "dropzone.hh"
#include "exprbuilder.hh"
#include <QPlainTextEdit>
#include <QDrag>
#include <QMimeData>
#include <QRegularExpression>

QList<QWidget*> DropZone::clipboard;

int DropZone::maxWidth = 300;
int DropZone::shortSize = 2;

 QFlags<Qt::DropAction> allowedActions;

class DrageableEdit: public QPlainTextEdit
{
public:
  QPoint dragStartPosition;
  DrageableEdit(QWidget* parent)
  : QPlainTextEdit(parent)
  {
  }
  void mousePressEvent(QMouseEvent *event)
  {
    if (event->button() == Qt::LeftButton)
      dragStartPosition = event->pos();
    else if (event->button() == Qt::RightButton
      && (event->modifiers() & Qt::ControlModifier))
    {
      parentWidget()->layout()->removeWidget(this);
      QApplication::postEvent(ExprBuilder::builder, new DeleteRequest(this));
      return;
    }
    QPlainTextEdit::mousePressEvent(event);
  }
  void mouseMoveEvent(QMouseEvent *event)
  {
    if (!(event->buttons() & Qt::LeftButton) || !event->modifiers())
    {
      QPlainTextEdit::mouseMoveEvent(event);
      return;
    }
    if ((event->pos() - dragStartPosition).manhattanLength()
      < QApplication::startDragDistance())
    return;
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText("Expr");
    drag->setMimeData(mimeData);
    // Only accept move if source is a dropzone
    DropZone* dz = dynamic_cast<DropZone*>(parentWidget());

    Qt::DropAction dropAction = drag->exec(
      dz ? dz->allowedActions : (Qt::CopyAction| Qt::MoveAction));
    if (dropAction && dz)
      dz->emitOnDragged(dropAction);
  }
};

 QPlainTextEdit* DropZone::newSequence()
 {
   QPlainTextEdit* sequence = new DrageableEdit(this);
   sequence->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   if (QMyApplication::textPixelWidth)
   {
     int w = QMyApplication::textPixelWidth + 16;
     sequence->setMinimumWidth(w);
     sequence->setMaximumWidth(w);
  }
  sequence->connect(sequence->document(), SIGNAL(contentsChanged()), this, SLOT(sequenceContentChanged()));
  
  return sequence;
 }

void DropZone::sequenceContentChanged()
{
  sequenceContentChanged(0);
}

void DropZone::sequenceContentChanged(QPlainTextEdit* sequence)
{
  QObject* doc = QObject::sender();
  if (!sequence)
  for (int index=0; index<layout()->count(); ++index)
  {
    QWidget* w = layout()->itemAt(index)->widget();
    QPlainTextEdit* s = dynamic_cast<QPlainTextEdit*>(w);
    if (s && s->document() == doc)
    {
      sequence = s;
      break;
    }
  }
  if (!sequence)
  {
    qDebug() << "Sequence not found!";
    return;
  }
  // sender is the qtextdocument...
  int h = sequence->document()->size().height();
  QFontMetrics m (sequence->font()) ;
  int r = m.lineSpacing();
  h++;
  sequence->setMinimumHeight(h*r);
  sequence->setMaximumHeight(h*r);
  if (QMyApplication::textPixelWidth)
  {
    sequence->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    int w = QMyApplication::textPixelWidth + 16;
    sequence->setMinimumWidth(w);
    sequence->setMaximumWidth(w);
  }
}

 void DropZone::dropEvent(QDropEvent *event)
 {
   DropZone* old = dynamic_cast<DropZone*>(dynamic_cast<QWidget*>(event->source())->parentWidget());
   if (!old)
     old = dynamic_cast<DropZone*>(dynamic_cast<QWidget*>(event->source())->parentWidget()->parentWidget());
   if (!old)
   {
     event->ignore();
     return;
   }
   qDebug() << "DROP " << event->pos() << " " << old << " " << old->allowedActions;
   QWidget* source = 0;
   if (Expr* expr = dynamic_cast<Expr*>(event->source()))
   {
     if (expr->expression() == "TEXTAREA")
       source = newSequence();
   }
   if (!source)
   {
     if (event->dropAction() == Qt::CopyAction
       || (old && old->allowedActions == Qt::CopyAction))
     {
       Expr* expr = dynamic_cast<Expr*>(event->source());
       if (!expr)
         return;
       Expr* expr2 = expr->clone(this);
       source = expr2;
     }
     else
     {
       QWidget* expr = dynamic_cast<QWidget*>(event->source());
       // FIXME if source and target container is the same, this will move
       // the layout and fsck up target coordinate computation
       auto exprParent = expr->parentWidget();
       if (auto* dz = dynamic_cast<DropZone*>(exprParent))
         dz->removeWidget(expr);
       else if (auto* dz = dynamic_cast<DropZone*>(exprParent->parentWidget()))
         dz->removeWidget(expr);
       else
         exprParent->layout()->removeWidget(expr);
       source = expr;
     }
   }
   source->setParent(this);
   int index = 0;
   if (multiline)
   {
     auto px = event->pos().x();
     auto py = event->pos().y();
     for (int j=0,count=layout()->count(); j < count; ++j)
     {
       QWidget* w = layout()->itemAt(j)->widget();
       QRect r = w->geometry();
       if (r.bottom() >= py || j == count-1)
       { // it goes on that one
         qDebug() << "Going at y=" << j;
         auto* l = w->layout();
         int i = 0;
         for (int count=l->count(); i < count; ++i)
         {
           QWidget* w = l->itemAt(i)->widget();
           QRect r = w->geometry();
           if (r.left() + r.width()/2 > px)
             break;
         }
         qDebug() << "Going at x=" << i;
         dynamic_cast<QBoxLayout*>(l)->insertWidget(i, source);
         if (j == count - 1)
           addLine();
         break;
       }
     }
   }
   else if (dynamic_cast<QBoxLayout*>(layout()))
   {
     if (dynamic_cast<QHBoxLayout*>(layout()))
     {
       for (index=0; index<layout()->count(); ++index)
       {
         QWidget* w = layout()->itemAt(index)->widget();
         QRect r = w->geometry();
         if (r.left() + r.width()/2 > event->pos().x())
           break;
       }
     }
     else if (dynamic_cast<QVBoxLayout*>(layout()))
     {
       for (index=0; index<layout()->count(); ++index)
       {
         QWidget* w = layout()->itemAt(index)->widget();
         QRect r = w->geometry();
         if (r.top() + r.height()/2 > event->pos().y())
           break;
       }
     }
     dynamic_cast<QBoxLayout*>(layout())->insertWidget(index, source);
   }
   else if (dynamic_cast<FlowLayout*>(layout()))
   {
     for (index=0; index<layout()->count(); ++index)
     {
       QWidget* w = layout()->itemAt(index)->widget();
       QRect r = w->geometry();
       qDebug() << "check " << r << " " << event->pos();
       if (r.left()+r.width()/2 > event->pos().x()
         && r.top() < event->pos().y()
       && r.bottom() > event->pos().y())
       break;
     }
     dynamic_cast<FlowLayout*>(layout())->insertWidget(index, source);
   }
   source->updateGeometry();
   event->acceptProposedAction();
   setMinimumHeight(QMyApplication::elemHeight+8); // XX HACK
   onDrop();
   QMetaObject::invokeMethod(this, "forceSize", Qt::QueuedConnection);
   QMetaObject::invokeMethod(old, "forceSize", Qt::QueuedConnection);
 }

void DropZone::forceSize()
{
  if (!enableFixedSize)
    return;
  // force size
  if (multiline)
  {
    int he = 0;
    int wi = 0;
    for (int index=0; index<layout()->count(); ++index)
    {
      QWidget* line = layout()->itemAt(index)->widget();
      int cw = 0;
      int ch = 5;
      for (int index=0; index<line->layout()->count(); ++index)
      {
        QWidget* w = line->layout()->itemAt(index)->widget();
        Expr* expr = dynamic_cast<Expr*>(w);
        if (expr)
        {
          expr->onChange(); // restore full text
          cw += w->sizeHint().width() + 10;
        }
        else
        {
          cw += w->width()+10;
        }
        ch = std::max(w->height(), ch);
      }
      wi = std::max(wi, cw);
      he += ch + 5;
    }
    setMaximumHeight(std::max(he+20, 30));
    setMinimumHeight(std::max(he+20, 30));
    setMaximumWidth(wi+20);
    setMinimumWidth(wi+20);
  }
  else if (dynamic_cast<QVBoxLayout*>(layout()))
  {
    int sz = 0;
    int wi = 0;
    for (int index=0; index<layout()->count(); ++index)
    {
      QWidget* w = layout()->itemAt(index)->widget();
      Expr* expr = dynamic_cast<Expr*>(w);
      if (expr)
      {
        expr->onChange(); // restore full text
        sz += w->sizeHint().height() + 5;
        wi = std::max(wi, w->sizeHint().width() + 10);
      }
      else
      {
        sz += w->height();
        wi = std::max(wi, w->width()+10);
      }
    }
    qDebug() << "FORCE SIZE " << sz << " " << wi;
    
    setMaximumHeight(std::max(sz+100, 50));
    setMinimumHeight(std::max(sz+100, 50));
    setMaximumWidth(wi+20);
    setMinimumWidth(wi+20);
  }
  else if (dynamic_cast<FlowLayout*>(layout()))
  {
    int maxw = maxWidth;
    int minw = std::max(50, layout()->minimumSize().width());
    int baseh = std::max(30, layout()->heightForWidth(maxw));
    while (baseh > 100)
    {
      maxw += 100;
      baseh = std::max(30, layout()->heightForWidth(maxw));
    }
    while (maxw > minw && layout()->heightForWidth(maxw - 10) <= baseh)
      maxw -= 10;
    setMaximumWidth(maxw);
    setMinimumWidth(maxw);
    setMaximumHeight(baseh);
    setMinimumHeight(baseh);
    /*
    for (int h=30; h < 100; h += 5)
    {
      for (int w=minw; w < maxw; w+= 10)
      {
        int rh = layout()->heightForWidth(w);
        if (rh <= h)
        {
          setMaximumWidth(w);
          setMinimumWidth(w);
          setMaximumHeight(h);
          setMinimumHeight(h);
          goto done;
        }
      }
    }
    // failure
    setMaximumHeight(50);
    setMinimumHeight(50);
    setMaximumWidth(3000);
    setMinimumWidth(3000);
  done: {}
  */
  }
  else if (dynamic_cast<QHBoxLayout*>(layout()))
  {
    int sz = 0;
    for (int index=0; index<layout()->count(); ++index)
    {
      QWidget* w = layout()->itemAt(index)->widget();
      Expr* expr = dynamic_cast<Expr*>(w);
      if (expr)
        expr->onChange(); // restore full text
      sz += w->sizeHint().width() + 10;
    }
    if (maxWidth && maxWidth < sz)
    {
      sz = 0;
      for (int index=0; index<layout()->count(); ++index)
      {
        QWidget* w = layout()->itemAt(index)->widget();
        Expr* expr = dynamic_cast<Expr*>(w);
        if (expr)
        {
          QString s = expr->expression(expr->model->showText);
          // try to reduce s
          s.remove(' ');
          auto re = QRegularExpression();
          re.setPattern("\\b");
          QStringList list = s.split(re);
          for (int i=0; i<list.size(); ++i)
            list[i]=list[i].left(shortSize);
          s = list.join("");
          expr->defaultAction()->setText(s);
          // restore tooltip
          QString fullExpr = expr->expression(expr->model->templateExpression);
          expr->setToolTip(fullExpr);
          qDebug() << "EXPR " << s;
        }
        sz += w->sizeHint().width() + 10;
      }
    }
    setMaximumWidth(std::min(5000, std::max(sz, 50)));
    setMinimumWidth(std::min(5000, std::max(sz, 50)));
  }
}

void DropZone::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton
    && event->modifiers() && multiline)
  {
    bool remove = event->modifiers() & Qt::ControlModifier;
    int y = event->pos().y();
    for (int j=0,count=layout()->count(); j < count; ++j)
    {
      QWidget* w = layout()->itemAt(j)->widget();
      QRect r = w->geometry();
      if (r.bottom() >= y)
      {
        if (remove && !w->layout()->count() && j < count-1)
        {
          layout()->removeWidget(w);
        }
        if (!remove)
        {
          QWidget* w = new QWidget(this);
          w->setLayout(new QHBoxLayout());
          w->layout()->setContentsMargins(0,0,0,0);
          dynamic_cast<QHBoxLayout*>(w->layout())->setAlignment(Qt::AlignLeft);
          dynamic_cast<QBoxLayout*>(layout())->insertWidget(j, w);
        }
        break;
      }
    }
    return;
  }
  if (!hasFocus())
    setFocus();
  origin = event->pos();
  if (!rubberBand)
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
  rubberBand->setGeometry(QRect(origin, QSize()));
  rubberBand->show();
}

void DropZone::mouseMoveEvent(QMouseEvent *event)
{
  if (rubberBand)
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
}

void DropZone::mouseReleaseEvent(QMouseEvent *)
{
  if (!rubberBand)
    return;
  QRect r = rubberBand->geometry();
  rubberBand->hide();
  selection.clear();
  if (!multiline)
    for (int index=0; index<layout()->count(); ++index)
    {
      QWidget* w = layout()->itemAt(index)->widget();
      QRect wr = w->geometry();
      if (r.intersects(wr))
      {
        w->setStyleSheet("border-color: red;");
        selection.append(w);
      }
    }
  else
    for (int line=0; line<layout()->count(); ++line)
    {
      auto* ll = layout()->itemAt(line)->widget()->layout();
      auto y = layout()->itemAt(line)->widget()->geometry().top();
      for (int index=0; index<ll->count(); ++index)
      {
        QWidget* w = ll->itemAt(index)->widget();
        QRect wr = w->geometry();
        wr.translate(0, y);
        if (r.intersects(wr))
        {
          w->setStyleSheet("border-color: red;");
          selection.append(w);
        }
      }
    }
  // determine selection, for example using QRect::intersects()
  // and QRect::contains().
}

void DropZone::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_C && (e->modifiers() & Qt::ControlModifier))
  {
    while (!clipboard.isEmpty())
      delete clipboard.takeFirst();
    for (int i=0; i<selection.size(); ++i)
    {
      clipboard.append(dynamic_cast<Expr*>(selection[i])->clone(0));
    }
  }
  else if (e->key() == Qt::Key_X && (e->modifiers() & Qt::ControlModifier))
  {
    while (!clipboard.isEmpty())
      delete clipboard.takeFirst();
    for (int i=0; i<selection.size(); ++i)
    {
      clipboard.append(selection[i]);
      removeWidget(selection[i]);
      selection[i]->hide();
    }
  }
  else if (e->key() == Qt::Key_V && (e->modifiers() & Qt::ControlModifier))
  {
    if (clipboard.empty())
      return;
    for (int i=0; i<clipboard.size(); ++i)
    {
      addWidget(dynamic_cast<Expr*>(clipboard[i])->clone(this));
    }
  }
}

void DropZone::onTimer()
{
  if (multiline)
    for (int l=0; l < layout()->count(); ++l)
    {
      auto* ll =  layout()->itemAt(l)->widget()->layout();
      for (int index=0; index<ll->count(); ++index)
      {
        QWidget* w = ll->itemAt(index)->widget();
        w->setStyleSheet("border-color: black;");
      }
    }
  else
    for (int index=0; index<layout()->count(); ++index)
    {
      QWidget* w = layout()->itemAt(index)->widget();
      w->setStyleSheet("border-color: black;");
    }
  selection.clear();
}

void DropZone::focusInEvent(QFocusEvent*)
{
  timer->stop();
}

void DropZone::focusOutEvent(QFocusEvent* event)
{
  QWidget::focusOutEvent(event);
  timer->start(200);
}

void DropZone::setMultiLine()
{
  Q_ASSERT(layout()->count() == 0);
  Q_ASSERT(dynamic_cast<QVBoxLayout*>(layout()));
  multiline = true;
  // Initialize with one empty line
  addLine();
}

void DropZone::addWidget(QWidget* w)
{
  if (!multiline)
  {
    layout()->addWidget(w);
    return;
  }
  auto count = layout()->count();
  Q_ASSERT(count);
  if (count == 1)
  {
    addLine();
    ++count;
  }
  Q_ASSERT(count >= 2);
  QWidget* lasth = layout()->itemAt(count-2)->widget();
  lasth->layout()->addWidget(w);
}

void DropZone::removeWidget(QWidget* w)
{
  if (!multiline)
  {
    layout()->removeWidget(w);
    return;
  }
  for (int i=0; i<layout()->count(); ++i)
  {
    layout()->itemAt(i)->widget()->layout()->removeWidget(w);
  }
}

void DropZone::addLine()
{
  Q_ASSERT(multiline);
  QWidget* w = new QWidget(this);
  w->setLayout(new QHBoxLayout());
  w->layout()->setContentsMargins(0,0,0,0);
  dynamic_cast<QHBoxLayout*>(w->layout())->setAlignment(Qt::AlignLeft);
  layout()->addWidget(w);
}

DropZone::~DropZone()
{
  delete timer;
}
