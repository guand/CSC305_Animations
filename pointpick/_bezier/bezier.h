#pragma once
#include "icg_common.h"

class Bezier{
private:
    class Hull{
    public:
        Hull() {
            _p1 = _p2 = _p3 = _p4 = vec3::Zero();
        }

        vec3& p1(){ return _p1; }
        vec3& p2(){ return _p2; }
        vec3& p3(){ return _p3; }
        vec3& p4(){ return _p4; }

    private:
        vec3 _p1;
        vec3 _p2;
        vec3 _p3;
        vec3 _p4;
    };
    
private:
    Hull _hull;                  ///< control points
    std::vector<vec3> _vertices; ///< bezier points
    std::vector<vec3> _triangulation;
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
    vec3 getCasteljauVector(vec3 pt1, vec3 pt2, vec3 pt3, vec3 pt4, const float t)
    {
        vec3 aTob, bToc, cTod, abTobc, bcTocd;
        aTob = lerp(pt1, pt2, t);
        bToc = lerp(pt2, pt3, t);
        cTod = lerp(pt3, pt4, t);
        abTobc = lerp(aTob, bToc, t);
        bcTocd = lerp(bToc, cTod, t);
        return lerp(abTobc, bcTocd, t);
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
            vec3 point = getCasteljauVector(p.p1(), p.p2(), p.p3(), p.p4(), t);
            _vertices.push_back(point);
            t += dt;
        }
    }

    /**
     * @brief bezier
     * @param p
     * Basic bezier cubic calculation for creation a a bezier curve
     */
    void bezier(Hull & p)
    {
        // set the dt which is the ratio of the number of vertices
        float dt = 1.0 / _uNum;
        // t is set to 0
        float t = 0.0;
        for(int i = 0; i <= _uNum; i++)
        {
            // set the values for both (1 - t) and t
            float omt1 = 1.0 - t;
            float omt2 = pow(omt1, 2);
            float omt3 = pow(omt1, 3);
            float t2 = pow(t, 2);
            float t3 = pow(t, 3);
            // get the point after the bezier curve calculation
            vec3 point = omt3 * p.p1() + 3.0 * t * omt2 * p.p2() + 3.0 * t2 * omt1 * p.p3() + t3 * p.p4();
            // add vertex to the vector of vertices
            _vertices.push_back(point);
            // increment t by the ratio dt for the next vertex
            t += dt;
        }

    }

    /**
     * @brief triangulationPt
     * @param pt1
     * @param pt2
     * @param pt3
     * @param pt4
     * @return Vector3f
     * Gets the interesection point
     */
    vec3 triangulationPt(vec3 pt1, vec3 pt2, vec3 pt3, vec3 pt4)
    {
        float x1 = pt1[0], x2 = pt2[0], x3 = pt3[0], x4 = pt4[0];
        float y1 = pt1[1], y2 = pt2[1], y3 = pt3[1], y4 = pt4[1];

        float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
        float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
        float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

        return vec3(x, y, 0);
    }

    /**
     * @brief triangulation
     * @param p
     * performs triangulation on the curve
     */
    void triangulation(Hull & p)
    {
        for(int i = 0; i < _vertices.size(); i++){
            _triangulation.push_back(_vertices.at(i));
            vec3 pt1 = _vertices.at(i);
            vec3 pt2 = vec3(pt1[0], 1, 0);
            vec3 pt3 = p.p1();
            vec3 pt4 = p.p4();

            vec3 intersect = triangulationPt(pt1, pt2, pt3, pt4);
            _triangulation.push_back(intersect);
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

    void set_points(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4) {
        _vertices.clear();
        _triangulation.clear();

        ///--- initialize data
        _hull.p1() = p1;
        _hull.p2() = p2;
        _hull.p3() = p3;
        _hull.p4() = p4;

        ///--- create the bezier using the de Casteljau
//        casteljau(_hull);
        bezier(_hull);
        triangulation(_hull);
    }

    void draw(const mat4& model, const mat4& view, const mat4& projection){
        if (_vertices.empty()) return;

        if (_triangulation.empty()) return;


        glUseProgram(_pid);
        glBindVertexArray(_vao);
        check_error_gl();

        ///--- Vertex Attribute ID for Vertex Positions
        GLuint position = glGetAttribLocation(_pid, "position");
        glEnableVertexAttribArray(position);
        glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        ///--- vertices
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*_triangulation.size(), &_triangulation[0], GL_STATIC_DRAW);

        ///--- setup view matrices
        GLuint projection_id = glGetUniformLocation(_pid, "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, projection.data());
        mat4 MV = view*model;
        GLuint model_view_id = glGetUniformLocation(_pid, "model_view");
        glUniformMatrix4fv(model_view_id, ONE, DONT_TRANSPOSE, MV.data());
        check_error_gl();
        glDrawArrays(GL_LINE_STRIP, 0, _triangulation.size());
        glDisableVertexAttribArray(position);
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }
};
