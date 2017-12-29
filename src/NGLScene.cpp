#include "NGLScene.h"

#include <ngl/Camera.h>
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>
#include "VAO.h"
#include <math.h>

//----------------------------------------------------------------------------------------------------------------------

NGLScene::NGLScene( QWidget *_parent ) : QOpenGLWidget( _parent )
{
	// set this widget to have the initial keyboard focus
	setFocusPolicy (Qt::StrongFocus);
	// re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
	this->resize(_parent->size());

	ngl::VAOFactory::registerVAOCreator("sponzaVAO",VAO::create);
	m_timer.start();

	m_lightPosition = ngl::Vec3(0.0f, 200.0f, 0.0f);

	m_specularAperture = 1.0;

	float intensity= 1.0;

	m_lightColor = ngl::Vec3(intensity, intensity, intensity);

	ngl::Vec3 from(0,40,-140);
	ngl::Vec3 to(0,40,0);
	ngl::Vec3 up(0,1,0);
	m_cam.set(from,to,up);
	// set the shape using FOV 50, Aspect Ratio based on Width and Height, near & far clip
	m_cam.setProjection(50,(float)m_win.width/m_win.height,1.0f,800.0f);

	m_isFBODirty = true;
	m_isLightingDirty = true;

	startTimer(10);
}

//----------------------------------------------------------------------------------------------------------------------

