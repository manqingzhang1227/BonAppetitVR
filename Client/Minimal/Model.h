#ifndef MODEL_H
#define MODEL_H


#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <limits>

#include "Mesh.h"
#include "BoundingBox.hpp"
#include "BoundingBoxLines.h"


using namespace std;



unsigned int TextureFromFile( const char* path, const string &directory,
                              bool gamma = false );


class Model {
public:
  /*  Model Data */
  vector <Texture> textures_loaded;  // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  vector <Mesh> meshes;

  glm::mat4 toWorld;

  float xmin, ymin, zmin, xmax, ymax, zmax;

  bool isVisible;

  string directory;

  BoundingBoxLines* box;

  bool gammaCorrection;

  /*  Functions   */
  // constructor, expects a filepath to a 3D model.
  Model(string const &path , bool visible = true, bool gamma = false) : gammaCorrection(gamma) {
	  isVisible = visible;
	  xmin = std::numeric_limits<float>::max();
	  ymin = std::numeric_limits<float>::max();
	  zmin = std::numeric_limits<float>::max();
	  xmax = std::numeric_limits<float>::min();
	  ymax = std::numeric_limits<float>::min();
	  zmax = std::numeric_limits<float>::min();
	  loadModel( path );
	  loadBoundingBoxCoordinates();
	  box = new BoundingBoxLines(getObjectSpaceBoundingBoxVerticesAsVector());

  }


  // draws the model, and thus all its meshes

  //draw function for pig model only
  void
  Draw( GLuint shaderId, const glm::mat4 &projection, const glm::mat4 &view , std::unordered_map<int,int> colorMap, bool debug, GLuint boxShaderId) {
	
	  for (unsigned int i = 0; i < meshes.size(); i++) {
		  meshes[i].Draw(shaderId, projection, view, toWorld, colorMap.at(i));
	  }

	  if (debug) {
		  box->toWorld = toWorld;
		  box->draw(boxShaderId, projection, view, 0);
	  }
  }


  
  // overload draw
  void
	  Draw(GLuint shaderId, const glm::mat4 &projection, const glm::mat4 &view) {

	  for (unsigned int i = 0; i < meshes.size(); i++) {
		  meshes[i].Draw(shaderId, projection, view, toWorld);
	  }
  }

  //draw with bounding box
  // overload draw
  void
	  Draw(GLuint shaderId, const glm::mat4 &projection, const glm::mat4 &view, bool debug, GLuint boxShaderId) {

	  for (unsigned int i = 0; i < meshes.size(); i++) {
		  meshes[i].Draw(shaderId, projection, view, toWorld);
	  }

	  if (debug) {
		  box->toWorld = toWorld;
		  box->draw(boxShaderId, projection, view, 0);
	  }
	  glUseProgram(shaderId);

  }

  //for multi texture

  void loadBoundingBoxCoordinates() {
	  std::vector<glm::vec3> vert = getObjectSpaceBoundingBoxVerticesAsVector();
	  b.v1 = vert.at(0);
	  b.v2 = vert.at(1);
	  b.v3 = vert.at(2);
	  b.v4 = vert.at(3);
	  b.v5 = vert.at(4);
	  b.v6 = vert.at(5);
	  b.v7 = vert.at(6);
	  b.v8 = vert.at(7);
  }

  std::vector<glm::vec3> getObjectSpaceBoundingBoxVerticesAsVector() {

	  std::vector<glm::vec3> output;
	  output.push_back(glm::vec3(xmin, ymax, zmax));
	  output.push_back(glm::vec3(xmin, ymax, zmin));
	  output.push_back(glm::vec3(xmax, ymax, zmin));
	  output.push_back(glm::vec3(xmax, ymax, zmax));
	  output.push_back(glm::vec3(xmin, ymin, zmax));
	  output.push_back(glm::vec3(xmin, ymin, zmin));
	  output.push_back(glm::vec3(xmax, ymin, zmin));
	  output.push_back(glm::vec3(xmax, ymin, zmax));

	  return output;

  }

