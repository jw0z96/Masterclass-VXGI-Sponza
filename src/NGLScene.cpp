#include <QMouseEvent>
#include <QApplication>

#include "NGLScene.h"

#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>
#include "VAO.h"

//----------------------------------------------------------------------------------------------------------------------

NGLScene::NGLScene( QWidget *_parent ) : QOpenGLWidget( _parent )
{
	// set this widget to have the initial keyboard focus
	setFocusPolicy (Qt::StrongFocus);
	// re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
	this->resize(_parent->size());

	ngl::VAOFactory::registerVAOCreator("sponzaVAO",VAO::create);
	m_timer.start();

		m_lightPositions = {{
		ngl::Vec3(0.0f,220.0f,0.0f) //,
		// ngl::Vec3(300.0f,1320.0f,0.0f),
		// ngl::Vec3(-300.0f,1320.0f,0.0f),
		// ngl::Vec3(-1000.0f,220.0f,0.0f)
	}};

	float intensity=55000.0f;

	m_lightColors = {{
		ngl::Vec3(intensity, intensity, intensity) //,
		// ngl::Vec3(intensity*10, intensity*10, intensity*10),
		// ngl::Vec3(intensity*10, intensity*10, intensity*10),
		// ngl::Vec3(intensity, intensity, intensity)

	}};
}


NGLScene::~NGLScene()
{
	std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL( int _w, int _h )
{
	m_cam.setProjection( 45.0f, static_cast<float>( _w ) / _h, 0.5f, 3500.0f );
	m_win.width  = static_cast<int>( _w * devicePixelRatio() );
	m_win.height = static_cast<int>( _h * devicePixelRatio() );
	m_isFBODirty = true;
}



void NGLScene::setXPosition(double _val)
{
	m_lightPositions[0].m_x = _val;
	update();
}

void NGLScene::setYPosition(double _val)
{
	m_lightPositions[0].m_y = _val;
	update();
}
void NGLScene::setZPosition(double _val)
{
	m_lightPositions[0].m_z = _val;
	update();
}

void NGLScene::initializeGL()
{
	// we must call this first before any other GL commands to load and link the
	// gl commands from the lib, if this is not done program will crash
	ngl::NGLInit::instance();
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();

	// create the gBuffer shader program
	shader->loadShader("gBufferPass",
		"shaders/gBuffer_vert.glsl",
		"shaders/gBuffer_frag.glsl");
	shader->use("gBufferPass");

	shader->setUniform("camPos",m_cam.getEye());

	// for(size_t i=0; i<m_lightPositions.size(); ++i)
	// {
	// 	shader->setUniform(("lightPositions[" + std::to_string(i) + "]").c_str(),m_lightPositions[i]);
	// 	shader->setUniform(("lightColors[" + std::to_string(i) + "]").c_str(),m_lightColors[i]);
	// }

	shader->setUniform("albedoMap", 0);
	shader->setUniform("normalMap", 1);
	shader->setUniform("metallicMap", 2);
	shader->setUniform("roughnessMap", 3);

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

	// Grey Background
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	// enable depth testing for drawing
	glEnable(GL_DEPTH_TEST);
	// enable multisampling for smoother drawing
	glEnable(GL_MULTISAMPLE);

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	ngl::Vec3 from(0,40,-140);
	ngl::Vec3 to(0,40,0);
	ngl::Vec3 up(0,1,0);
	m_cam.set(from,to,up);
	// set the shape using FOV 45 Aspect Ratio based on Width and Height
	// The final two are near and far clipping planes of 0.5 and 10
	m_cam.setProjection(50,(float)1024/720,1.0f,800.0f);

	// load mtl file
	m_mtl.reset(new Mtl("models/sponza.mtl"));
	// load obj file
	m_model.reset(new GroupedObj("models/sponza.obj"));

	// generate screen aligned quad
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	prim->createTrianglePlane("ScreenAlignedQuad", 2, 2, 1, 1, ngl::Vec3(0,1,0));

	// as re-size is not explicitly called we need to do this.
	glViewport(0,0,width(),height());
}

void NGLScene::initFBO()
{
	std::cout<<"initFBO call\n";

	// SETUP THE G-BUFFER FBOS
	// First delete the FBO if it has been created previously
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE) {
		glDeleteTextures(1, &m_FBOWSPositionId);
		glDeleteTextures(1, &m_FBOWSNormalId);
		glDeleteTextures(1, &m_FBODepthId);
		glDeleteTextures(1, &m_FBOAlbedoId);
		glDeleteTextures(1, &m_FBOMetalRoughId);
		glDeleteFramebuffers(1, &m_gBufferFBOId);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 1);

	auto setParams=[]()
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	};

	// Generate a texture to write the Position to
	glGenTextures(1, &m_FBOWSPositionId);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSPositionId);
	glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGB16F,
				m_win.width,
				m_win.height,
				0,
				GL_RGB,
				GL_FLOAT,
				NULL);
	setParams();
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Normals to
	glGenTextures(1, &m_FBOWSNormalId);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSNormalId);
	glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGB16F,
				m_win.width,
				m_win.height,
				0,
				GL_RGB,
				GL_FLOAT,
				NULL);
	setParams();
	glBindTexture(GL_TEXTURE_2D, 0);

	// The depth buffer is rendered to a texture buffer too,
	glGenTextures(1, &m_FBODepthId);
	glBindTexture(GL_TEXTURE_2D, m_FBODepthId);
	glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_DEPTH_COMPONENT,
				m_win.width,
				m_win.height,
				0,
				GL_DEPTH_COMPONENT,
				GL_UNSIGNED_BYTE,
				NULL);
	setParams();
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Albedo to
	glGenTextures(1, &m_FBOAlbedoId);
	glBindTexture(GL_TEXTURE_2D, m_FBOAlbedoId);
	glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGB,
				m_win.width,
				m_win.height,
				0,
				GL_RGB,
				GL_FLOAT,
				NULL);
	setParams();
	glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Metallness and Roughness to
	glGenTextures(1, &m_FBOMetalRoughId);
	glBindTexture(GL_TEXTURE_2D, m_FBOMetalRoughId);
	glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RG,
				m_win.width,
				m_win.height,
				0,
				GL_RG,
				GL_FLOAT,
				NULL);
	setParams();
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create the frame buffer
	glGenFramebuffers(1, &m_gBufferFBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBOWSPositionId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_FBOWSNormalId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_FBOAlbedoId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_FBOMetalRoughId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_FBODepthId, 0);

	// Set the fragment shader output targets DEPTH_ATTACHMENT is done automatically apparently
	GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, drawBufs);

	// Check it is ready to rock and roll
	CheckFrameBuffer();

	// Unbind the framebuffer to revert to default render pipeline
	glBindFramebuffer(GL_FRAMEBUFFER, 1);
}

