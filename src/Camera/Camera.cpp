#include "Camera.h"

float Camera::deltaTime = 0.0f;
float Camera::lastFrame = 0.0f;

Camera::Camera()
	: cameraPos(glm::vec3(0.0f, 64.0f, 1.0f)),
	cameraFront(glm::vec3(0.0f, -1.0f, 0.0f)),
	cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	firstMouse(true),
	yaw(-90.0f),
	pitch(0.0f),
	//@TODO: change thing here
	lastX((float)1920 / 2.0f),
	lastY((float)1080 / 2.0f),
    fov(0.0f)
{
}

void Camera::keyboard_input(GLFWwindow* window)
{
    //change this to "glm::normalize(glm::vec3(cameraFront.x, 16.0f, cameraFront.z));"
    //for true FPS camera
    float cameraSpeed = static_cast<float>(15 * deltaTime);
    glm::vec3 flatFront = cameraFront;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * flatFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * flatFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
}

void Camera::Set_Data(float fovIn)
{
    fov = fovIn;
}

void Camera::set_cursor_callback(GLFWwindow* window, Camera* camera)
{
    glfwSetWindowUserPointer(window, camera);
    //check whether this works
    auto func = [](GLFWwindow* w, double xposIn, double yposIn) {
        static_cast<Camera*>(glfwGetWindowUserPointer(w))->mouse_callback(w, xposIn, yposIn);
    };

    glfwSetCursorPosCallback(window, func);
}

void Camera::calc_time(float currentFrame)
{
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}


void Camera::mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sens = 0.1f;
    xoffset *= sens;
    yoffset *= sens;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}

glm::mat4 Camera::getPerspectiveMatrix() {
    //change into SCR_WIDTH and SCR_HEIGHT
    return glm::perspective(glm::radians(fov), (float)1920 / (float)1080, 0.1f, 10000.0f);
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::vec3 Camera::getCameraFront() {
    return cameraFront;
}
