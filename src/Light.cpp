//
// Created by 何振邦(m_iDev_0792) on 2018/12/23.
//

#include "Light.h"
#undef near
#undef far
/*
 * Implementation of Light class
 */
HJGraphics::Shader* HJGraphics::Light::debugShader=nullptr;
HJGraphics::Light::Light() :Light(LightType::ParallelLightType,glm::vec3(0.0f,0.0f,0.0f)){

}
HJGraphics::Light::Light(LightType _type, glm::vec3 _pos, glm::vec3 _lightColor) {
	type=_type;
	position=_pos;
	color=_lightColor;
	if(debugShader== nullptr)debugShader=makeShader("../shader/lineVertex.glsl","../shader/lineFragment.glsl");
	setShadowZValue(0.1f,50.0f);

	glGenFramebuffers(1,&shadowFramebuffer);
	glGenTextures(1,&shadowMap);

	glGenVertexArrays(1,&debugVAO);
	glGenBuffers(1,&debugVBO);
	glBindVertexArray(debugVAO);
	glBindBuffer(GL_ARRAY_BUFFER,debugVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6* sizeof(GLfloat),(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6* sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);

}
void HJGraphics::Light::debugDrawLight(GLuint sharedBindPoint) {
	//do nothing
}
void HJGraphics::Light::writeDebugData() {
	//do nothing
}
void HJGraphics::Light::setShadowMapSize(GLuint width, GLuint height) {
	shadowMapWidth=width;
	shadowMapHeight=height;
}
void HJGraphics::Light::setShadowZValue(GLfloat _zNear, GLfloat _zFar) {
	shadowZNear=_zNear;
	shadowZFar=_zFar;
}
void HJGraphics::Light::writeLightInfoUniform(Shader *lightShader) {
}
void HJGraphics::Light::updateLightMatrix() {

}
HJGraphics::ParallelLight::ParallelLight(glm::vec3 _dir, glm::vec3 _pos, glm::vec3 _color):Light(LightType::ParallelLightType,_pos,_color) {
	direction=_dir;
	setShadowMapSize(1024,1024);
	glBindTexture(GL_TEXTURE_2D,shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[4] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER,shadowFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	updateLightMatrix();
	writeDebugData();
}
/*
 * USED FOR LIGHT ILLUMINATION SHADING
 */
void HJGraphics::ParallelLight::writeLightInfoUniform(Shader *lightShader) {
	updateLightMatrix();

	lightShader->use();
	lightShader->set4fm("lightSpaceMatrix",lightMatrix);
	lightShader->set3fv("lightDirection",glm::normalize(direction));
	lightShader->set3fv("lightColor",color);
	lightShader->set3fv("lightPosition",position);
	lightShader->setInt("shadowMap",10);//activated in Scene::drawLight()
}
/*
 * USED FOR SHADOW GENERATION
 */
void HJGraphics::ParallelLight::updateLightMatrix() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);
	glm::mat4 lightView = glm::lookAt(position, position+direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection= glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowZNear, shadowZFar);
	lightMatrix=lightProjection*lightView;
}
void HJGraphics::ParallelLight::writeDebugData() {
	glm::vec3 color(0,0,1);
	glm::vec3 near= position + shadowZNear * direction;
	glm::vec3 far= position + shadowZFar * direction;
	glm::vec3 right=glm::normalize(glm::cross(direction,glm::vec3(0,1,0)));
	glm::vec3 up=glm::normalize(glm::cross(right,direction));
	float r=5;
	glm::vec3 bound[4]={near+r*(right+up),near+r*(-right+up),near+r*(-right-up),near+r*(right-up)};
	GLfloat data[]={
			//position        color
			position.x,position.y,position.z,1,0,0,
			near.x,near.y,near.z,1,0,0,

			near.x,near.y,near.z,0.976, 0.788, 0.000,
			far.x,far.y,far.z,0.976, 0.788, 0.000,

			bound[0].x,bound[0].y,bound[0].z,0.976, 0.788, 0.000,
			bound[1].x,bound[1].y,bound[1].z,0.976, 0.788, 0.000,

			bound[1].x,bound[1].y,bound[1].z,0.976, 0.788, 0.000,
			bound[2].x,bound[2].y,bound[2].z,0.976, 0.788, 0.000,

			bound[2].x,bound[2].y,bound[2].z,0.976, 0.788, 0.000,
			bound[3].x,bound[3].y,bound[3].z,0.976, 0.788, 0.000,

			bound[3].x,bound[3].y,bound[3].z,0.976, 0.788, 0.000,
			bound[0].x,bound[0].y,bound[0].z,0.976, 0.788, 0.000,

			bound[0].x,bound[0].y,bound[0].z,0.976, 0.788, 0.000,
			bound[2].x,bound[2].y,bound[2].z,0.976, 0.788, 0.000,

			bound[1].x,bound[1].y,bound[1].z,0.976, 0.788, 0.000,
			bound[3].x,bound[3].y,bound[3].z,0.976, 0.788, 0.000,
	};
	glBindBuffer(GL_ARRAY_BUFFER,debugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data),data,GL_STREAM_DRAW);
}
void HJGraphics::ParallelLight::debugDrawLight(GLuint sharedBindPoint) {

	debugShader->use();
	debugShader->set4fm("model",glm::mat4(1));
	debugShader->bindBlock("sharedMatrices",sharedBindPoint);
	glBindVertexArray(debugVAO);
	glDrawArrays(GL_LINES,0, 16);
	glBindVertexArray(0);
}
HJGraphics::SpotLight::SpotLight(glm::vec3 _dir, glm::vec3 _pos, glm::vec3 _color) :Light(LightType::SpotLightType,_pos,_color){
	linearAttenuation=0.0014f;
	quadraticAttenuation=0.007f;
	constantAttenuation=1.0f;
	innerAngle=10.0f;
	outerAngle=20.0f;
	direction=_dir;
	setShadowMapSize(1024,1024);
	glBindTexture(GL_TEXTURE_2D,shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[4] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER,shadowFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	updateLightMatrix();
	writeDebugData();
}
void HJGraphics::SpotLight::writeDebugData() {
	glm::vec3 color(0,0,1);
	glm::vec3 right=glm::length(glm::cross(direction,glm::vec3(0,1,0)))<0.05?
	                glm::vec3(1,0,0):
	                glm::normalize(glm::cross(direction,glm::vec3(0,1,0)));
	glm::vec3 up=glm::normalize(glm::cross(right,direction));
	glm::vec3 near=position+shadowZNear*direction;
	glm::vec3 far=position+shadowZFar*direction;
	float r=glm::tan(glm::radians(outerAngle))*shadowZFar;
	glm::vec3 bound[4]={far+r*(right),far+r*(up),far+r*(-right),far+r*(-up)};
	GLfloat data[]={
			//position        color
			position.x,position.y,position.z,1,0,0,
			near.x,near.y,near.z,1,0,0,
			near.x,near.y,near.z,0.976, 0.788, 0.000,
			far.x,far.y,far.z,0.976, 0.788, 0.000,

			position.x,position.y,position.z,0.976, 0.788, 0.000,
			bound[0].x,bound[0].y,bound[0].z,0.976, 0.788, 0.000,

			position.x,position.y,position.z,0.976, 0.788, 0.000,
			bound[1].x,bound[1].y,bound[1].z,0.976, 0.788, 0.000,

			position.x,position.y,position.z,0.976, 0.788, 0.000,
			bound[2].x,bound[2].y,bound[2].z,0.976, 0.788, 0.000,

			position.x,position.y,position.z,0.976, 0.788, 0.000,
			bound[3].x,bound[3].y,bound[3].z,0.976, 0.788, 0.000,

			bound[0].x,bound[0].y,bound[0].z,0.976, 0.788, 0.000,
			bound[1].x,bound[1].y,bound[1].z,0.976, 0.788, 0.000,

			bound[1].x,bound[1].y,bound[1].z,0.976, 0.788, 0.000,
			bound[2].x,bound[2].y,bound[2].z,0.976, 0.788, 0.000,

			bound[2].x,bound[2].y,bound[2].z,0.976, 0.788, 0.000,
			bound[3].x,bound[3].y,bound[3].z,0.976, 0.788, 0.000,

			bound[3].x,bound[3].y,bound[3].z,0.976, 0.788, 0.000,
			bound[0].x,bound[0].y,bound[0].z,0.976, 0.788, 0.000
	};
	glBindBuffer(GL_ARRAY_BUFFER,debugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data),data,GL_STREAM_DRAW);
}
void HJGraphics::SpotLight::debugDrawLight(GLuint sharedBindPoint) {
	debugShader->use();
	debugShader->set4fm("model",glm::mat4(1));
	debugShader->bindBlock("sharedMatrices",sharedBindPoint);
	glBindVertexArray(debugVAO);
	glDrawArrays(GL_LINES,0, 20);
	glBindVertexArray(0);
}
/*
 * USED FOR LIGHT ILLUMINATION SHADING
 */
