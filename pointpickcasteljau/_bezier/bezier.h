#pragma once
#include "icg_common.h"

class Bezier{
private:
    class Hull{
    public:
        Hull() {
            _p1 = _p2 = _p3 = _p4 = _p5 = _p6 = vec3::Zero();
        }

        vec3& p1(){ return _p1; }
        vec3& p2(){ return _p2; }
        vec3& p3(){ return _p3; }
        vec3& p4(){ return _p4; }
        vec3& p5(){ return _p5; }
        vec3& p6(){ return _p6; }

    private:
        vec3 _p1;
        vec3 _p2;
        vec3 _p3;
        vec3 _p4;
        vec3 _p5;
        vec3 _p6;
    };
    
private:
    Hull _hull;                  ///< control points
    std::vector<vec3> _vertices; ///< bezier points
    GLuint _vao;                 ///< Vertex array objects
    GLuint _pid;          ///< GLSL program ID
    GLuint _vbo;
    float const _uNum = 100;

private:

    /**
     * @brief lerp
     * @param point1
     * @param point2
     * @param t
     * @return vector3f
     * simple linear interpolation between two points
     */
    vec3 lerp(vec3 point1, vec3 point2, const float t)
    {
        float x = ((1 - t) * point1[0] + t * point2[0]);
        float y = ((1- t) * point1[1] + t * point2[1]);
        return vec3(x, y, 0);
    }

    /**
     * @brief getCasteljauVector
     * @param pt1
     * @param pt2
     * @param pt3
     * @param pt4
     * @param t
     * @return vector3f
     * get interpolation of all the points recursively for the bezier curve
     */
    vec3 getCasteljauVector(vec3 pt1, vec3 pt2, vec3 pt3, vec3 pt4, vec3 pt5, vec3 pt6, const float t)
    {
        vec3 aTob, bToc, cTod, dToe, eTof, abTobc, bcTocd, cdTode, deToef, abbcTobccd, bccdTocdde, cddeTodeef,
        abbcbccdTobccdcdde, bccdcddeTocddedeef;
        aTob = lerp(pt1, pt2, t);
        bToc = lerp(pt2, pt3, t);
        cTod = lerp(pt3, pt4, t);
        dToe = lerp(pt4, pt5, t);
        eTof = lerp(pt5, pt6, t);
        abTobc = lerp(aTob, bToc, t);
        bcTocd = lerp(bToc, cTod, t);
        cdTode = lerp(cTod, dToe, t);
        deToef = lerp(dToe, eTof, t);
        abbcTobccd = lerp(abTobc, bcTocd, t);
        bccdTocdde = lerp(bcTocd, cdTode, t);
        cddeTodeef = lerp(cdTode, deToef, t);
        abbcbccdTobccdcdde = lerp(abbcTobccd, bccdTocdde, t);
        bccdcddeTocddedeef = lerp(bccdTocdde, cddeTodeef, t);
        return lerp(abbcbccdTobccdcdde, bccdcddeTocddedeef, t);
    }

    /**
     * @brief casteljau
     * @param p
     * Calculate the bezier curve using the de Casteljau method
     */
    void casteljau(Hull & p)
    {
        float dt = 1.0 / _uNum;
        float t = 0.0;
        for(int i = 0; i <= _uNum; i++)
        {
            vec3 point = getCasteljauVector(p.p1(), p.p2(), p.p3(), p.p4(), p.p5(), p.p6(), t);
            _vertices.push_back(point);
            t += dt;
        }
    }

public:
    void init(GLuint pid){
        ///--- Set the (compiled) shaders
        _pid = pid;
        
        /// Generate the vertex array
        glGenVertexArrays(ONE, &_vao);
        glBindVertexArray(_vao);
        
        /// Generate one buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(ONE, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void set_points(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec3& p5, const vec3& p6) {
        _vertices.clear();

        ///--- initialize data
        _hull.p1() = p1;
        _hull.p2() = p2;
        _hull.p3() = p3;
        _hull.p4() = p4;
        _hull.p5() = p5;
        _hull.p6() = p6;

        ///--- create the bezier using the de Casteljau
        casteljau(_hull);
//        bezier(_hull);
    }

    void draw(const mat4& model, const mat4& view, const mat4& projection){
        if (_vertices.empty()) return;


        glUseProgram(_pid);
        glBindVertexArray(_vao);
        check_error_gl();

        ///--- Vertex Attribute ID for Vertex Positions
        GLuint position = glGetAttribLocation(_pid, "position");
        glEnableVertexAttribArray(position);
        glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        ///--- vertices
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*_vertices.size(), &_vertices[0], GL_STATIC_DRAW);

        ///--- setup view matrices
        GLuint projection_id = glGetUniformLocation(_pid, "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, projection.data());
        mat4 MV = view*model;
        GLuint model_view_id = glGetUniformLocation(_pid, "model_view");
        glUniformMatrix4fv(model_view_id, ONE, DONT_TRANSPOSE, MV.data());
        check_error_gl();
        glDrawArrays(GL_LINE_STRIP, 0, _vertices.size());
        glDisableVertexAttribArray(position);
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }
};