void NGLScene::loadMatricesToShader()
{
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	ngl::Mat4 MV;
	ngl::Mat4 MVP;
	ngl::Mat3 normalMatrix;
	ngl::Mat4 M;
	M            = m_transform.getMatrix() * m_mouseGlobalTX ;
	MV           = m_cam.getView() * M;
	MVP          = m_cam.getVP() * M;

	normalMatrix = MV;
	normalMatrix.inverse().transpose();
	shader->setUniform( "MVP", MVP );
	shader->setUniform( "normalMatrix", normalMatrix );
	shader->setUniform( "M", M );
	shader->setUniform("camPos",m_cam.getEye());
}

void NGLScene::paintGL()
{
	// Check if the FBO needs to be recreated. This occurs after a resize.
	if (m_isFBODirty)
	{
		initFBO();
		m_isFBODirty = false;
		// m_mtl->debugPrint();
	}

	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

	//----------------------------------------------------------------------------------------------------------------------
	/// G BUFFER PASS START
	//----------------------------------------------------------------------------------------------------------------------

	// bind the gBuffer FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);
	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);

	shader->use("gBufferPass");

	float currentFrame = m_timer.elapsed()*0.001f;
	// std::cout<<"Current Frame "<<currentFrame<<'\n';
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;
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
	// if the set is non zero size we can update the ship movement
	// then tell openGL to re-draw
	if(m_keysPressed.size() !=0)
	{
		m_cam.move(xDirection,yDirection,m_deltaTime);
	}

	auto end=m_model->numMeshes();
	std::string matName;

	if(m_drawGeo == true)
	{
		for(unsigned int i=0; i<end; ++i)
		{
			//m_mtl->use(m_model->getMaterial(i));
			mtlItem *currMaterial=m_mtl->find(m_model->getMaterial(i));
			if(currMaterial == 0) continue;
			// see if we need to switch the material or not this saves on OpenGL calls and
			// should speed things up
			if(matName !=m_model->getMaterial(i))
			{
				auto setParams=[]()
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

				};
				// bind albedo texture to texture unit 0
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, currMaterial->map_KdId);
				setParams();
				// bind normal texture to texture unit 1
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, currMaterial->map_bumpId);
				setParams();
				// bind metallic texture to texture unit 2
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, currMaterial->map_KaId);
				setParams();
				// bind roughness texture to texture unit 3
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, currMaterial->map_NsId);
				setParams();
				loadMatricesToShader();
			}
			m_model->draw(i);
		}
	}

	// Draw Lights
	if(m_drawLights)
	{
		( *shader )[ ngl::nglColourShader ]->use();
		ngl::Mat4 MVP;
		ngl::Transformation tx;
		tx.setScale(10.0, 10.0, 10.0); //make the light bigger
		shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);

		for(size_t i=0; i<m_lightPositions.size(); ++i)
		{
			if(m_lightOn[i]==true)
			{
				tx.setPosition(m_lightPositions[i]);
				MVP=m_cam.getVP()* m_mouseGlobalTX * tx.getMatrix() ;
				shader->setUniform("MVP",MVP);
				prim->draw("cube");
			}
		}
	}

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

	shader->use("outputPass");
	shader->setUniform("windowSize", ngl::Vec2(m_win.width, m_win.height));

	// MVP for screenspace effects
	ngl::Mat4 SSMVP = ngl::Mat4(1.0f);
	SSMVP.rotateX(90);
	shader->setUniform("MVP", SSMVP);

	// camera position
	shader->setUniform("camPos",m_cam.getEye());

	for(size_t i=0; i<m_lightPositions.size(); ++i)
	{
		shader->setUniform(("lightPositions[" + std::to_string(i) + "]").c_str(),m_lightPositions[i]);
		shader->setUniform(("lightColors[" + std::to_string(i) + "]").c_str(),m_lightColors[i]);
	}

	prim->draw("ScreenAlignedQuad");
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
	// auto setLight=[](std::string _num,size_t _index,bool _mode)
	// {
	// 	ngl::ShaderLib *shader= ngl::ShaderLib::instance();
	// 	shader->use("PBR");
	// 	if(_mode == true)
	// 	{
	// 		shader->setUniform(_num,m_lightColors[_index]);
	// 	}
	// 	else
	// 	{
	// 		ngl::Vec3 colour={0.0f,0.0f,0.0f};
	// 		shader->setUniform(_num,colour);

	// 	}

	// };

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
		// case Qt::Key_1 :
		// setLight("lightColors[0]",0,m_lightOn[0]^=true); break;
		// case Qt::Key_2 :
		// setLight("lightColors[1]",1,m_lightOn[1]^=true); break;
		// case Qt::Key_3 :
		// setLight("lightColors[2]",2,m_lightOn[2]^=true); break;
		// case Qt::Key_4 :
		// setLight("lightColors[3]",3,m_lightOn[3]^=true); break;

		default : break;


	}
	// finally update the GLWindow and re-draw
	//if (isExposed())
	update();
}

