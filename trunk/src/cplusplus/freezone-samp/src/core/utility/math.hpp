#ifndef MATH_HPP
#define MATH_HPP
#include <cmath>
#include <cstdlib>

static const float pi = 3.141592653589793238462643383279502884197f;
static const float rad2deg = 180.0f / pi;

inline void get_rotated_xy(float& x, float& y, float distance, float angle) {
    x += (distance * std::sin(-angle*pi/180.0f));
    y += (distance * std::cos(-angle*pi/180.0f));
}

inline float my_rand(float val_min, float val_max) {
    return ((val_max-val_min)*((float)std::rand()/RAND_MAX))+val_min;
}

// Код работы с кватерионом с http://willperone.net/Code/quaternion.php
struct vector3f {
    float x;
    float y;
    float z;
    vector3f(float x, float y, float z): x(x), y(y), z(z) {}
};

struct quaternion {
    float    s; //!< the real component
    vector3f v; //!< the imaginary components
    quaternion(float s, float x, float y, float z): s(s), v(x,y,z) {}

    //! returns the euler angles from a rotation quaternion
    inline vector3f euler_angles() const {
        float sqw = s*s;    
        float sqx = v.x*v.x;    
        float sqy = v.y*v.y;    
        float sqz = v.z*v.z;    

        return vector3f(
             atan2f(2.f * (v.x*v.y + v.z*s), sqx - sqy - sqz + sqw)
            ,asinf(-2.f * (v.x*v.z - v.y*s))
            ,atan2f(2.f * (v.y*v.z + v.x*s), -sqx - sqy + sqz + sqw)
            );
    }
};

#endif // MATH_HPP
