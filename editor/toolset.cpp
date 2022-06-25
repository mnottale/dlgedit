#include <iostream>
#include <QDebug>
#include <QFileDialog>
#include <QDataStream>
#include <QInputDialog>
#include <QGroupBox>
#include "toolset.hh"
#include "dropzone.hh"
#include "flowlayout.h"

ToolSet::ToolSet()
{
  setupUi(this);
  root = new DropZone(this);
  setCentralWidget(root);
  root->setLayout(new QVBoxLayout());
  connect(action_New, SIGNAL(triggered()), this, SLOT(onActionNew()));
  connect(action_Open, SIGNAL(triggered()), this, SLOT(onActionOpen()));
  connect(action_Save, SIGNAL(triggered()), this, SLOT(onActionSave()));
  connect(actionSave_as, SIGNAL(triggered()), this, SLOT(onActionSaveAs()));
  connect(actionNew_group, SIGNAL(triggered()), this, SLOT(onActionNewGroup()));
  connect(&backupTimer, SIGNAL(timeout()), this, SLOT(onBackupTimer()));
  backupTimer.setSingleShot(false);
  backupTimer.start(60000);
}

void ToolSet::onActionNewGroup()
{
  bool ok = false;
  QString name = QInputDialog::getText(this, "Group name", "Enter group name",
     QLineEdit::Normal, "", &ok);
  if (!ok)
    return;
  makeGroup(name, root);
  //root->layout()->invalidate();
  //root->layout()->itemAt(0)->widget()->layout()->invalidate();
}

QWidget* ToolSet::makeGroup(QString name, QWidget* parent)
{
  /*
  FlowLayout* fl = new FlowLayout();
  DropZone* dz = new DropZone(root);
  dz->setLayout(fl);
  root->layout()->addWidget(dz);
  dz->allowedActions = Qt::CopyAction;
  dz->setAcceptDrops(true);
  dz->setStyleSheet(
    "DropZone {"
    "border: 1px solid black;"
    "}");
    */
  QGroupBox *groupBox = new QGroupBox(name);
  groupBox->setFlat(true);
  QVBoxLayout* fl1 = new QVBoxLayout();
  fl1->setSizeConstraint(QLayout::SetMinAndMaxSize);
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  groupBox->setSizePolicy(sizePolicy);
  DropZone* dz = new DropZone(parent);
  FlowLayout* fl2 = new FlowLayout();
  fl2->setSizeConstraint(QLayout::SetMinAndMaxSize);
  groupBox->setLayout(fl1);
  groupBox->setMinimumHeight(50);
  fl1->addWidget(dz);
  dz->setLayout(fl2);
  dz->setSizePolicy(sizePolicy);
  parent->layout()->addWidget(groupBox);
  dz->allowedActions = Qt::CopyAction;
  dz->setAcceptDrops(true);

  return dz;
}

void ToolSet::onBackupTimer()
{
  if (path != "")
    doSave(path + "#");
}

void ToolSet::onActionNew()
{
  ToolSet* dlg = new ToolSet();
  dlg->show();
}

void ToolSet::onActionOpen()
{
  QString where = QFileDialog::getOpenFileName(this, "File to load", "",
    "Toolbox files (*.tbx)");
  if (where == "")
    return;
  load(where);
}
QWidget* ToolSet::loadContent()
{
  QWidget* res = new QWidget(0);
  QFile file(path);
  file.open(QIODevice::ReadOnly);
  QDataStream in(&file);
  QList<QVariant> widgets;
  QMap<QString, QVariant> ptrs;
  in >> ptrs >> widgets;
  ExprModel::deserializeModels(ptrs);

  res->setLayout(new QVBoxLayout());
  QWidget* curgroup = 0;
  for (int i=0; i<widgets.count(); ++i)
  {
    QVariant w = widgets.at(i);
    if (w.type() == QVariant::String)
    {
      curgroup = makeGroup(w.toString(), res);
      continue;
    }
    if (!curgroup)
      curgroup = makeGroup("default", res);
    Expr* e = Expr::deserialize(w, res);
    curgroup->layout()->addWidget(e);
  }
  return res;
}

void ToolSet::load(QString where)
{
  path = where;
  delete root;
  root = loadContent();
  setCentralWidget(root);
}

void ToolSet::onActionSave()
{
  doSave("");
}

void ToolSet::doSave(QString path)
{
  if (path == "")
    path = this->path;
  if (getenv("DLGEDIT_CLONE_MODELS"))
  {
    QMap<QString, QSharedPointer<ExprModel> >::iterator it = ExprModel::models.begin();
    while (it != ExprModel::models.end())
    {
      it.value()->uid = QString::number((qulonglong)&it.value())
      + QString::number(((qulonglong)QDateTime::currentDateTime().toSecsSinceEpoch()));
      ++it;
    }
  }
  if (path == "")
  {
    onActionSaveAs();
    return;
  }
  QList<QVariant> val;
  QMap<QString, QVariant> ptrs;
  for (int i=0; i<root->layout()->count(); ++i)
  {
    QGroupBox* gb = &dynamic_cast<QGroupBox&>(*root->layout()->itemAt(i)->widget());
    val.append(gb->title());
    QLayout* db = gb->layout()->itemAt(0)->widget()->layout();
    for (int j=0; j<db->count(); ++j)
    {
      Expr* item = dynamic_cast<Expr*>(db->itemAt(j)->widget());
      if (!item)
      {
        qCritical()  << "item is not an expr!";
        continue;
      }
      QVariant v = item->serialize(ptrs);
      val.append(v);
    }
  }
  QFile file(path);
  file.open(QIODevice::WriteOnly);
  QDataStream stream(&file);
  stream << ptrs << val;
}

void ToolSet::onActionSaveAs()
{
  path = QFileDialog::getSaveFileName(this, "File to save to", "",
    "Toolbox files (*.tbx)");
  if (path == "")
    return;
  onActionSave();
}
