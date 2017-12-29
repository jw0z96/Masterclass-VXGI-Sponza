#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NGLScene.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	public slots:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this is called to set the positions of the lights
		//----------------------------------------------------------------------------------------------------------------------
		void setLightPosition();

	private:
		// QWidget *m_ui;
		Ui::MainWindow *m_ui;
		/// @brief our openGL widget
		NGLScene *m_gl;
};

#endif // MAINWINDOW_H
