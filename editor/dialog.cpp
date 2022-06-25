#include <iostream>
#include <QScrollArea>
#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QLabel>
#include <QSettings>
#include <QInputDialog>
#include <QSplitter>
#include "dialog.hh"
#include "exprbuilder.hh"
#include "imagelibrary.hh"
#include "main.hh"
#include "toolset.hh"


QList<Dialog*> Dialog::dialogs;
QString Dialog::lastUsedPath;

DialogFind::DialogFind(Dialog* owner)
: QMainWindow()
{
  setWindowTitle("find in " + owner->path);
  this->owner = owner;
  QWidget* c = new QWidget(this);
  QLayout* vl = new QVBoxLayout(c);
  text = new QLineEdit(c);
  find = new QPushButton(c);
  find->setText("find");
  findNext = new QPushButton(c);
  findNext->setText("find next");
  vl->addWidget(text);
  vl->addWidget(find);
  vl->addWidget(findNext);
  c->setLayout(vl);
  setCentralWidget(c);
  connect(find, SIGNAL(clicked()), this, SLOT(onFindClicked()));
  connect(findNext, SIGNAL(clicked()), this, SLOT(onFindNextClicked()));
}

void DialogFind::onFindClicked()
{
  stack.clear();
  findone(owner->root);
}

void DialogFind::onFindNextClicked()
{
  if (!lastMatch)
    onFindClicked();
  else
    findone(lastMatch, true);
}

void DialogFind::findone(Node* current, bool skip)
{
  QString search = text->text();
  while (true)
  {
    if (!skip)
    {
      QString t = current->say ? current->say->text() : current->sequence->toPlainText();
      int p = t.indexOf(search, Qt::CaseInsensitive);
      if (p != -1)
      { // found one
        current->parentScroll()->ensureWidgetVisible(current);
        if (current->say)
          current->say->setSelection(p, search.length());
        else
        {
          QTextCursor c = current->sequence->textCursor();
          c.setPosition(p);
          c.setPosition(p + search.length(), QTextCursor::KeepAnchor);
          current->sequence->setTextCursor(c);
        }
        lastMatch = current;
        return;
      }
    }
    skip = false;
    QLayout* layout = current->childrenLayout;
    if (layout->count())
    {
      current = dynamic_cast<Node*>(layout->itemAt(0)->widget());
      stack.push_back(0);
    }
    else
    {
      while (true)
      {
        current = dynamic_cast<Node*>(current->parentWidget());
        if (!current)
        {
          lastMatch = 0;
          return;
        }
        if (current->childrenLayout->count() <= stack.back()+1)
        {
          stack.pop_back();
          continue;
        }
        stack.back() += 1;
        QWidget* next = current->childrenLayout->itemAt(stack.back())->widget();
        current = dynamic_cast<Node*>(next);
        break;
      }
    }
  }
}


Dialog::Dialog()
{
  dialogs.append(this);
  setupUi(this);
  find = 0;
  socket = 0;
  root = new Node();
  scroll = new QScrollArea(this);
  setCentralWidget(scroll);
  scroll->setWidget(root);
  connect(actionIcon16, SIGNAL(triggered()), this, SLOT(onActionIcon16()));
  connect(actionIcon24, SIGNAL(triggered()), this, SLOT(onActionIcon24()));
  connect(actionIcon32, SIGNAL(triggered()), this, SLOT(onActionIcon32()));
  connect(actionFont, SIGNAL(triggered()), this, SLOT(onActionFont()));
  connect(actionBuilder, SIGNAL(triggered()), this, SLOT(onActionBuilder()));
  connect(actionIcons, SIGNAL(triggered()), this, SLOT(onActionIcons()));
  connect(action_New, SIGNAL(triggered()), this, SLOT(onActionNew()));
  connect(action_Open, SIGNAL(triggered()), this, SLOT(onActionOpen()));
  connect(action_Save, SIGNAL(triggered()), this, SLOT(onActionSave()));
  connect(actionSave_as, SIGNAL(triggered()), this, SLOT(onActionSaveAs()));
  connect(actionHelp, SIGNAL(triggered()), this, SLOT(onActionHelp()));
  connect(action_Find, SIGNAL(triggered()), this, SLOT(onActionFind()));
  connect(actionQuit, SIGNAL(triggered()), this, SLOT(onActionQuit()));
  connect(actionCollapse_all, SIGNAL(triggered()), this, SLOT(collapseAll()));
  connect(actionExpand_all, SIGNAL(triggered()), this, SLOT(expandAll()));
  connect(actionFixed_text_layout, SIGNAL(triggered()), this, SLOT(onActionFixedTextLayout()));
  connect(actionFlow_text_layout, SIGNAL(triggered()), this, SLOT(onActionFlowTextLayout()));
  connect(actionAttach_toolbox, SIGNAL(triggered()), this, SLOT(onActionAttachToolbox()));
  connect(actionDropzone_max_width, SIGNAL(triggered()), this, SLOT(onDropzoneMaxWidth()));
  connect(actionDropzone_short_size, SIGNAL(triggered()), this, SLOT(onDropzoneShortSize()));
  connect(actionConnect_DDB, SIGNAL(triggered()), this, SLOT(onActionConnectDDB()));
  connect(&backupTimer, SIGNAL(timeout()), this, SLOT(onBackupTimer()));
  connect(&dirtyTimer, SIGNAL(timeout()), this, SLOT(onDirtyTimer()));
}

