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

// lights
static std::array<ngl::Vec3,4> g_lightPositions = {{
	ngl::Vec3(1000.0f,220.0f,0.0f),
	ngl::Vec3(300.0f,1320.0f,0.0f),
	ngl::Vec3(-300.0f,1320.0f,0.0f),
	ngl::Vec3(-1000.0f,220.0f,0.0f)
}};

constexpr float intensity=55000.0f;
static std::array<ngl::Vec3,4>  s_lightColors = {{
	ngl::Vec3(intensity, intensity, intensity),
	ngl::Vec3(intensity*10, intensity*10, intensity*10),
	ngl::Vec3(intensity*10, intensity*10, intensity*10),
	ngl::Vec3(intensity, intensity, intensity)

}};

void NGLScene::initializeGL()
{
	// we must call this first before any other GL commands to load and link the
	// gl commands from the lib, if this is not done program will crash
	ngl::NGLInit::instance();
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
	constexpr auto shaderProgram = "PBR";
	constexpr auto vertexShader  = "PBRVertex";
	constexpr auto fragShader    = "PBRFragment";
	// create the shader program
	shader->createShaderProgram( shaderProgram );
	// now we are going to create empty shaders for Frag and Vert
	shader->attachShader( vertexShader, ngl::ShaderType::VERTEX );
	shader->attachShader( fragShader, ngl::ShaderType::FRAGMENT );
	// attach the source
	shader->loadShaderSource( vertexShader, "shaders/PBRVertex.glsl" );
	shader->loadShaderSource( fragShader, "shaders/PBRFragment.glsl" );
	// compile the shaders
	shader->compileShader( vertexShader );
	shader->compileShader( fragShader );
	// add them to the program
	shader->attachShaderToProgram( shaderProgram, vertexShader );
	shader->attachShaderToProgram( shaderProgram, fragShader );


	// now we have associated that data we can link the shader
	shader->linkProgramObject( shaderProgram );
	// and make it active ready to load values
	( *shader )[ shaderProgram ]->use();
	shader->setUniform("camPos",m_cam.getEye());


	for(size_t i=0; i<g_lightPositions.size(); ++i)
	{
		shader->setUniform(("lightPositions[" + std::to_string(i) + "]").c_str(),g_lightPositions[i]);
		shader->setUniform(("lightColors[" + std::to_string(i) + "]").c_str(),s_lightColors[i]);
	}
	shader->setUniform("albedoMap", 0);
	shader->setUniform("normalMap", 1);
	shader->setUniform("metallicMap", 2);
	shader->setUniform("roughnessMap", 3);
	shader->setUniform("aoMap", 4);


	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
	// enable depth testing for drawing
	glEnable(GL_DEPTH_TEST);
	// enable multisampling for smoother drawing
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

	ngl::Vec3 from(0,40,-140);
	ngl::Vec3 to(0,40,0);
	ngl::Vec3 up(0,1,0);
	m_cam.set(from,to,up);
	// set the shape using FOV 45 Aspect Ratio based on Width and Height
	// The final two are near and far clipping planes of 0.5 and 10
	m_cam.setProjection(50,(float)1024/720,1.0f,800.0f);
	// now to load the shader and set the values
	// grab an instance of shader manager

	glEnable(GL_DEPTH_TEST);

	m_mtl.reset(  new Mtl);
	//bool loaded=m_mtl->loadBinary("sponzaMtl.bin");
	bool loaded=m_mtl->load("models/sponza.mtl");

	if(loaded == false)
	{
		std::cerr<<"error loading mtl file ";
		exit(EXIT_FAILURE);
	}


	m_model.reset(  new GroupedObj("models/sponza.obj"));
	//loaded=m_model->loadBinary("SponzaMesh.bin");
	if(loaded == false)
	{
		std::cerr<<"error loading obj file ";
		exit(EXIT_FAILURE);
	}

	// as re-size is not explicitly called we need to do this.
	glViewport(0,0,width(),height());

}

void NGLScene::initFBO()
{
	std::cout<<"initFBO call\n";
	// Unbind the framebuffer to revert to default render pipeline
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
//  ngl::Vec4 eye=m_cam.getEye();
//  eye=MV*eye;
	shader->setUniform("camPos",m_cam.getEye());

}

void NGLScene::paintGL()
{
	// Check if the FBO needs to be recreated. This occurs after a resize.
	if (m_isFBODirty) {
		initFBO();
		m_isFBODirty = false;
	}

	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	shader->use("PBR");

	float currentFrame = m_timer.elapsed()*0.001f;
	std::cout<<"Current Frame "<<currentFrame<<'\n';
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;
	/// first we reset the movement values
	float xDirection=0.0;
	float yDirection=0.0;
	// now we loop for each of the pressed keys in the the set
	// and see which ones have been pressed. If they have been pressed
	// we set the movement value to be an incremental value
	constexpr float s_update=40.0f;
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
	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);
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
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);

				};
				glActiveTexture(GL_TEXTURE0);
				glBindTexture (GL_TEXTURE_2D,currMaterial->map_KdId);
				setParams();
				glActiveTexture(GL_TEXTURE1);
				glBindTexture (GL_TEXTURE_2D,currMaterial->bumpId);
				setParams();
				glActiveTexture(GL_TEXTURE2);
				glBindTexture (GL_TEXTURE_2D,currMaterial->map_KaId);
				setParams();
				glActiveTexture(GL_TEXTURE3);
				glBindTexture (GL_TEXTURE_2D,currMaterial->map_KaId);
				setParams();
				glActiveTexture(GL_TEXTURE4);
				glBindTexture (GL_TEXTURE_2D,currMaterial->map_NsId);
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
		shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);

		for(size_t i=0; i<g_lightPositions.size(); ++i)
		{
			if(m_lightOn[i]==true)
			{
				tx.setPosition(g_lightPositions[i]);
				MVP=m_cam.getVP()* m_mouseGlobalTX * tx.getMatrix() ;
				shader->setUniform("MVP",MVP);
				ngl::VAOPrimitives::instance()->draw("cube");
			}
		}
	}

}


//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
	auto setLight=[](std::string _num,size_t _index,bool _mode)
	{
		ngl::ShaderLib *shader= ngl::ShaderLib::instance();
		shader->use("PBR");
		if(_mode == true)
		{
			shader->setUniform(_num,s_lightColors[_index]);
		}
		else
		{
			ngl::Vec3 colour={0.0f,0.0f,0.0f};
			shader->setUniform(_num,colour);

		}

	};

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
		case Qt::Key_1 :
		setLight("lightColors[0]",0,m_lightOn[0]^=true); break;
		case Qt::Key_2 :
		setLight("lightColors[1]",1,m_lightOn[1]^=true); break;
		case Qt::Key_3 :
		setLight("lightColors[2]",2,m_lightOn[2]^=true); break;
		case Qt::Key_4 :
		setLight("lightColors[3]",3,m_lightOn[3]^=true); break;

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
