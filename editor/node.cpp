#include <iostream>
#include <functional>
#include <QScrollArea>
#include <QScrollBar>
#include "node.hh"
#include "exprbuilder.hh"
#include "previewer.hh"
#include "script.hh"
#include <QRegularExpression>

static int nextId = 0;
Node::Node()
{
  sequence = 0;
  hook = 0;
  _pcSpeaks = true;
  colapsed = false;
  setupUi(this);
  showChildren->hide();
  childrenBar->hide();
  preConditions->setLayout(new FlowLayout()); //new QHBoxLayout());
  preConditions->enableFixedSize = true;
  preConditions->layout()->setContentsMargins(0,0,0,0);
  //postConditions->setLayout(new FlowLayout()); // QHBoxLayout());
  //postConditions->layout()->setContentsMargins(0,0,0,0);
  postConditions->setLayout(new QVBoxLayout());
  postConditions->layout()->setContentsMargins(0,0,0,0);
  postConditions->setMultiLine();
  postConditions->enableFixedSize = true;
  preConditions->hide();
  postConditions->hide();
  say->setFrame(false);
  sayLayout->setAlignment(addChildButton, Qt::AlignLeft);
  connect(say, SIGNAL(textEdited(QString)), SLOT(onSayEdited(QString)));
  connect(say, SIGNAL(cursorPositionChanged(int, int)), SLOT(onSayCursor(int, int)));

  if (QMyApplication::textPixelWidth)
    toSequence();

  connect(addChildButton, SIGNAL(clicked(bool)), SLOT(onAddChildClick(bool)));
  connect(showChildren, SIGNAL(clicked(bool)), SLOT(onColapseClick(bool)));
  //connect(deleteNode, SIGNAL(clicked(bool)), SLOT(onDeleteNodeClick(bool)));
  setFocusPolicy(Qt::ClickFocus);
  connect(&dropTimer, SIGNAL(timeout()), SLOT(dropTimerElapsed()));
}

void Node::onSayCursor(int, int)
{
  onSayFocus();
}
void Node::onSayFocus()
{
  if (say)
    previewer->preview(say->text());
  else
    sequenceContentChanged();
}
void Node::onSayEdited(QString val)
{
  previewer->preview(val);
}

void Node::pcSpeaks(bool v)
{
  _pcSpeaks = v;
  QWidget* target = say ? (QWidget*)say : (QWidget*)sequence;
  if (_pcSpeaks)
    target->setStyleSheet(QString::fromUtf8("color: black; background-color: rgb(210,210,255);"));
  else
    target->setStyleSheet(QString::fromUtf8("color: black; background-color: rgb(255,255,210);"));
  target->setToolTip(_pcSpeaks? "pc" : "npc");
}

void Node::sequenceContentChanged()
{
  // BIG WTF, document height is in line, not pixels
  int h = sequence->document()->size().height();
  QFontMetrics m (sequence->font()) ;
  int r = m.lineSpacing();
  h++;
  sequence->setMinimumHeight(h*r);
  sequence->setMaximumHeight(h*r);

  QString txt = sequence->toPlainText();
  int pos = sequence->textCursor().position();
  int crcount = 0;
  for (int i=0; i<pos; ++i)
    if (txt[i] == '\n')
      ++crcount;
  QString zone = txt.section('\n', crcount, crcount);
  QString beg = zone.section(' ', 0, 0);
  if (beg.endsWith(":"))
    zone = zone.section(' ', 1, -1);
  onSayEdited(zone);
}

