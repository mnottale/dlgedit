#include <QFile>
#include <QVariant>
#include <QStringList>
#include <QDebug>
#include <QMap>
#include <QDir>
#include <QToolBar>

#include "expr.hh"
#include "toolset.hh"
#include "imagelibrary.hh"
#include "main.hh"
#include "exprbuilder.hh"
#include <cassert>
#include <iostream>


enum IType
{
  TYPE_NONE,
  TYPE_IF,
  TYPE_WHEN
};
struct INode
{
  INode() : parent(0) {init();}
  INode(INode*p): parent(p){init();}
  void init()
  {
    type = TYPE_NONE;
    cIf = cElse = 0;
    next = 0; prev = 0;
    exit = false;
  }
  bool empty()
  {
    return type == TYPE_NONE && !next && label.size()==0
    && conditions.empty() && effects.empty()
    && jumpto.size() == 0 && text.size() == 0
    && pjtext.size() == 0;
  }
  IType type;
  INode* parent;
  INode* cIf;
  INode* cElse;
  QList<INode*> cWhen;
  INode* prev;
  INode* next;
  QString label;
  QString jumpto;
  QString text;
  QString pjtext;
  QList<QString> conditions;
  QList<QString> effects;
  bool exit;
  void prune();
  void show(int d = 0);
};

QMap<QString, INode*> dialogs;
INode* root = 0;
INode* current = 0;
QString last_who;

QSharedPointer<ExprModel> model_id, model_jumpto, model_custom, model_quests;

void INode::show(int d)
{
  QString indent(d, ' ');
  qDebug() << indent << (type==TYPE_IF? "if" : (type==TYPE_WHEN ? "co": "ot")) << label << text;
  if (cIf)
    cIf->show(d+2);
  if (cElse)
    cElse->show(d+2);
  if (next)
    next->show(d);
}

void INode::prune()
{
  if (next)
    next->prune();
  if (cIf)
    cIf->prune();
  if (cElse)
    cElse->prune();
  for (int i=0; i<cWhen.size(); ++i)
    cWhen[i]->prune();
  if (parent && !next && type == TYPE_NONE && label.size() == 0 && text.size() == 0
    && pjtext.size() == 0 && conditions.empty() && effects.empty()
    && parent->cIf != this && parent->cElse != this
    && parent->cWhen.indexOf(this) == -1)
  {
    if (prev)
      prev->next = 0;
  }
}

INode* transform(INode* src, QString jtarget)
{
  if (!src)
    return 0;
  /*
  if (src->type  == TYPE_IF)
  {
    if (src->child)
    if (src->child->next && src->child->next->next)
    { // should be empty
      assert(!src->child->next->next->next);
      assert(!src->child->next->next->child);
    }
    transform(src->child, jtarget);
    //transform(src->child->next);
    transform(src->next, jtarget);
  }
  else if (src->isChoices)
  {
    assert(src->child);
    transform(src->child, jtarget);
  }
  else
  {
    assert(!src->child);
    transform(src->next, jtarget);
  }*/
  return src;
}

void process_line(QString line)
{
  QStringList l = line.split('^');
  QString cmd = l[8];
  QString args = l[9];
  QString who = l[4];
  QStringList whosplit = who.split(' ');
  who = whosplit[whosplit.size()-1];
  int level = 0;
  while (args[0] == '.' && args[1] == ' ')
  {
    ++level;
    args = args.right(args.size() -2);
  }

  if (last_who != who)
  {
    if (last_who != "variables")
    {
      if (root && last_who=="ruinarde_04")
      {
        root->prune();
        root->show();
        transform(root, "");
      }
    }
    root = current = new INode();
    last_who = who;
    dialogs[who] = root;
  }
  int depth =0;
  INode* p = current;
  while (p->parent)
  {
    ++depth;
    p = p->parent;
  }
  //qDebug() << level << ' ' << depth <<' ' << who << ' ' << cmd << ' ' << args;
  if (cmd == "Show Text")
  {
    if (args.left(5) == "Face:")
      return;
    current->text += args + " ";
  }
  else if (cmd == "Label")
  {
    //current->next = new INode(current->parent);
    //current = current->next;
    current->label = args;
  }
  else if (cmd == "Comment")
    ;
  else if (cmd == "Jump to Label")
    current->jumpto = args;
  else if (cmd == "Show Choices")
  {
    assert(current->type == TYPE_NONE);
    current->type = TYPE_WHEN;
    current->cWhen.append(new INode(current));
    current = current->cWhen.back();
  }
  else if (cmd == "When [**]")
  {
    assert(current->parent->type == TYPE_WHEN);
    if (!current->empty())
    {
      current = current->parent;
      current->cWhen.append(new INode(current));
      current = current->cWhen.back();
    }
    current->pjtext = args.right(args.size() - 5);
  }
  else if (cmd == "End Show Choices")
  {
    assert(current->parent->type == TYPE_WHEN);
    current = current->parent;
    current->next = new INode(current->parent);
    current->next->prev = current;
    current = current->next;
  }
  else if (cmd == "Conditional Branch")
  {
    assert(current->type == TYPE_NONE);
    current->type = TYPE_IF;
    current->conditions += "if: " + args.right(args.size() -3);
    current->cIf = new INode(current);
    current = current->cIf;
  }
  else if (cmd == "Else")
  {
    assert(current->parent->type == TYPE_IF);
    current = current->parent;
    current->cElse = new INode(current);
    current = current->cElse;
  }
  else if (cmd == "End Conditional Branch")
  {
    assert(current->parent->type == TYPE_IF);
    current = current->parent;
    current->next = new INode(current->parent);
    current->next->prev = current;
    current = current->next;
  }
  else if (cmd == "Exit Event Processing")
  {
    current->exit = true;
  }
  else if (cmd == "Control Variables")
    current->effects.append("var: " + args);
  else if (cmd == "Control Switches" || cmd == "Control Self Switch")
  {
    current->effects.append("switch: " + args);
  }
  else if (cmd == "Control Self Switches")
  {
    current->effects.append("switch: " + args);
  }
  else
    qDebug() << "unknown command " << cmd;
}


