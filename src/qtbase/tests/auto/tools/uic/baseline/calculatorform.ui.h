/********************************************************************************
** Form generated from reading UI file 'calculatorform.ui'
**
** Created by: Qt User Interface Compiler version 6.0.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef CALCULATORFORM_H
#define CALCULATORFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CalculatorForm
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QSpinBox *inputSpinBox1;
    QLabel *label_3;
    QVBoxLayout *vboxLayout1;
    QLabel *label_2;
    QSpinBox *inputSpinBox2;
    QLabel *label_3_2;
    QVBoxLayout *vboxLayout2;
    QLabel *label_2_2_2;
    QLabel *outputWidget;
    QSpacerItem *spacerItem;
    QSpacerItem *spacerItem1;

    void setupUi(QWidget *CalculatorForm)
    {
        if (CalculatorForm->objectName().isEmpty())
            CalculatorForm->setObjectName("CalculatorForm");
        CalculatorForm->resize(276, 98);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CalculatorForm->sizePolicy().hasHeightForWidth());
        CalculatorForm->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(CalculatorForm);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout->setObjectName("gridLayout");
        gridLayout->setObjectName(QString::fromUtf8(""));
        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(1, 1, 1, 1);
        hboxLayout->setObjectName("hboxLayout");
        hboxLayout->setObjectName(QString::fromUtf8(""));
        vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
        vboxLayout->setContentsMargins(1, 1, 1, 1);
        vboxLayout->setObjectName("vboxLayout");
        vboxLayout->setObjectName(QString::fromUtf8(""));
        label = new QLabel(CalculatorForm);
        label->setObjectName("label");
        label->setGeometry(QRect(1, 1, 45, 19));

        vboxLayout->addWidget(label);

        inputSpinBox1 = new QSpinBox(CalculatorForm);
        inputSpinBox1->setObjectName("inputSpinBox1");
        inputSpinBox1->setGeometry(QRect(1, 26, 45, 25));
        inputSpinBox1->setMouseTracking(true);

        vboxLayout->addWidget(inputSpinBox1);


        hboxLayout->addLayout(vboxLayout);

        label_3 = new QLabel(CalculatorForm);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(54, 1, 7, 52));
        label_3->setAlignment(Qt::AlignCenter);

        hboxLayout->addWidget(label_3);

        vboxLayout1 = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
        vboxLayout1->setContentsMargins(1, 1, 1, 1);
        vboxLayout1->setObjectName("vboxLayout1");
        vboxLayout1->setObjectName(QString::fromUtf8(""));
        label_2 = new QLabel(CalculatorForm);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(1, 1, 45, 19));

        vboxLayout1->addWidget(label_2);

        inputSpinBox2 = new QSpinBox(CalculatorForm);
        inputSpinBox2->setObjectName("inputSpinBox2");
        inputSpinBox2->setGeometry(QRect(1, 26, 45, 25));
        inputSpinBox2->setMouseTracking(true);

        vboxLayout1->addWidget(inputSpinBox2);


        hboxLayout->addLayout(vboxLayout1);

        label_3_2 = new QLabel(CalculatorForm);
        label_3_2->setObjectName("label_3_2");
        label_3_2->setGeometry(QRect(120, 1, 7, 52));
        label_3_2->setAlignment(Qt::AlignCenter);

        hboxLayout->addWidget(label_3_2);

        vboxLayout2 = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout2->setSpacing(6);
#endif
        vboxLayout2->setContentsMargins(1, 1, 1, 1);
        vboxLayout2->setObjectName("vboxLayout2");
        vboxLayout2->setObjectName(QString::fromUtf8(""));
        label_2_2_2 = new QLabel(CalculatorForm);
        label_2_2_2->setObjectName("label_2_2_2");
        label_2_2_2->setGeometry(QRect(1, 1, 37, 17));

        vboxLayout2->addWidget(label_2_2_2);

        outputWidget = new QLabel(CalculatorForm);
        outputWidget->setObjectName("outputWidget");
        outputWidget->setGeometry(QRect(1, 24, 37, 27));
        outputWidget->setFrameShape(QFrame::Box);
        outputWidget->setFrameShadow(QFrame::Sunken);
        outputWidget->setAlignment(Qt::AlignAbsolute|Qt::AlignBottom|Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignHorizontal_Mask|Qt::AlignJustify|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing|Qt::AlignVCenter|Qt::AlignVertical_Mask);

        vboxLayout2->addWidget(outputWidget);


        hboxLayout->addLayout(vboxLayout2);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 1);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(spacerItem, 1, 0, 1, 1);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(spacerItem1, 0, 1, 1, 1);


        retranslateUi(CalculatorForm);

        QMetaObject::connectSlotsByName(CalculatorForm);
    } // setupUi

    void retranslateUi(QWidget *CalculatorForm)
    {
        CalculatorForm->setWindowTitle(QCoreApplication::translate("CalculatorForm", "Calculator Builder", nullptr));
        label->setText(QCoreApplication::translate("CalculatorForm", "Input 1", nullptr));
        label_3->setText(QCoreApplication::translate("CalculatorForm", "+", nullptr));
        label_2->setText(QCoreApplication::translate("CalculatorForm", "Input 2", nullptr));
        label_3_2->setText(QCoreApplication::translate("CalculatorForm", "=", nullptr));
        label_2_2_2->setText(QCoreApplication::translate("CalculatorForm", "Output", nullptr));
        outputWidget->setText(QCoreApplication::translate("CalculatorForm", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CalculatorForm: public Ui_CalculatorForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // CALCULATORFORM_H
