#pragma once
#include "icg_common.h"

class ControlPoint {
public:
    vec3& position() {return _p;}
    unsigned int &id() {return _id;}
    bool &selected() {return _selected;}

    ControlPoint() {
        _p = vec3(0.0, 0.0, 0.0);
        _id = 0;
        _selected = false;
    }

    ControlPoint(float x, float y, float z, unsigned int id) {
        _p = vec3(x, y, z);
        _id = id;
        _selected = false;
    }

private:
    vec3 _p;
    unsigned int _id;
    bool _selected;

};
