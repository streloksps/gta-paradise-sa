#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP
inline bool is_key_just_down(int key, int keys_new, int keys_old) {
    return (keys_new & key) && !(keys_old & key);
}
inline bool is_key_just_up(int key, int keys_new, int keys_old) {
    return !(keys_new & key) && (keys_old & key);
}

// Кнопки
enum {KEY_ACTION                = 1};
enum {KEY_CROUCH                = 2};
enum {KEY_FIRE                  = 4};
enum {KEY_SPRINT                = 8};
enum {KEY_SECONDARY_ATTACK      = 16}; // F
enum {KEY_JUMP                  = 32};
enum {KEY_LOOK_RIGHT            = 64};
enum {KEY_HANDBRAKE             = 128};
enum {KEY_LOOK_LEFT             = 256};
enum {KEY_SUBMISSION            = 512};
enum {KEY_LOOK_BEHIND           = 512};
enum {KEY_WALK                  = 1024};
enum {KEY_ANALOG_UP             = 2048};
enum {KEY_ANALOG_DOWN           = 4096};
enum {KEY_ANALOG_RIGHT          = 16384};
enum {KEY_ANALOG_LEFT           = 8192};

enum {KEY_UP                    = 65408};
enum {KEY_DOWN                  = 128};
enum {KEY_LEFT                  = 65408};
enum {KEY_RIGHT                 = 128};

enum keys_e {
     key_f = KEY_SECONDARY_ATTACK
    ,key_space = KEY_SPRINT
    ,key_alt = KEY_WALK
    ,key_shift = KEY_JUMP
    ,mouse_lbutton = KEY_FIRE
    ,mouse_rbutton = KEY_HANDBRAKE
    ,vehicle_accelerate = 8
    ,vehicle_decelerate = 32
};

#endif // KEYBOARD_HPP
