#include <iostream>

#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QMouseEvent>
#include <QComboBox>
#include <QDir>
#include <QDrag>
#include <QMimeData>
#include "expr.hh"
#include "exprbuilder.hh"
#include "dropzone.hh"
#include "main.hh"

Expr::Expr(QWidget* parent)
: QToolButton(parent)
, model(ExprModel::make())
, async(false)
, popup(0)
{
  setMinimumHeight(QMyApplication::elemHeight);
  setMaximumHeight(QMyApplication::elemHeight);
  QAction* action = new QAction(this);
  setDefaultAction(action);
  action->connect(action, SIGNAL(triggered()), this, SLOT(onClick()));
}

QSharedPointer<ExprModel> ExprModel::make()
{
  auto res = new ExprModel();
  return models[res->uid];
}

ExprModel::ExprModel()
  : uid(QString::number((qulonglong)this)
    + QString::number(((qulonglong)QDateTime::currentDateTime().toSecsSinceEpoch())))
  {
    models[uid] = QSharedPointer<ExprModel>(this);
  }

void Expr::setIcon(QString path)
{
  model->iconPath = path;
  QIcon icon;
  icon.addFile(path, QSize(QMyApplication::elemHeight, QMyApplication::elemHeight));
  defaultAction()->setIcon(icon);
}

void Expr::setOperator(QString op)
{
  selectedOperator = op;
  onChange();
}

void Expr::setOperator(int idx)
{
  selectedOperator = model->operators[idx];
  onChange();
}

void Expr::setRhs(int idx)
{
  if (model->multipleSelection)
  {
    QString res = "";
    int i = 0;
    while(true)
    {
      QAbstractButton* button = rhsGroup->button(i);
      if (!button)
        break;
      if (button->isChecked())
      {
        if (res != "")
          res += ",";
        res += rhss[i];
      }
      ++i;
    }
    selectedRhs = res;
  }
  else
    selectedRhs = rhss[idx];
  onChange();
  if (popup && (
    !model->multipleSelection
    || (model->templateExpression == "quests"
        && quests.contains(quest)
        && (quests[quest][0] == "number" || quests[quest].size() == 2)
    )))
    popup->hide();
}

QString Expr::expression(QString templ)
{
  if (templ == "")
    templ = model->templateExpression;
  if (model->templateExpression == "quests")
  {
    if (quests.contains(quest) && quests[quest][0] == "number")
      return QString(quest +"  " + selectedOperator + " " + selectedRhs);
    else
      return QString(quest + ":" + selectedRhs);
  }
  QString v = QString(templ).replace("%o", selectedOperator).replace("%v", selectedRhs);
  for (int i=0; i < model->extras.size(); ++i)
  {
    v.replace("%" + QString().setNum(i), selectedExtras.value(i, ""));
  }
  v.replace("%'o", escapeString(selectedOperator))
   .replace("%'v", escapeString(selectedRhs));
  for (int i=0; i < model->extras.size(); ++i)
  {
    v.replace("%'" + QString().setNum(i), escapeString(selectedExtras.value(i, "")));
  }
  return v;
}

void Expr::onChange()
{
  qDebug() << "on change " << selectedOperator << "  " << selectedRhs;
  QString fullExpr = expression(model->templateExpression);

  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  if (model->showAll)
    defaultAction()->setText(fullExpr);
  else if (model->showValue)
    defaultAction()->setText(selectedOperator + " " + selectedRhs);
  else
    defaultAction()->setText(expression(model->showText));
  QFont f = defaultAction()->font();
  f.setUnderline(async);
  defaultAction()->setFont(f);
  setToolTip(fullExpr);
}

void Expr::drawOperators(QStringList const& operators, QButtonGroup* opGroup, QLayout* opLayout)
{
  if (model->lhsFreeText)
  {
    QLineEdit* line = new QLineEdit(selectedOperator);
    opLayout->addWidget(line);
    line->connect(line, SIGNAL(textChanged(QString)), this, SLOT(setOperator(QString)));
    line->connect(line, SIGNAL(returnPressed()), popup, SLOT(hide()));
  }
  for (int i=0; i<operators.count(); ++i)
  {
    if (operators[i] == "")
      continue;
    QPushButton* b = new QPushButton(model->operators[i]);
    opGroup->addButton(b, i);
    opLayout->addWidget(b);
  }
}