void Node::toSequence()
{
  QString text = say->text();
  delete say;
  say = 0;
  sequence = new QPlainTextEdit(this);
  sequence->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  if (QMyApplication::textPixelWidth)
  {
    qDebug() << "extra width " << sequence->verticalScrollBar()->width();
    int w = QMyApplication::textPixelWidth + 16;
    sequence->setMinimumWidth(w);
    sequence->setMaximumWidth(w);
  }
  sequence->setPlainText(text);
  sequence->connect(sequence->document(), SIGNAL(contentsChanged()), this, SLOT(sequenceContentChanged()));
  connect(sequence, SIGNAL(cursorPositionChanged()), this, SLOT(onSayFocus()));
  sayLayout->insertWidget(0, sequence);
  sayLayout->setAlignment(sequence, Qt::AlignLeft);
  sequenceContentChanged();
}

void Node::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Return && (e->modifiers() & Qt::ShiftModifier))
  {
    if (say)
    {
      toSequence();
    }
    else
    {
      QString text = sequence->toPlainText().replace("\n", " ");
      delete sequence;
      sequence = 0;
      say = new QLineEdit();
      say->setText(text);
      say->setFrame(false);
      sayLayout->insertWidget(0, say);
      connect(say, SIGNAL(textEdited(QString)), SLOT(onSayEdited(QString)));
      connect(say, SIGNAL(cursorPositionChanged(int, int)), SLOT(onSayCursor(int, int)));
    }
    pcSpeaks(_pcSpeaks);
  }
  else if (e->key() == Qt::Key_Space && (e->modifiers() & Qt::ShiftModifier))
    reLayout();
  else if (e->key() == Qt::Key_Return)
  {
    qDebug() << "return pc=" << _pcSpeaks <<" " << !_pcSpeaks;
    pcSpeaks(!_pcSpeaks);
  }
  else if (e->key() == Qt::Key_Space)
    onColapseClick(true);
  else if (e->key() == Qt::Key_H)
    onColapseClick(false);
  else if (e->key() == Qt::Key_Down && (e->modifiers() & Qt::ShiftModifier))
    moveDown();
  else if (e->key() == Qt::Key_Up && (e->modifiers() & Qt::ShiftModifier))
    moveUp();
  else if (e->key() == Qt::Key_Left && (e->modifiers() & Qt::ShiftModifier))
    moveTop();
  else if (e->key() == Qt::Key_Right && (e->modifiers() & Qt::ShiftModifier))
    moveBottom();
  else if (e->key() == Qt::Key_Delete && (e->modifiers() & Qt::ShiftModifier))
  {
    hide();
    clipboard.append(this);
    removeFromParent();
  }
  else if (e->key() == Qt::Key_C && (e->modifiers() & Qt::ControlModifier))
  {
    QMap<QString, QVariant> ptrs;
    QVariant v = serialize(ptrs);
    clipboard.append(Node::deserialize(v));
  }
  else if (e->key() == Qt::Key_X && (e->modifiers() & Qt::ControlModifier))
  {
    hide();
    clipboard.append(this);
    removeFromParent();
  }
  else if (e->key() == Qt::Key_V && (e->modifiers() & Qt::ControlModifier))
  {
    if (clipboard.empty())
      return;
    Node* n = clipboard.back();
    QMap<QString, QVariant> ptrs;
    QVariant v = n->serialize(ptrs);
    Node* copy = Node::deserialize(v);
    addChild(copy);
    copy->show();
  }
  else if (e->key() == Qt::Key_S)
  {
    (new Script(this))->show();
  }
}

void Node::moveUp()
{
  Node* p = parent();
  int pidx = parentIndex();
  if (pidx > 0)
  {
    removeFromParent();
    p->addChild(this, pidx - 1);
  }
  reLayout();
  setFocus();
  parentScroll()->ensureWidgetVisible(this);
}

void Node::moveDown()
{
  Node* p = parent();
  int pidx = parentIndex();
  if (pidx < p->childCount()-1)
  {
    removeFromParent();
    p->addChild(this, pidx + 1);
  }
  reLayout();
  setFocus();
  parentScroll()->ensureWidgetVisible(this);
}

void Node::moveTop()
{
  if (parent()->parent() == 0)
    return;
  removeFromParent();
  parent()->parent()->addChild(this, parent()->parentIndex() + 1);
  reLayout();
  setFocus();
  parentScroll()->ensureWidgetVisible(this);
}

