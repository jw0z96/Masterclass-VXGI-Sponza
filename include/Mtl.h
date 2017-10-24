#ifndef MTL_H_
#define MTL_H_

/// @file Mtl.h
/// @brief a class to read Alias Mtl files and store the
/// data. It will also load in the textures and create GL textures
///
/// @author Jonathan Macey
/// @version 1.0
/// @date 5/11/12
/// Revision History :
/// @class Mtl
/// @brief Alias MTL loader and accessor
/// @todo add serialisation to save the data to binary formats for quick read / write
#include <ngl/Vec3.h>
#include <string>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

/* @note the illum section is as follows should enum at some stage
0		Color on and Ambient off
1		Color on and Ambient on
2		Highlight on
3		Reflection on and Ray trace on
4		Transparency: Glass on Reflection: Ray trace on
5		Reflection: Fresnel on and Ray trace on
6		Transparency: Refraction on Reflection: Fresnel off and Ray trace on
7		Transparency: Refraction on Reflection: Fresnel on and Ray trace on
8		Reflection on and Ray trace off
9		Transparency: Glass on Reflection: Ray trace off
10	Casts shadows onto invisible surfaces

*/

/// @brief a simple container for the material elements using a struct as these
/// are just dumb values also not using m_ for members as this makes it easier to
/// read and write the code to be compatible with the spec
/// using Vec3 for storage of the Colour elements not ngl::colour for size reasons
typedef struct
{
  float Ns;
  float Ni;
  float d;
  float Tr;
  int illum;
  ngl::Vec3 Tf;
  ngl::Vec3 Ka;
  ngl::Vec3 Kd;
  ngl::Vec3 Ks;
  ngl::Vec3 Ke;
  std::string map_Ka;
  std::string map_Kd;
  std::string map_d;
  std::string map_bump;
  std::string map_Ns;
  std::string bump;
  std::string map_Ks;
  GLuint map_KaId;
  GLuint map_KdId;
  GLuint map_dId;
  GLuint map_bumpId;
  GLuint map_NsId;
  GLuint bumpId;
  GLuint map_KsId;

}mtlItem;


class Mtl
{
  public :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor to load a mtl file
    //----------------------------------------------------------------------------------------------------------------------
    Mtl(){;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor to load a mtl file
    /// @param[in] _fname the name of the mtl file to load
    /// @param[in] _loadTextures flag to indicate if we load the openGL
    /// textures and store the id's this is default to on
    //----------------------------------------------------------------------------------------------------------------------
    Mtl(const std::string &_fname, bool _loadTextures=true);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor will clear the data and all glTextures
    //----------------------------------------------------------------------------------------------------------------------
    ~Mtl();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dump the materials stored to std::cout
    //----------------------------------------------------------------------------------------------------------------------
    void debugPrint() const;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to load mtl file
    /// @param[in] _fname the name of the file to load.
    /// @note no real check of the actual file type is done this could
    /// be a problem
    //----------------------------------------------------------------------------------------------------------------------
    bool load(const std::string &_fname);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to load textures as it is quite possible that different materials
    /// actually have the same textures we need to do some processing for this and
    /// only load them once, we then associate the id's in the mtlItem structure
    //----------------------------------------------------------------------------------------------------------------------
    void loadTextures();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief clear and tidy up everything (call this from the dtor as well as we need to
    /// clear quite a lot of GL texture stuff as well)
    //----------------------------------------------------------------------------------------------------------------------
    void clear();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief const iterator to the begining of the material list
    //----------------------------------------------------------------------------------------------------------------------
    inline std::unordered_map<std::string, mtlItem *>::const_iterator begin() const {return m_materials.begin(); }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief const iterator to the end of the material list
    //----------------------------------------------------------------------------------------------------------------------
    inline std::unordered_map<std::string, mtlItem *>::const_iterator end() const {return m_materials.end(); }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief  iterator to the begining of the material list
    //----------------------------------------------------------------------------------------------------------------------
    inline std::unordered_map<std::string, mtlItem *>::iterator begin() {return m_materials.begin(); }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief  iterator to the end of the material list
    //----------------------------------------------------------------------------------------------------------------------
    inline std::unordered_map<std::string, mtlItem *>::iterator end()  {return m_materials.end(); }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief  method to find the material by name
    /// @param [in] _n the name of the material to find
    /// @returns the material structure for that name
    //----------------------------------------------------------------------------------------------------------------------
    mtlItem * find(const std::string &_n)const;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief  save file in binary format
    /// @param[in] _fname the name of the file to load
    /// @returns true or false depending upon success
    //----------------------------------------------------------------------------------------------------------------------
    bool saveBinary(const std::string &_fname) const;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief  load file in binary format
    /// @param[in] _fname the name of the file to load
    /// @returns true or false depending upon success
    //----------------------------------------------------------------------------------------------------------------------
    bool loadBinary(const std::string &_fname);

  private :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a flag to indicate if we should load textures when reading file
    /// as this takes time we may just skip for later be default flag is on
    //----------------------------------------------------------------------------------------------------------------------
    bool m_loadTextures;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief map used to store the material for name lookup
    //----------------------------------------------------------------------------------------------------------------------
    std::unordered_map <std::string, mtlItem *> m_materials;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief current item used in the parser for storing values
    //----------------------------------------------------------------------------------------------------------------------
    mtlItem *m_current;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief name of the current material being parsed used for map
    //----------------------------------------------------------------------------------------------------------------------
    std::string m_currentName;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a list  of texture id's loaded so we can delete them in dtor
    //----------------------------------------------------------------------------------------------------------------------
    std::vector <GLuint> m_textureID;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief create a typecast to tokenizer as it's quicker to wrie than the whole
    /// line
    //----------------------------------------------------------------------------------------------------------------------
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief parse function for Vec3
    /// @param[in] _firstWord the iterator for the tokens
    /// @param[in out] io_value the value to set for this token
    //----------------------------------------------------------------------------------------------------------------------
    void parseVec3( tokenizer::iterator &_firstWord, ngl::Vec3 &io_value );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief parse function for float
    /// @param[in] _firstWord the iterator for the tokens
    /// @param[in out] io_value the value to set for this token
    //----------------------------------------------------------------------------------------------------------------------

    void parseFloat(tokenizer::iterator &_firstWord,  float &io_f);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief parse function for int
    /// @param[in] _firstWord the iterator for the tokens
    /// @param[in out] io_value the value to set for this token
    //----------------------------------------------------------------------------------------------------------------------
    void parseInt( tokenizer::iterator &_firstWord, int &io_f );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief parse function for string
    /// @param[in] _firstWord the iterator for the tokens
    /// @param[in out] io_value the value to set for this token
    //----------------------------------------------------------------------------------------------------------------------
    void parseString( tokenizer::iterator &_firstWord, std::string &io_s);
};



#endif
