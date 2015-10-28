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
    std::vector<vec3> _triangulation; ///< bezier points
    GLuint _vao;                 ///< Vertex array objects
    GLuint _pid;          ///< GLSL program ID
    GLuint _vbo;
    GLuint _tex;
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
            vec3 pt4 = p.p6();

            vec3 intersect = triangulationPt(pt1, pt2, pt3, pt4);
            _triangulation.push_back(intersect);
        }
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
    void init(string filename = ""){
        ///--- Compile the shaders
        _pid = opengp::load_shaders("_bezier/bezier_vshader.glsl", "_bezier/bezier_fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);
        glUseProgram(_pid);

        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        ///--- Vertex coordinates
        {
            const GLfloat vpoint[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                       /*V2*/ +1.0f, -1.0f, 0.0f,
                                       /*V3*/ -1.0f, +1.0f, 0.0f,
                                       /*V4*/ +1.0f, +1.0f, 0.0f };
            ///--- Buffer
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

            ///--- Attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        ///--- Texture coordinates
        {
            const GLfloat vtexcoord[] = { /*V1*/ 0.0f, 0.0f,
                                          /*V2*/ 1.0f, 0.0f,
                                          /*V3*/ 0.0f, 1.0f,
                                          /*V4*/ 1.0f, 1.0f};

            ///--- Buffer
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

            ///--- Attribute
            GLuint vtexcoord_id = glGetAttribLocation(_pid, "vtexcoord");
            glEnableVertexAttribArray(vtexcoord_id);
            glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        ///--- Load texture
        if(!filename.empty()){
            glGenTextures(1, &_tex);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glfwLoadTexture2D(filename.c_str(), 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        ///--- Texture uniforms
        GLuint tex_id = glGetUniformLocation(_pid, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

        ///--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void set_points(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4, const vec3& p5, const vec3& p6) {
        _vertices.clear();
        _triangulation.clear();

        ///--- initialize data
        _hull.p1() = p1;
        _hull.p2() = p2;
        _hull.p3() = p3;
        _hull.p4() = p4;
        _hull.p5() = p5;
        _hull.p6() = p6;

        ///--- create the bezier using the de Casteljau
        casteljau(_hull);
        triangulation(_hull);
//        bezier(_hull);
    }

    void draw(const mat4& MVP){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
            ///--- Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);

            ///--- Setup MVP
            GLuint MVP_id = glGetUniformLocation(_pid, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

            ///--- Draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glUseProgram(0);
    }
};