void Node::moveBottom()
{
  int i = parentIndex();
  if (i == 0)
    return;
  removeFromParent();
  parent()->childAt(i-1)->addChild(this);
  reLayout();
  setFocus();
  parentScroll()->ensureWidgetVisible(this);
}


Node* Node::parent()
{
  return dynamic_cast<Node*>(parentWidget());
}

Node* Node::childAt(int index)
{
  return dynamic_cast<Node*>(childrenLayout->itemAt(index)->widget());
}

int Node::parentIndex()
{
  Node* p = parent();
  for (int i=0; i<p->childCount(); ++i)
  {
    if (p->childAt(i) == this)
      return i;
  }
  std::cerr << "parentIndex failed" << std::endl;
  return -1;
}

int Node::childCount()
{
  return childrenLayout->count();
}

void Node::removeFromParent()
{
  parent()->childrenLayout->removeWidget(this);
}

void Node::addChild(Node* n, int index)
{
  if (index == -1)
    index = childCount();
  childrenLayout->insertWidget(index, n);
  showChildren->show();
  childrenBar->show();
}

void Node::onAddChildClick(bool )
{
  if (colapsed)
    onColapseClick(true);
  Node* child = new Node();
  childrenLayout->addWidget(child);
  showChildren->show();
  childrenBar->show();
  std::cerr << "add child" << std::endl;
  child->show();
  reLayout();
  updateGeometry();
  child->updateGeometry();
  child->pcSpeaks(false /*!_pcSpeaks requested by DM*/);
}

void Node::onDeleteNodeClick(bool)
{
  Node* parent = dynamic_cast<Node*>(parentWidget());
  parent->childrenLayout->removeWidget(this);
  QApplication::postEvent(ExprBuilder::builder, new DeleteRequest(this));
}

void Node::onColapseClick(bool hijack)
{
  if (!hijack && QApplication::keyboardModifiers() == Qt::ShiftModifier)
  {
    if (colapsed)
      expandAll();
    else
      collapseAll();
    return;
  }
  colapsed = !colapsed;
  std::cerr << colapsed << " " << childrenLayout->count() <<  std::endl;

  if (colapsed)
  {
    QString tooltip;
    showChildren->setText("+");
    for (int i=0; i<childrenLayout->count(); ++i)
    {
      childrenLayout->itemAt(i)->widget()->hide();
      Node* n = dynamic_cast<Node*>(childrenLayout->itemAt(i)->widget());
      QVariant v = n->toDialog();
      tooltip += "\n" + ::toDialog(v, true);
    }
    auto re = QRegularExpression();
    re.setPattern("[a-zA-Z]");
    tooltip = QStringList(tooltip.split("\n")
      .filter(re)
      .mid(0,10)).join("\n");
    showChildren->setToolTip(tooltip);
  }
  else
  {
    showChildren->setText("-");
    for (int i=0; i<childrenLayout->count(); ++i)
    {
      childrenLayout->itemAt(i)->widget()->show();
    }
  }
  reLayout();
}

void Node::collapseAll()
{
  if (!colapsed)
    onColapseClick(true);
  for (int i=0; i<childrenLayout->count(); ++i)
  {
    dynamic_cast<Node*>(childrenLayout->itemAt(i)->widget())->collapseAll();
  }
}

void Node::expandAll()
{
  if (colapsed)
    onColapseClick(true);
  for (int i=0; i<childrenLayout->count(); ++i)
  {
    dynamic_cast<Node*>(childrenLayout->itemAt(i)->widget())->expandAll();
  }
}

