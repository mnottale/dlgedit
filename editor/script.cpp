#include "script.hh"
#include <QScrollArea>
#include <QPlainTextEdit>


Script::Script(Node* owner) {
  this->node = owner;
  QScrollArea* qsa = new QScrollArea(this);
  qsa->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Expanding));
  setCentralWidget(qsa);
  this->dropZone = new DropZone(qsa);
  dropZone->setLayout(new QVBoxLayout());
  dropZone->setAcceptDrops(true);
  dropZone->enableFixedSize = true;
  dropZone->setMinimumSize(QSize(300, 200));
  dropZone->setMaximumSize(QSize(16777215, 65536));
  dropZone->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding));
  dropZone->layout()->setSizeConstraint(QLayout::SetDefaultConstraint);
  dropZone->setStyleSheet("Expr { background: transparent; border: none; margin:0px;padding:0px}");
  //qsa->setStyleSheet("background: blue;");
  qsa->setWidget(dropZone);
  read();
  capture();
}

void Script::closeEvent(QCloseEvent* e)
{
  onClose();
  QMainWindow::closeEvent(e);
}

void Script::capture()
{
  node->getPostConditions()->hide();
  node->getPostConditions()->setAcceptDrops(false);
  node->getPostConditions()->setStyleSheet("background: black");
}

void Script::release()
{
  node->getPostConditions()->show();
  node->getPostConditions()->setAcceptDrops(true);
  node->getPostConditions()->setStyleSheet("");
}

void Script::read()
{
  node->hook = this;
  QLayout* layout = node->getPostConditions()->layout();
  while (layout->count())
  {
    QWidget* w = layout->takeAt(0)->widget();
    w->setParent(dropZone);
    Expr* e = dynamic_cast<Expr*>(w);
    if (e)
      e->onChange(); // restore full text
    if (e && e->expression() == "TEXTAREA")
    {
      QPlainTextEdit* pte = dropZone->newSequence();
      pte->document()->setPlainText(e->selectedRhs);
      dropZone->layout()->addWidget(pte);
      dropZone->sequenceContentChanged(pte);
      delete e;
    }
    else
      dropZone->layout()->addWidget(w);
  }
}

void Script::write()
{
  QLayout* target = node->getPostConditions()->layout();
  QLayout* layout = dropZone->layout();
  while (layout->count())
  {
    QWidget* w = layout->takeAt(0)->widget();
    w->setParent(target->parentWidget());
    if (QPlainTextEdit* seq = dynamic_cast<QPlainTextEdit*>(w))
    {
      QSharedPointer<ExprModel> model = ExprModel::find("TEXTAREA");
      Expr* e = new Expr(node->getPostConditions());
      e->model = model;
      e->selectedRhs = seq->toPlainText();
      target->addWidget(e);
      delete w;
    }
    else
      target->addWidget(w);
  }
  node->hook = 0;
}

void Script::onClose()
{
  write();
  release();
}