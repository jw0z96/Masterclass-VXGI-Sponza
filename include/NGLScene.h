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
	//----------------------------------------------------------------------------------------------------------------------
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
	inline ngl::Vec3 getLightPosition() const {return m_lightPosition;};
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called to set the positions of the lights
	//----------------------------------------------------------------------------------------------------------------------
	void setLightPosition(ngl::Vec3 _pos);
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called to set the intensity of the light
	//----------------------------------------------------------------------------------------------------------------------
	inline void setLightIntensity(float _intensity){m_lightIntensity = _intensity; m_isLightingDirty = true;};
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called to set the reflection specular aperture
	//----------------------------------------------------------------------------------------------------------------------
	inline void setSpecularAperture(float _aperture){m_specularAperture = _aperture;};
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called to set the amount of direct light to be composited
	//----------------------------------------------------------------------------------------------------------------------
	inline void setDirectLightAmount(float _amount){m_directLightAmount = _amount;};
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called to set the amount of indirect light to be composited
	//----------------------------------------------------------------------------------------------------------------------
	inline void setIndirectLightAmount(float _amount){m_indirectLightAmount = _amount;};
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called to set the amount of specular reflections to be composited
	//----------------------------------------------------------------------------------------------------------------------
	inline void setReflectionsAmount(float _amount){m_reflectionsAmount = _amount;};

public slots:
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this is called toggle the g buffer debug view mode
	//----------------------------------------------------------------------------------------------------------------------
	inline void toggleDirectLightView(){m_viewDirectLight = !m_viewDirectLight;};
	inline void toggleIndirectLightView(){m_viewIndirectLight = !m_viewIndirectLight;};
	inline void toggleReflectionView(){m_viewReflections = !m_viewReflections;};

private:

	//----------------------------------------------------------------------------------------------------------------------
	/* GL UTILS */
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to load the textures and draw the scene geometry
	//----------------------------------------------------------------------------------------------------------------------
	void drawScene();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to initialise FBOS for the deferred shading pipeline
	//----------------------------------------------------------------------------------------------------------------------
	void initFBO();
	void initTestFBO();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to generate 3d texure (for voxel texture testing) TODO : adapt for node pool configuration
	//----------------------------------------------------------------------------------------------------------------------
	unsigned int gen3DTextureRGBA8(int dim) const;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to load transform matrices to the shader
	//----------------------------------------------------------------------------------------------------------------------
	void loadMatricesToShader();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to check framebuffer completion
	//----------------------------------------------------------------------------------------------------------------------
	void checkFrameBuffer() const;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief method to check for any openGL errors
	//----------------------------------------------------------------------------------------------------------------------
	void checkGLerror() const;
	//----------------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------------
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
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief this method is called everytime there is a timer event
	/// inherited from QObject and overridden here.
	/// @param _event the Qt Event structure
	//----------------------------------------------------------------------------------------------------------------------
	void timerEvent( QTimerEvent *) override;
	//----------------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the windows params such as mouse and rotations etc
	//----------------------------------------------------------------------------------------------------------------------
	WinParams m_win;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief timer for measurment
	/// -----------------------------------------------------------------------------
	QElapsedTimer m_timer;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief timing for camera update
	//----------------------------------------------------------------------------------------------------------------------
	float m_deltaTime = 0.0f;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief frame time for camera update
	//----------------------------------------------------------------------------------------------------------------------
	float m_lastFrame = 0.0f;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the keys being pressed
	//----------------------------------------------------------------------------------------------------------------------
	QSet<Qt::Key> m_keysPressed;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief draw lights toggle
	//----------------------------------------------------------------------------------------------------------------------
	bool m_drawLights=true;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief draw geo toggle
	//----------------------------------------------------------------------------------------------------------------------
	bool m_drawGeo=true;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief render pass view toggles
	//----------------------------------------------------------------------------------------------------------------------
	bool m_viewDirectLight = true;
	bool m_viewIndirectLight = true;
	bool m_viewReflections = true;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief specular aperture
	//----------------------------------------------------------------------------------------------------------------------
	float m_specularAperture;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the amount of each render pass
	//----------------------------------------------------------------------------------------------------------------------
	float m_directLightAmount;
	float m_indirectLightAmount;
	float m_reflectionsAmount;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the material file to load
	//----------------------------------------------------------------------------------------------------------------------
	std::unique_ptr<Mtl> m_mtl;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the mesh to draw
	//----------------------------------------------------------------------------------------------------------------------
	std::unique_ptr <GroupedObj> m_model;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief used to track whether the lights have been moved
	//----------------------------------------------------------------------------------------------------------------------
	bool m_isLightingDirty;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the positions of our lights
	//----------------------------------------------------------------------------------------------------------------------
	ngl::Vec3 m_lightPosition;
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief the intensity of our lights
	//----------------------------------------------------------------------------------------------------------------------
	float m_lightIntensity;
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
	/// @brief used to store the global mouse transforms
	//----------------------------------------------------------------------------------------------------------------------
	ngl::Mat4 m_mouseGlobalTX;

	//----------------------------------------------------------------------------------------------------------------------
	/// Keep track of whether the FBO needs to be recreated
	//----------------------------------------------------------------------------------------------------------------------
	bool m_isFBODirty;
	//----------------------------------------------------------------------------------------------------------------------
	/// Id's used for the frame buffer object and associated textures
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_gBufferFBOId, m_FBOWSPositionId, m_FBOWSNormalId, m_FBODepthId, m_FBOAlbedoId, m_FBOMetalRoughId;
	//----------------------------------------------------------------------------------------------------------------------
	/// Keep track of whether the voxel texture needs to be created TODO : adapt for node pool configuration
	//----------------------------------------------------------------------------------------------------------------------
	bool m_isVoxelTexConstructed = false;
	//----------------------------------------------------------------------------------------------------------------------
	/// voxel texture resolution
	//----------------------------------------------------------------------------------------------------------------------
	int m_voxelDim;
	//----------------------------------------------------------------------------------------------------------------------
	/// Id's used for the 3d texture
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_voxelAlbedoTex, m_voxelNormalTex, m_voxelEmissiveTex;
	//----------------------------------------------------------------------------------------------------------------------
	/// Id's used for the testing frame buffer object and associated textures
	//----------------------------------------------------------------------------------------------------------------------
	GLuint m_testFBO, m_testTexture;
};

#endif // NGLSCENE_H_