void Node::reLayout()
{
  if (!preConditions->layout()->count() && !postConditions->layout()->count())
  {
    preConditions->hide();
    postConditions->hide();
  }
  else
  {
    postConditions->forceSize();
    preConditions->forceSize();
  }
  childrenLayout->invalidate();
  hChildrenLayout->invalidate();
  layout()->invalidate();
  QWidget *parent = this;
  QWidget *prev = this;
  if (sequence)
  {
    //int h = sequence->document()->size().height();
    //sequence->setMinimumHeight(h);
  }
  while (parent) {
    if (dynamic_cast<QScrollArea*>(parent->parentWidget()))
    {
      qDebug() << "RESIZE " << parent->size().width();
      prev->resize(parent->parentWidget()->size().width() - 10, size().height());
      break;
    }
    if (Node* parentNode = dynamic_cast<Node*>(parent))
    {
      parentNode->childrenLayout->invalidate();
      parentNode->hChildrenLayout->invalidate();
      parentNode->layout()->invalidate();
    }
    parent->adjustSize();
    prev = parent;
    parent = parent->parentWidget();
  }
}

QVariant Node::serialize(QMap<QString, QVariant>& ptrs)
{
  Script* savedHook = hook;
  if (savedHook)
    savedHook->write();
  QMap<QString, QVariant> result;
  if (say)
    result["say"] = say->text();
  else
    result["say"] = sequence->toPlainText();
  auto serialize = [&](DropZone* source) {
    QList<QVariant> children;
    QLayout* layout = source->layout();
    qDebug() << "serializing dropzone, ml=" << source->isMultiLine();
    if (source->isMultiLine())
    {
      children.append("multiline");
      for (int l=0; l < layout->count(); ++l)
      {
        QLayout* ll = layout->itemAt(l)->widget()->layout();
        if (!ll->count())
          continue;
        for (int i=0; i<ll->count(); ++i)
          children.append(dynamic_cast<Expr*>(ll->itemAt(i)->widget())->serialize(ptrs));
        if (l < layout->count()-1)
          children.append("line");
      }
    }
    else
    {
      for (int i=0; i<layout->count(); ++i)
        children.append(dynamic_cast<Expr*>(layout->itemAt(i)->widget())->serialize(ptrs));
    }
    return children;
  };
  result["preConditions"] = serialize(preConditions);
  result["postConditions"] = serialize(postConditions);
  QList<QVariant> children;
  auto* layout = childrenLayout;
  for (int i=0; i<layout->count(); ++i)
  {
    children.append(dynamic_cast<Node*>(layout->itemAt(i)->widget())->serialize(ptrs));
  }
  result["children"] = children;

  result["colapsed"] = colapsed;
  result["pcSpeaks"] = _pcSpeaks;
  if (savedHook)
    savedHook->read();
  return result;
}

void Node::refreshSequence()
{
  sequenceContentChanged();
  sequence->verticalScrollBar()->setValue(
    sequence->verticalScrollBar()->minimum());
}