void NGLScene::keyReleaseEvent( QKeyEvent *_event	)
{
	// remove from our key set any keys that have been released
	m_keysPressed -= static_cast<Qt::Key>(_event->key());
}

/**
 * @brief Scene::CheckFrameBuffer Outputs result of test on the Framebuffer as nice string.
 * @return Nothing!
 */
void NGLScene::CheckFrameBuffer() noexcept
{
	switch(glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
	case GL_FRAMEBUFFER_UNDEFINED:
		std::cerr<<"GL_FRAMEBUFFER_UNDEFINED: returned if target is the default framebuffer, but the default framebuffer does not exist.\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: returned if any of the framebuffer attachment points are framebuffer incomplete.\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: returned if the framebuffer does not have at least one image attached to it.\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: returned if the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAWBUFFERi.\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: returned if GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.\n";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr<<"GL_FRAMEBUFFER_UNSUPPORTED: returned if the combination of internal formats of the attached images violates an implementation-dependent set of restrictions. GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE is also returned if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.\n";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr<<"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: returned if any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target.\n";
		break;
	case GL_FRAMEBUFFER_COMPLETE:
		std::cerr<<"GL_FRAMEBUFFER_COMPLETE: returned if everything is groovy!\n";
		break;
	default:
		std::cerr<<glCheckFramebufferStatus(GL_FRAMEBUFFER)<<": Undefined framebuffer return value: possible error elsewhere?\n";
		break;
	}
}
