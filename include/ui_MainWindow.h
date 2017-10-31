/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *s_mainWindowGridLayout;
    QGroupBox *s_transformGB;
    QGridLayout *gridLayout;
    QLabel *lightPositionLabel;
    QDoubleSpinBox *lightPositionX;
    QDoubleSpinBox *lightPositionZ;
    QDoubleSpinBox *lightPositionY;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(972, 656);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        s_mainWindowGridLayout = new QGridLayout(centralwidget);
        s_mainWindowGridLayout->setObjectName(QStringLiteral("s_mainWindowGridLayout"));
        s_transformGB = new QGroupBox(centralwidget);
        s_transformGB->setObjectName(QStringLiteral("s_transformGB"));
        gridLayout = new QGridLayout(s_transformGB);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        lightPositionLabel = new QLabel(s_transformGB);
        lightPositionLabel->setObjectName(QStringLiteral("lightPositionLabel"));

        gridLayout->addWidget(lightPositionLabel, 0, 0, 1, 1);

        lightPositionX = new QDoubleSpinBox(s_transformGB);
        lightPositionX->setObjectName(QStringLiteral("lightPositionX"));
        lightPositionX->setMinimum(-10000);
        lightPositionX->setMaximum(10000);
        lightPositionX->setSingleStep(20);

        gridLayout->addWidget(lightPositionX, 1, 0, 1, 1);

        lightPositionZ = new QDoubleSpinBox(s_transformGB);
        lightPositionZ->setObjectName(QStringLiteral("lightPositionZ"));
        lightPositionZ->setMinimum(-10000);
        lightPositionZ->setMaximum(10000);
        lightPositionZ->setSingleStep(20);

        gridLayout->addWidget(lightPositionZ, 1, 2, 1, 1);

        lightPositionY = new QDoubleSpinBox(s_transformGB);
        lightPositionY->setObjectName(QStringLiteral("lightPositionY"));
        lightPositionY->setMinimum(-10000);
        lightPositionY->setMaximum(10000);
        lightPositionY->setSingleStep(20);
        lightPositionY->setValue(200);

        gridLayout->addWidget(lightPositionY, 1, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 0, 1, 1);


        s_mainWindowGridLayout->addWidget(s_transformGB, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        s_mainWindowGridLayout->addItem(horizontalSpacer, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 972, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Sponza VXGI Demo", Q_NULLPTR));
        s_transformGB->setTitle(QApplication::translate("MainWindow", "Light", Q_NULLPTR));
        lightPositionLabel->setText(QApplication::translate("MainWindow", "Position", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
