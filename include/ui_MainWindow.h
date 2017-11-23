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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *s_mainWindowGridLayout;
    QVBoxLayout *verticalLayout;
    QGroupBox *debugControlsGB;
    QGridLayout *gridLayout_2;
    QCheckBox *gBufferCheckBox;
    QGroupBox *lightControlsGB;
    QGridLayout *gridLayout;
    QSpinBox *lightIndexSpinBox;
    QDoubleSpinBox *lightPositionX;
    QDoubleSpinBox *lightPositionY;
    QDoubleSpinBox *lightPositionZ;
    QLabel *lightPositionLabel_2;
    QLabel *lightPositionLabel_1;
    QLabel *lightPositionLabel_3;
    QLabel *lightPositionLabel_4;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(972, 649);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        s_mainWindowGridLayout = new QGridLayout(centralwidget);
        s_mainWindowGridLayout->setObjectName(QStringLiteral("s_mainWindowGridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        debugControlsGB = new QGroupBox(centralwidget);
        debugControlsGB->setObjectName(QStringLiteral("debugControlsGB"));
        gridLayout_2 = new QGridLayout(debugControlsGB);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gBufferCheckBox = new QCheckBox(debugControlsGB);
        gBufferCheckBox->setObjectName(QStringLiteral("gBufferCheckBox"));

        gridLayout_2->addWidget(gBufferCheckBox, 0, 0, 1, 1);


        verticalLayout->addWidget(debugControlsGB);

        lightControlsGB = new QGroupBox(centralwidget);
        lightControlsGB->setObjectName(QStringLiteral("lightControlsGB"));
        gridLayout = new QGridLayout(lightControlsGB);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        lightIndexSpinBox = new QSpinBox(lightControlsGB);
        lightIndexSpinBox->setObjectName(QStringLiteral("lightIndexSpinBox"));
        lightIndexSpinBox->setMinimum(1);
        lightIndexSpinBox->setMaximum(4);

        gridLayout->addWidget(lightIndexSpinBox, 0, 2, 1, 1);

        lightPositionX = new QDoubleSpinBox(lightControlsGB);
        lightPositionX->setObjectName(QStringLiteral("lightPositionX"));
        lightPositionX->setDecimals(2);
        lightPositionX->setMinimum(-10000);
        lightPositionX->setMaximum(10000);
        lightPositionX->setSingleStep(20);
        lightPositionX->setValue(1000);

        gridLayout->addWidget(lightPositionX, 3, 2, 1, 1);

        lightPositionY = new QDoubleSpinBox(lightControlsGB);
        lightPositionY->setObjectName(QStringLiteral("lightPositionY"));
        lightPositionY->setDecimals(2);
        lightPositionY->setMinimum(-10000);
        lightPositionY->setMaximum(10000);
        lightPositionY->setSingleStep(20);
        lightPositionY->setValue(200);

        gridLayout->addWidget(lightPositionY, 4, 2, 1, 1);

        lightPositionZ = new QDoubleSpinBox(lightControlsGB);
        lightPositionZ->setObjectName(QStringLiteral("lightPositionZ"));
        lightPositionZ->setDecimals(2);
        lightPositionZ->setMinimum(-10000);
        lightPositionZ->setMaximum(10000);
        lightPositionZ->setSingleStep(20);

        gridLayout->addWidget(lightPositionZ, 5, 2, 1, 1);

        lightPositionLabel_2 = new QLabel(lightControlsGB);
        lightPositionLabel_2->setObjectName(QStringLiteral("lightPositionLabel_2"));

        gridLayout->addWidget(lightPositionLabel_2, 0, 0, 1, 1);

        lightPositionLabel_1 = new QLabel(lightControlsGB);
        lightPositionLabel_1->setObjectName(QStringLiteral("lightPositionLabel_1"));

        gridLayout->addWidget(lightPositionLabel_1, 3, 0, 1, 1);

        lightPositionLabel_3 = new QLabel(lightControlsGB);
        lightPositionLabel_3->setObjectName(QStringLiteral("lightPositionLabel_3"));

        gridLayout->addWidget(lightPositionLabel_3, 4, 0, 1, 1);

        lightPositionLabel_4 = new QLabel(lightControlsGB);
        lightPositionLabel_4->setObjectName(QStringLiteral("lightPositionLabel_4"));

        gridLayout->addWidget(lightPositionLabel_4, 5, 0, 1, 1);


        verticalLayout->addWidget(lightControlsGB);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        s_mainWindowGridLayout->addLayout(verticalLayout, 0, 1, 1, 1);

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
        debugControlsGB->setTitle(QApplication::translate("MainWindow", "Debug Controls", Q_NULLPTR));
        gBufferCheckBox->setText(QApplication::translate("MainWindow", "G Buffer View", Q_NULLPTR));
        lightControlsGB->setTitle(QApplication::translate("MainWindow", "Light Controls", Q_NULLPTR));
        lightPositionLabel_2->setText(QApplication::translate("MainWindow", "Light", Q_NULLPTR));
        lightPositionLabel_1->setText(QApplication::translate("MainWindow", "Pos X", Q_NULLPTR));
        lightPositionLabel_3->setText(QApplication::translate("MainWindow", "Pos Y", Q_NULLPTR));
        lightPositionLabel_4->setText(QApplication::translate("MainWindow", "Pos Z", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