Dialog::~Dialog()
{
  qDebug() << "~qdialog";
  dialogs.removeOne((Dialog*)this);
}

void Dialog::collapseAll()
{
  root->collapseAll();
}

void Dialog::expandAll()
{
  root->expandAll();
}

void Dialog::onActionFind()
{
  if (!find)
  {
    find = new DialogFind(this);
  }
  find->show();
}

void Dialog::onActionIcon16()
{
  QMyApplication::elemHeight = 16;
}
void Dialog::onActionIcon24()
{
  QMyApplication::elemHeight = 24;
}
void Dialog::onActionIcon32()
{
  QMyApplication::elemHeight = 32;
}


void Dialog::onActionFont()
{
  bool ok;
  QFont font = QFontDialog::getFont(&ok, scroll->font());
  if (ok)
  {
    scroll->setFont(font);
    root->setFont(font);
    QApplication::setFont(font);
    QMyApplication::computeTextSize();
  }
}

void Dialog::onActionFlowTextLayout()
{
  QMyApplication::textPixelWidth = 0;
  QMyApplication::textCharacterWidth = 0;
  QMyApplication::textLines = 0;
}

void Dialog::onDropzoneMaxWidth()
{
  bool ok = false;
  QString l = QInputDialog::getText(this, "Max width",
                                       "max dropzone width",
                                       QLineEdit::Normal, "", &ok);
  if (!ok)
    return;
  DropZone::maxWidth = l.toInt();
}

void Dialog::onDropzoneShortSize()
{
  bool ok = false;
  QString l = QInputDialog::getText(this, "Short size",
                                       "max dropzone short size",
                                       QLineEdit::Normal, "", &ok);
  if (!ok)
    return;
  DropZone::shortSize = l.toInt();
}

void Dialog::onActionFixedTextLayout()
{
  bool ok = false;
  QString query = "Enter <width>x<height> (current:"
    + QString::number(QMyApplication::textCharacterWidth)
    + "x"
    + QString::number(QMyApplication::textLines)
    + ")";
  QString l = QInputDialog::getText(this, "Layout parameters",
                                    query,
                                    QLineEdit::Normal, "", &ok);
  if (!ok)
    return;
  QStringList le = l.split("x");
  QMyApplication::textCharacterWidth = le[0].toInt();
  QMyApplication::textLines = le[1].toInt();
  QMyApplication::computeTextSize();
}

void Dialog::onActionAttachToolbox()
{
  bool notthere = dynamic_cast<QScrollArea*>(centralWidget());
  if (notthere)
  {
    QWidget* left = centralWidget();
    QSplitter* split = new QSplitter(this);
    split->addWidget(left);
    split->addWidget(QMyApplication::baseToolset->loadContent());
    setCentralWidget(split);
  }
  else
  {
    QSplitter* split = dynamic_cast<QSplitter*>(centralWidget());
    QWidget* left = split->widget(0);
    left->setParent(this);
    setCentralWidget(left);
  }
}

void Dialog::onActionBuilder()
{
  (new ExprBuilder())->show();
}

void Dialog::onActionIcons()
{
  (new ImageLibrary("assets/"))->show();
}

