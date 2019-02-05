//
// Created by 何振邦(m_iDev_0792) on 2018/12/20.
//

#ifndef TESTINGFIELD_SCENE_H
#define TESTINGFIELD_SCENE_H
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "DebugUtility.h"
namespace HJGraphics {
	const int BIND_POINT_MAX = 74;

/*
 * Declare of Scene class ,which is used to manage objects lights and cameras in a scene
 * 用于管理一个对象 灯光 相机等物件的场景类
 */
	class Scene {
	private:
		int getBindPointSlot();

		GLuint sharedUBO;//This UBO includes View mat Projection mat and ambient light factor
		GLuint lightNum;
		int sharedBindPoint;
		GLuint sceneWidth;
		GLuint sceneHeight;
		GLfloat ambientFactor;
		glm::vec3 clearColor;
		std::vector<BasicGLObject *> objects;
		std::vector<Camera *> cameras;
		std::vector<Light *> lights;
		Camera *mainCamera;
	public:
		static bool bindPointList[BIND_POINT_MAX];

		Scene();

		Scene(GLuint _sceneWidth, GLuint _sceneHeight, GLfloat _ambient, glm::vec3 _clearColor);

		//TODO. removeObject removeCamera addLight removeLight
		void addObject(BasicGLObject &object);

		void addObject(Model &model);

		void addCamera(Camera &camera);

		void addLight(Light &light);

		void setMainCamera(Camera &camera);

		void draw();

		void drawShadow();

		void drawLight();

		void writeSharedUBOData();
	};
}

#endif //TESTINGFIELD_SCENE_H