void HJGraphics::SpotLight::writeLightInfoUniform(Shader *lightShader) {
	updateLightMatrix();

	glm::vec3 attenuationVec(linearAttenuation,quadraticAttenuation,constantAttenuation);
	glm::vec2 innerOuterCos(glm::cos(glm::radians(innerAngle)),glm::cos(glm::radians(outerAngle)));
	lightShader->use();
	lightShader->set4fm("lightSpaceMatrix",lightMatrix);
	lightShader->set3fv("lightDirection",glm::normalize(direction));
	lightShader->set3fv("lightColor",color);
	lightShader->set3fv("lightPosition",position);
	lightShader->set3fv("attenuationVec",attenuationVec);
	lightShader->set2fv("innerOuterCos",innerOuterCos);
	lightShader->setInt("shadowMap",10);//activated in Scene::drawLight()

}
/*
 * USED FOR SHADOW GENERATION
 */
void HJGraphics::SpotLight::updateLightMatrix() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);
	glm::mat4 lightView = glm::lookAt(position, position+direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection=glm::perspective<float>(glm::radians(2*outerAngle),1.0f,shadowZNear,shadowZFar);
	lightMatrix=lightProjection*lightView;
}
HJGraphics::PointLight::PointLight(glm::vec3 _pos, glm::vec3 _color):Light(LightType::PointLightType,_pos,_color) {
	linearAttenuation=0.0014f;
	quadraticAttenuation=0.007f;
	constantAttenuation=1.0f;
	setShadowMapSize(1024,1024);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP,shadowMap);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
		             shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER,shadowFramebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
	getGLError(__LINE__,__FILE__);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	updateLightMatrix();
	writeDebugData();
}
void HJGraphics::PointLight::writeDebugData() {
	glm::vec3 color(0,0,1);
	float len=1;
	float len2=len/std::sqrt(3);
	GLfloat data[]={
			//position        color
			position.x+len,position.y,position.z,0.976, 0.788, 0.000,
			position.x-len,position.y,position.z,0.976, 0.788, 0.000,

			position.x,position.y+len,position.z,0.976, 0.788, 0.000,
			position.x,position.y-len,position.z,0.976, 0.788, 0.000,

			position.x,position.y,position.z+len,0.976, 0.788, 0.000,
			position.x,position.y,position.z-len,0.976, 0.788, 0.000,

			position.x+len2,position.y-len2,position.z-len2,0.976, 0.788, 0.000,
			position.x-len2,position.y+len2,position.z+len2,0.976, 0.788, 0.000,

			position.x-len2,position.y+len2,position.z-len2,0.976, 0.788, 0.000,
			position.x+len2,position.y-len2,position.z+len2,0.976, 0.788, 0.000,

			position.x-len2,position.y-len2,position.z+len2,0.976, 0.788, 0.000,
			position.x+len2,position.y+len2,position.z-len2,0.976, 0.788, 0.000,

			position.x+len2,position.y+len2,position.z+len2,0.976, 0.788, 0.000,
			position.x-len2,position.y-len2,position.z-len2,0.976, 0.788, 0.000
	};
	glBindBuffer(GL_ARRAY_BUFFER,debugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data),data,GL_STREAM_DRAW);
}

