#include "Camera.h"

Camera::Camera(glm::mat4 in_projection, glm::mat4 in_view) 
    : m_projectonMatrix(in_projection), m_viewMatrix(in_view) {}

Camera::~Camera()
{
}

glm::mat4 Camera::getProjection() const {
    return m_projectonMatrix;
}

void Camera::setProjection(const glm::mat4& in_projection) {
    m_projectonMatrix = in_projection;
}

glm::mat4 Camera::getView() const {
    return m_viewMatrix;
}

void Camera::setView(const glm::mat4& in_view) {
    m_viewMatrix = in_view;
}

std::shared_ptr<Camera> mainCamera = nullptr;
//std::shared_ptr<Camera> secondaryCamera = nullptr;