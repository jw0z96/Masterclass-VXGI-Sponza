/*
  Copyright (C) 2009 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "boost/bind.hpp"

#include "boost/spirit.hpp"
#include <boost/algorithm/string.hpp>
#include "GroupedObj.h"
#include <ngl/VAOFactory.h>
#include <VAO.h>
//----------------------------------------------------------------------------------------------------------------------
/// @file GroupedObj.cpp

//----------------------------------------------------------------------------------------------------------------------



// make a namespace for our parser to save writing boost::spirit:: all the time
namespace spt=boost::spirit;

// syntactic sugar for specifying our grammar
typedef spt::rule<spt::phrase_scanner_t> srule;


//----------------------------------------------------------------------------------------------------------------------
// parse a vertex
void GroupedObj::parseVertex( const char *_begin )
{
  std::vector<float> values;
  // here is the parse rule to load the data into a vector (above)
  srule vertex = "v" >> spt::real_p[spt::append(values)] >>
                        spt::real_p[spt::append(values)] >>
                        spt::real_p[spt::append(values)];
  // now parse the data
  spt::parse_info<> result = spt::parse(_begin, vertex, spt::space_p);
  // should check this at some stage
  NGL_UNUSED(result);
  // and add it to our vert list in abstact mesh parent
  m_verts.push_back(ngl::Vec3(values[0],values[1],values[2]));
}


void GroupedObj::parseMaterial( const char *_begin )
{
  // set current group
  std::vector<std::string> material;
  srule grouping = "usemtl" >> *(spt::lexeme_d[spt::anychar_p >> *(spt::anychar_p)][spt::append(material)]);

  spt::parse_info<> result = parse(_begin, grouping, spt::space_p);
  //std::cout<<"parsing material "<<_begin<<"\n";
  NGL_UNUSED(result);
  if(material.empty())
  {
    m_currentMaterial="default";
  }
  else
  {
  m_currentMaterial=material[0];
  boost::trim(m_currentMaterial);
  }
  //std::cout<<"current material "<<m_currentMaterial<<"\n";
}


void GroupedObj::parseGroup(const char *_begin )
{

  // as the group is defined then the face data follows
  // we need to load the first set of faces before saving the data
  // this static int does that
  static int firstTime=0;

  std::cout<<"current group "<<m_currentMeshName<<"\n";
  if(firstTime++ >0)
  {
    // now we add the group to our list
    m_currentMesh.m_material=m_currentMaterial;
    m_currentMesh.m_name=m_currentMeshName;
    m_currentMesh.m_numVerts=m_faceCount;
    // index into the VAO data 3 tris with uv, normal and x,y,z as floats
    m_currentMesh.m_startIndex=m_offset;
    std::cout<<"Stored values fc "<<m_faceCount<<"\n";
    std::cout<<"offset "<<m_offset<<"\n";
    m_meshes.push_back(m_currentMesh);

    m_offset+=m_faceCount;
    m_faceCount=0;
  }


  // set current group
  std::vector<std::string> group;
  srule grouping = "g" >> *(spt::lexeme_d[spt::anychar_p >> *(spt::anychar_p)][spt::append(group)]);

  spt::parse_info<> result = parse(_begin, grouping, spt::space_p);
  NGL_UNUSED(result);
  if(group.empty())
  {
    m_currentMeshName="none";
  }
  else
  {
  m_currentMeshName=group[0];
  }

}



//----------------------------------------------------------------------------------------------------------------------
// parse a texture coordinate
void GroupedObj::parseTextureCoordinate( const char * _begin  )
{
  std::vector<float> values;
  // generate our parse rule for a tex cord,
  // this can be either a 2 or 3 d text so the *rule looks for an additional one
  srule texcord = "vt" >> spt::real_p[spt::append(values)] >>
                          spt::real_p[spt::append(values)] >>
                          *(spt::real_p[spt::append(values)]);
  spt::parse_info<> result = spt::parse(_begin, texcord, spt::space_p);
  // should check the return values at some stage
  NGL_UNUSED(result);

  // build tex cord
  // if we have a value use it other wise set to 0
  float vt3 = values.size() == 3 ? values[2] : 0.0f;
  m_tex.push_back(ngl::Vec3(values[0],values[1],vt3));
}

//----------------------------------------------------------------------------------------------------------------------
// parse a normal
void GroupedObj::parseNormal(const char *_begin )
{
  std::vector<float> values;
  // here is our rule for normals
  srule norm = "vn" >> spt::real_p[spt::append(values)] >>
                       spt::real_p[spt::append(values)] >>
                       spt::real_p[spt::append(values)];
  // parse and push back to the list
  spt::parse_info<> result = spt::parse(_begin, norm, spt::space_p);
  // should check the return values at some stage
  NGL_UNUSED(result);
  m_norm.push_back(ngl::Vec3(values[0],values[1],values[2]));
}


void GroupedObj::splitFace(const std::vector<unsigned int> &_v, const std::vector<unsigned int> &_t,  const std::vector<unsigned int> &_n )
{
  //std::cout<< "splitting quad face to triangles \n";
  // so now build a face structure.
  ngl::Face f1; // index 0 1 2
  ngl::Face f2; // index 0 2 3

  // verts are -1 the size
  f1.m_numVerts=2;
  f1.m_textureCoord=false;
  f2.m_numVerts=2;
  f2.m_textureCoord=false;
  // -1 as the index in obj is one less
  f1.m_vert.push_back(_v[0]-1);
  f1.m_vert.push_back(_v[1]-1);
  f1.m_vert.push_back(_v[2]-1);
  f2.m_vert.push_back(_v[0]-1);
  f2.m_vert.push_back(_v[2]-1);
  f2.m_vert.push_back(_v[3]-1);

  // merge in texture coordinates and normals, if present
  // OBJ format requires an encoding for faces which uses one of the vertex/texture/normal specifications
  // consistently across the entire face.  eg. we can have all v/vt/vn, or all v//vn, or all v, but not
  // v//vn then v/vt/vn ...
  if(!_n.empty())
  {
    if(_n.size() != _v.size())
    {
     std::cerr <<"Something wrong with the face data will continue but may not be correct\n";
    }

    f1.m_norm.push_back(_n[0]-1);
    f1.m_norm.push_back(_n[1]-1);
    f1.m_norm.push_back(_n[2]-1);
    f2.m_norm.push_back(_n[0]-1);
    f2.m_norm.push_back(_n[2]-1);
    f2.m_norm.push_back(_n[3]-1);
  }

  //
  // merge in texture coordinates, if present
  //
  if(!_t.empty())
  {
    if(_t.size() != _v.size())
    {
     std::cerr <<"Something wrong with the face data will continue but may not be correct\n";
    }

    // copy in these references to normal vectors to the mesh's normal vector

    f1.m_tex.push_back(_t[0]-1);
    f1.m_tex.push_back(_t[1]-1);
    f1.m_tex.push_back(_t[2]-1);
    f2.m_tex.push_back(_t[0]-1);
    f2.m_tex.push_back(_t[2]-1);
    f2.m_tex.push_back(_t[3]-1);

    f1.m_textureCoord=true;
    f2.m_textureCoord=true;

  }
// finally save the face into our face list
  m_face.push_back(f1);
  m_face.push_back(f2);
  m_faceCount+=6;
}

//----------------------------------------------------------------------------------------------------------------------
// parse face
void GroupedObj::parseFace( const char * _begin  )
{
  // ok this one is quite complex first create some lists for our face data
  // list to hold the vertex data indices
  std::vector<unsigned int> vec;
  // list to hold the tex cord indices
  std::vector<unsigned int> tvec;
  // list to hold the normal indices
  std::vector<unsigned int> nvec;

  // create the parse rule for a face entry V/T/N
  // so our entry can be always a vert, followed by optional t and norm seperated by /
  // also it is possible to have just a V value with no / so the rule should do all this
  srule entry = spt::int_p[spt::append(vec)] >>
    (
      ("/" >> (spt::int_p[spt::append(tvec)] | spt::epsilon_p) >>
       "/" >> (spt::int_p[spt::append(nvec)] | spt::epsilon_p)
      )
      | spt::epsilon_p
    );
  // a face has at least 3 of the above entries plus many optional ones
  srule face = "f"  >> entry >> entry >> entry >> *(entry);
  // now we've done this we can parse
 spt::parse(_begin, face, spt::space_p);

  size_t numVerts=vec.size();
  if(numVerts !=3 )
  {
    //std::cerr<<"Warning non-triangular face if quad will split else re-model. size is "<<numVerts<<"\n";
    if(numVerts == 4)
      splitFace(vec,tvec,nvec);
  }
  else
  {
  // so now build a face structure.
  ngl::Face f;
  // verts are -1 the size
  f.m_numVerts=numVerts-1;
  f.m_textureCoord=false;
  // copy the vertex indices into our face data structure index in obj start from 1
  // so we need to do -1 for our array index
  for(auto i : vec)
  {
    f.m_vert.push_back(i-1);
  }

  // merge in texture coordinates and normals, if present
  // OBJ format requires an encoding for faces which uses one of the vertex/texture/normal specifications
  // consistently across the entire face.  eg. we can have all v/vt/vn, or all v//vn, or all v, but not
  // v//vn then v/vt/vn ...
  if(!nvec.empty())
  {
    if(nvec.size() != vec.size())
    {
     std::cerr <<"Something wrong with the face data will continue but may not be correct\n";
    }

    // copy in these references to normal vectors to the mesh's normal vector
    for(auto i : nvec)
    {
      f.m_norm.push_back(i-1);
    }

  }

  //
  // merge in texture coordinates, if present
  //
  if(!tvec.empty())
  {
    if(tvec.size() != vec.size())
    {
     std::cerr <<"Something wrong with the face data will continue but may not be correct\n";
    }

    // copy in these references to normal vectors to the mesh's normal vector
    for(auto i : tvec)
    {
      f.m_tex.push_back(i-1);
    }

    f.m_textureCoord=true;

  }
// finally save the face into our face list
  m_face.push_back(f);
  m_faceCount+=3;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool GroupedObj::load(const std::string &_fname,CalcBB _calcBB  ) noexcept
{
 // here we build up our ebnf rules for parsing
  // so first we have a comment
  srule comment = spt::comment_p("#");

  // see below for the rest of the obj spec and other good format data
  // http://local.wasp.uwa.edu.au/~pbourke/dataformats/obj/

  // vertices rule v is a parse of 3 reals and we run the parseVertex function
  srule vertex= ("v"  >> spt::real_p >> spt::real_p >> spt::real_p) [bind(&GroupedObj::parseVertex,boost::ref(*this), _1)];
  /// our tex rule and binding of the parse function
  srule tex= ("vt" >> spt::real_p >> spt::real_p) [bind(&GroupedObj::parseTextureCoordinate, boost::ref(*this), _1)];
  // the normal rule and parsing function
  srule norm= ("vn" >> spt::real_p >> spt::real_p >> spt::real_p) [bind(&GroupedObj::parseNormal,boost::ref(*this), _1)];

  srule mat= ("usemtl" >> *(spt::anychar_p)) [bind(&GroupedObj::parseMaterial,boost::ref(*this), _1)];

  srule group= ("g" >> *(spt::anychar_p)) [bind(&GroupedObj::parseGroup,boost::ref(*this), _1)];

  // our vertex data can be any of the above values
  srule vertex_type = vertex | tex | norm ;

  // the rule for the face and parser
  srule  face = (spt::ch_p('f') >> *(spt::anychar_p))[bind(&GroupedObj::parseFace, boost::ref(*this), _1)];
  // open the file to parse
  std::ifstream in(_fname.c_str());
	if (in.is_open() != true)
	{
		std::cout<<"FILE NOT FOUND !!!! "<<_fname.c_str()<<"\n";
		return false;

	}
  std::string str;
  // loop grabbing a line and then pass it to our parsing framework
  while(getline(in, str))
  {
    spt::parse(str.c_str(), vertex_type  | mat | group | face | comment, spt::space_p);
  }
  // now we are done close the file
  in.close();

  // grab the sizes used for drawing later
  m_nVerts=m_verts.size();
  m_nNorm=m_norm.size();
  m_nTex=m_tex.size();
  m_nFaces=m_face.size();

  // Calculate the center of the object.
  if(_calcBB == CalcBB::True)
  {
    this->calcDimensions();
  }
  return true;

//	fileIn.close(); // close the file.
}

//----------------------------------------------------------------------------------------------------------------------
GroupedObj::GroupedObj( const std::string& _fname ) : ngl::AbstractMesh()
{
    m_vbo=false;
    m_ext=0;
    // set default values
    m_nVerts=m_nNorm=m_nTex=m_nFaces=0;
    //set the default extents to 0
    m_maxX=0.0f; m_maxY=0.0f; m_maxZ=0.0f;
    m_minX=0.0f; m_minY=0.0f; m_minZ=0.0f;
    m_offset=0;

    // load the file in

    m_texture = false;
    m_faceCount=0;
    m_currentMesh.m_startIndex=0;
    m_currentMesh.m_numVerts=0;

    m_loaded=load(_fname);
    // as the face triggers the push back of the meshes once we have finished the load we need to add the rest
    m_currentMesh.m_material=m_currentMaterial;
    m_currentMesh.m_name=m_currentMeshName;
    m_currentMesh.m_numVerts=m_faceCount;
    // index into the VAO data 3 tris with uv, normal and x,y,z as floats
    m_currentMesh.m_startIndex=m_offset;
    m_meshes.push_back(m_currentMesh);
    std::sort(m_meshes.begin(),m_meshes.end());
    // now create the VAO
    createVAO();
}

//----------------------------------------------------------------------------------------------------------------------
void GroupedObj::save( const std::string& _fname )const
{
  // Open the stream and parse
  std::fstream fileOut;
  fileOut.open(_fname.c_str(),std::ios::out);
  if (!fileOut.is_open())
  {
    std::cout <<"File : "<<_fname<<" Not founds "<<std::endl;
    return;
  }
  // write out some comments
  fileOut<<"# This file was created by ngl Obj exporter "<<_fname.c_str()<<std::endl;
  // was c++ 11  for(ngl::Vec3 v : m_norm) for all of these
  // write out the verts
  for(auto v : m_verts)
  {
    fileOut<<"v "<<v.m_x<<" "<<v.m_y<<" "<<v.m_z<<std::endl;
  }

  // write out the tex cords
  for(auto v : m_tex)
  {
    fileOut<<"vt "<<v.m_x<<" "<<v.m_y<<std::endl;
  }
  // write out the normals

  for(auto v : m_norm)
  {
    fileOut<<"vn "<<v.m_x<<" "<<v.m_y<<" "<<v.m_z<<std::endl;
  }

  // finally the faces
  for(auto f : m_face)
  {
  fileOut<<"f ";
  // we now have V/T/N for each to write out
  for(size_t i=0; i<f.m_numVerts; ++i)
  {
    // don't forget that obj indices start from 1 not 0 (i did originally !)
    fileOut<<f.m_vert[i]+1;
    fileOut<<"/";
    fileOut<<f.m_tex[i]+1;
    fileOut<<"/";

    fileOut<<f.m_norm[i]+1;
    fileOut<<" ";
  }
  fileOut<<'\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------

void GroupedObj::debugPrint()
{
  for(auto m : m_meshes)
  {
    std::cout<<"------------------------------------\n";
    std::cout<<"Name "<<m.m_name<<"\n";
    std::cout<<"Material "<<m.m_material<<"\n";
    std::cout<<"Num verts "<<m.m_numVerts<<"\n";
    std::cout<<"Start Index "<<m.m_startIndex <<"\n";
    std::cout<<"------------------------------------\n";
  }
}

void GroupedObj::draw(size_t _meshID) const
{
//  std::cout<<"Drawing \n";
  m_vaoMesh->bind();

  reinterpret_cast<VAO *>( m_vaoMesh.get())->draw(m_meshes[_meshID].m_startIndex,m_meshes[_meshID].m_numVerts );

  m_vaoMesh->unbind();

}

// first write out the mesh data m_meshes.size() then for each
// length std::string m_name;
// lenght std::string m_material;
//  unsigned int m_startIndex;
//  unsigned int m_numVerts;

bool GroupedObj::saveBinary(const std::string &_fname)
{
  std::ofstream fileOut;
  fileOut.open(_fname.c_str(),std::ios::out | std::ios::binary);
  if (!fileOut.is_open())
  {
    std::cout <<"File : "<<_fname<<" could not be written for output\n";
    return false;
  }

  // write our own id into the file so we can check we have the correct type
  // when loading
  const std::string header("ngl::objbin");
  fileOut.write(header.c_str(),header.length());

  size_t size=m_meshes.size();
  fileOut.write(reinterpret_cast<char *>(&size),sizeof(size));
  for(auto m : m_meshes)
  {
    size=m.m_name.length();
    fileOut.write(reinterpret_cast<char *>(&size),sizeof(size));
    // now the string
    fileOut.write(reinterpret_cast<const char *>(m.m_name.c_str()),size);
    size=m.m_material.length();
    fileOut.write(reinterpret_cast<char *>(&size),sizeof(size));
    // now the string
    fileOut.write(reinterpret_cast<const char *>(m.m_material.c_str()),size);
    fileOut.write(reinterpret_cast<char *>(&m.m_startIndex),sizeof(unsigned int));
    fileOut.write(reinterpret_cast<char *>(&m.m_numVerts),sizeof(unsigned int));
  }

  m_vaoMesh->bind();

  size=reinterpret_cast <VAO *>(m_vaoMesh.get())->getSize();

  fileOut.write(reinterpret_cast <char *>(&size),sizeof(unsigned int));



  ngl::Real *vboMem=this->mapVAOVerts();
  fileOut.write(reinterpret_cast<char *>(vboMem),size);
  this->unMapVAO();

  fileOut.close();
  return true;
}


// a simple structure to hold our vertex data
struct vertData
{
	GLfloat u; // tex cords from obj
	GLfloat v; // tex cords
	GLfloat nx; // normal from obj mesh
	GLfloat ny;
	GLfloat nz;
	GLfloat x; // position from obj
	GLfloat y;
	GLfloat z;
	GLfloat tx; // tangent calculated by us
	GLfloat ty;
	GLfloat tz;
	GLfloat bx; // binormal (bi-tangent really) calculated by us
	GLfloat by;
	GLfloat bz;
};

bool GroupedObj::loadBinary(const std::string &_fname)
{
  m_meshes.clear();
  std::ifstream fileIn;
  fileIn.open(_fname.c_str(),std::ios::in | std::ios::binary);
  if (!fileIn.is_open())
  {
    std::cout <<"File : "<<_fname<<" could not be opened for reading"<<std::endl;
    return false;
  }
  char header[12];
  fileIn.read(header,11*sizeof(char));
  header[11]=0; // for strcmp we need \n
  // basically I used the magick string ngl::bin (I presume unique in files!) and
  // we test against it.
  if(strcmp(header,"ngl::objbin"))
  {
    // best close the file and exit
    fileIn.close();
    std::cout<<"this is not an ngl::objbin file "<<std::endl;
    return false;
  }

  unsigned int vecsize;
  fileIn.read(reinterpret_cast<char *>(&vecsize),sizeof(vecsize));
  unsigned int size;
  std::string s;
  for(size_t i=0; i<vecsize; ++i)
  {
    MeshData d;
    fileIn.read(reinterpret_cast<char *>(&size),sizeof(size));
      // now the string we first need to allocate space then copy in
    s.resize(size);
    fileIn.read(reinterpret_cast<char *>(&s[0]),size);
    d.m_name=s;
    fileIn.read(reinterpret_cast<char *>(&size),sizeof(size));
      // now the string we first need to allocate space then copy in
    s.resize(size);
    fileIn.read(reinterpret_cast<char *>(&s[0]),size);
    d.m_material=s;
    fileIn.read(reinterpret_cast<char *>(&d.m_startIndex),sizeof(unsigned int));
    fileIn.read(reinterpret_cast<char *>(&d.m_numVerts),sizeof(unsigned int));
    m_meshes.push_back(d);
  }

  fileIn.read(reinterpret_cast<char *>(&size),sizeof(size));
  std::unique_ptr<ngl::Real []> data( new ngl::Real[size]);
  fileIn.read(reinterpret_cast<char *>(&data[0]),size);
  fileIn.close();

  // first we grab an instance of our VOA
  m_vaoMesh.reset(ngl::VAOFactory::createVAO("sponzaVAO"));
  // next we bind it so it's active for setting data
  m_vaoMesh->bind();
  m_meshSize=size;

  // now we have our data add it to the VAO, we need to tell the VAO the following
  // how much (in bytes) data we are copying
  // a pointer to the first element of data (in this case the address of the first element of the
  // std::vector
  m_vaoMesh->setData(VAO::VertexData(size,data[0]));
  // in this case we have packed our data in interleaved format as follows
  // u,v,nx,ny,nz,x,y,z
  // If you look at the shader we have the following attributes being used
  // attribute vec3 inVert; attribute 0
  // attribute vec2 inUV; attribute 1
  // attribute vec3 inNormal; attribure 2
  // so we need to set the vertexAttributePointer so the correct size and type as follows
  // vertex is attribute 0 with x,y,z(3) parts of type GL_FLOAT, our complete packed data is
  // sizeof(vertData) and the offset into the data structure for the first x component is 5 (u,v,nx,ny,nz)..x
  // a simple structure to hold our vertex data

  m_vaoMesh->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(vertData),5);
  // uv same as above but starts at 0 and is attrib 1 and only u,v so 2
  m_vaoMesh->setVertexAttributePointer(1,2,GL_FLOAT,sizeof(vertData),0);
  // normal same as vertex only starts at position 2 (u,v)-> nx
  m_vaoMesh->setVertexAttributePointer(2,3,GL_FLOAT,sizeof(vertData),2);
  // tangent same as vertex only starts at position 8 (u,v)-> nx
  m_vaoMesh->setVertexAttributePointer(3,3,GL_FLOAT,sizeof(vertData),8);

	// bi-tangent (or Binormal) same as vertex only starts at position 11 (u,v)-> nx
  m_vaoMesh->setVertexAttributePointer(4,3,GL_FLOAT,sizeof(vertData),11);


  // now we have set the vertex attributes we tell the VAO class how many indices to draw when
  // glDrawArrays is called, in this case we use buffSize (but if we wished less of the sphere to be drawn we could
  // specify less (in steps of 3))
  m_vaoMesh->setNumIndices(size/sizeof(vertData));
  // finally we have finished for now so time to unbind the VAO
  m_vaoMesh->unbind();

  // indicate we have a vao now
  m_vao=true;
  return true;

}



void GroupedObj::createVAO() noexcept
{

// else allocate space as build our VAO
	m_dataPackType=0;
	if(isTriangular())
	{
		m_dataPackType=GL_TRIANGLES;
		std::cout <<"Doing Tri Data"<<std::endl;
	}
	// data is mixed of > quad so exit error
	if(m_dataPackType == 0)
	{
		std::cerr<<"Can only create VBO from all Triangle or ALL Quad data at present"<<std::endl;
		exit(EXIT_FAILURE);
	}

  // now we are going to process and pack the mesh into an ngl::VertexArrayObject
  std::vector <vertData> vboMesh;
  vertData d;
  size_t loopFaceCount=3;


	// loop for each of the faces
  for(size_t i=0;i<m_nFaces;++i)
	{
		// now for each triangle in the face (remember we ensured tri above)
    for(size_t j=0;j<loopFaceCount;++j)
		{

			// pack in the vertex data first
			d.x=m_verts[m_face[i].m_vert[j]].m_x;
			d.y=m_verts[m_face[i].m_vert[j]].m_y;
			d.z=m_verts[m_face[i].m_vert[j]].m_z;
			// now if we have norms of tex (possibly could not) pack them as well
			if(m_nNorm >0 && m_nTex > 0)
			{

        d.nx=m_norm[m_face[i].m_norm[j]].m_x;
        d.ny=m_norm[m_face[i].m_norm[j]].m_y;
        d.nz=m_norm[m_face[i].m_norm[j]].m_z;

				d.u=m_tex[m_face[i].m_tex[j]].m_x;
				d.v=m_tex[m_face[i].m_tex[j]].m_y;

      }
      // now if neither are present (only verts like Zbrush models)
      else if(m_nNorm ==0 && m_nTex==0)
      {
        d.nx=0;
        d.ny=0;
        d.nz=0;
        d.u=0;
        d.v=0;
      }
      // here we've got norms but not tex
      else if(m_nNorm >0 && m_nTex==0)
      {
        d.nx=m_norm[m_face[i].m_norm[j]].m_x;
        d.ny=m_norm[m_face[i].m_norm[j]].m_y;
        d.nz=m_norm[m_face[i].m_norm[j]].m_z;
        d.u=0;
        d.v=0;
      }
      // here we've got tex but not norm least common
      else if(m_nNorm ==0 && m_nTex>0)
      {
        d.nx=0;
        d.ny=0;
        d.nz=0;
        d.u=m_tex[m_face[i].m_tex[j]].m_x;
        d.v=m_tex[m_face[i].m_tex[j]].m_y;
      }
      // now we calculate the tangent / bi-normal (tangent) based on the article here
      // http://www.terathon.com/code/tangent.html

      ngl::Vec3 c1 = m_norm[m_face[i].m_norm[j]].cross(ngl::Vec3(0.0, 0.0, 1.0));
      ngl::Vec3 c2 = m_norm[m_face[i].m_norm[j]].cross(ngl::Vec3(0.0, 1.0, 0.0));
      ngl::Vec3 tangent;
      ngl::Vec3 binormal;
      if(c1.length()>c2.length())
      {
        tangent = c1;
      }
      else
      {
        tangent = c2;
      }
      // now we normalize the tangent so we don't need to do it in the shader
      tangent.normalize();
      // now we calculate the binormal using the model normal and tangent (cross)
      binormal = m_norm[m_face[i].m_norm[j]].cross(tangent);
      // normalize again so we don't need to in the shader
      binormal.normalize();
      d.tx=tangent.m_x;
      d.ty=tangent.m_y;
      d.tz=tangent.m_z;
      d.bx=binormal.m_x;
      d.by=binormal.m_y;
      d.bz=binormal.m_z;

    vboMesh.push_back(d);
    }
  }

  // first we grab an instance of our VOA
  m_vaoMesh.reset( ngl::VAOFactory::createVAO ("sponzaVAO",m_dataPackType) );
  // next we bind it so it's active for setting data
  m_vaoMesh->bind();
  m_meshSize=vboMesh.size();

	// now we have our data add it to the VAO, we need to tell the VAO the following
	// how much (in bytes) data we are copying
	// a pointer to the first element of data (in this case the address of the first element of the
	// std::vector
  m_vaoMesh->setData(VAO::VertexData(m_meshSize*sizeof(vertData),vboMesh[0].u));
  // in this case we have packed our data in interleaved format as follows
	// u,v,nx,ny,nz,x,y,z
	// If you look at the shader we have the following attributes being used
	// attribute vec3 inVert; attribute 0
	// attribute vec2 inUV; attribute 1
	// attribute vec3 inNormal; attribure 2
	// so we need to set the vertexAttributePointer so the correct size and type as follows
	// vertex is attribute 0 with x,y,z(3) parts of type GL_FLOAT, our complete packed data is
	// sizeof(vertData) and the offset into the data structure for the first x component is 5 (u,v,nx,ny,nz)..x
  m_vaoMesh->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(vertData),5);
	// uv same as above but starts at 0 and is attrib 1 and only u,v so 2
  m_vaoMesh->setVertexAttributePointer(1,2,GL_FLOAT,sizeof(vertData),0);
	// normal same as vertex only starts at position 2 (u,v)-> nx
  m_vaoMesh->setVertexAttributePointer(2,3,GL_FLOAT,sizeof(vertData),2);
	// tangent same as vertex only starts at position 8 (u,v)-> nx
  m_vaoMesh->setVertexAttributePointer(3,3,GL_FLOAT,sizeof(vertData),8);

	// bi-tangent (or Binormal) same as vertex only starts at position 11 (u,v)-> nx
  m_vaoMesh->setVertexAttributePointer(4,3,GL_FLOAT,sizeof(vertData),11);


	// now we have set the vertex attributes we tell the VAO class how many indices to draw when
	// glDrawArrays is called, in this case we use buffSize (but if we wished less of the sphere to be drawn we could
	// specify less (in steps of 3))
  m_vaoMesh->setNumIndices(m_meshSize);
	// finally we have finished for now so time to unbind the VAO
  m_vaoMesh->unbind();

	// indicate we have a vao now
	m_vao=true;

}

