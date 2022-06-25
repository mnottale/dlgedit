/********************************************************************************
** Form generated from reading UI file 'ExprBuilder.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXPRBUILDER_H
#define UI_EXPRBUILDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "dropzone.hh"

QT_BEGIN_NAMESPACE

class Ui_ExprBuilder
{
public:
    QWidget *centralwidget;
    QLineEdit *expression;
    QLabel *label;
    QPlainTextEdit *operators;
    QPlainTextEdit *values;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *compButton;
    QPushButton *eqButton;
    QPushButton *tenButton;
    QPushButton *hundredButton;
    DropZone *result;
    QCheckBox *showValue;
    QCheckBox *showAll;
    QCheckBox *freeText;
    QPushButton *newButton;
    QCheckBox *multipleSelection;
    QCheckBox *lhsFreeText;
    QLineEdit *showText;
    QLabel *label_4;
    QPlainTextEdit *extras;
    QLabel *label_5;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ExprBuilder)
    {
        if (ExprBuilder->objectName().isEmpty())
            ExprBuilder->setObjectName(QString::fromUtf8("ExprBuilder"));
        ExprBuilder->resize(319, 479);
        centralwidget = new QWidget(ExprBuilder);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        expression = new QLineEdit(centralwidget);
        expression->setObjectName(QString::fromUtf8("expression"));
        expression->setGeometry(QRect(90, 10, 161, 20));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 71, 16));
        operators = new QPlainTextEdit(centralwidget);
        operators->setObjectName(QString::fromUtf8("operators"));
        operators->setGeometry(QRect(10, 90, 104, 64));
        values = new QPlainTextEdit(centralwidget);
        values->setObjectName(QString::fromUtf8("values"));
        values->setGeometry(QRect(130, 90, 104, 64));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(40, 70, 55, 14));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(160, 70, 55, 14));
        compButton = new QPushButton(centralwidget);
        compButton->setObjectName(QString::fromUtf8("compButton"));
        compButton->setGeometry(QRect(10, 160, 41, 24));
        eqButton = new QPushButton(centralwidget);
        eqButton->setObjectName(QString::fromUtf8("eqButton"));
        eqButton->setGeometry(QRect(60, 160, 41, 24));
        tenButton = new QPushButton(centralwidget);
        tenButton->setObjectName(QString::fromUtf8("tenButton"));
        tenButton->setGeometry(QRect(130, 160, 31, 24));
        hundredButton = new QPushButton(centralwidget);
        hundredButton->setObjectName(QString::fromUtf8("hundredButton"));
        hundredButton->setGeometry(QRect(160, 160, 31, 24));
        result = new DropZone(centralwidget);
        result->setObjectName(QString::fromUtf8("result"));
        result->setGeometry(QRect(50, 330, 120, 80));
        result->setAcceptDrops(true);
        result->setStyleSheet(QString::fromUtf8("#result {\n"
"background-color: rgb(255, 255, 255);\n"
"}"));
        showValue = new QCheckBox(centralwidget);
        showValue->setObjectName(QString::fromUtf8("showValue"));
        showValue->setGeometry(QRect(70, 280, 101, 19));
        showAll = new QCheckBox(centralwidget);
        showAll->setObjectName(QString::fromUtf8("showAll"));
        showAll->setGeometry(QRect(70, 300, 82, 19));
        freeText = new QCheckBox(centralwidget);
        freeText->setObjectName(QString::fromUtf8("freeText"));
        freeText->setGeometry(QRect(200, 160, 82, 19));
        newButton = new QPushButton(centralwidget);
        newButton->setObjectName(QString::fromUtf8("newButton"));
        newButton->setGeometry(QRect(70, 410, 76, 24));
        multipleSelection = new QCheckBox(centralwidget);
        multipleSelection->setObjectName(QString::fromUtf8("multipleSelection"));
        multipleSelection->setGeometry(QRect(134, 190, 101, 20));
        lhsFreeText = new QCheckBox(centralwidget);
        lhsFreeText->setObjectName(QString::fromUtf8("lhsFreeText"));
        lhsFreeText->setGeometry(QRect(10, 190, 85, 20));
        showText = new QLineEdit(centralwidget);
        showText->setObjectName(QString::fromUtf8("showText"));
        showText->setGeometry(QRect(90, 30, 161, 21));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 30, 58, 15));
        extras = new QPlainTextEdit(centralwidget);
        extras->setObjectName(QString::fromUtf8("extras"));
        extras->setGeometry(QRect(50, 210, 251, 64));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(0, 230, 58, 15));
        ExprBuilder->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ExprBuilder);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 319, 20));
        ExprBuilder->setMenuBar(menubar);
        statusbar = new QStatusBar(ExprBuilder);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ExprBuilder->setStatusBar(statusbar);

        retranslateUi(ExprBuilder);

        QMetaObject::connectSlotsByName(ExprBuilder);
    } // setupUi

    void retranslateUi(QMainWindow *ExprBuilder)
    {
        ExprBuilder->setWindowTitle(QCoreApplication::translate("ExprBuilder", "Builder", nullptr));
        label->setText(QCoreApplication::translate("ExprBuilder", "expression", nullptr));
        label_2->setText(QCoreApplication::translate("ExprBuilder", "operators", nullptr));
        label_3->setText(QCoreApplication::translate("ExprBuilder", "values", nullptr));
        compButton->setText(QCoreApplication::translate("ExprBuilder", "comp", nullptr));
        eqButton->setText(QCoreApplication::translate("ExprBuilder", "eq", nullptr));
        tenButton->setText(QCoreApplication::translate("ExprBuilder", "10", nullptr));
        hundredButton->setText(QCoreApplication::translate("ExprBuilder", "100", nullptr));
        showValue->setText(QCoreApplication::translate("ExprBuilder", "Show value", nullptr));
        showAll->setText(QCoreApplication::translate("ExprBuilder", "show all", nullptr));
        freeText->setText(QCoreApplication::translate("ExprBuilder", "free", nullptr));
        newButton->setText(QCoreApplication::translate("ExprBuilder", "new", nullptr));
        multipleSelection->setText(QCoreApplication::translate("ExprBuilder", "multiple", nullptr));
        lhsFreeText->setText(QCoreApplication::translate("ExprBuilder", "free", nullptr));
        label_4->setText(QCoreApplication::translate("ExprBuilder", "show as", nullptr));
        label_5->setText(QCoreApplication::translate("ExprBuilder", "extra:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ExprBuilder: public Ui_ExprBuilder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXPRBUILDER_H