void HJGraphics::PointLight::debugDrawLight(GLuint sharedBindPoint) {
	debugShader->use();
	debugShader->set4fm("model",glm::mat4(1));
	debugShader->bindBlock("sharedMatrices",sharedBindPoint);
	glBindVertexArray(debugVAO);
	glDrawArrays(GL_LINES,0, 14);
	glBindVertexArray(0);
}
/*
 * USED FOR LIGHT ILLUMINATION SHADING
 */
void HJGraphics::PointLight::writeLightInfoUniform(Shader *lightShader) {
	//no need to update lightMatrices here because point light shading doesn't need light matrices data

	glm::vec3 attenuationVec(linearAttenuation,quadraticAttenuation,constantAttenuation);
	lightShader->use();
	lightShader->set3fv("lightColor",color);
	lightShader->set3fv("lightPosition",position);
	lightShader->set3fv("attenuationVec",attenuationVec);
	lightShader->setFloat("shadowZFar",shadowZFar);
	lightShader->setInt("shadowMap",10);

}
/*
 * USED FOR SHADOW GENERATION
 */
void HJGraphics::PointLight::updateLightMatrix() {

	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), static_cast<float>(shadowMapWidth)/shadowMapHeight, shadowZNear, shadowZFar);

	lightMatrices[0]=shadowProj * glm::lookAt(position, position + glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0));
	lightMatrices[1]=shadowProj * glm::lookAt(position, position + glm::vec3(-1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0));
	lightMatrices[2]=shadowProj * glm::lookAt(position, position + glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,0.0,1.0));
	lightMatrices[3]=shadowProj * glm::lookAt(position, position + glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,-1.0));
	lightMatrices[4]=shadowProj * glm::lookAt(position, position + glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,-1.0,0.0));
	lightMatrices[5]=shadowProj * glm::lookAt(position, position + glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,-1.0,0.0));

}


