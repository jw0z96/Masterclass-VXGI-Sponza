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
#include <QtWidgets/QSpacerItem>
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
    QGroupBox *lightControlsGB;
    QGridLayout *gridLayout;
    QDoubleSpinBox *lightPositionY;
    QDoubleSpinBox *lightPositionX;
    QDoubleSpinBox *lightPositionZ;
    QLabel *lightPositionLabel_1;
    QLabel *lightPositionLabel_2;
    QLabel *lightPositionLabel_3;
    QLabel *lightIntensityLabel;
    QDoubleSpinBox *lightIntensitySpinBox;
    QDoubleSpinBox *shadowApertureSpinBox;
    QDoubleSpinBox *lightFalloffSpinBox;
    QLabel *shadowApertureLabel;
    QLabel *lightFalloffLabel;
    QGroupBox *renderingControlsGB;
    QGridLayout *gridLayout_2;
    QCheckBox *reflectionCheckBox;
    QCheckBox *directLightCheckBox;
    QCheckBox *indirectLightCheckBox;
    QLabel *label;
    QDoubleSpinBox *reflectionsAmountSpinBox;
    QDoubleSpinBox *directLightAmountSpinBox;
    QDoubleSpinBox *specularApertureSpinBox;
    QDoubleSpinBox *indirectLightAmountSpinBox;
    QComboBox *toneMappingComboBox;
    QLabel *toneMappingLabel;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(972, 641);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        s_mainWindowGridLayout = new QGridLayout(centralwidget);
        s_mainWindowGridLayout->setObjectName(QStringLiteral("s_mainWindowGridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        lightControlsGB = new QGroupBox(centralwidget);
        lightControlsGB->setObjectName(QStringLiteral("lightControlsGB"));
        gridLayout = new QGridLayout(lightControlsGB);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        lightPositionY = new QDoubleSpinBox(lightControlsGB);
        lightPositionY->setObjectName(QStringLiteral("lightPositionY"));
        lightPositionY->setDecimals(2);
        lightPositionY->setMinimum(-10000);
        lightPositionY->setMaximum(10000);
        lightPositionY->setSingleStep(20);
        lightPositionY->setValue(200);

        gridLayout->addWidget(lightPositionY, 4, 2, 1, 1);

        lightPositionX = new QDoubleSpinBox(lightControlsGB);
        lightPositionX->setObjectName(QStringLiteral("lightPositionX"));
        lightPositionX->setDecimals(2);
        lightPositionX->setMinimum(-10000);
        lightPositionX->setMaximum(10000);
        lightPositionX->setSingleStep(20);
        lightPositionX->setValue(0);

        gridLayout->addWidget(lightPositionX, 3, 2, 1, 1);

        lightPositionZ = new QDoubleSpinBox(lightControlsGB);
        lightPositionZ->setObjectName(QStringLiteral("lightPositionZ"));
        lightPositionZ->setDecimals(2);
        lightPositionZ->setMinimum(-10000);
        lightPositionZ->setMaximum(10000);
        lightPositionZ->setSingleStep(20);

        gridLayout->addWidget(lightPositionZ, 5, 2, 1, 1);

        lightPositionLabel_1 = new QLabel(lightControlsGB);
        lightPositionLabel_1->setObjectName(QStringLiteral("lightPositionLabel_1"));

        gridLayout->addWidget(lightPositionLabel_1, 3, 0, 1, 1);

        lightPositionLabel_2 = new QLabel(lightControlsGB);
        lightPositionLabel_2->setObjectName(QStringLiteral("lightPositionLabel_2"));

        gridLayout->addWidget(lightPositionLabel_2, 4, 0, 1, 1);

        lightPositionLabel_3 = new QLabel(lightControlsGB);
        lightPositionLabel_3->setObjectName(QStringLiteral("lightPositionLabel_3"));

        gridLayout->addWidget(lightPositionLabel_3, 5, 0, 1, 1);

        lightIntensityLabel = new QLabel(lightControlsGB);
        lightIntensityLabel->setObjectName(QStringLiteral("lightIntensityLabel"));

        gridLayout->addWidget(lightIntensityLabel, 0, 0, 1, 1);

        lightIntensitySpinBox = new QDoubleSpinBox(lightControlsGB);
        lightIntensitySpinBox->setObjectName(QStringLiteral("lightIntensitySpinBox"));
        lightIntensitySpinBox->setDecimals(0);
        lightIntensitySpinBox->setMaximum(99999);
        lightIntensitySpinBox->setSingleStep(10);
        lightIntensitySpinBox->setValue(100);

        gridLayout->addWidget(lightIntensitySpinBox, 0, 2, 1, 1);

        shadowApertureSpinBox = new QDoubleSpinBox(lightControlsGB);
        shadowApertureSpinBox->setObjectName(QStringLiteral("shadowApertureSpinBox"));
        shadowApertureSpinBox->setMinimum(0.01);
        shadowApertureSpinBox->setSingleStep(0.01);

        gridLayout->addWidget(shadowApertureSpinBox, 1, 2, 1, 1);

        lightFalloffSpinBox = new QDoubleSpinBox(lightControlsGB);
        lightFalloffSpinBox->setObjectName(QStringLiteral("lightFalloffSpinBox"));
        lightFalloffSpinBox->setMaximum(3);
        lightFalloffSpinBox->setSingleStep(0.1);
        lightFalloffSpinBox->setValue(2);

        gridLayout->addWidget(lightFalloffSpinBox, 2, 2, 1, 1);

        shadowApertureLabel = new QLabel(lightControlsGB);
        shadowApertureLabel->setObjectName(QStringLiteral("shadowApertureLabel"));

        gridLayout->addWidget(shadowApertureLabel, 1, 0, 1, 1);

        lightFalloffLabel = new QLabel(lightControlsGB);
        lightFalloffLabel->setObjectName(QStringLiteral("lightFalloffLabel"));

        gridLayout->addWidget(lightFalloffLabel, 2, 0, 1, 1);


        verticalLayout->addWidget(lightControlsGB);

        renderingControlsGB = new QGroupBox(centralwidget);
        renderingControlsGB->setObjectName(QStringLiteral("renderingControlsGB"));
        gridLayout_2 = new QGridLayout(renderingControlsGB);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        reflectionCheckBox = new QCheckBox(renderingControlsGB);
        reflectionCheckBox->setObjectName(QStringLiteral("reflectionCheckBox"));
        reflectionCheckBox->setChecked(true);

        gridLayout_2->addWidget(reflectionCheckBox, 2, 1, 1, 1);

        directLightCheckBox = new QCheckBox(renderingControlsGB);
        directLightCheckBox->setObjectName(QStringLiteral("directLightCheckBox"));
        directLightCheckBox->setChecked(true);

        gridLayout_2->addWidget(directLightCheckBox, 0, 1, 1, 1);

        indirectLightCheckBox = new QCheckBox(renderingControlsGB);
        indirectLightCheckBox->setObjectName(QStringLiteral("indirectLightCheckBox"));
        indirectLightCheckBox->setChecked(true);

        gridLayout_2->addWidget(indirectLightCheckBox, 1, 1, 1, 1);

        label = new QLabel(renderingControlsGB);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 3, 1, 1, 1);

        reflectionsAmountSpinBox = new QDoubleSpinBox(renderingControlsGB);
        reflectionsAmountSpinBox->setObjectName(QStringLiteral("reflectionsAmountSpinBox"));
        reflectionsAmountSpinBox->setMaximum(2);
        reflectionsAmountSpinBox->setSingleStep(0.1);
        reflectionsAmountSpinBox->setValue(1);

        gridLayout_2->addWidget(reflectionsAmountSpinBox, 2, 2, 1, 1);

        directLightAmountSpinBox = new QDoubleSpinBox(renderingControlsGB);
        directLightAmountSpinBox->setObjectName(QStringLiteral("directLightAmountSpinBox"));
        directLightAmountSpinBox->setMaximum(2);
        directLightAmountSpinBox->setSingleStep(0.1);
        directLightAmountSpinBox->setValue(1);

        gridLayout_2->addWidget(directLightAmountSpinBox, 0, 2, 1, 1);

        specularApertureSpinBox = new QDoubleSpinBox(renderingControlsGB);
        specularApertureSpinBox->setObjectName(QStringLiteral("specularApertureSpinBox"));
        specularApertureSpinBox->setDecimals(2);
        specularApertureSpinBox->setMinimum(0.01);
        specularApertureSpinBox->setSingleStep(0.1);
        specularApertureSpinBox->setValue(1);

        gridLayout_2->addWidget(specularApertureSpinBox, 3, 2, 1, 1);

        indirectLightAmountSpinBox = new QDoubleSpinBox(renderingControlsGB);
        indirectLightAmountSpinBox->setObjectName(QStringLiteral("indirectLightAmountSpinBox"));
        indirectLightAmountSpinBox->setMaximum(2);
        indirectLightAmountSpinBox->setSingleStep(0.1);
        indirectLightAmountSpinBox->setValue(1);

        gridLayout_2->addWidget(indirectLightAmountSpinBox, 1, 2, 1, 1);

        toneMappingComboBox = new QComboBox(renderingControlsGB);
        toneMappingComboBox->setObjectName(QStringLiteral("toneMappingComboBox"));

        gridLayout_2->addWidget(toneMappingComboBox, 4, 2, 1, 1);

        toneMappingLabel = new QLabel(renderingControlsGB);
        toneMappingLabel->setObjectName(QStringLiteral("toneMappingLabel"));

        gridLayout_2->addWidget(toneMappingLabel, 4, 1, 1, 1);


        verticalLayout->addWidget(renderingControlsGB);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        s_mainWindowGridLayout->addLayout(verticalLayout, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        s_mainWindowGridLayout->addItem(horizontalSpacer, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 972, 23));
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
        lightControlsGB->setTitle(QApplication::translate("MainWindow", "Light Controls", Q_NULLPTR));
        lightPositionLabel_1->setText(QApplication::translate("MainWindow", "Pos X", Q_NULLPTR));
        lightPositionLabel_2->setText(QApplication::translate("MainWindow", "Pos Y", Q_NULLPTR));
        lightPositionLabel_3->setText(QApplication::translate("MainWindow", "Pos Z", Q_NULLPTR));
        lightIntensityLabel->setText(QApplication::translate("MainWindow", "Intensity", Q_NULLPTR));
        shadowApertureLabel->setText(QApplication::translate("MainWindow", "Shadow Aperture", Q_NULLPTR));
        lightFalloffLabel->setText(QApplication::translate("MainWindow", "Falloff Exponent", Q_NULLPTR));
        renderingControlsGB->setTitle(QApplication::translate("MainWindow", "Rendering Controls", Q_NULLPTR));
        reflectionCheckBox->setText(QApplication::translate("MainWindow", "Reflections", Q_NULLPTR));
        directLightCheckBox->setText(QApplication::translate("MainWindow", "Direct Light", Q_NULLPTR));
        indirectLightCheckBox->setText(QApplication::translate("MainWindow", "Indirect Light", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Cone Angle ", Q_NULLPTR));
        toneMappingComboBox->clear();
        toneMappingComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Linear", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Reinhard", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Filmic", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ACES Filmic", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Uncharted", Q_NULLPTR)
        );
        toneMappingLabel->setText(QApplication::translate("MainWindow", "Tonemapping", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
