#ifndef FIRSTPERSONCAMERA_H_
#define FIRSTPERSONCAMERA_H_
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
class FirstPersonCamera
{
  public :
    FirstPersonCamera()=default ;
    FirstPersonCamera(ngl::Vec3 _eye, ngl::Vec3 _look, ngl::Vec3 _up=ngl::Vec3::up()) noexcept;
    void set(ngl::Vec3 _eye, ngl::Vec3 _look, ngl::Vec3 _up=ngl::Vec3::up()) noexcept;
    void setProjection(ngl::Real _fov, ngl::Real _aspect, ngl::Real _near, ngl::Real _far) noexcept;
    ngl::Mat4 getView() const noexcept { return m_view;}
    ngl::Mat4 getProjection() const noexcept {return m_projection;}
    ngl::Mat4 getVP() const noexcept{return m_projection*m_view;}
    ngl::Vec3 getEye() const noexcept { return m_eye;}
    void updateCameraVectors() noexcept;

    void move(float _x, float _y, float _deltaTime);
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) noexcept;
    void processMouseScroll(float _yoffset) noexcept;
  private :
    ngl::Mat4 m_view;
    ngl::Mat4 m_projection;
    ngl::Vec3 m_eye={2,2,2};
    ngl::Vec3 m_look={0,0,0};
    ngl::Vec3 m_worldUp={0,1,0};
    ngl::Vec3 m_front;
    ngl::Vec3 m_up;
    ngl::Vec3 m_right;
    ngl::Real m_yaw=-90.0f;
    ngl::Real m_pitch=0.0f;
    ngl::Real m_speed=2.5f;
    ngl::Real m_sensitivity=0.1f;
    ngl::Real m_zoom=45.0f;
    ngl::Real m_near=0.1f;
    ngl::Real m_far=100.0f;
    ngl::Real m_aspect=1.2f;

};

#endif
