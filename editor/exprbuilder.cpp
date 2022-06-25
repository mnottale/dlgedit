#include <QVBoxLayout>
#include "exprbuilder.hh"
#include "expr.hh"

ExprBuilder* ExprBuilder::builder = 0;

ExprBuilder::ExprBuilder()
{
  builder = this;
  setupUi(this);
  expression->setText("  %o %v");
  result->setLayout(new QVBoxLayout());
  expr = new Expr(result);
  readonly = false;
  result->layout()->addWidget(expr);
  connect(expression, SIGNAL(textChanged(QString)), this, SLOT(setAll()));
  connect(operators, SIGNAL(textChanged()), this, SLOT(setAll()));
  connect(extras, SIGNAL(textChanged()), this, SLOT(setAll()));
  connect(values, SIGNAL(textChanged()), this, SLOT(setAll()));
  connect(showAll, SIGNAL(clicked()), this, SLOT(setAll()));
  connect(showValue, SIGNAL(clicked()), this, SLOT(setAll()));
  connect(showText, SIGNAL(textChanged(QString)), this, SLOT(setAll()));
  connect(freeText, SIGNAL(clicked()), this, SLOT(setAll()));
  connect(lhsFreeText, SIGNAL(clicked()), this, SLOT(setAll()));
  connect(multipleSelection, SIGNAL(clicked()), this, SLOT(setAll()));
  connect(compButton,    SIGNAL(clicked()), this, SLOT(preset()));
  connect(eqButton,      SIGNAL(clicked()), this, SLOT(preset()));
  connect(tenButton,     SIGNAL(clicked()), this, SLOT(preset()));
  connect(hundredButton, SIGNAL(clicked()), this, SLOT(preset()));
  connect(result, SIGNAL(onDragged(Qt::DropAction)), this, SLOT(dragged(Qt::DropAction)));
  connect(result, SIGNAL(onDrop()), this, SLOT(dropped()));
  connect(newButton, SIGNAL(clicked()), this, SLOT(reset()));
  result->allowedActions = Qt::MoveAction;
}

void ExprBuilder::reset()
{
  QBoxLayout* l = (QBoxLayout*) result->layout();
  QLayoutItem* item = l->takeAt(0);
  delete item->widget();
  delete item;
  expr = new Expr(result);
  result->layout()->addWidget(expr);
}

void ExprBuilder::dragged(Qt::DropAction)
{
  qDebug() << "dragged";
  expr = new Expr(result);
  result->layout()->addWidget(expr);
}

void ExprBuilder::dropped()
{
  QBoxLayout* l = dynamic_cast<QBoxLayout*>(result->layout());
  qDebug() << "dropped " << l->count();
  Q_ASSERT(l->count() == 2);
  QLayoutItem* item = l->takeAt(0);
  Q_ASSERT(item);
  item->widget()->hide();
  delete item;
  qDebug() << "del " << l->count();
  expr = dynamic_cast<Expr*>(l->itemAt(0)->widget());
  Q_ASSERT(expr);
  qDebug() << "building " << expr << " m " << expr->model.data();
  readonly = true;
  showAll->setChecked(expr->model->showAll);
  showValue->setChecked(expr->model->showValue);
  showText->setText(expr->model->showText);
  freeText->setChecked(expr->model->freeText);
  lhsFreeText->setChecked(expr->model->lhsFreeText);
  multipleSelection->setChecked(expr->model->multipleSelection);
  expression->setText(expr->model->templateExpression);
  values->setPlainText(QStringList(expr->model->rhss).join("\n"));
  qDebug() << "ops " << QStringList(expr->model->operators).join(",");
  operators->setPlainText(QStringList(expr->model->operators).join("\n"));
  QString ex;
  for (int i=0; i<expr->model->extras.size(); ++i)
    ex += QStringList(expr->model->extras[i]).join(" ") + "\n";
  ex.resize(ex.size()-1);
  extras->setPlainText(ex);
  readonly = false;
}


void ExprBuilder::preset()
{
  QPushButton* b = dynamic_cast<QPushButton*>(sender());
  if (b == compButton)
    operators->setPlainText("<\n<=\n==\n!=\n>=\n>");
  else if (b == eqButton)
    operators->setPlainText("==\n!=");
  else if (b == tenButton)
    values->setPlainText("0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10");
  else if (b == hundredButton)
  {
    QString v;
    for (int i=0; i<100; ++i)
      v += QString().setNum(i) + "\n";
    values->setPlainText(v);
  }
}

void ExprBuilder::setIcon(QString icon)
{
  expr->setIcon(icon);
}

void ExprBuilder::setAll()
{
  qDebug() << "builder setAll " << expr << " m " << expr->model.data();
  if (readonly)
    return;
  expr->model->operators = operators->toPlainText().split("\n");
  expr->model->rhss = values->toPlainText().split("\n");
  expr->model->templateExpression = expression->text();
  expr->model->showAll = showAll->isChecked();
  expr->model->showValue = showValue->isChecked();
  expr->model->freeText = freeText->isChecked();
  expr->model->lhsFreeText = lhsFreeText->isChecked();
  expr->model->multipleSelection = multipleSelection->isChecked();
  expr->model->showText = showText->text();
  expr->model->extras.clear();
  if (extras->toPlainText() != "")
  {
    QStringList sl = extras->toPlainText().split("\n");
    for (int i=0; i<sl.size(); ++i)
      expr->model->extras.append(sl[i].split(" "));
  }
  expr->onChange();
}

bool ExprBuilder::event(QEvent* e)
{
  if (e->type() == 1666)
  {
    delete dynamic_cast<DeleteRequest*>(e)->who;
    return true;
  }
  else
    return QMainWindow::event(e);
}