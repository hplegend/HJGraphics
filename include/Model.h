//
// Created by 何振邦(m_iDev_0792) on 2019-02-03.
//

#ifndef HJGRAPHICS_MODEL_H
#define HJGRAPHICS_MODEL_H

#include <algorithm>
#include <map>
#include "Mesh.h"
#include "stb/stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace HJGraphics {
	class Model {
	public:
		std::map<std::string,std::shared_ptr<Texture>> textures_loaded;
		std::map<aiMaterial*,std::shared_ptr<Material>> materialLib;

		std::vector<std::shared_ptr<Mesh>> meshes;
		std::string directory;//used for reading images from same directory of model
		std::string format;
		MaterialType materialType;
		Model(const std::string& _path,MaterialType _materialType=MaterialType::BlinnPhong);

		void scale(float _ratio);

		bool setMaterialValue(const std::string& name,float value){
			bool flag=true;
			for(auto &m:meshes){
				if(!m->material->setValue(name,value))flag=false;
			}
			return flag;
		}

		bool setMaterialValue(const std::string& name,glm::vec3 value){
			bool flag=true;
			for(auto &m:meshes){
				if(!m->material->setValue(name,value))flag=false;
			}
			return flag;
		}

	private:
		void loadModel(const std::string& path);
		void processNode(aiNode *node, const aiScene *scene);
		std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		                                            std::string texUsage);
	};
}
#endif //HJGRAPHICS_MODEL_H
