#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_selectedLight = 0;
	m_ui->setupUi(this);
	m_gl = new NGLScene(this);
	m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,2,1);

	/// set the light index signal
	connect(m_ui->lightIndexSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSelectedLight()));
	/// set the position signals
	connect(m_ui->lightPositionX, SIGNAL(valueChanged(double)), this, SLOT(setLightPosition()));
	connect(m_ui->lightPositionY, SIGNAL(valueChanged(double)), this, SLOT(setLightPosition()));
	connect(m_ui->lightPositionZ, SIGNAL(valueChanged(double)), this, SLOT(setLightPosition()));
}

MainWindow::~MainWindow()
{
	delete m_ui;
	delete m_gl;
}

void MainWindow::setLightPosition()
{
	ngl::Vec3 pos = ngl::Vec3(
		m_ui->lightPositionX->value(),
		m_ui->lightPositionY->value(),
		m_ui->lightPositionZ->value()
		);
	m_gl->setLightPosition(m_selectedLight, pos);
}

void MainWindow::setSelectedLight()
{
	m_selectedLight = m_ui->lightIndexSpinBox->value() - 1; //ui is 1-indexed
	ngl::Vec3 pos = m_gl->getLightPosition(m_selectedLight);
	m_ui->lightPositionX->setValue(pos.m_x);
	m_ui->lightPositionY->setValue(pos.m_y);
	m_ui->lightPositionZ->setValue(pos.m_z);
}

