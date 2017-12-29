#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
	m_gl = new NGLScene(this);
	m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,2,1);

	// set the rendering passes toggles
	connect(m_ui->directLightCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleDirectLightView()));
	connect(m_ui->indirectLightCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleIndirectLightView()));
	connect(m_ui->reflectionCheckBox, SIGNAL(stateChanged(int)), m_gl, SLOT(toggleReflectionView()));

	// set the position signals
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
	m_gl->setLightPosition(pos);
}
