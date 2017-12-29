#include "NGLScene.h"

#include <QMouseEvent>
#include <QApplication>

#include <ngl/ShaderLib.h>

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::resizeGL( int _w, int _h )
{
	m_cam.setProjection( 45.0f, static_cast<float>( _w ) / _h, 0.5f, 3500.0f );
	m_win.width  = static_cast<int>( _w * devicePixelRatio() );
	m_win.height = static_cast<int>( _h * devicePixelRatio() );
	m_isFBODirty = true;
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::setLightPosition(ngl::Vec3 _pos)
{
	m_lightPosition = _pos;
	m_isLightingDirty = true;
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
	// add to our keypress set the values of any keys pressed
	m_keysPressed += static_cast<Qt::Key>(_event->key());

	// this method is called every time the main window recives a key event.
	// we then switch on the key value and set the camera in the GLWindow
	switch (_event->key())
	{
	// escape key to quite
		case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
	// turn on wirframe rendering
		case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
	// turn off wire frame
		case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
	// show full screen
		case Qt::Key_F : showFullScreen(); break;
	// show windowed
		case Qt::Key_N : showNormal(); break;
		case Qt::Key_L : m_drawLights^=true; break;
		case Qt::Key_G : m_drawGeo^=true; break;
		default : break;
	}
	// finally update the GLWindow and re-draw
	//if (isExposed())
	update();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyReleaseEvent( QKeyEvent *_event	)
{
	// remove from our key set any keys that have been released
	m_keysPressed -= static_cast<Qt::Key>(_event->key());
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::timerEvent( QTimerEvent *)
{
	update();
}