  BoundingBox getObjectSpaceBoundingBox() {
	  return b;
  }


private:
  BoundingBox b;
  /*  Functions   */
  // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void loadModel( string const &path ) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( path, aiProcess_Triangulate |
                                                    aiProcess_FlipUVs |
                                                    aiProcess_CalcTangentSpace );
    // check for errors
    if( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode ) // if is Not Zero
    {
      cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
      return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr( 0, path.find_last_of( '/' ) );

    // process ASSIMP's root node recursively
    processNode( scene->mRootNode, scene );
  }


  // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
  void processNode( aiNode* node, const aiScene* scene ) {
    // process each mesh located at the current node
    for( unsigned int i = 0; i < node->mNumMeshes; i++ ) {
      // the node object only contains indices to index the actual objects in the scene.
      // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back( processMesh( mesh, scene ) );
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for( unsigned int i = 0; i < node->mNumChildren; i++ ) {
      processNode( node->mChildren[i], scene );
    }

  }


  Mesh processMesh( aiMesh* mesh, const aiScene* scene ) {
    // data to fill
    vector <Vertex> vertices;
    vector <unsigned int> indices;
    vector <Texture> textures;

    // Walk through each of the mesh's vertices
    for( unsigned int i = 0; i < mesh->mNumVertices; i++ ) {
      Vertex vertex;
      glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
      // positions
      vector.x = mesh->mVertices[i].x;
	  xmin = std::min(xmin,vector.x);
	  xmax = std::max(xmax, vector.x);
      vector.y = mesh->mVertices[i].y;
	  ymin = std::min(ymin, vector.y);
	  ymax = std::max(ymax, vector.y);
      vector.z = mesh->mVertices[i].z;
	  zmin = std::min(zmin, vector.z);
	  zmax = std::max(zmax, vector.z);
      vertex.Position = vector;
      // normals
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.Normal = vector;
      // texture coordinates
      if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
      {
      	glm::vec2 vec;
      	// a vertex can contain up to 8 different texture coordinates. we thus make the assumption that we won't
      	// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
      	vec.x = mesh->mTextureCoords[0][i].x;
      	vec.y = mesh->mTextureCoords[0][i].y;
      	vertex.TexCoords = vec;
      }
      else{
      	vertex.TexCoords = glm::vec2(0.0f, 0.0f);
      // tangent
      //vector.x = mesh->mTangents[i].x;
      //vector.y = mesh->mTangents[i].y;
      //vector.z = mesh->mTangents[i].z;
      //vertex.Tangent = vector;
      //// bitangent
      //vector.x = mesh->mBitangents[i].x;
      //vector.y = mesh->mBitangents[i].y;
      //vector.z = mesh->mBitangents[i].z;
      //vertex.Bitangent = vector;
	  }
      vertices.push_back( vertex );
	  
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for( unsigned int i = 0; i < mesh->mNumFaces; i++ ) {
      aiFace face = mesh->mFaces[i];
      // retrieve all indices of the face and store them in the indices vector
      for( unsigned int j = 0; j < face.mNumIndices; j++ )
        indices.push_back( face.mIndices[j] );
    }

    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector <Texture> diffuseMaps = loadMaterialTextures( material,
                                                         aiTextureType_DIFFUSE,
                                                         "texture_diffuse" );
    textures.insert( textures.end(), diffuseMaps.begin(), diffuseMaps.end() );
    // 2. specular maps
    vector <Texture> specularMaps = loadMaterialTextures( material,
                                                          aiTextureType_SPECULAR,
                                                          "texture_specular" );
    textures.insert( textures.end(), specularMaps.begin(), specularMaps.end() );
    // 3. normal maps
    std::vector <Texture> normalMaps = loadMaterialTextures( material,
                                                             aiTextureType_HEIGHT,
                                                             "texture_normal" );
    textures.insert( textures.end(), normalMaps.begin(), normalMaps.end() );
    // 4. height maps
    std::vector <Texture> heightMaps = loadMaterialTextures( material,
                                                             aiTextureType_AMBIENT,
                                                             "texture_height" );
    textures.insert( textures.end(), heightMaps.begin(), heightMaps.end() );



    // return a mesh object created from the extracted mesh data
    return Mesh( vertices, indices, textures );
  }


  // checks all material textures of a given type and loads the textures if they're not loaded yet.
  // the required info is returned as a Texture struct.
  vector <Texture>
  loadMaterialTextures( aiMaterial* mat, aiTextureType type, string typeName ) {
    vector <Texture> textures;
    for( unsigned int i = 0; i < mat->GetTextureCount( type ); i++ ) {
      aiString str;
      mat->GetTexture( type, i, &str );
      // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
      bool skip = false;
      for( unsigned int j = 0; j < textures_loaded.size(); j++ ) {
        if( std::strcmp( textures_loaded[j].path.data(), str.C_Str() ) == 0 ) {
          textures.push_back( textures_loaded[j] );
          skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
          break;
        }
      }
      if( !skip ) {   // if texture hasn't been loaded already, load it
        Texture texture;
        texture.id = TextureFromFile( str.C_Str(), this->directory );
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back( texture );
        textures_loaded.push_back(
          texture );  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
      }
    }
    return textures;
  }
};


unsigned int
TextureFromFile(const char* path, const string &directory, bool gamma);


#endif
