/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_New;
    QAction *action_Open;
    QAction *action_Save;
    QAction *actionSave_as;
    QAction *actionHelp;
    QAction *actionBuilder;
    QAction *actionIcons;
    QAction *actionFont;
    QAction *actionIcon32;
    QAction *actionIcon24;
    QAction *actionIcon16;
    QAction *action_Find;
    QAction *actionNew_group;
    QAction *actionQuit;
    QAction *actionCollapse_all;
    QAction *actionExpand_all;
    QAction *actionFixed_text_layout;
    QAction *actionFlow_text_layout;
    QAction *actionAttach_toolbox;
    QAction *actionDropzone_max_width;
    QAction *actionDropzone_short_size;
    QAction *actionConnect_DDB;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menuWindow;
    QMenu *menuSettings;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(344, 354);
        action_New = new QAction(MainWindow);
        action_New->setObjectName(QString::fromUtf8("action_New"));
        action_Open = new QAction(MainWindow);
        action_Open->setObjectName(QString::fromUtf8("action_Open"));
        action_Save = new QAction(MainWindow);
        action_Save->setObjectName(QString::fromUtf8("action_Save"));
        actionSave_as = new QAction(MainWindow);
        actionSave_as->setObjectName(QString::fromUtf8("actionSave_as"));
        actionHelp = new QAction(MainWindow);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionBuilder = new QAction(MainWindow);
        actionBuilder->setObjectName(QString::fromUtf8("actionBuilder"));
        actionIcons = new QAction(MainWindow);
        actionIcons->setObjectName(QString::fromUtf8("actionIcons"));
        actionFont = new QAction(MainWindow);
        actionFont->setObjectName(QString::fromUtf8("actionFont"));
        actionIcon32 = new QAction(MainWindow);
        actionIcon32->setObjectName(QString::fromUtf8("actionIcon32"));
        actionIcon24 = new QAction(MainWindow);
        actionIcon24->setObjectName(QString::fromUtf8("actionIcon24"));
        actionIcon16 = new QAction(MainWindow);
        actionIcon16->setObjectName(QString::fromUtf8("actionIcon16"));
        action_Find = new QAction(MainWindow);
        action_Find->setObjectName(QString::fromUtf8("action_Find"));
        actionNew_group = new QAction(MainWindow);
        actionNew_group->setObjectName(QString::fromUtf8("actionNew_group"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionCollapse_all = new QAction(MainWindow);
        actionCollapse_all->setObjectName(QString::fromUtf8("actionCollapse_all"));
        actionExpand_all = new QAction(MainWindow);
        actionExpand_all->setObjectName(QString::fromUtf8("actionExpand_all"));
        actionFixed_text_layout = new QAction(MainWindow);
        actionFixed_text_layout->setObjectName(QString::fromUtf8("actionFixed_text_layout"));
        actionFlow_text_layout = new QAction(MainWindow);
        actionFlow_text_layout->setObjectName(QString::fromUtf8("actionFlow_text_layout"));
        actionAttach_toolbox = new QAction(MainWindow);
        actionAttach_toolbox->setObjectName(QString::fromUtf8("actionAttach_toolbox"));
        actionDropzone_max_width = new QAction(MainWindow);
        actionDropzone_max_width->setObjectName(QString::fromUtf8("actionDropzone_max_width"));
        actionDropzone_short_size = new QAction(MainWindow);
        actionDropzone_short_size->setObjectName(QString::fromUtf8("actionDropzone_short_size"));
        actionConnect_DDB = new QAction(MainWindow);
        actionConnect_DDB->setObjectName(QString::fromUtf8("actionConnect_DDB"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 344, 20));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menuWindow = new QMenu(menubar);
        menuWindow->setObjectName(QString::fromUtf8("menuWindow"));
        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName(QString::fromUtf8("menuSettings"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menuSettings->menuAction());
        menubar->addAction(menuWindow->menuAction());
        menu_File->addAction(action_New);
        menu_File->addAction(action_Open);
        menu_File->addSeparator();
        menu_File->addAction(action_Save);
        menu_File->addAction(actionSave_as);
        menu_File->addSeparator();
        menu_File->addAction(actionCollapse_all);
        menu_File->addAction(actionExpand_all);
        menu_File->addAction(actionNew_group);
        menu_File->addSeparator();
        menu_File->addAction(actionConnect_DDB);
        menu_File->addAction(actionHelp);
        menu_File->addAction(action_Find);
        menu_File->addAction(actionQuit);
        menuWindow->addAction(actionBuilder);
        menuWindow->addAction(actionIcons);
        menuSettings->addAction(actionFont);
        menuSettings->addAction(actionIcon32);
        menuSettings->addAction(actionIcon24);
        menuSettings->addAction(actionIcon16);
        menuSettings->addAction(actionFixed_text_layout);
        menuSettings->addAction(actionFlow_text_layout);
        menuSettings->addAction(actionAttach_toolbox);
        menuSettings->addAction(actionDropzone_max_width);
        menuSettings->addAction(actionDropzone_short_size);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Expressions", nullptr));
        action_New->setText(QCoreApplication::translate("MainWindow", "&New", nullptr));
#if QT_CONFIG(shortcut)
        action_New->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Open->setText(QCoreApplication::translate("MainWindow", "&Open...", nullptr));
#if QT_CONFIG(shortcut)
        action_Open->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        action_Save->setText(QCoreApplication::translate("MainWindow", "&Save", nullptr));
#if QT_CONFIG(shortcut)
        action_Save->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_as->setText(QCoreApplication::translate("MainWindow", "Save &as...", nullptr));
        actionHelp->setText(QCoreApplication::translate("MainWindow", "Help", nullptr));
        actionBuilder->setText(QCoreApplication::translate("MainWindow", "Builder", nullptr));
        actionIcons->setText(QCoreApplication::translate("MainWindow", "Icons", nullptr));
        actionFont->setText(QCoreApplication::translate("MainWindow", "Font...", nullptr));
        actionIcon32->setText(QCoreApplication::translate("MainWindow", "Icon32", nullptr));
        actionIcon24->setText(QCoreApplication::translate("MainWindow", "Icon24", nullptr));
        actionIcon16->setText(QCoreApplication::translate("MainWindow", "Icon16", nullptr));
        action_Find->setText(QCoreApplication::translate("MainWindow", "&Find", nullptr));
#if QT_CONFIG(shortcut)
        action_Find->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F", nullptr));
#endif // QT_CONFIG(shortcut)
        actionNew_group->setText(QCoreApplication::translate("MainWindow", "New group...", nullptr));
        actionQuit->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
        actionCollapse_all->setText(QCoreApplication::translate("MainWindow", "Collapse all", nullptr));
        actionExpand_all->setText(QCoreApplication::translate("MainWindow", "Expand all", nullptr));
        actionFixed_text_layout->setText(QCoreApplication::translate("MainWindow", "Fixed text layout...", nullptr));
        actionFlow_text_layout->setText(QCoreApplication::translate("MainWindow", "Flow text layout", nullptr));
        actionAttach_toolbox->setText(QCoreApplication::translate("MainWindow", "Attach toolbox", nullptr));
        actionDropzone_max_width->setText(QCoreApplication::translate("MainWindow", "dropzone max width...", nullptr));
        actionDropzone_short_size->setText(QCoreApplication::translate("MainWindow", "dropzone short size...", nullptr));
        actionConnect_DDB->setText(QCoreApplication::translate("MainWindow", "Connect DDB", nullptr));
        menu_File->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
        menuWindow->setTitle(QCoreApplication::translate("MainWindow", "Window", nullptr));
        menuSettings->setTitle(QCoreApplication::translate("MainWindow", "Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