Node* Node::deserialize(QVariant vdata)
{
  QMap<QString, QVariant> data = vdata.toMap();
  Node* result = new Node();
  result->colapsed = data["colapsed"].toBool();
  QString sayText = data["say"].toString();
  if (sayText.indexOf("\n") == -1 && sayText.length() < 90 && result->say)
    result->say->setText(data["say"].toString());
  else
  {
    if (!result->sequence)
      result->toSequence();
    result->sequence->setPlainText(sayText);
    result->sequence->verticalScrollBar()->setValue(
      result->sequence->verticalScrollBar()->minimum());
    QMetaObject::invokeMethod(result, "refreshSequence", Qt::QueuedConnection);
  }
  result->pcSpeaks(data["pcSpeaks"].toBool());
  auto deserialize = [&](QList<QVariant> conds, DropZone* target) {
    if (!conds.count())
      return;
    auto qv = conds.at(0);
    if (qv.canConvert<QString>() && qv == "multiline")
    { // box mode
      qDebug() << "Box mode deserialization: " << (conds.count()-1) << " lines";
      for (int l=1; l < conds.count(); ++l)
      {
        auto qv = conds.at(l);
        if (qv.canConvert<QString>() && qv == "line")
          target->addLine();
        else
        {
          Expr* expr = Expr::deserialize(qv, target);
          target->addWidget(expr);
          target->setMinimumHeight(QMyApplication::elemHeight + 12);
        }
      }
    }
    else
    { // bare mode
      qDebug() << "bare mode deserialization: " << conds.count() << " entries";
      for (int i=0; i<conds.count(); ++i)
      {
        Expr* expr = Expr::deserialize(conds.at(i), target);
        target->addWidget(expr);
        target->setMinimumHeight(QMyApplication::elemHeight + 12);
      }
    }
    dynamic_cast<DropZone*>(target)->forceSize();
  };
  QList<QVariant> conds = data["preConditions"].toList();
  deserialize(data["preConditions"].toList(), result->preConditions);
  deserialize(data["postConditions"].toList(), result->postConditions);

  conds = data["children"].toList();
  for (int i=0; i<conds.count(); ++i)
  {
    Node* n = Node::deserialize(conds.at(i));
    result->childrenLayout->addWidget(n);
  }
  if (conds.count())
  {
    result->showChildren->show();
    result->childrenBar->show();
  }
  if (result->preConditions->layout()->count() || result->postConditions->layout()->count())
  {
    result->preConditions->show();
    result->postConditions->show();
  }
  return result;
}

QVariant Node::rootToDialog()
{
  QList<QVariant> children;
  for (int i=0; i<childrenLayout->count(); ++i)
  {
    Node* node = dynamic_cast<Node*>(
      childrenLayout->itemAt(i)->widget());
    children.append(node->toDialog());
  }
  return children;
}

QString processStack(QStringList& v)
{
  if (v.empty())
    return "true";
  QString front = v.front();
  bool op = (front == "||" || front == "&&");
  v.pop_front();
  if (op)
  {
    QString lhs = processStack(v);
    QString rhs = processStack(v);
    return "(" + lhs + " " + front + " "  + rhs + ")";
  }
  else
    return front;
}

Node* Node::find(QString const& sid)
{
  if (id == sid)
    return this;
  for (int i=0; i<childrenLayout->count(); ++i)
  {
    Node* r = dynamic_cast<Node*>(childrenLayout->itemAt(i)->widget());
    r = r->find(sid);
    if (r)
      return r;
  }
  return 0;
}

QString Node::getExprCode(Expr* expr)
{
  QString expression = expr->expression();
  if ( expression.indexOf("jumpto:")!=-1
    || expression.indexOf("priority:")!=-1
    || expression.indexOf("disabledHint:")!=-1
    || expression == "single"
    || expression == "once"
    || expression == "OR"
    || expression == "AND")
    return "";

  if (expr->model->templateExpression == "quests")
  {
    QString qname = expr->quest;
    if (Expr::quests.contains(qname) && Expr::quests[qname][0] == "number")
    {
      QString e = "quests.GetNumber(\"" + qname + "\") " + expr->selectedOperator + expr->selectedRhs;
      return e;
    }
    else
    {
      QStringList values = expr->selectedRhs.split(',');
      for (int i=0; i<values.size(); ++i)
      {
        if (values[i] == "default")
          values[i] = "\"default\",\"\"";
        else
          values[i] = '"' + values[i] + '"';
      }
      QString e = "contains(quests.Get(\"" + qname + "\"), " + values.join(",") + ")";
      //QString e = "quests.Get(\"" + qname + "\") in [" + values.join(",") + "]";
      return e;
    }
  }
  else
    return expr->expression();
}

