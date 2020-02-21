//
// Created by 何振邦(m_iDev_0792) on 2018/12/23.
//

#ifndef TESTINGFIELD_LIGHT_H
#define TESTINGFIELD_LIGHT_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLHeader.h"
#include "DebugUtility.h"
#include "Shader.h"
namespace HJGraphics {
	enum LightType {
		ParallelLightType,
		PointLightType,
		SpotLightType
	};

	class Light {
	protected:
		GLuint debugVAO;
		GLuint debugVBO;
	public:
		glm::vec3 position;// position parameter will be used in shadow map rendering
		glm::vec3 color;
		glm::mat4 lightMatrix;
		GLuint shadowMap;
		GLuint shadowFramebuffer;
		GLuint shadowMapWidth;
		GLuint shadowMapHeight;
		GLfloat shadowZNear;
		GLfloat shadowZFar;
		LightType type;
		static Shader* debugShader;

		void setShadowMapSize(GLuint width, GLuint height);

		void setShadowZValue(GLfloat _zNear,GLfloat _zFar);

		virtual void writeLightInfoUniform(Shader *lightShader);

		virtual void updateLightMatrix();

		virtual void debugDrawLight(GLuint sharedBindPoint);

		virtual void writeDebugData();

		Light();

		Light(LightType _type, glm::vec3 _pos,glm::vec3 _lightColor = glm::vec3(1.0f, 1.0f, 1.0f));
	};

	class ParallelLight:public Light{
	public:
		glm::vec3 direction;

		ParallelLight(glm::vec3 _dir,glm::vec3 _pos= glm::vec3(0.0f, 5.0f, 0.0f),glm::vec3 _color= glm::vec3(1.0f, 1.0f, 1.0f));

		void writeLightInfoUniform(Shader *lightShader) override;

		void updateLightMatrix()override;

		void debugDrawLight(GLuint sharedBindPoint)override;

		void writeDebugData() override ;
	};
	class Mesh2;
	class SpotLight:public Light{
	public:
		glm::vec3 direction;
		GLfloat innerAngle;
		GLfloat outerAngle;

		GLfloat linearAttenuation;
		GLfloat quadraticAttenuation;
		GLfloat constantAttenuation;

		std::shared_ptr<Mesh2> boundingMesh;

		SpotLight(glm::vec3 _dir,glm::vec3 _pos= glm::vec3(0.0f, 5.0f, 0.0f),glm::vec3 _color= glm::vec3(1.0f, 1.0f, 1.0f));

		void writeLightInfoUniform(Shader *lightShader) override;

		void updateLightMatrix()override;

		void debugDrawLight(GLuint sharedBindPoint)override;

		void writeDebugData() override ;
	};

	class PointLight:public Light{
	public:
		GLfloat linearAttenuation;
		GLfloat quadraticAttenuation;
		GLfloat constantAttenuation;
		glm::mat4 lightMatrices[6];

		std::shared_ptr<Mesh2> boundingMesh;

		PointLight(glm::vec3 _pos= glm::vec3(0.0f, 5.0f, 0.0f),glm::vec3 _color= glm::vec3(1.0f, 1.0f, 1.0f));

		void writeLightInfoUniform(Shader *lightShader) override;

		void updateLightMatrix()override;

		void debugDrawLight(GLuint sharedBindPoint)override;

		void writeDebugData() override ;
	};
}
#endif //TESTINGFIELD_LIGHT_H
