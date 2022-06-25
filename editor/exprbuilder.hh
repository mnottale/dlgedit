#ifndef EXPRBUILDER_HH
# define EXPRBUILDER_HH
#include <QIcon>
#include <QMainWindow>
#include "ui_ExprBuilder.h"
class Expr;

class DeleteRequest: public QEvent
{
public:
  DeleteRequest(QWidget* who)
  : QEvent((QEvent::Type)1666)
  , who(who)
  {}
  virtual ~DeleteRequest() {}
  QWidget* who;
};

class ExprBuilder: public QMainWindow, private Ui::ExprBuilder
{
  Q_OBJECT
public:
  ExprBuilder();
  virtual bool event(QEvent* ev);
  Expr* expr;
  bool readonly;
  static ExprBuilder* builder;
  void setIcon(QString icon);
public slots:
  void setAll();
  void dragged(Qt::DropAction);
  void dropped();
  void preset();
  void reset();
};
#endif