QList<QVariant> sequenceToDialog(QString data)
{
  // sequence markers present
  QList<QVariant> rseq;
  QString speaker;
  QString buffer;
  bool isEffect = false;
  bool speakerSet = false;
  //superduper hack to avoid duplicating flush code outside loop
  data.append("\n:");
  QStringList items = data.split("\n");
  for (int i=0; i<items.size(); ++i)
  {
    QString line = items[i];
    if (line.size() && line[0] == ':')
    { // flush buffer maybe
      if (!isEffect && buffer.size() && speaker != "void")
      {
        buffer = buffer.remove(buffer.size()-1, 1);
        //buffer = buffer.replace("\"", "\\\"").replace("\n", "\\n");
        // remove trailing \n
        QMap<QString, QVariant> map;
        if (!speakerSet)
          rseq.append(buffer);
        else
        {
          if (speaker == "pnj")
            map["say"] = buffer;
          else
            map["speak"] = speaker + ")" + buffer;
          rseq.append(map);
        }
      }
      // set new mode
      buffer = "";
      speaker = "";
      isEffect = (line == ":effects" || line == ":effect");
      if (!isEffect)
      {
        speaker = line.remove(0, 1);
        speakerSet = true;
      }
    }
    else
    {
      if (isEffect)
      {
        QMap<QString, QVariant> map;
        map["effect"] = line; // .replace("\"", "\\\"");
        rseq.append(map);
      }
      else
        buffer += line + "\n";
    }
  }
  return rseq;
}

