#ifndef POS_CHANGE_CHECKER_HPP
#define POS_CHANGE_CHECKER_HPP
#include "basic_types.hpp"
class pos_change_checker {
    float threshold;
    pos3  last_pos;
public:
    pos_change_checker(float threshold = 5.0f): threshold(threshold) {}
    bool operator()(pos3 const& pos) {
        if (!is_points_in_sphere(pos, last_pos, threshold)) {
            last_pos = pos;
            return true;
        }
        return false;
    }
    void reset() {last_pos = pos3();}
};
#endif // POS_CHANGE_CHECKER_HPP