////////////////////////////////////////////////////////////
///
///           Deferred shading version
///
///////////////////////////////////////////////////////////
std::shared_ptr<HJGraphics::Mesh2> HJGraphics::ParallelLight2::boundingMesh = nullptr;
std::vector<glm::mat4> HJGraphics::ParallelLight2::getLightMatrix() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);//TODO. potential bug. when direction is close to worldUp
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);
	glm::mat4 lightView = glm::lookAt(position, position + direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection = glm::ortho(-range, range, -range, range, shadowZNear, shadowZFar);
	std::vector<glm::mat4> mats;
	mats.push_back(lightProjection * lightView);
	return mats;
}
void HJGraphics::ParallelLight2::writeUniform(std::shared_ptr<Shader> lightShader) {
	auto mat = getLightMatrix();
	lightShader->set4fm("lightSpaceMatrix", mat[0]);
	lightShader->set3fv("lightDirection", glm::normalize(direction));
	lightShader->set3fv("lightColor", color);
	lightShader->set3fv("lightPosition", position);
}

std::vector<glm::mat4> HJGraphics::SpotLight2::getLightMatrix() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);
	glm::mat4 lightView = glm::lookAt(position, position + direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection = glm::perspective<float>(glm::radians(2 * outerAngle), 1.0f, shadowZNear, shadowZFar);
	std::vector<glm::mat4> mats;
	mats.push_back(lightProjection * lightView);
	return mats;
}

void HJGraphics::SpotLight2::writeUniform(std::shared_ptr<Shader> lightShader) {
	glm::vec3 attenuationVec(linearAttenuation, quadraticAttenuation, constantAttenuation);
	glm::vec2 innerOuterCos(glm::cos(glm::radians(innerAngle)), glm::cos(glm::radians(outerAngle)));
	
	auto mat = getLightMatrix();
	lightShader->set4fm("lightSpaceMatrix", mat[0]);
	lightShader->set3fv("lightDirection", glm::normalize(direction));
	lightShader->set3fv("lightColor", color);
	lightShader->set3fv("lightPosition", position);
	lightShader->set3fv("attenuationVec", attenuationVec);
	lightShader->set2fv("innerOuterCos", innerOuterCos);
}

std::vector<glm::mat4> HJGraphics::PointLight2::getLightMatrix() {
	std::vector<glm::mat4> lightMatrices;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, shadowZNear, shadowZFar);
	lightMatrices[0] = shadowProj * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[1] = shadowProj * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[2] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	lightMatrices[3] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
	lightMatrices[4] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[5] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

	return lightMatrices;
}

void HJGraphics::PointLight2::writeUniform(std::shared_ptr<Shader> lightShader) {
	//no need to update lightMatrices here because point light shading doesn't need light matrices data
	glm::vec3 attenuationVec(linearAttenuation, quadraticAttenuation, constantAttenuation);
	lightShader->set3fv("lightColor", color);
	lightShader->set3fv("lightPosition", position);
	lightShader->set3fv("attenuationVec", attenuationVec);
	lightShader->setFloat("shadowZFar", shadowZFar);
}
