#ifndef BAT_H
#define BAT_H

#pragma once
#include "icg_common.h"
#include "_triangle/Triangle.h"
#include "_quad/Quad.h"

class Bat
{
public:
    typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

    Quad body;
    Quad wingR;
    Quad wingL;

    void init()
    {

        wingR.init("_quad/wing.tga");
        wingL.init("_quad/wing.tga");
        body.init("_quad/bat.tga");
    }

    void cleanup()
    {
        body.cleanup();
        wingR.cleanup();
        wingL.cleanup();
    }

    void draw(float x, float y, float wingFlap, float scale)
    {
//        Transform head_M = Transform::Identity();
//        head_M *= Eigen::AlignedScaling3f(0.25, 0.25, 1.0);
//        head_M *= Eigen::AlignedScaling3f(scale, scale, 1.0);
//        head_M *= Eigen::Translation3f(x, y, 0.0);
//        head_M *= Eigen::Translation3f(0.0, 1.5, 0.0);
//        head_M *= Eigen::AlignedScaling3f(.5, .5, 1.0);

//        head.draw(head_M.matrix());

        Transform wingL_M = Transform::Identity();
        wingL_M *= Eigen::AlignedScaling3f(1.0, 1.0, 1.0);
        wingL_M *= Eigen::AlignedScaling3f(scale, scale, 1.0);
        wingL_M *= Eigen::Translation3f(x, y, 0.0);
        wingL_M *= Eigen::Translation3f(-0.15, -0.1, 0.0);
//        wingL_M *= Eigen::Affine3f(Eigen::AngleAxisf(M_PI/-2.0, vec3::UnitZ())).matrix();
        wingL_M *= Eigen::Affine3f(Eigen::AngleAxisf(wingFlap, vec3::UnitZ())).matrix();
        wingL_M *= Eigen::Translation3f(-0.45, 0.0, 0.0);
        wingL.draw(wingL_M.matrix());
        Transform wingR_M = Transform::Identity();
        wingR_M *= Eigen::AlignedScaling3f(-1.0, 1.0, 1.0);
        wingR_M *= Eigen::AlignedScaling3f(scale, scale, 1.0);
        wingR_M *= Eigen::Translation3f(-x, y, 0.0);
        wingR_M *= Eigen::Translation3f(-0.15, -0.1, 0.0);
        wingR_M *= Eigen::Affine3f(Eigen::AngleAxisf(wingFlap, vec3::UnitZ())).matrix();
        wingR_M *= Eigen::Translation3f(-0.45, 0.0, 0.0);
        wingR.draw(wingR_M.matrix());
        Transform body_M = Transform::Identity();
        body_M *= Eigen::AlignedScaling3f(1.0, 1.0, 1.0);
        body_M *= Eigen::AlignedScaling3f(scale, scale, 1.0);
        body_M *= Eigen::Translation3f(x, y, 0.0);
        body.draw(body_M.matrix());
    }



};

#endif // BAT_H