void Dialog::onActionHelp()
{
  QMainWindow* mw = new QMainWindow();
  QLabel* b = new QLabel();
  QFile f("help.html");
  f.open(QIODevice::ReadOnly);
  QByteArray help = f.readAll();
  b->setText(QString::fromLatin1(help.data()));
  mw->setCentralWidget(b);
  mw->show();
}

void Dialog::onActionNew()
{
  Dialog* dlg = new Dialog();
  dlg->show();
}


void Dialog::load(QString path)
{
  this->path = path;
  QFile file(path);
  file.open(QIODevice::ReadOnly);
  QDataStream in(&file);
  QVariant data;
  QMap<QString, QVariant> ptrs;
  in >> ptrs >> data;
  ExprModel::deserializeModels(ptrs);
  delete root;
  root = Node::deserialize(data);
  scroll->setWidget(root);
  savedPtrs = ptrs;
  savedData = data;
  setWindowModified(false);
  setWindowTitle(QFileInfo(path).fileName() + "[*]");
  dirtyTimer.setSingleShot(false);
  dirtyTimer.start(10000);
}

void Dialog::onActionOpen()
{
  if (isWindowModified() && !maybeSave())
    return;
  QString where = QFileDialog::getOpenFileName(this, "File to save to", lastUsedPath,
    "Dialog files (*.dlg)");
  if (where == "")
    return;
  lastUsedPath = QFileInfo(where).path();
  load(where);
}

void Dialog::onActionSave()
{
  save();
}

void Dialog::save(QString path, bool json, bool backup)
{
  if (path == "")
    path = this->path;
  if (path == "")
  {
    onActionSaveAs();
    return;
  }
  QMap<QString, QVariant> ptrs;
  QVariant val = root->serialize(ptrs);
  QFile file(path);
  file.open(QIODevice::WriteOnly);
  QDataStream stream(&file);
  stream << ptrs << val;
  savedPtrs = ptrs;
  savedData = val;
  if (json)
  {
    val = root->rootToDialog();
    QString ser = toDialog(val, true);
    QFile file2(path + ".json");
    file2.open(QIODevice::WriteOnly);
    file2.write(ser.toUtf8());
  }
  if (!backupTimer.isActive())
  {
    backupTimer.setSingleShot(false);
    backupTimer.start(60000 * 5);
  }
  if (!backup)
  {
    setWindowModified(false);
    setWindowTitle(QFileInfo(path).fileName() + "[*]");
  }
  dirtyTimer.setSingleShot(false);
  dirtyTimer.start(10000);
}

void Dialog::onBackupTimer()
{
  save(path + "#", false, true);
}

void Dialog::onDirtyTimer()
{
  QMap<QString, QVariant> ptrs;
  QVariant val = root->serialize(ptrs);
  if (ptrs != savedPtrs || val != savedData)
  {
    dirtyTimer.stop();
    setWindowModified(true);
    setWindowTitle(QFileInfo(path).fileName() + "[*]");
  }
}

void Dialog::onActionSaveAs()
{
  path = QFileDialog::getSaveFileName(this, "File to save to", lastUsedPath,
    "Dialog files (*.dlg)");
  if (path == "")
    return;
  lastUsedPath = QFileInfo(path).path();
  if (QFileInfo(path).suffix() == "")
    path = path + ".dlg";
  onActionSave();
}

