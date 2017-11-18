#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include "WindowParams.h"
#include <ngl/Transformation.h>
#include "Mtl.h"
#include "GroupedObj.h"
#include "FirstPersonCamera.h"
#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QSet>

#include <memory>

//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt QOpenGLWidget and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWidget
{
Q_OBJECT
public:
	/// @brief Constructor for GLWindow
	/// @param [in] _parent the parent window to create the GL context in
	//----------------------------------------------------------------------------------------------------------------------
	NGLScene(QWidget *_parent );
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief dtor must close down ngl and release OpenGL resources
	//----------------------------------------------------------------------------------------------------------------------
	~NGLScene();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the initialize class is called once when the window is created and we have a valid GL context
	/// use this to setup any default GL stuff
	//----------------------------------------------------------------------------------------------------------------------
	void initializeGL() override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called everytime we want to draw the scene
	//----------------------------------------------------------------------------------------------------------------------
	void paintGL() override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called everytime we resize
	//----------------------------------------------------------------------------------------------------------------------
	void resizeGL(int _w, int _h) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called to set the positions of the lights
	//----------------------------------------------------------------------------------------------------------------------
	inline ngl::Vec3 getLightPosition(int _i){return m_lightPositions[_i];};

public slots:
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called toggle the g buffer debug view mode
	//----------------------------------------------------------------------------------------------------------------------
	inline void toggleGBufferView(){m_gBufferView = !m_gBufferView; update();};
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called to set the positions of the lights
	//----------------------------------------------------------------------------------------------------------------------
	void setLightPosition(int _i, ngl::Vec3 _pos);

private:
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the windows params such as mouse and rotations etc
	//----------------------------------------------------------------------------------------------------------------------
	WinParams m_win;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief used to store the global mouse transforms
	//----------------------------------------------------------------------------------------------------------------------
	ngl::Mat4 m_mouseGlobalTX;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Our Camera
	//----------------------------------------------------------------------------------------------------------------------
	FirstPersonCamera m_cam;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief transformation stack for the gl transformations etc
	//----------------------------------------------------------------------------------------------------------------------
	ngl::Transformation m_transform;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the model position for mouse movement
	//----------------------------------------------------------------------------------------------------------------------
	ngl::Vec3 m_modelPos;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the material file to load
	//----------------------------------------------------------------------------------------------------------------------
	std::unique_ptr<Mtl> m_mtl;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the mesh to draw
	//----------------------------------------------------------------------------------------------------------------------
	std::unique_ptr <GroupedObj> m_model;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief timing for camera update
	//----------------------------------------------------------------------------------------------------------------------
	float m_deltaTime = 0.0f;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief frame time for camera update
	//----------------------------------------------------------------------------------------------------------------------
	float m_lastFrame = 0.0f;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief timer for measurment
	/// -----------------------------------------------------------------------------
	QElapsedTimer m_timer;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the keys being pressed
	//----------------------------------------------------------------------------------------------------------------------
	QSet<Qt::Key> m_keysPressed;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief choose which texture map to draw
	//----------------------------------------------------------------------------------------------------------------------
	int m_whichMap;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief draw lights toggle
	//----------------------------------------------------------------------------------------------------------------------
	bool m_drawLights=true;
	bool m_drawGeo=true;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief debug view toggle
	//----------------------------------------------------------------------------------------------------------------------
	bool m_gBufferView=false;
	// Light modes
	std::array<bool,4> m_lightOn={{true,true,true,true}};
	// lights
	std::vector<ngl::Vec3> m_lightPositions;

	std::vector<ngl::Vec3> m_lightColors;

	/* RENDER PIPELINE HELPER FUNCTIONS */

	//----------------------------------------------------------------------------------------------------------------------
	/// voxel texture resolution
	//----------------------------------------------------------------------------------------------------------------------
	int m_voxelDim;
	//----------------------------------------------------------------------------------------------------------------------
	/// Id's used for the testing frame buffer object and associated textures
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_testFBO, m_testTexture;
	//----------------------------------------------------------------------------------------------------------------------
	/// Id's used for the 3d texture
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_voxel3DTex;
	//----------------------------------------------------------------------------------------------------------------------
	/// Id's used for the frame buffer object and associated textures
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_gBufferFBOId, m_FBOWSPositionId,
		m_FBOWSNormalId, m_FBODepthId,
		m_FBOAlbedoId, m_FBOMetalRoughId;
	//----------------------------------------------------------------------------------------------------------------------
	/// Keep track of whether the FBO needs to be recreated
	//----------------------------------------------------------------------------------------------------------------------
	bool m_isFBODirty = true;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to initialise FBOS for the deferred shading pipeline
	//----------------------------------------------------------------------------------------------------------------------
	void initFBO();
	void initTestFBO();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to load transform matrices to the shader
	//----------------------------------------------------------------------------------------------------------------------
	void loadMatricesToShader();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to load transform matrices to the shader
	//----------------------------------------------------------------------------------------------------------------------
	static void CheckFrameBuffer() noexcept;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to load transform matrices to the shader
	//----------------------------------------------------------------------------------------------------------------------
	void drawScene();

	/* EVENT HANDLING */

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief Qt Event called when a key is pressed
	/// @param [in] _event the Qt event to query for size etc
	//----------------------------------------------------------------------------------------------------------------------
	void keyPressEvent(QKeyEvent *_event) override;
	void keyReleaseEvent(QKeyEvent *_event) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called every time a mouse is moved
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void mouseMoveEvent (QMouseEvent * _event ) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called everytime the mouse button is pressed
	/// inherited from QObject and overridden here.
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void mousePressEvent ( QMouseEvent *_event) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called everytime the mouse button is released
	/// inherited from QObject and overridden here.
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void mouseReleaseEvent ( QMouseEvent *_event ) override;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called everytime the mouse wheel is moved
	/// inherited from QObject and overridden here.
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void wheelEvent( QWheelEvent *_event) override;


};



#endif
