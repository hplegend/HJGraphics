//
// Created by 何振邦 on 2020/1/20.
//

#ifndef HJGRAPHICS_VERTEX_H
#define HJGRAPHICS_VERTEX_H

#include "glm/glm.hpp"
namespace HJGraphics{
	using Vertex3=glm::vec3;
	struct Vertex6{
		glm::vec3 position;
		glm::vec3 normal;
		Vertex6()= default;
		Vertex6(glm::vec3 _position,glm::vec3 _normal){
			position=_position;
			normal=_normal;
		}
	};
	struct Vertex8{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		Vertex8()= default;
		Vertex8(const Vertex6& v3){
			position=v3.position;
			normal=v3.normal;
		}
		Vertex8(glm::vec3 _position,glm::vec3 _normal,glm::vec2 _texCoord){
			position=_position;
			normal=_normal;
			texCoord=_texCoord;
		}
	};
	struct Vertex14{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		Vertex14()= default;
		Vertex14(const Vertex6& v3){
			position=v3.position;
			normal=v3.normal;
		}
		Vertex14(const Vertex8& v8){
			position=v8.position;
			normal=v8.normal;
			texCoord=v8.texCoord;
		}
		Vertex14(glm::vec3 _position,glm::vec3 _normal,glm::vec2 _texCoord,glm::vec3 _tangent,glm::vec3 _bitangent){
			position=_position;
			normal=_normal;
			texCoord=_texCoord;
			tangent=_tangent;
			bitangent=_bitangent;
		}
	};
}
#endif //HJGRAPHICS_VERTEX_H
