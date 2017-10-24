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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
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
    QLabel *label;
    QDoubleSpinBox *m_rotationX;
    QDoubleSpinBox *m_rotationY;
    QDoubleSpinBox *m_rotationZ;
    QLabel *label_2;
    QDoubleSpinBox *m_scaleX;
    QDoubleSpinBox *m_scaleY;
    QDoubleSpinBox *m_scaleZ;
    QLabel *label_3;
    QDoubleSpinBox *m_positionX;
    QDoubleSpinBox *m_positionY;
    QDoubleSpinBox *m_positionZ;
    QGroupBox *s_drawGB;
    QGridLayout *gridLayout_2;
    QComboBox *m_objectSelection;
    QCheckBox *m_wireframe;
    QPushButton *m_colour;
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
        label = new QLabel(s_transformGB);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        m_rotationX = new QDoubleSpinBox(s_transformGB);
        m_rotationX->setObjectName(QStringLiteral("m_rotationX"));
        m_rotationX->setMinimum(-360);
        m_rotationX->setMaximum(360);

        gridLayout->addWidget(m_rotationX, 1, 0, 1, 1);

        m_rotationY = new QDoubleSpinBox(s_transformGB);
        m_rotationY->setObjectName(QStringLiteral("m_rotationY"));
        m_rotationY->setMinimum(-360);
        m_rotationY->setMaximum(360);

        gridLayout->addWidget(m_rotationY, 1, 1, 1, 1);

        m_rotationZ = new QDoubleSpinBox(s_transformGB);
        m_rotationZ->setObjectName(QStringLiteral("m_rotationZ"));
        m_rotationZ->setMinimum(-360);
        m_rotationZ->setMaximum(360);

        gridLayout->addWidget(m_rotationZ, 1, 2, 1, 1);

        label_2 = new QLabel(s_transformGB);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        m_scaleX = new QDoubleSpinBox(s_transformGB);
        m_scaleX->setObjectName(QStringLiteral("m_scaleX"));
        m_scaleX->setMinimum(-4);
        m_scaleX->setMaximum(4);
        m_scaleX->setSingleStep(0.01);
        m_scaleX->setValue(1);

        gridLayout->addWidget(m_scaleX, 3, 0, 1, 1);

        m_scaleY = new QDoubleSpinBox(s_transformGB);
        m_scaleY->setObjectName(QStringLiteral("m_scaleY"));
        m_scaleY->setMinimum(-4);
        m_scaleY->setMaximum(4);
        m_scaleY->setSingleStep(0.01);
        m_scaleY->setValue(1);

        gridLayout->addWidget(m_scaleY, 3, 1, 1, 1);

        m_scaleZ = new QDoubleSpinBox(s_transformGB);
        m_scaleZ->setObjectName(QStringLiteral("m_scaleZ"));
        m_scaleZ->setMinimum(-4);
        m_scaleZ->setMaximum(4);
        m_scaleZ->setSingleStep(0.01);
        m_scaleZ->setValue(1);

        gridLayout->addWidget(m_scaleZ, 3, 2, 1, 1);

        label_3 = new QLabel(s_transformGB);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        m_positionX = new QDoubleSpinBox(s_transformGB);
        m_positionX->setObjectName(QStringLiteral("m_positionX"));
        m_positionX->setMinimum(-20);
        m_positionX->setMaximum(20);
        m_positionX->setSingleStep(0.01);

        gridLayout->addWidget(m_positionX, 5, 0, 1, 1);

        m_positionY = new QDoubleSpinBox(s_transformGB);
        m_positionY->setObjectName(QStringLiteral("m_positionY"));
        m_positionY->setMinimum(-20);
        m_positionY->setMaximum(20);
        m_positionY->setSingleStep(0.01);

        gridLayout->addWidget(m_positionY, 5, 1, 1, 1);

        m_positionZ = new QDoubleSpinBox(s_transformGB);
        m_positionZ->setObjectName(QStringLiteral("m_positionZ"));
        m_positionZ->setMinimum(-20);
        m_positionZ->setMaximum(20);
        m_positionZ->setSingleStep(0.01);

        gridLayout->addWidget(m_positionZ, 5, 2, 1, 1);


        s_mainWindowGridLayout->addWidget(s_transformGB, 0, 1, 1, 1);

        s_drawGB = new QGroupBox(centralwidget);
        s_drawGB->setObjectName(QStringLiteral("s_drawGB"));
        gridLayout_2 = new QGridLayout(s_drawGB);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        m_objectSelection = new QComboBox(s_drawGB);
        m_objectSelection->setObjectName(QStringLiteral("m_objectSelection"));

        gridLayout_2->addWidget(m_objectSelection, 0, 0, 1, 1);

        m_wireframe = new QCheckBox(s_drawGB);
        m_wireframe->setObjectName(QStringLiteral("m_wireframe"));

        gridLayout_2->addWidget(m_wireframe, 1, 0, 1, 1);

        m_colour = new QPushButton(s_drawGB);
        m_colour->setObjectName(QStringLiteral("m_colour"));

        gridLayout_2->addWidget(m_colour, 3, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 4, 0, 1, 1);


        s_mainWindowGridLayout->addWidget(s_drawGB, 1, 1, 1, 1);

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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "ngl Qt Demo", Q_NULLPTR));
        s_transformGB->setTitle(QApplication::translate("MainWindow", "Transform", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Rotation", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Scale", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "Position", Q_NULLPTR));
        s_drawGB->setTitle(QApplication::translate("MainWindow", "Draw", Q_NULLPTR));
        m_objectSelection->clear();
        m_objectSelection->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Teapot", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Sphere", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Cube", Q_NULLPTR)
        );
        m_wireframe->setText(QApplication::translate("MainWindow", "WireFrame", Q_NULLPTR));
        m_colour->setText(QApplication::translate("MainWindow", "Choose Colour", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
