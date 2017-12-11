#include "NGLScene.h"
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>

//----------------------------------------------------------------------------------------------------------------------

unsigned int NGLScene::gen3DTextureRGBA8(int dim) const
{
	GLuint texId;
	std::vector<GLfloat> emptyData(dim * dim * dim * 4, 0.0);
	glGenTextures(1, &texId );
	glBindTexture(GL_TEXTURE_3D, texId);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 3);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, dim, dim, dim, 0, GL_RGBA, GL_FLOAT, &emptyData[0]);
	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
	return texId;
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::initFBO()
{
	std::cout<<"initFBO call\n";

	// First delete the FBO if it has been created previously
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		glDeleteTextures(1, &m_FBOWSPositionId);
		glDeleteTextures(1, &m_FBOWSNormalId);
		glDeleteTextures(1, &m_FBODepthId);
		glDeleteTextures(1, &m_FBOAlbedoId);
		glDeleteTextures(1, &m_FBOMetalRoughId);
		glDeleteFramebuffers(1, &m_gBufferFBOId);
	}

	auto setParams=[]()
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	};

	// Create the frame buffer
	glGenFramebuffers(1, &m_gBufferFBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBOId);

	// Generate a texture to write the Position to
	glGenTextures(1, &m_FBOWSPositionId);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSPositionId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_win.width, m_win.height, 0, GL_RGB, GL_FLOAT, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBOWSPositionId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Normals to
	glGenTextures(1, &m_FBOWSNormalId);
	glBindTexture(GL_TEXTURE_2D, m_FBOWSNormalId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_win.width, m_win.height, 0, GL_RGB, GL_FLOAT, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_FBOWSNormalId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// The depth buffer is rendered to a texture buffer too,
	glGenTextures(1, &m_FBODepthId);
	glBindTexture(GL_TEXTURE_2D, m_FBODepthId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_win.width, m_win.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_FBODepthId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Albedo to
	glGenTextures(1, &m_FBOAlbedoId);
	glBindTexture(GL_TEXTURE_2D, m_FBOAlbedoId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_win.width, m_win.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_FBOAlbedoId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Generate a texture to write the Metallness and Roughness to
	glGenTextures(1, &m_FBOMetalRoughId);
	glBindTexture(GL_TEXTURE_2D, m_FBOMetalRoughId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_win.width, m_win.height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);
	setParams();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_FBOMetalRoughId, 0);
	// glBindTexture(GL_TEXTURE_2D, 0);

	// Set the fragment shader output targets DEPTH_ATTACHMENT is done automatically apparently
	unsigned int drawBufs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, drawBufs);

	// Check it is ready to rock and roll
	checkFrameBuffer();
	// Unbind the framebuffer to revert to default render pipeline
	glBindFramebuffer(GL_FRAMEBUFFER, 1);

	m_isFBODirty = false;
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::initTestFBO()
{
	std::cout<<"initTestFBO call\n";

	// First delete the FBO if it has been created previously
	glBindFramebuffer(GL_FRAMEBUFFER, m_testFBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE)
	{
		glDeleteTextures(1, &m_testTexture);
		glDeleteFramebuffers(1, &m_testFBO);
	}

	auto setParams=[]()
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	};

	// Generate a texture to write the Position to
	glGenTextures(1, &m_testTexture);
	glBindTexture(GL_TEXTURE_2D, m_testTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_voxelDim, m_voxelDim, 0, GL_RGB, GL_FLOAT, NULL);
	setParams();
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create the frame buffer
	glGenFramebuffers(1, &m_testFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_testFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_testTexture, 0);

	// Set the fragment shader output targets DEPTH_ATTACHMENT is done automatically apparently
	GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBufs);

	// Check it is ready to rock and roll
	checkFrameBuffer();
	// Unbind the framebuffer to revert to default render pipeline
	glBindFramebuffer(GL_FRAMEBUFFER, 1);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::checkFrameBuffer() const
{
	switch(glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
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

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::checkGLerror() const
{
	// check OpenGL error
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout <<"OpenGL error: "<<glewGetErrorString(err)<<"\n";
	}
}

//----------------------------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::drawScene()
{
	// get singleton instances
	ngl::ShaderLib* shader = ngl::ShaderLib::instance();
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

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
				auto setParams=[&]()
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
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
}