void Expr::drawRhs(QStringList const& rhss, QButtonGroup* rhsGroup, QWidget* w2)
{
  QStringList currentValues = selectedRhs.split(",");
  QLineEdit* line = 0;
  if (rhss.count() <= 20)
  {
    QVBoxLayout* rhsLayout = new QVBoxLayout(w2);
    if (model->freeText)
    {
      line = new QLineEdit(selectedRhs);
      rhsLayout->addWidget(line);
      line->connect(line, SIGNAL(textChanged(QString)), this, SLOT(setSelectedRhs(QString)));
      line->connect(line, SIGNAL(returnPressed()), popup, SLOT(hide()));
    }
    for (int i=0; i<rhss.count(); ++i)
    {
      if (rhss[i] == "")
        continue;
      QPushButton* b = new QPushButton(rhss[i]);
      b->setCheckable(true);
      if (currentValues.contains(rhss[i]))
        b->setChecked(true);
      rhsGroup->addButton(b, i);
      rhsLayout->addWidget(b);
    }
  }
  else
  { // use a grid to display all those buttons
    QGridLayout* rhsLayout = new QGridLayout(w2);
    int ft = model->freeText? 1:0;
    if (model->freeText)
    {
      line = new QLineEdit(selectedRhs);
      rhsLayout->addWidget(line, 0, 0);
      line->connect(line, SIGNAL(textChanged(QString)), this, SLOT(setSelectedRhs(QString)));
      line->connect(line, SIGNAL(returnPressed()), popup, SLOT(hide()));
    }
    for (int i=0; i<rhss.count(); ++i)
    {
      if (rhss[i] == "")
        continue;
      QPushButton* b = new QPushButton(rhss[i]);
      b->setCheckable(true);
      if (currentValues.contains(rhss[i]))
        b->setChecked(true);
      rhsGroup->addButton(b, i);
      rhsLayout->addWidget(b, (i+ft)%10, (i+ft)/10);
    }
  }
}

void Expr::setSelectedExtra(int buttonIndex)
{
  QObject* obj = QObject::sender();
  //obj must be one of our button groups
  int idx = extraGroups.indexOf(static_cast<QButtonGroup*>(obj));
  if (idx == -1)
    qDebug() << "##### object not found";
  while (selectedExtras.size() <= idx)
    selectedExtras.append("");
  selectedExtras[idx] = model->extras[idx][buttonIndex];
  onChange();
}

void Expr::setExtraText(QString const& str)
{
  QObject* obj = QObject::sender();
  // figure out which one this is
  int idx = extraFree.indexOf(static_cast<QLineEdit*>(obj));
  if (idx == -1)
    qDebug() << "#### extratext index not found";
  while (selectedExtras.size() <= idx)
    selectedExtras.append("");
  selectedExtras[idx] = str;
  onChange();
}

void Expr::setExtraCombo(QString const& str)
{
  QObject* obj = QObject::sender();
  int idx = extraCombos.indexOf(static_cast<QComboBox*>(obj));
  if (idx == -1)
    qDebug() << "#### combo index not found";
  while (selectedExtras.size() <= idx)
    selectedExtras.append("");
  selectedExtras[idx] = str;
  onChange();
}