QVariant Node::toDialog()
{
  Script* savedHook = hook;
  if (savedHook)
    savedHook->write();
  QMap<QString, QVariant> result;
  if (say)
  {
    if (say->text() != "")
      result[_pcSpeaks?"pjsay":"say"] = say->text(); // .replace("\"", "\\\"");
  }
  else
  {
    if (QMyApplication::textPixelWidth)
    {
      QString data = sequence->toPlainText();
      if (data.size() && (data[0] == ':' || data.contains("\n:")))
      {
        result["sequence"] = sequenceToDialog(data);
      }
      else
      {
        if (sequence->toPlainText() != "")
          result[_pcSpeaks?"pjsay":"say"] = sequence->toPlainText();
            //.replace("\"", "\\\"")
            //.replace("\n", "\\n");
      }
    }
    else
    {
       // sequence handler
       QStringList seq = sequence->toPlainText().split("\n");
       QList<QVariant> rseq;
       for (int i=0; i<seq.count(); ++i)
       {
         QString elem = seq[i];
         int sep = elem.indexOf(':');
         if (sep == -1)
         {
           rseq.append(elem);
           continue;
         }
         QMap<QString, QVariant> map;
         map[elem.left(sep)] = elem.right(elem.length() - sep - 1);
         rseq.append(map);
       }
       result["sequence"] = rseq;
    }
  }
  id = QString().setNum(++nextId);
  result["id"] = id;
  if (preConditions->layout()->count())
  {
    QStringList condStack;
    int opCount = 0;
    for (int i=0; i<preConditions->layout()->count(); ++i)
    {
      Expr* expr = dynamic_cast<Expr*>(
        preConditions->layout()->itemAt(i)->widget());
      if (expr->expression().indexOf("id:")!=-1)
      {
        result["id"] = expr->selectedRhs;
        id = expr->selectedRhs;
      }

      else if (expr->expression().indexOf("disabledHint:")!=-1)
        result["disabledHint"] = expr->selectedRhs;
      else if (expr->expression().indexOf("jumpto:")!=-1)
        result["jumpto"] = expr->selectedRhs;
      else if (expr->expression().indexOf("priority:")!=-1)
        result["priority"] = expr->selectedRhs;
      else if (expr->expression() == "single")
        result["single"] = 1;
      else if (expr->expression() == "once")
        result["once"] = 1;
      else if (expr->expression() == "OR")
      {
        condStack.push_back("||");
        opCount++;
      }
      else if (expr->expression() == "AND")
      {
        condStack.push_back("&&");
        opCount++;
      }
      else
      {
        QString cmd = getExprCode(expr);
        condStack.push_back(cmd); //.replace("\"", "\\\""));
      }
    }
    // process condstack
    bool frontor = !condStack.empty() && condStack.front() == "||";
    while (opCount < condStack.size()-opCount - 1)
    {
      condStack.push_front(frontor? "||" : "&&");
      ++opCount;
    }
    QString cond = processStack(condStack);
    qDebug() << cond;
    result["condition"] = cond;
  }
  if (postConditions->layout()->count())
  {
    QList<QVariant> effect;
    QList<QVariant> effectSequence;
    bool toSequence = false;
#define PUSH(e) if (toSequence) {      \
      QMap<QString, QVariant> map;     \
      map["effect"] = e;               \
      effectSequence.append(map);      \
    } else {                           \
      effect.append(QVariant(e));      \
    }

    // check for a TEXTAREA that will force us into sequence mode
    /* This feature makes no sense anymore.
    for (int i=0; i<postConditions->layout()->count(); ++i)
    {
      Expr* expr = dynamic_cast<Expr*>(
        postConditions->layout()->itemAt(i)->widget());
      if (expr->expression() == "TEXTAREA")
      {
        toSequence = true;
        break;
      }
    }*/
    for (int l=0; l<postConditions->layout()->count(); ++l)
    {
      auto* ll = postConditions->layout()->itemAt(l)->widget()->layout();
      if (!ll->count())
        continue;
      QList<QVariant> line;
      for (int i=0; i<ll->count(); ++i)
      {
        Expr* expr = dynamic_cast<Expr*>(
          ll->itemAt(i)->widget());
        Q_ASSERT(expr);
        if (expr->expression(expr->model->templateExpression).indexOf("jumpto:")!=-1)
          result["jumpto"] = expr->selectedRhs;
        else if (expr->expression() == "TEXTAREA")
        {
          QString data = expr->selectedRhs;
          effectSequence.append(sequenceToDialog(data));
        }
        else if (expr->model->templateExpression == "quests")
        {
          QString qname = expr->quest;
          if (Expr::quests.contains(qname) && Expr::quests[qname][0] == "number")
          {
            QString value = expr->selectedRhs;
            int v = value.toInt();
            QString op = expr->selectedOperator;
            QString e;
            if (op == "=")
              e = "quests.Set(\"" + qname + "\",\"" + value + "\")";
            if (op == "-=")
              e = "quests.Inc(\"" + qname + "\"," + QString::number(-v) + ")";
            if (op == "+=")
              e = "quests.Inc(\"" + qname + "\"," + QString::number(v) + ")";
            line.append(e);
          }
          else
          {
            QString v = expr->selectedRhs;
            if (v == "default") v = "";
            QString e = "quests.Set(\"" + qname + "\",\"" + expr->selectedRhs + "\")";
            line.append(e);
          }
        }
        else if (expr->expression() == "detached")
          result["detached"] = true;
        else
        {
          QString e = expr->expression(); // .replace("\"", "\\\"");
          if (expr->async)
            e = "makeAsync(" + e + ")";
          line.append(e);
        }
      }
      if (!line.empty())
      {
        PUSH(line);
      }
    }
    if (!toSequence)
      result["effect"] = QVariant(effect);
    else
    {
      if (result.contains("sequence"))
      {
        QList<QVariant> s = result["sequence"].toList();
        s.append(effectSequence);
        result["sequence"] = QVariant(s);
      }
      else
        result["sequence"] = QVariant(effectSequence);
    }
  }
  if (childrenLayout->count())
  {
    QList<QVariant> children;
    bool pcSpeak = true; // the list should be homogenous
    for (int i=0; i<childrenLayout->count(); ++i)
    {
      Node* node = dynamic_cast<Node*>(
        childrenLayout->itemAt(i)->widget());
      children.append(node->toDialog());
      pcSpeak = node->_pcSpeaks;
    }
    result[pcSpeak?"choices":"list"] = children;
  }
  if (savedHook)
    savedHook->read();
  return result;
}