//Ex:   var: Variable 2501 [ruinard_0x_event] += 5
QMap<QString, QString> newQuests;
QVariant to_expr_effect(QString text, QMap<QString, QVariant>& ptrs)
{
  QStringList comps = text.split(" ");
  Expr e(0);
  if (comps[0] == "var:" || comps[0] == "if:" || comps[0] == "switch:")
  {
    if (comps[1] == "Variable")
    { // var: Variable 2501 [ruinard_0x_event] += 5
      e.model = model_quests;
      e.quest = comps[3].left(comps[3].size()-1).right(comps[3].size()-2);
      e.selectedOperator = comps[4];
      e.selectedRhs = comps[5];
      newQuests[e.quest] = "number";
    }
    else if (comps[1] == "Switch")
    { // if: Switch 0021 [pj_noise_bcp] is ON
      // switch: Switch 1201 [wastes_lore_01] = ON
      e.model = model_quests;
      e.quest = comps[3].left(comps[3].size()-1).right(comps[3].size()-2);
      e.selectedRhs = comps[5];
      e.selectedOperator = (comps[0] == "if:") ? "==" : "=";
      e.selectedRhs = comps[5];
      newQuests[e.quest] = "OFF ON";
    }
  }
  else
  { // unrecognized stuff, go in custom
    e.model = model_custom;
    e.selectedRhs = text;
  }
  return e.serialize(ptrs);
}

QVariant serialize(INode* n, QMap<QString, QVariant>& ptrs);
QList<QVariant> serialize0(INode* n, QMap<QString, QVariant>& ptrs)
{
  QList<QVariant> res;
  while (n)
  {
    res.append(serialize(n, ptrs));
    n = n->next;
  }
  return res;
}

QVariant serialize(INode* n, QMap<QString, QVariant>& ptrs)
{
  QMap<QString, QVariant> res;
  res["pcSpeaks"] = n->pjtext.size() != 0;
  res["say"] = n->pjtext.size() ==0? n->text : n->pjtext;
  QList<QVariant> children;
  if (n->label.size() != 0)
  {
    Expr e(0);
    e.model = model_id;
    e.selectedRhs = n->label;
    children.append(e.serialize(ptrs));
  }
  for (int i=0; i<n->conditions.size(); ++i)
  {
    children.append(to_expr_effect(n->conditions[i], ptrs));
  }
  res["preConditions"] = children;
  children.clear();
  if (n->jumpto.size() != 0)
  {
    Expr e(0);
    e.model = model_jumpto;
    e.selectedRhs = n->jumpto;
    children.append(e.serialize(ptrs));
  }
  for (int i=0; i<n->effects.size(); ++i)
  {
    children.append(to_expr_effect(n->effects[i], ptrs));
  }
  res["postConditions"] = children;
  children.clear();
  res["colapsed"] = false;
  if (n->type == TYPE_IF)
  {
    children += serialize0(n->cIf, ptrs);
    children += serialize0(n->cElse, ptrs);
  }
  else if (n->type == TYPE_WHEN)
  {
    for (int i=0; i<n->cWhen.size(); ++i)
      children += serialize0(n->cWhen[i], ptrs);
  }
  res["children"] = children;
  return res;
}



int QMyApplication::elemHeight = 32;

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


int main(int argc, char** argv)
{
  QMyApplication app(argc, argv);
  Expr::loadQuests("quests.txt");
  ToolSet ts;
  ts.load("base.tbx");

  // locate expr models for id and jumpto
  qDebug() << "Scanning " << ExprModel::models.size();
  QMap<QString, QSharedPointer<ExprModel> >::iterator it
    = ExprModel::models.begin();
  while (it != ExprModel::models.end())
  {
    QSharedPointer<ExprModel> m = it.value();
    if (m->templateExpression == "id: %v")
      model_id = m;
    else if (m->templateExpression == "jumpto: %v")
      model_jumpto = m;
    else if (m->templateExpression == "%v")
      model_custom = m;
    else if (m->templateExpression == "quests")
      model_quests = m;
    ++it;
  }

  QFile file(argv[1]);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return 0;
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    process_line(line);
  }
  QMap<QString, INode*>::iterator i = dialogs.begin();
  while (i != dialogs.end()) {
    qDebug() << i.key();
    QFile file(i.key() + ".dlg");
    file.open(QIODevice::WriteOnly);
    QDataStream stream(&file);
    QMap<QString, QVariant> ptrs;
    QList<QVariant> children = serialize0(i.value(), ptrs);
    QMap<QString, QVariant> res;
    res["pcSpeaks"] = true;
    res["say"] = "";
    res["colapsed"] = false;
    res["preconditions"] = res["postConditions"] = res["children"] = QList<QVariant>();
    res["children"] = children;
    stream << ptrs << QVariant(res);
    i++;
  }
  {
    QMap<QString, QString>::iterator it = newQuests.begin();
    while (it != newQuests.end())
    {
      std::cout << it.key().toStdString() <<' ' << it.value().toStdString() << std::endl;
      it++;
    }
  }
}