void Expr::drawExtras()
{
  qDebug() << "drawExtras " << model->extras.size();
  extraGroups.clear();
  extraFree.clear();
  extraCombos.clear();
  for (int i=0; i< model->extras.size(); ++i)
  {
    extraFree.append(0);
    extraCombos.append(0);
    QWidget* w = new QWidget(popup);
    QVBoxLayout* layout = new QVBoxLayout(w);
    QButtonGroup* group = new QButtonGroup(popup);
    extraGroups.append(group);
    group->setExclusive(true);
    for (int j=0; j<model->extras[i].size(); ++j)
    {
      QString what = model->extras[i][j];
      if (what[0] == '%')
      {
        QStringList items;
        if (what.mid(1) == "quests" || what.mid(1) == "quests.txt")
        {
          items = quests.keys();
        }
        else
        {
          QFile f(what.mid(1));
          f.open(QIODevice::ReadOnly | QIODevice::Text);
          QTextStream in(&f);
          while (!in.atEnd()) {
            QString line = in.readLine();
            if (line == "")
            continue;
          items.append(line);
          }
        }
        items.sort();
        QComboBox* b = new QComboBox();
        b->addItems(items);
        int current = b->findText(selectedExtras.size() > i ? selectedExtras[i] : "");
        b->setCurrentIndex(current == -1? 0:current);
        connect(b, SIGNAL(currentIndexChanged(QString)), this, SLOT(setExtraCombo(QString)));
        extraCombos[i] = b;
        layout->addWidget(b);
        break;
      }
      if (what == "$free")
      {
        QLineEdit* le = new QLineEdit(w);
        le->setText(selectedExtras.size() > i ? selectedExtras[i] : "");
        layout->addWidget(le);
        le->connect(le, SIGNAL(textChanged(QString)), this, SLOT(setExtraText(QString)));
        extraFree[i] = le;
      }
      else if (what == "$combo")
      {
        QComboBox* b = new QComboBox();
        b->addItems(model->extras[i].mid(j+1));
        int current = b->findText(selectedExtras.size() > i ? selectedExtras[i] : "");
        b->setCurrentIndex(current == -1? 0:current);
        connect(b, SIGNAL(currentIndexChanged(QString)), this, SLOT(setExtraCombo(QString)));
        extraCombos[i] = b;
        layout->addWidget(b);
        break;
      }
      else
      {
        QPushButton* b = new QPushButton(what);
        b->setCheckable(true);
        if (selectedExtras.size() > i && selectedExtras[i] == what)
          b->setChecked(true);
        group->addButton(b, j);
        layout->addWidget(b);
      }
    }
    connect(group, SIGNAL(buttonClicked(int)), this, SLOT(setSelectedExtra(int)));
    l1->addWidget(w);
  }
}

void Expr::setQuestRhss(QString const& quest)
{
  this->quest = quest;
  selectedRhs = "";
  QWidget* w2 = new QWidget();
  QWidget* w1 = new QWidget();
  QVBoxLayout* opLayout = new QVBoxLayout(w1);
  QButtonGroup* opGroup = new QButtonGroup(popup);
  rhsGroup = new QButtonGroup(popup);
  rhsGroup->setExclusive(quests[quest][0] == "number" || quests[quest].size() == 2);
  qDebug() << "setEXclusive " << quests[quest][0] << " " << rhsGroup->exclusive();
  if (quests[quest][0] != "number")
    rhss = quests[quest];
  else
  {
    rhss = model->rhss;
    drawOperators(model->operators, opGroup, opLayout);
  }
  drawRhs(rhss, rhsGroup, w2);
  QLayoutItem* li =  popup->layout()->takeAt(1);
  delete li->widget();
  delete li;
  li =  popup->layout()->takeAt(1);
  delete li->widget();
  delete li;
  popup->layout()->addWidget(w1);
  popup->layout()->addWidget(w2);
  popup->adjustSize();
  connect(opGroup, SIGNAL(buttonClicked(int)), this, SLOT(setOperator(int)));
  connect(rhsGroup, SIGNAL(buttonClicked(int)), this, SLOT(setRhs(int)));
}

