#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.h>
#include <shader_s.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory);

class Model
{
public:
	vector<Mesh> meshes;
	string directory;
	// 已加载的纹理，如果新纹理与之一致就使用已加载的
	vector<Texture> texture_loaded;
	Model(char* path)
	{
		loadModel(path);
	}
	void Draw(Shader shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}
private:
	void loadModel(string path)
	{
		// std::cout << "[Model] Loading: " << path << std::endl;

		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			// std::cout << "[Model] Assimp error: " << import.GetErrorString() << std::endl;
			throw std::runtime_error("Assimp failed to load scene");
		}

		// std::cout << "[Model] Scene loaded, meshes: " << scene->mNumMeshes << std::endl;

		directory = path.substr(0, path.find_last_of('/'));

		// std::cout << "[Model] scene=" << scene << "  mRootNode=" << scene->mRootNode << std::endl;

		// std::cout << "[Model] Directory: " << directory << std::endl;

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene)
	{
		if (!node) {
			std::cout << "[processNode] node is null!" << std::endl;
			return;
		}
		// std::cout << "[processNode] node->mName.length=" << node->mName.length << std::endl;
		// std::cout << "[processNode] Node name: " << node->mName.C_Str() << ", meshes: " << node->mNumMeshes << std::endl;
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// unsigned int meshIndex = node->mMeshes[i];   // ← 可能这里越界
			// std::cout << "[processNode]   mesh index: " << meshIndex << " (scene->mNumMeshes=" << scene->mNumMeshes << ")" << std::endl;

			// if (meshIndex >= scene->mNumMeshes) {
				// std::cout << "[processNode]   mesh index out of range!" << std::endl;
			// 	throw std::runtime_error("mesh index out of range");
			// }
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		// 处理子节点
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// 处理顶点、法线和纹理
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;

			// 顶点
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector; 
			// 法线
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			// 纹理
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoord = vec;
			}
			else
			{
				vertex.TexCoord = glm::vec2(0.0f, 0.0f);
			}
			vertices.push_back(vertex);
		}
		// 处理索引
		// 顶点索引信息在模型的面（图元）信息里，把面的顶点索引遍历一遍就可以获取
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				/* unsigned int idx = face.mIndices[j];
				if (idx >= mesh->mNumVertices) {   // ← 硬检查
					std::cout << "[processMesh] INDEX OUT OF RANGE: " << idx << " >= " << mesh->mNumVertices << '\n';
					continue;                      // 丢弃这个索引
				} */
				indices.push_back(face.mIndices[j]);
			}
		}
		// 处理材质
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		// if (!vertices.empty()) {
			// for (size_t i = 0; i < (indices.size() < 30 ? indices.size() : 30); ++i)
				// std::cout << indices[i] << (i + 1 == (indices.size() < 30 ? indices.size() : 30) ? "\n" : ", ");
		// }

		// std::cout << "[processMesh] vertices=" << vertices.size() << " indices=" << indices.size() << std::endl;

		return Mesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < texture_loaded.size(); j++)
			{
				if (strcmp(texture_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(texture_loaded[j]);
					skip = true;
					break;
				}
			}
			// 纹理未加载
			if (!skip)
			{
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.path = str.C_Str();
				texture.type = typeName;
				textures.push_back(texture);
				texture_loaded.push_back(texture);
			}
		}

		return textures;
	}
};

unsigned int TextureFromFile(const char* path, const string& directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

#endif