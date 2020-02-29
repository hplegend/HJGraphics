//
// Created by 何振邦(m_iDev_0792) on 2019-02-07.
//

#include "Window.h"
HJGraphics::Window::Window(){

}
HJGraphics::Window::Window(int _width,int _height,std::string _title):GLFWWindowObject(_width,_height,_title){
	fov = 45.0f;
	firstMouse = true;
	mouseDown = false;
	mouseSensitivity = 0.2;
	lastX = static_cast<float>(width) / 2;
	lastY = static_cast<float>(height) / 2;
	yaw = 0.0f;
	pitch = 0.0f;
	moveSpeed=0.01;
	fps=60;
}
void HJGraphics::Window::inputCallback(long long deltaTime) {
	if (glfwGetKey(windowPtr, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(windowPtr, true);
		return;
	}
	float move = moveSpeed * deltaTime;
	auto pCamera=renderer->mainScene->getMainCamera();
	if(glfwGetKey(windowPtr, GLFW_KEY_A) == GLFW_PRESS){
		//left
		glm::vec3 cameraRight=glm::normalize(glm::cross(pCamera->direction,glm::vec3(0.0f,1.0f,0.0f)));
		pCamera->position-=cameraRight*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_D) == GLFW_PRESS){
		//right
		glm::vec3 cameraRight=glm::normalize(glm::cross(pCamera->direction,glm::vec3(0.0f,1.0f,0.0f)));
		pCamera->position+=cameraRight*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_W) == GLFW_PRESS){
		//front
		glm::vec3 cameraRight=glm::normalize(glm::cross(pCamera->direction,glm::vec3(0.0f,1.0f,0.0f)));
		glm::vec3 cameraFront=glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f),cameraRight));
		pCamera->position+=cameraFront*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_S) == GLFW_PRESS){
		//back
		glm::vec3 cameraRight=glm::normalize(glm::cross(pCamera->direction,glm::vec3(0.0f,1.0f,0.0f)));
		glm::vec3 cameraFront=glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f),cameraRight));
		pCamera->position-=cameraFront*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_Q) == GLFW_PRESS){
		//up
		pCamera->position+=glm::vec3(0,1,0)*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_E) == GLFW_PRESS){
		//down
		pCamera->position+=glm::vec3(0,-1,0)*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_M) == GLFW_PRESS){
		static bool wireMode=false;
		wireMode=!wireMode;
		if(wireMode){
			glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
		}else{
			glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
		}
	}
}
void HJGraphics::Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	auto pCamera=renderer->mainScene->getMainCamera();
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseDown = true;
		originalDirection=pCamera->direction;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseDown = false;
		firstMouse = true;
		yaw=0;
		pitch=0;
	}
}
void HJGraphics::Window::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	if (!mouseDown)return;
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	glm::mat4 yawMat(1.0f);
	glm::mat4 pitchMat(1.0f);
	glm::vec3 cameraRight=glm::normalize(glm::cross(originalDirection,glm::vec3(0.0f,1.0f,0.0f)));
	glm::vec3 cameraUp=glm::normalize(glm::cross(cameraRight,originalDirection));
	yawMat=glm::rotate(yawMat,glm::radians(yaw),-cameraUp);
	pitchMat=glm::rotate(pitchMat,glm::radians(pitch),cameraRight);
	glm::vec4 newDir=yawMat*glm::vec4(originalDirection,0.0f);
	newDir=pitchMat*newDir;
	auto pCamera=renderer->mainScene->getMainCamera();
	pCamera->direction=newDir;

}
void HJGraphics::Window::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	if (fov >= 1 && fov <= 60)fov -= yoffset;
	if (fov <= 1)fov = 1.0f;
	else if (fov >= 60)fov = 60.0f;
	auto pCamera=renderer->mainScene->getMainCamera();
	pCamera->fov=fov;
}
void HJGraphics::Window::framebufferSizeCallback(GLFWwindow *window, int width, int height) {

}
void HJGraphics::Window::customInit() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
//	glEnable(GL_CULL_FACE);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	if(renderer)renderer->renderInit();
}
void HJGraphics::Window::run() {
	glfwMakeContextCurrent(windowPtr);
	customInit();
	auto lastTime=std::chrono::high_resolution_clock::now();
	while(!shouldClose()){
		auto currentTime=std::chrono::high_resolution_clock::now();
		auto frameDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		if(frameDeltaTime<1000.0/fps)continue;
		inputCallback(frameDeltaTime);
		lastTime = currentTime;
		render();
		swapBuffer();
		glfwPollEvents();
	}
}
void HJGraphics::Window::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	if(renderer)renderer->render();
	else{
		currentScene->writeSharedUBOData();
		currentScene->draw();
	}
}

//-------------------------- NEW FUNCTION ----------------------------------//

void HJGraphics::Window::switchScene(int index) {
	if(index<0){
		std::cout<<"WARNING @ Window::switchScene(int) : index is less than 0"<<std::endl;
		return;
	}
	if(index>=scenes.size()){
		std::cout<<"WARNING @ Window::switchScene(int) : index is more than scenes size"<<std::endl;
		return;
	}
	currentScene=scenes.at(index);
}
void HJGraphics::Window::addScene(HJGraphics::Scene &_scene) {
	scenes.push_back(&_scene);
	if(scenes.size() == 1)currentScene=&_scene;
}