bool Dialog::maybeSave()
{
  QMessageBox::StandardButton ret;
  ret = QMessageBox::warning(this, "Dialog " + path,
    tr("The dialog has been modified.\n"
      "Do you want to save your changes?"),
    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  qDebug() << "close query: " << ret;
  if (ret == QMessageBox::Save)
  {
    onActionSave();
    return true;
  }
  if (ret == QMessageBox::Discard)
    return true;
  return false;
}

void Dialog::closeEvent(QCloseEvent* event)
{
  qDebug() << "closeevent";
  if (isWindowModified() && !maybeSave())
      event->ignore();
  else
  {
    dialogs.removeOne((Dialog*)this);
    event->accept();
  }
}

void Dialog::onActionQuit()
{
  // first pass to save unmodified data
  for (int i=0; i<dialogs.size(); ++i)
  {
    if (dialogs[i]->isWindowModified() && !dialogs[i]->maybeSave())
      return;
  }
  // save to settings
  QSettings settings;
  qDebug() << "Saving " << dialogs.size() << " dialogs";
  settings.beginWriteArray("dialogs");
  for (int i=0; i<dialogs.size(); ++i)
  {
    dialogs[i]->disconnect();
    settings.setArrayIndex(i);
    settings.setValue("geometry", dialogs[i]->geometry());
    settings.setValue("file", dialogs[i]->path);
    bool notthere = dynamic_cast<QScrollArea*>(centralWidget());
    settings.setValue("hasToolbox", !notthere);
    if (!notthere)
    {
      QSplitter* splitter = dynamic_cast<QSplitter*>(centralWidget());
      settings.setValue("splitterSizes", splitter->saveState());
    }
  }
  settings.endArray();
  QApplication::quit();
}

void Dialog::disconnect()
{
  if (socket)
    socket->write("QUIT\n");
}

void Dialog::onActionConnectDDB()
{
  if (socket)
  {
    socket->disconnectFromHost();
    delete socket;
  }
  socket = new QTcpSocket();
  connect(socket, SIGNAL(disconnected()), this, SLOT(socketError()));
  connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
  connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
  connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
  socket->connectToHost("127.0.0.1", 12000);
}

void Dialog::socketConnected()
{
  qDebug() << "Socket connected";
  socketRequestPending();
}

void Dialog::socketError()
{
  qDebug() << "Socket error";
}

void Dialog::socketRequestPending()
{
  if (!socket || socket->state() != QAbstractSocket::ConnectedState)
    return;
  socket->write("P PENDING\n");
}

void Dialog::socketReadyRead()
{
  //qDebug() << "socketReadyRead";
  while (socket->canReadLine())
  {
    char line[4096];
    qint64 res = socket->readLine(line, 4095);
    if (res <= 0)
      return;
    // line contains the \n
    QString s(line);
    s = s.left(s.size()-1);
    s = s.trimmed();
    //qDebug() << "line " << s;
    if (s == "")
      continue;
    QStringList elems = s.split(' ');
    if (elems[0][0] == 'E')
    { // eval reply
      QString nid = elems[0].right(elems[0].size()-1);
      Node* n = root->find(nid);
      if (n)
      {
        QString val = elems[1].mid(1, elems[1].size()-3);
        qDebug() << "eval reply data: " << val;
        QStringList elems = val.split(",");
        int idx = 0;
        n->forEachPre([&](Expr* e)
          {
            QString color = (elems[idx] == "True") ? "green" : "red";
            e->setStyleSheet("border-width: 2px; border-color: " + color + ";");
            ++idx;
          });
      }
    }
    if (elems[0] == "P")
    {
      if (elems.size() >=4 && elems[1] == "DLG")
      {
        if (elems[2] == "FILTERED")
        {
          Node* n = root->find(elems[4]);
          if (!n) continue;
          n->color("red");
          QString reason = elems[3];
          if (reason == "COND")
          { // get more details by evaluating all boxes
            QStringList exprs;
            n->forEachPre([&](Expr* e) {
                QString code = Node::getExprCode(e);
                if (code != "")
                  code = "!!(" + code +")"; // force boolean for easier checking
                else
                  code = "true";
                exprs.append(code);
            });
            QString query = "[" + exprs.join(",") + "]";
            qDebug() << "eval request: " << query;
            socket->write(("E" + elems[4] + " EVAL " + query + "\n").toLatin1());
          }
          else
          {
            QString exprKey = reason.toLower();
            n->forEachPre([&](Expr* e) {
                if (e->expression().indexOf(exprKey) != -1)
                {
                  e->setStyleSheet("border-color: red; border-width: 2px;");
                }
            });
          }
        }
        else if (elems[2] == "ACCEPTED")
        {
          Node* n = root->find(elems[3]);
          if (!n) continue;
          qDebug() << "accepted";
          n->color("yellow");
        }
        else if (elems[2] == "CHOICE")
        {
          Node* n = root->find(elems[3]);
          if (!n) continue;
          qDebug() << "choice";
          root->forEach([](Node* n) {
              n->color("");
          });
          n->color("green");
        }
      }
      else if (elems[1] == "END")
      {
        QTimer::singleShot(100, this, SLOT(socketRequestPending()));
      }
    }
  }
}