NGLScene::~NGLScene()
{
	std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
	glDeleteTextures(1, &m_voxelAlbedoTex);
	glDeleteTextures(1, &m_voxelNormalTex);

	glDeleteTextures(1, &m_FBOWSPositionId);
	glDeleteTextures(1, &m_FBOWSNormalId);
	glDeleteTextures(1, &m_FBODepthId);
	glDeleteTextures(1, &m_FBOAlbedoId);
	glDeleteTextures(1, &m_FBOMetalRoughId);
	glDeleteFramebuffers(1, &m_gBufferFBOId);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::initializeGL()
{
	// we must call this first before any other GL commands to load and link the
	// gl commands from the lib, if this is not done program will crash
	ngl::NGLInit::instance();
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();

	// create the voxelization shader program
	shader->loadShader("voxelizationShader",
		"shaders/voxelize_vert.glsl",
		"shaders/voxelize_frag.glsl",
		"shaders/voxelize_geo.glsl");

	shader->setUniform("albedoMap", 0);
	shader->setUniform("metallicMap", 2);
	shader->setUniform("roughnessMap", 3);

	// create the gBuffer shader program
	shader->loadShader("gBufferPass",
		"shaders/gBuffer_vert.glsl",
		"shaders/gBuffer_frag.glsl");
	shader->use("gBufferPass");

	shader->setUniform("albedoMap", 0);
	shader->setUniform("normalMap", 1);
	shader->setUniform("metallicMap", 2);
	shader->setUniform("roughnessMap", 3);
	// shader->setUniform("voxelAlbedoTex", 5);
	// shader->setUniform("voxelNormalTex", 6);
	// shader->setUniform("voxelEmissiveTex", 7);

	// create the output shader program
	shader->loadShader("outputPass",
		"shaders/screen_space_vert.glsl",
		"shaders/output_frag.glsl");

	shader->use("outputPass");
	shader->setUniform("WSPositionTex", 0);
	shader->setUniform("WSNormalTex", 1);
	shader->setUniform("depthTex", 2);
	shader->setUniform("albedoTex", 3);
	shader->setUniform("metalRoughTex", 4);
	// shader->setUniform("voxelAlbedoTex", 5);
	// shader->setUniform("voxelNormalTex", 6);
	shader->setUniform("voxelEmissiveTex", 5);

	// create the output shader program
	// shader->loadShader("outputTestPass",
	// 	"shaders/screen_space_vert.glsl",
	// 	"shaders/test_frag.glsl");

	// shader->use("outputTestPass");
	// shader->setUniform("inTex", 0);

	// create the compute shader program
	shader->createShaderProgram("lightInjectionPass");
	shader->attachShader("lightInjectionPassComp", ngl::ShaderType::COMPUTE );
	shader->loadShaderSource("lightInjectionPassComp", "shaders/lightInjectionComp.glsl" );
	shader->compileShader("lightInjectionPassComp");
	shader->attachShaderToProgram("lightInjectionPass", "lightInjectionPassComp");
	shader->linkProgramObject("lightInjectionPass");

	shader->use("lightInjectionPass");
	shader->setUniform("voxelAlbedoTex", 1);
	shader->setUniform("voxelNormalTex", 2);

	// Grey Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// enable depth testing for drawing
	glEnable(GL_DEPTH_TEST);
	// enable multisampling for smoother drawing
	glEnable(GL_MULTISAMPLE);
	// set the blending mode??
	glEnable(GL_BLEND);
	// glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
	// glDisable(GL_BLEND);

	// generate screen aligned quad
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	prim->createTrianglePlane("ScreenAlignedQuad", 2, 2, 1, 1, ngl::Vec3(0,1,0));

	// load mtl file
	m_mtl.reset(new Mtl("models/sponza.mtl"));
	// load obj file
	m_model.reset(new GroupedObj("models/sponza.obj"));

	// initialize voxel texture params
	m_voxelDim = 256;
	m_voxelAlbedoTex = gen3DTextureRGBA8(m_voxelDim);
	m_voxelNormalTex = gen3DTextureRGBA8(m_voxelDim);
	m_voxelEmissiveTex = gen3DTextureRGBA8(m_voxelDim);

	// as re-size is not explicitly called we need to do this.
	glViewport(0,0,width(),height());

	// initTestFBO();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::paintGL()
{
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

	float currentFrame = m_timer.elapsed()*0.001f;
	std::cout<<"FPS: "<<1.0f / m_deltaTime<<'\n';
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	//----------------------------------------------------------------------------------------------------------------------
	/// VOXELIZE SCENE
	//----------------------------------------------------------------------------------------------------------------------

	float orthoWidth = 1400.0;
	ngl::Vec3 objectCenter = ngl::Vec3(-60.0, 600.0, 0.0); // gives a good fit for the voxel projections

	if (!m_isVoxelTexConstructed)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_voxelDim, m_voxelDim);
		// Disable some fixed-function opeartions
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		shader->use("voxelizationShader");
		// Orthograhic projection
		ngl::Mat4 Ortho;
		Ortho = ngl::ortho(-orthoWidth, orthoWidth, -orthoWidth, orthoWidth, -orthoWidth, orthoWidth);
		// Create an modelview-orthographic projection matrix see from +X axis
		ngl::Mat4 mvpX = Ortho * ngl::lookAt(objectCenter + ngl::Vec3(0, 0, 0), objectCenter + ngl::Vec3(-1, 0, 0), ngl::Vec3(0, 1, 0));
		// Create an modelview-orthographic projection matrix see from +Y axis
		ngl::Mat4 mvpY = Ortho * ngl::lookAt(objectCenter + ngl::Vec3(0, 0, 0), objectCenter + ngl::Vec3(0, -1, 0), ngl::Vec3(0, 0, -1));
		// Create an modelview-orthographic projection matrix see from +Z axis
		ngl::Mat4 mvpZ = Ortho * ngl::lookAt(objectCenter + ngl::Vec3(0, 0, 0), objectCenter + ngl::Vec3(0, 0, -1), ngl::Vec3(0, 1, 0));
		shader->setUniform("mvpX", mvpX);
		shader->setUniform("mvpY", mvpY);
		shader->setUniform("mvpZ", mvpZ);
		shader->setUniform("voxelDim", m_voxelDim);
		shader->setUniform("orthoWidth", orthoWidth);
		shader->setUniform("sceneCenter", objectCenter);

		glBindTexture(GL_TEXTURE_3D, m_voxelAlbedoTex);
		glBindImageTexture(0, m_voxelAlbedoTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glBindTexture(GL_TEXTURE_3D, m_voxelNormalTex);
		glBindImageTexture(1, m_voxelNormalTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

		// draw our scene geometry
		drawScene();

		// set our voxel construction flag
		m_isVoxelTexConstructed = true;
		// re enable depth testing for drawing
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	}

	//----------------------------------------------------------------------------------------------------------------------
	/// LIGHT INJECTION PASS START
	//----------------------------------------------------------------------------------------------------------------------

	// Check if the emissive tex needs to be recreated, This occurs after a light is moved.
	if (m_isLightingDirty)
	{
		shader->use("lightInjectionPass");

		glBindTexture(GL_TEXTURE_3D, m_voxelEmissiveTex);
		glBindImageTexture(0, m_voxelEmissiveTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, m_voxelAlbedoTex);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, m_voxelNormalTex);

		shader->setUniform("voxelDim", m_voxelDim);
		shader->setUniform("orthoWidth", orthoWidth);
		shader->setUniform("sceneCenter", objectCenter);

		shader->setUniform("lightPosition", m_lightPosition);
		shader->setUniform("lightColors", m_lightColor);

		glDispatchCompute(ceil(m_voxelDim / 8.0), ceil(m_voxelDim / 8.0), ceil(m_voxelDim / 8.0));

		glBindTexture(GL_TEXTURE_3D, m_voxelEmissiveTex);
		glGenerateMipmap(GL_TEXTURE_3D);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, 0);

		m_isLightingDirty = false;
	}

	//----------------------------------------------------------------------------------------------------------------------
	/// G BUFFER PASS START
	//----------------------------------------------------------------------------------------------------------------------

	// Check if the FBO needs to be recreated. This occurs after a resize.
	if (m_isFBODirty)
	{
		initFBO();
	}

	// bind the gBuffer FBO
	// glBindFramebuffer(GL_FRAMEBUFFER, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);
	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);

	shader->use("gBufferPass");
	shader->setUniform("orthoWidth", orthoWidth);
	shader->setUniform("sceneCenter", objectCenter);

	/// first we reset the movement values
	float xDirection=0.0;
	float yDirection=0.0;
	// now we loop for each of the pressed keys in the the set
	// and see which ones have been pressed. If they have been pressed
	// we set the movement value to be an incremental value
	constexpr float s_update = 100.0f;
	foreach(Qt::Key key, m_keysPressed)
	{
		switch (key)
		{
			case Qt::Key_Left :  { yDirection=-s_update; break;}
			case Qt::Key_Right : { yDirection=s_update; break;}
			case Qt::Key_Up :		 { xDirection=s_update; break;}
			case Qt::Key_Down :  { xDirection=-s_update; break;}
			default : break;
		}
	}
	// if the set is non zero size we can update the camera movement
	if(m_keysPressed.size() !=0)
	{
		m_cam.move(xDirection,yDirection,m_deltaTime);
	}

	shader->setUniform("voxelDim", m_voxelDim);
	shader->setUniform("orthoWidth", orthoWidth);
	shader->setUniform("sceneCenter", objectCenter);

	// glActiveTexture(GL_TEXTURE5);
	// glBindTexture(GL_TEXTURE_3D, m_voxelAlbedoTex);
	// glActiveTexture(GL_TEXTURE6);
	// glBindTexture(GL_TEXTURE_3D, m_voxelNormalTex);
	// glActiveTexture(GL_TEXTURE7);
	// glBindTexture(GL_TEXTURE_3D, m_voxelEmissiveTex);
	// draw our scene geometry
	drawScene();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	//----------------------------------------------------------------------------------------------------------------------
	/// OUTPUT PASS START
	//----------------------------------------------------------------------------------------------------------------------

	// unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSPositionId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSNormalId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_FBODepthId);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_FBOAlbedoId);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_FBOMetalRoughId);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_3D, m_voxelEmissiveTex);

	shader->use("outputPass");
	shader->setUniform("windowSize", ngl::Vec2(m_win.width, m_win.height));

	// MVP for screenspace effects
	ngl::Mat4 SSMVP = ngl::Mat4(1.0f);
	SSMVP.rotateX(90);
	shader->setUniform("MVP", SSMVP);

	// camera position
	shader->setUniform("camPos", m_cam.getEye());

	// render passes bool
	shader->setUniform("viewDirectLight", m_viewDirectLight);
	shader->setUniform("viewIndirectLight", m_viewIndirectLight);
	shader->setUniform("viewReflections", m_viewReflections);

	shader->setUniform("specularApertureMultiplier", m_specularAperture);

	shader->setUniform("voxelDim", m_voxelDim);
	shader->setUniform("orthoWidth", orthoWidth);
	shader->setUniform("sceneCenter", objectCenter);

	shader->setUniform("lightPosition", m_lightPosition);
	shader->setUniform("lightColor", m_lightColor);

	prim->draw("ScreenAlignedQuad");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_3D, 0);

	//----------------------------------------------------------------------------------------------------------------------
	/// OUTPUT TESTING PASS START
	//----------------------------------------------------------------------------------------------------------------------
	/*
	// unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	// glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_testTexture);

	shader->use("outputTestPass");
	shader->setUniform("windowSize", ngl::Vec2(m_win.width, m_win.height));

	// MVP for screenspace effects
	ngl::Mat4 SSMVP = ngl::Mat4(1.0f);
	SSMVP.rotateX(90);
	shader->setUniform("MVP", SSMVP);

	prim->draw("ScreenAlignedQuad");
	*/
}
