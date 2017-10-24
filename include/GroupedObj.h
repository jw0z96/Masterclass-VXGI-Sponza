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
#ifndef GROUPEDOBJ_H_
#define GROUPEDOBJ_H_
//----------------------------------------------------------------------------------------------------------------------
/// @file GroupedObj.h
/// @brief This class loads in an Alias Obj file containing groups of meshes
/// it will extract each group and stored the name and associated material for each one
/// but still store the data as a single VertexArrayObjet. This allows for quick transfer of
/// data to and from the GPU but switching of model materials / textures. The mesh offset data is stored
/// in order of material name to reduce switching of textures / materials as well.
/// we also have iterator access to the internal data which will give name / material info as well
/// as the offset to the data in the VAO. Meshes with Quads will be split to triangles. Other meshes will give
/// errors but not crash.
/// @author Jonathan Macey
/// @version 1.0
/// @date 5/11/12
/// @todo write code to serialise the data in a binary format, best to store the VAO as a single blob and then
/// re-create it later.
/// Revision History :

//----------------------------------------------------------------------------------------------------------------------
// must include types.h first for ngl::Real and GLEW if required
#include <ngl/Types.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <ngl/Vec4.h>
#include <ngl/AbstractMesh.h>
#include <ngl/BBox.h>
#include <ngl/RibExport.h>
#include <cmath>

/// @brief simple data structure to store the Mesh information, it has an overloaded < operator
/// to allow for sorting by the Material name type
typedef struct MeshData
{
  /// @brief the name of the mesh group from the obj file
  std::string m_name;
  /// @brief the name of the material to use
  std::string m_material;
  /// @brief the starting index of the group in the VertexArrayObject
  size_t m_startIndex;
  /// @brief the number of vertices to draw from the start index
  size_t m_numVerts;
  /// @brief overloaded < operator for mesh sorting
  bool operator <(const MeshData &_r)const {return m_material < _r.m_material;}
}M;


