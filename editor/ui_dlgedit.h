/********************************************************************************
** Form generated from reading UI file 'dlgedit.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGEDIT_H
#define UI_DLGEDIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "dropzone.hh"

QT_BEGIN_NAMESPACE

class Ui_Node
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    DropZone *preConditions;
    QHBoxLayout *sayLayout;
    QLineEdit *say;
    QPushButton *addChildButton;
    DropZone *postConditions;
    QHBoxLayout *hChildrenLayout;
    QPushButton *showChildren;
    QWidget *childrenBar;
    QVBoxLayout *childrenLayout;

    void setupUi(QWidget *Node)
    {
        if (Node->objectName().isEmpty())
            Node->setObjectName(QString::fromUtf8("Node"));
        Node->resize(566, 254);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Node->sizePolicy().hasHeightForWidth());
        Node->setSizePolicy(sizePolicy);
        Node->setMinimumSize(QSize(0, 20));
        Node->setAcceptDrops(true);
        Node->setStyleSheet(QString::fromUtf8("#preConditions,#postConditions {\n"
"  border-color: rgb(0, 0, 0);\n"
"  border-width: 1px;\n"
"  border-style: outset;\n"
"  color: black;\n"
"}\n"
"QWidget#childrenBar {\n"
"  border-color: rgb(0, 100,100);\n"
"  border-width: 10px;\n"
"  border-style: outset;\n"
"  color: black;\n"
"}\n"
"\n"
"QWidget#say {\n"
"  color: black;\n"
"}\n"
"\n"
"QWidget#preConditions {\n"
"	background-color: rgb(207, 255, 201);\n"
"}\n"
"QWidget#postConditions {\n"
"	background-color: rgb(255, 201, 201);\n"
"}\n"
"\n"
"QWidget#preConditions:focus {\n"
"	background-color: rgb(150, 200, 150);\n"
"}\n"
"QWidget#postConditions:focus {\n"
"	background-color: rgb(200, 150, 150);\n"
"}\n"
"\n"
"Node#Node:focus {\n"
"	background-color: rgb(160, 160, 160);\n"
"}\n"
"\n"
"Expr {\n"
"  border: none;\n"
"  border-style: outset;\n"
"  border-width: 1px;\n"
"  border-color: black;\n"
"  color: black;\n"
"}\n"
""));
        verticalLayout = new QVBoxLayout(Node);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        verticalLayout->setContentsMargins(-1, 0, -1, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        horizontalLayout->setContentsMargins(0, -1, -1, -1);
        preConditions = new DropZone(Node);
        preConditions->setObjectName(QString::fromUtf8("preConditions"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(preConditions->sizePolicy().hasHeightForWidth());
        preConditions->setSizePolicy(sizePolicy1);
        preConditions->setMinimumSize(QSize(50, 30));
        preConditions->setMaximumSize(QSize(16777215, 50));
        preConditions->setAcceptDrops(true);
        preConditions->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout->addWidget(preConditions, 0, Qt::AlignLeft);


        verticalLayout->addLayout(horizontalLayout);

        sayLayout = new QHBoxLayout();
        sayLayout->setObjectName(QString::fromUtf8("sayLayout"));
        sayLayout->setSizeConstraint(QLayout::SetNoConstraint);
        say = new QLineEdit(Node);
        say->setObjectName(QString::fromUtf8("say"));
        say->setMinimumSize(QSize(300, 0));

        sayLayout->addWidget(say);

        addChildButton = new QPushButton(Node);
        addChildButton->setObjectName(QString::fromUtf8("addChildButton"));
        addChildButton->setMaximumSize(QSize(30, 16777215));
        addChildButton->setFlat(true);

        sayLayout->addWidget(addChildButton);


        verticalLayout->addLayout(sayLayout);

        postConditions = new DropZone(Node);
        postConditions->setObjectName(QString::fromUtf8("postConditions"));
        sizePolicy1.setHeightForWidth(postConditions->sizePolicy().hasHeightForWidth());
        postConditions->setSizePolicy(sizePolicy1);
        postConditions->setMinimumSize(QSize(50, 30));
        postConditions->setMaximumSize(QSize(16777215, 50));
        postConditions->setAcceptDrops(true);
        postConditions->setStyleSheet(QString::fromUtf8(""));

        verticalLayout->addWidget(postConditions);

        hChildrenLayout = new QHBoxLayout();
        hChildrenLayout->setSpacing(1);
        hChildrenLayout->setObjectName(QString::fromUtf8("hChildrenLayout"));
        showChildren = new QPushButton(Node);
        showChildren->setObjectName(QString::fromUtf8("showChildren"));
        showChildren->setEnabled(true);
        showChildren->setMinimumSize(QSize(0, 0));
        showChildren->setMaximumSize(QSize(30, 16777215));
        showChildren->setFlat(true);

        hChildrenLayout->addWidget(showChildren);

        childrenBar = new QWidget(Node);
        childrenBar->setObjectName(QString::fromUtf8("childrenBar"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(childrenBar->sizePolicy().hasHeightForWidth());
        childrenBar->setSizePolicy(sizePolicy2);
        childrenBar->setMinimumSize(QSize(5, 0));
        childrenBar->setMaximumSize(QSize(5, 16777215));

        hChildrenLayout->addWidget(childrenBar);

        childrenLayout = new QVBoxLayout();
        childrenLayout->setObjectName(QString::fromUtf8("childrenLayout"));

        hChildrenLayout->addLayout(childrenLayout);

        hChildrenLayout->setStretch(2, 1);

        verticalLayout->addLayout(hChildrenLayout);

        verticalLayout->setStretch(1, 1);
        verticalLayout->setStretch(3, 1000);

        retranslateUi(Node);

        QMetaObject::connectSlotsByName(Node);
    } // setupUi

    void retranslateUi(QWidget *Node)
    {
        Node->setWindowTitle(QCoreApplication::translate("Node", "Form", nullptr));
        addChildButton->setText(QCoreApplication::translate("Node", "+", nullptr));
        showChildren->setText(QCoreApplication::translate("Node", "-", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Node: public Ui_Node {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGEDIT_H
