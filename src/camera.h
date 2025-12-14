#pragma once
#include <cmath>
#include "math.h" // or your Vec3 / Mat4 definitions

struct Camera {
    Vec3 position{0.0f, 1.7f, 0.0f}; // eye height
    float yaw   = 0.0f;
    float pitch = 0.0f;
    float speed = 5.0f;
    float sensitivity = 0.002f;

    Vec3 forward() const {
        return normalize({
            cosf(pitch) * sinf(yaw),
            sinf(pitch),
            cosf(pitch) * cosf(yaw)
        });
    }

    Vec3 right() const {
        return normalize(cross(forward(), {0,1,0}));
    }

    Mat4 viewMatrix() const {
        return Mat4::lookAt(position, position + forward(), {0,1,0});
    }

    void mouseLook(float dx, float dy) {
        yaw   += dx * sensitivity;
        pitch -= dy * sensitivity;

        const float limit = 1.55f;
        if(pitch >  limit) pitch =  limit;
        if(pitch < -limit) pitch = -limit;
    }

    void move(bool w,bool a,bool s,bool d,bool space,bool ctrl,float dt) {
        Vec3 dir{0,0,0};
        if(w) dir = dir + forward();
        if(s) dir = dir - forward();
        if(a) dir = dir - right();
        if(d) dir = dir + right();
        if(space) dir.y += 1.0f;
        if(ctrl)  dir.y -= 1.0f;

        if(dot(dir,dir) > 0)
            position = position + normalize(dir) * speed * dt;
    }
};