class  GroupedObj : public ngl::AbstractMesh
{

public :

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief default constructor
  //----------------------------------------------------------------------------------------------------------------------
    GroupedObj(){;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  constructor to load an objfile as a parameter
  /// @param[in]  &_fname the name of the obj file to load
  //----------------------------------------------------------------------------------------------------------------------
  GroupedObj(const std::string& _fname );

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  Method to load the file in
  /// @param[in]  _fname the name of the obj file to load
  /// @param[in] _calcBB if we only want to load data and not use GL then set this to false
  //----------------------------------------------------------------------------------------------------------------------
  bool load(const std::string& _fname, CalcBB _calcBB=CalcBB::True) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  method to save the obj
  /// @param[in] _fname the name of the file to save
  //----------------------------------------------------------------------------------------------------------------------
  void save(const std::string& _fname ) const;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  method to save data structures and VAO as binary
  /// @param[in] _fname the name of the file to save
  /// @note as this maps the VAO it can't be const as OpenGL make this possible to
  /// mutate the data stored.
  //----------------------------------------------------------------------------------------------------------------------
  bool saveBinary(const std::string& _fname );

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  method to load the data structures and VAO as binary and re-build the
  /// mesh and all associations
  /// @param[in] _fname the name of the file to save
  //----------------------------------------------------------------------------------------------------------------------
  bool loadBinary( const std::string& _fname);


  //----------------------------------------------------------------------------------------------------------------------
  /// @brief const iterator to the begining of the mesh list
  //----------------------------------------------------------------------------------------------------------------------
  std::vector <MeshData>::const_iterator begin() const {return m_meshes.begin(); }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief const iterator to the end of the mesh list
  //----------------------------------------------------------------------------------------------------------------------
  std::vector <MeshData>::const_iterator end() const {return m_meshes.end(); }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  iterator to the begining of the mesh list
  //----------------------------------------------------------------------------------------------------------------------
  std::vector <MeshData>::iterator begin() {return m_meshes.begin(); }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  iterator to the end of the mesh list
  //----------------------------------------------------------------------------------------------------------------------
  std::vector <MeshData>::iterator end()  {return m_meshes.end(); }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  debug printing of the mesh group information
  //----------------------------------------------------------------------------------------------------------------------
  void debugPrint();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  draw a group item based on the index into the mesh array
  /// @param[in] _meshID the index into the groups to draw
  //----------------------------------------------------------------------------------------------------------------------
  void draw(size_t _meshID) const;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  accessor for the number of meshes in the array
  /// @returns m_meshes.size()
  //----------------------------------------------------------------------------------------------------------------------
  size_t numMeshes()const {return m_meshes.size(); }
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  accessor for the material name at index i
  /// @param[in] _m the index to access no error checking.
  /// @returns material at index.
  //----------------------------------------------------------------------------------------------------------------------
  std::string getMaterial(unsigned int _m)const {return m_meshes[_m].m_material;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  accessor for the group name name at index i
  /// @param[in] _m the index to access no error checking.
  /// @returns group name at index.
  //----------------------------------------------------------------------------------------------------------------------
  std::string getName(unsigned int _m)const {return m_meshes[_m].m_name;}

protected :
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief parser function to parse the vertex used by boost::spirit parser
  /// @param[in] _begin the start of the string to parse
  //----------------------------------------------------------------------------------------------------------------------
  void parseVertex(const char *_begin);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief parser function to parse the Norma used by boost::spirit parser
  /// @param[in] _begin the start of the string to parse
  //----------------------------------------------------------------------------------------------------------------------
  void parseNormal(const char *_begin  );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief parser function to parse the text cord used by boost::spirit parser
  /// @param[in] _begin the start of the string to parse
  //----------------------------------------------------------------------------------------------------------------------
  void parseTextureCoordinate( const char * _begin  );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief parser function to parse the Face data used by boost::spirit parser
  /// @param[in] _begin the start of the string to parse
  //----------------------------------------------------------------------------------------------------------------------
  void parseFace(const char * _begin );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief parser function to parse the material name used by boost::spirit parser
  /// @param[in] _begin the start of the string to parse
  //----------------------------------------------------------------------------------------------------------------------
  void parseMaterial(const char * _begin );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief parser function to parse the group name used by boost::spirit parser
  /// @param[in] _begin the start of the string to parse
  //----------------------------------------------------------------------------------------------------------------------
  void parseGroup(const char * _begin );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief split a quad face into two triangles and add them to the face list
  /// @param[in] _v the list of vertices found by the parser
  /// @param[in] _t the list of texture co-ords found by the parser
  /// @param[in] _n the list of normals found by the parser
  //----------------------------------------------------------------------------------------------------------------------

  void splitFace(const std::vector<unsigned int> &_v,const std::vector<unsigned int> &_t,const std::vector<unsigned int> &_n);
 private :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the list of mesh data that stores the group information
    //----------------------------------------------------------------------------------------------------------------------
    std::vector <MeshData> m_meshes;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the current mesh being populated by the parser. This is added to the list once a new group is found
    //----------------------------------------------------------------------------------------------------------------------
    MeshData m_currentMesh;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the name of the current mesh
    //----------------------------------------------------------------------------------------------------------------------
    std::string m_currentMeshName;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the name of the current material
    //----------------------------------------------------------------------------------------------------------------------
    std::string m_currentMaterial;
    /// @brief this is used to count the number of faces in the current group, updated by 3 each time we encounter a face
    /// if we split a face it updates by 6.
    //----------------------------------------------------------------------------------------------------------------------
    unsigned int m_faceCount;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the current face index offset into the VAO , this is used to count the offsets into the VAO and is incremented by m_faceCount each
    /// time we find a new group. it will also re-set faceCount to zero once this is done
    //----------------------------------------------------------------------------------------------------------------------
    unsigned int m_offset;
    void createVAO() noexcept;


};


#endif
//----------------------------------------------------------------------------------------------------------------------

