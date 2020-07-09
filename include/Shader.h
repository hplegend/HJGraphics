//
// Created by 何振邦(m_iDev_0792) on 2017/9/10.
//


#define GL_SILENCE_DEPRECATION

#ifndef HJGRAPICS_SHADER_H
#define HJGRAPICS_SHADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLHeader.h"
namespace HJGraphics {
	class Shader {
	public:
		Shader(const std::string& vsCode, const std::string& fsCode, const std::string& gsCode);

		void use() { glUseProgram(id); };

		void setFloat(const std::string &name, float value) {
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		};

		void setBool(const std::string &name, bool value) {
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int) value);
		};

		void setInt(const std::string &name, int value) { glUniform1i(glGetUniformLocation(id, name.c_str()), value); };

		void setIntArray(const std::string &name, int *value, int count) {
			glUniform1iv(glGetUniformLocation(id, name.c_str()), count, value);
		}

		void set3fv(const std::string &name, glm::vec3 value) {
			glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
		};

		void set2fv(const std::string &name, glm::vec2 value) {
			glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
		};

		void set4fm(const std::string &name, glm::mat4 value) {
			glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
		};

		void bindBlock(const std::string &name, GLuint bindPoint) {
			glUniformBlockBinding(id, glGetUniformBlockIndex(id, name.c_str()), bindPoint);
		}

		GLuint getID() { return id; };

	private:
		GLuint id;

		bool checkCompileError(GLuint shader, std::string type);

	};
	void preprocessShaderCode(std::string &source, const std::string &basePath);

	Shader* makeShader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath = "");
	
	std::shared_ptr<Shader> makeSharedShader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath = "");
}

#endif //HJGRAPICS_SHADER_H