void Expr::onClick()
{
  if (QApplication::keyboardModifiers() == Qt::ControlModifier)
  {
    DropZone* dz = dynamic_cast<DropZone*>(parentWidget());
    if (!dz)
      dz = dynamic_cast<DropZone*>(parentWidget()->parentWidget());
    if (dz->selection.contains(this))
    {
      setStyleSheet("border-color: black;");
      dz->selection.removeOne(this);
    }
    else
    {
      setStyleSheet("border-color: red;");
      dz->selection.append(this);
    }
    if (!parentWidget()->hasFocus())
      parentWidget()->setFocus();
    return;
  }
  qDebug() << "OnClick " << this << " m " << model.data();
  setIcon(model->iconPath);
  delete popup;
  popup = new QWidget(this->parentWidget()->parentWidget());
  l1 = new QHBoxLayout(popup);
  drawExtras();
  QWidget* w0 = new QWidget();
  QWidget* w1 = new QWidget();
  QWidget* w2 = new QWidget();
  QVBoxLayout* questLayout = new QVBoxLayout(w0);
  QVBoxLayout* opLayout = new QVBoxLayout(w1);
  QButtonGroup* opGroup = new QButtonGroup(popup);
  asyncBox = new QCheckBox("async", popup);
  rhsGroup = new QButtonGroup(popup);
  QStringList currentValues = selectedRhs.split(",");
  rhss = model->rhss;
  if (model->templateExpression == "quests")
  {
    QComboBox* b = new QComboBox();
    b->addItems(quests.keys());
    int current = b->findText(quest);
    b->setCurrentIndex(current == -1? 0:current);
    quest = b->currentText();
    if (quests[quest][0] != "number")
      rhss = quests[quest];
    questLayout->addWidget(b);
    connect(b, SIGNAL(currentIndexChanged(QString)), this, SLOT(setQuestRhss(QString)));
    questLayout->addWidget(asyncBox);
  }
  if (model->templateExpression != "quests"
    || quests[quest][0] == "number")
  {
     drawOperators(model->operators, opGroup, opLayout);
     if (model->templateExpression != "quests")
       opLayout->addWidget(asyncBox);
  }
  rhsGroup->setExclusive(!model->multipleSelection
    || (
      model->templateExpression == "quests"
       && (quests[quest][0] == "number" || quests[quest].size() == 2)
      ));
  asyncBox->setChecked(async);
  connect(asyncBox, SIGNAL(clicked()), this, SLOT(setAsync()));
  drawRhs(rhss, rhsGroup, w2);
  connect(opGroup, SIGNAL(buttonClicked(int)), this, SLOT(setOperator(int)));
  connect(rhsGroup, SIGNAL(buttonClicked(int)), this, SLOT(setRhs(int)));
  l1->addWidget(w0);
  l1->addWidget(w1);
  l1->addWidget(w2);
  popup->setWindowFlags(Qt::Popup);
  QPoint p = QCursor::pos();
  popup->show();
  QSize sz = popup->size();
  /*qDebug() << "screen " << sn << " popup sz " << sz << " screen " <<rec;
  if (p.y() + sz.height() > rec.height()+rec.top())
  {
    p.setY(rec.top() + rec.height() - sz.height());
  }
  if (p.x() + sz.width() > rec.width() + rec.left())
    p.setX(rec.width() +rec.left() - sz.width());
  */
  popup->move(p);
}

void Expr::setAsync()
{
  async = asyncBox->isChecked();
  onChange();
}

void Expr::setSelectedRhs(QString const& str)
{
  selectedRhs = str;
  onChange();
}

