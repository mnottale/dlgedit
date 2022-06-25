#ifndef EXPR_HH
# define EXPR_HH

#include <QDebug>
#include <QToolButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QAction>
#include <QSharedPointer>
#include <QDateTime>
#include <QCheckBox>
#include <QComboBox>


QString escapeString(QString const& input);

struct ExprModel
{
  ExprModel();
  QString templateExpression;
  QList<QString> operators;
  QList<QString> rhss;
  QList<QList<QString> > extras;
  QString iconPath;
  bool showValue;
  bool showAll;
  QString showText;
  bool freeText;
  bool lhsFreeText;
  bool multipleSelection;
  QString uid;
  QVariant serialize();
  void deserialize(QVariant data);
  static QMap<QString, QSharedPointer<ExprModel> > models;
  static void deserializeModels(QVariant models);
  static QSharedPointer<ExprModel> find(QString const& expr);
  static QSharedPointer<ExprModel> make();
};

class Expr: public QToolButton
{
  Q_OBJECT
public:
  Expr(QWidget* parent);
  QSharedPointer<ExprModel> model;
  QString quest;
  QString selectedOperator;
  QString selectedRhs;
  QList<QString> selectedExtras;
  bool async;

  QWidget* popup;
  QPoint dragStartPosition;
  QString expression(QString templ = QString());
  void setIcon(QString iconPath);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  Expr* clone(QWidget* parent);

  QVariant serialize(QMap<QString, QVariant>& addresses);
  static Expr* deserialize(QVariant data, QWidget* parent);
  static QMap<QString, QList<QString> > quests;
  static void loadQuests(QString file);
public slots:
  void onChange();
  void onClick();
  void setOperator(int idx);
  void setOperator(QString txt);
  void setRhs(int idx);
  void setSelectedRhs(QString const& str);
  void setQuestRhss(QString const& v);
  void setAsync();
  void setSelectedExtra(int);
  void setExtraText(QString const& str);
  void setExtraCombo(QString const& str);
private:
  void drawExtras();
  void drawRhs(QStringList const& rhs, QButtonGroup* rhsGroup, QWidget* parent);
  void drawOperators(QStringList const& ops, QButtonGroup* group, QLayout* layout);
  QButtonGroup* rhsGroup;
  QStringList rhss;
  QCheckBox* asyncBox;
  QHBoxLayout* l1;
  QList<QButtonGroup*> extraGroups;
  QList<QLineEdit*> extraFree;
  QList<QComboBox*> extraCombos;
};

#endif