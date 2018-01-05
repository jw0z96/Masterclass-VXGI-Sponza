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
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this is called to set the specular aperture for the reflections
		//----------------------------------------------------------------------------------------------------------------------
		void setSpecularAperture();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this is called to set the amount of direct light to be composited
		//----------------------------------------------------------------------------------------------------------------------
		void setDirectLightAmount();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this is called to set the amount of indirect light to be composited
		//----------------------------------------------------------------------------------------------------------------------
		void setIndirectLightAmount();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this is called to set the amount of specular reflections to be composited
		//----------------------------------------------------------------------------------------------------------------------
		void setReflectionsAmount();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this is called to set the light intensity
		//----------------------------------------------------------------------------------------------------------------------
		void setLightIntensity();

	private:
		// QWidget *m_ui;
		Ui::MainWindow *m_ui;
		/// @brief our openGL widget
		NGLScene *m_gl;
};

#endif // MAINWINDOW_H