void Expr::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    dragStartPosition = event->pos();
  else if (event->button() == Qt::RightButton
    && (event->modifiers() & Qt::ControlModifier))
  {
    auto* pw = parentWidget();
    if (auto* dz = dynamic_cast<DropZone*>(pw))
      dz->removeWidget(this);
    else
      parentWidget()->layout()->removeWidget(this);
    QApplication::postEvent(ExprBuilder::builder, new DeleteRequest(this));
    return;
  }
  QToolButton::mousePressEvent(event);
}

 void Expr::mouseMoveEvent(QMouseEvent *event)
 {
   if (!(event->buttons() & Qt::LeftButton))
     return;
   if ((event->pos() - dragStartPosition).manhattanLength()
     < QApplication::startDragDistance())
     return;
   QDrag *drag = new QDrag(this);
   QMimeData *mimeData = new QMimeData;

   mimeData->setText("Expr");
   drag->setMimeData(mimeData);
   // Only accept move if source is a dropzone
   DropZone* dz = dynamic_cast<DropZone*>(parentWidget());
   if (!dz)
     dz = dynamic_cast<DropZone*>(parentWidget()->parentWidget());

   Qt::DropAction dropAction = drag->exec(
     dz ? dz->allowedActions : (Qt::CopyAction| Qt::MoveAction));
   if (dropAction && dz)
     dz->emitOnDragged(dropAction);
 }

 Expr* Expr::clone(QWidget* parent)
 {
   Expr* result = new Expr(parent);
   result->model = model;
   result->quest = quest;
   result->selectedOperator = selectedOperator;
   result->selectedRhs = selectedRhs;
   result->selectedExtras = selectedExtras;
   result->async = async;
   result->setIcon(model->iconPath);
   result->onChange();
   return result;
 }

 QVariant ExprModel::serialize()
 {
   QMap<QString, QVariant> res;
   res["uid"] = uid;
   res["templateExpression"] = templateExpression;
   res["operators"] = QVariant(operators);
   res["rhss"] = QVariant(rhss);
   res["showValue"] = QVariant(showValue);
   res["showAll"] = QVariant(showAll);
   res["showText"] = QVariant(showText);
   res["freeText"] = QVariant(freeText);
   res["lhsFreeText"] = QVariant(lhsFreeText);
   res["icon"] = QVariant(iconPath);
   res["multipleSelection"] = QVariant(multipleSelection);
   for (int i=0; i<extras.size(); ++i)
     res["extra_" + QString().setNum(i)] = QVariant(extras[i]);
   return res;
 }

 QVariant Expr::serialize(QMap<QString, QVariant>& pointers)
 {
   QMap<QString, QVariant> res;
   res["modelId"] = model->uid;
   if (!pointers[model->uid].isValid())
     pointers[model->uid] = model->serialize();
   res["selectedOperator"] = QVariant(selectedOperator);
   res["selectedRhs"] = QVariant(selectedRhs);
   res["quest"] = QVariant(quest);
   res["async"] = QVariant(async);
   res["selectedExtras"] = QVariant(selectedExtras);
   return res;
 }

 Expr* Expr::deserialize(QVariant vdata, QWidget* parent)
 {
   Expr* result = new Expr(parent);
   QMap<QString, QVariant> data = vdata.toMap();
   QSharedPointer<ExprModel> model = ExprModel::models[data["modelId"].toString()];
   if (!model)
     qDebug() << "Model not found in library!";
   result->model = model;
   result->selectedOperator = data["selectedOperator"].toString();
   result->selectedRhs = data["selectedRhs"].toString();
   result->selectedExtras = data.value("selectedExtras", QStringList()).toStringList();
   result->quest = data["quest"].toString();
   result->async = data.value("async", false).toBool();
   result->setIcon(model->iconPath);
   result->onChange();
   return result;
 }

 void Expr::keyPressEvent(QKeyEvent* e)
 {
   if (e->key() == Qt::Key_Delete && (e->modifiers() & Qt::ShiftModifier))
   {
     parentWidget()->layout()->removeWidget(this);
     QApplication::postEvent(ExprBuilder::builder, new DeleteRequest(this));
   }
 }

 void ExprModel::deserialize(QVariant vdata)
 {
   QMap<QString, QVariant> data = vdata.toMap();
   uid = data["uid"].toString();
   templateExpression = data["templateExpression"].toString();
   iconPath = data["icon"].toString();
   showValue = data["showValue"].toBool();
   showAll = data["showAll"].toBool();
   showText = data.value("showText", "").toString();
   freeText = data["freeText"].toBool();
   lhsFreeText = data.value("lhsFreeText", false).toBool();
   operators = data["operators"].toStringList();
   multipleSelection  = data["multipleSelection"].toBool();
   rhss = data["rhss"].toStringList();
   extras.clear();
   for (int i=0; i<1000; ++i)
   {
     QStringList qsl = data.value("extra_" + QString().setNum(i), QStringList()).toStringList();
     if (qsl.empty())
       break;
     extras.append(qsl);
   }
 }


QMap<QString, QSharedPointer<ExprModel> > ExprModel::models;

void ExprModel::deserializeModels(QVariant vdata)
{
  QMap<QString, QVariant> data = vdata.toMap();
  QMap<QString, QVariant>::iterator it;
  for (it = data.begin(); it != data.end(); ++it)
  {
    if (models[it.key()])
      continue;
    auto model = ExprModel::make();
    model->deserialize(it.value());
    models[it.key()] = model;
  }
}


void Expr::loadQuests(QString path)
{
  QFile f(path);
  if (!f.exists())
  {
    QDir dir(".");
    qDebug() << dir.absolutePath();
    qDebug() << "File " << path << " does not exist.";
    return;
  }
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  quests.clear();
  QTextStream in(&f);
  while (!in.atEnd()) {
    QString line = in.readLine();
    if (line == "")
      continue;
    QStringList elements = line.split(' ');
    QString name = elements[0];
    elements.pop_front();
    quests[name] = elements;
  }
}

QMap<QString, QList<QString> > Expr::quests;

QSharedPointer<ExprModel> ExprModel::find(QString const& expr)
{
  QMap<QString, QSharedPointer<ExprModel> >::iterator it;
  for (it = models.begin(); it != models.end(); ++it)
  {
    if (it.value()->templateExpression == expr)
      return it.value();
  }
  return QSharedPointer<ExprModel>();
}




