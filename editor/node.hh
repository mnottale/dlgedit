#ifndef NODE_HH
# define NODE_HH

#include <QWidget>
#include <QTimer>
#include <functional>
#include "ui_dlgedit.h"

class QKeyEvent;
class QPlainTextEdit;
class QScrollArea;
class Script;
class Node: public QWidget, private Ui::Node
{
  Q_OBJECT
public:
  Node();
  bool colapsed;
  QVariant serialize(QMap<QString, QVariant>& ptrs);
  static Node* deserialize(QVariant data);
  QVariant toDialog();
  QVariant rootToDialog();

  Node* parent();
  Node* childAt(int index);
  int parentIndex();
  void addChild(Node* n, int index = -1);
  int childCount();
  void removeFromParent();
  void moveDown();
  void moveUp();
  void moveTop();
  void moveBottom();
  QScrollArea* parentScroll();
  void toSequence();
  void forEach(std::function<void (Node*)> f);
  void forEachPre(std::function<void (Expr*)> f);
  void color(QString const& c);
  Node* find(QString const& id);

  DropZone* getPostConditions() { return postConditions;}
  static QString getExprCode(Expr* expr);

  Script* hook;
protected:
  void keyPressEvent(QKeyEvent* event);
  void paintEvent(QPaintEvent*);
  void dragEnterEvent(QDragEnterEvent* event);
  void dragLeaveEvent(QDragLeaveEvent* event);
public slots:
  void onDeleteNodeClick(bool);
  void onAddChildClick(bool);
  void onColapseClick(bool);
  void reLayout();
  void dropTimerElapsed();
  void sequenceContentChanged();
  void refreshSequence();
  void onSayEdited(QString);
  void onSayFocus();
  void onSayCursor(int, int);
  void collapseAll();
  void expandAll();
private:
  void pcSpeaks(bool v);
  bool _pcSpeaks;
  QPlainTextEdit* sequence;
  QTimer dropTimer;
  QString id;
  static QList<Node*> clipboard;
  friend class DialogFind;
  friend class Dialog;
};

QString toDialog(QVariant data, bool indent);


#endif