void Node::paintEvent(QPaintEvent *)
{
  QStyleOption opt;
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Node::dragEnterEvent(QDragEnterEvent* event)
{
  event->accept();
  preConditions->show();
  postConditions->show();
}

void Node::dragLeaveEvent(QDragLeaveEvent*)
{
  if (preConditions->layout()->count() || postConditions->layout()->count())
    return;
  dropTimer.setSingleShot(true);
  dropTimer.start(1500);
}

void Node::dropTimerElapsed()
{
  if (preConditions->layout()->count() || postConditions->layout()->count())
    return;
  preConditions->hide();
  postConditions->hide();
  reLayout();
}

QScrollArea* Node::parentScroll()
{
  QWidget* parent = this;
  while (true)
  {
    QScrollArea* sa = dynamic_cast<QScrollArea*>(parent);
    if (sa)
      return sa;
    parent = parent->parentWidget();
  }
}

QString indented(int indent)
{
  if (indent < 0)
    return "";
  else
    return QString(indent, ' ');
}

QString escapeString(QString const& input)
{
   QString res;
   for (int i=0; i<input.length(); ++i)
   {
     auto c = input[i];
     if (c == '\\' || c == '"')
     {
       res += '\\';
       res += c;
     }
     else if (c=='\n')
     {
       res += '\\';
       res += 'n';
     }
     else
       res += c;
   }
   return res;
}

void toDialog(QString& result, QVariant data, int indent)
{
  switch(data.type())
  {
    case QVariant::Map:
    {
      result += "{ \n";
      QMap<QString, QVariant> map = data.toMap();
      for (QMap<QString, QVariant>::iterator it = map.begin(); it != map.end(); ++it)
      {
        result += indented(indent) + "\"" + it.key() + "\" : ";
        toDialog(result, it.value(), indent>=0? indent + 1:-1);
        result += ",\n";
      }
      result = result.left(result.length() -2);
      result += "\n" + indented(indent) + "}";
      break;
    }
    case QVariant::List:
    {
      QList<QVariant> list = data.toList();
      result += "[ \n";
      for (int i=0; i<list.count(); ++i)
      {
        result += indented(indent);
        toDialog(result, list[i], indent>=0? indent + 1 : -1);
        result += ",\n";
      }
      result = result.left(result.length() -2);
      result += "\n" + indented(indent) + "]";
      break;
    }
  case QVariant::String:
    result += "\"" + escapeString(data.toString()) + "\"";
    break;
  case QVariant::Bool:
    result += data.toBool()?"1":"0";
    break;
  case QVariant::Int:
  case QVariant::Double:
    result += QString::number(data.toDouble());
    break;
  default:
    result += "mystery " + QString::number(data.type());
    break;
  }
}

QString toDialog(QVariant data, bool indent)
{
  QString result;
  toDialog(result, data, indent?0:-1);
  return result;
}

void Node::forEach(std::function<void (Node*)> f)
{
  f(this);
  for (int i=0; i<childrenLayout->count(); ++i)
  {
    Node* node = dynamic_cast<Node*>(
      childrenLayout->itemAt(i)->widget());
    node->forEach(f);
  }
}

void Node::forEachPre(std::function<void (Expr*)> f)
{
  for (int i=0; i<preConditions->layout()->count(); ++i)
  {
    Expr* node = dynamic_cast<Expr*>(
      preConditions->layout()->itemAt(i)->widget());
    f(node);
  }
}


void Node::color(QString const& c)
{
  QWidget* tgt = say?(QWidget*)say:(QWidget*)sequence;
  QString ss;
  if (_pcSpeaks)
    ss = "color: black; background-color: rgb(240,240,255);";
  else
    ss = "color: black; background-color: rgb(255,255,240);";

  if (c == "")
    tgt->setStyleSheet(ss);
  else
    tgt->setStyleSheet(ss + "border-style: solid; border-color: " + c + "; border-width: 2px;");
}

QList<Node*> Node::clipboard;
