//
//  Display.h
//  xmxGame
//
//  Created by Mingxian Xiao on 09/10/2019.
//  Copyright © 2019 XMX. All rights reserved.
//

#ifndef Display_h
#define Display_h

#include "Program.h"
#include "Box2D/Box2D.h"

struct Camera
{
    Camera()
    {
        center.Set(0.0f, 20.0f);
        width = 1280;
        height = 800;
    }

    void BuildProjectionMatrix(float32* m) {
        b2Vec2 extents(25.0f, 25.0f);

        b2Vec2 lower = center - extents;
        b2Vec2 upper = center + extents;

        m[0] = 2.0f / (upper.x - lower.x);
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;

        m[4] = 0.0f;
        m[5] = 2.0f / (upper.y - lower.y);
        m[6] = 0.0f;
        m[7] = 0.0f;

        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;

        m[12] = -(upper.x + lower.x) / (upper.x - lower.x);
        m[13] = -(upper.y + lower.y) / (upper.y - lower.y);
        m[14] = 0.0f;
        m[15] = 1.0f;
    }

    b2Vec2 center;
    int32 width;
    int32 height;
};

Camera cam;

struct GLRenderPoints
{
    void Create()
    {
        Program program("../Shaders/PointVS.glsl", "../Shaders/PointFS.glsl");
        programID = program.ID;
        std::cout << "Point Program ID: " << programID << std::endl;
        
        uniProjMat = glGetUniformLocation(programID, "uniProjMat");
        vAttrib = 0;
        cAttrib = 1;
        sAttrib = 2;
        
        // Generate
        glGenVertexArrays(1, &vaoID);
        glGenBuffers(3, vboIDs);
        
        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(vAttrib);
        glEnableVertexAttribArray(cAttrib);
        glEnableVertexAttribArray(sAttrib);
        
        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glVertexAttribPointer(vAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vboV), vboV, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glVertexAttribPointer(cAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vboC), vboC, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
        glVertexAttribPointer(sAttrib, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vboS), vboS, GL_DYNAMIC_DRAW);
        
        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        vCount = 0;
    }
    
    void Destroy()
    {
        if (vaoID)
        {
            glDeleteVertexArrays(1, &vaoID);
            glDeleteBuffers(2, vboIDs);
            vaoID = 0;
        }
        
        if (programID)
        {
            glDeleteProgram(programID);
            programID = 0;
        }
    }
    
    void Vertex(const b2Vec2& v, const b2Color& c, float32 size)
    {
        if (vCount == vNumMax)
            Flush();
        
        vboV[vCount] = v;
        vboC[vCount] = c;
        vboS[vCount] = size;
        ++vCount;
    }
    
    void Flush()
    {
        if (vCount == 0)
            return;
        
        glUseProgram(programID);
        
        float32 proj[16] = { 0.0f };
        cam.BuildProjectionMatrix(proj);
        
        glUniformMatrix4fv(uniProjMat, 1, GL_FALSE, proj);
        
        glBindVertexArray(vaoID);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(b2Vec2), vboV);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(b2Color), vboC);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(float32), vboS);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, vCount);
        glDisable(GL_PROGRAM_POINT_SIZE);
//
//        sCheckGLError();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
        vCount = 0;
    }
    
    static const int vNumMax = 512;
    b2Vec2 vboV[vNumMax];
    b2Color vboC[vNumMax];
    float32 vboS[vNumMax];

    int32 vCount;
    
    GLuint vaoID;
    GLuint vboIDs[3];
    GLuint programID;
    GLint uniProjMat;
    GLint vAttrib;
    GLint cAttrib;
    GLint sAttrib;
};

//
struct GLRenderLines
{
    void Create()
    {
        Program program("../Shaders/LineVS.glsl", "../Shaders/LineFS.glsl");
        programID = program.ID;
        std::cout << "Line Program ID: " << programID << std::endl;
        
        uniProjMat = glGetUniformLocation(programID, "uniProjMat");
        vAttrib = 0;
        cAttrib = 1;
        
        // Generate
        glGenVertexArrays(1, &vaoID);
        glGenBuffers(2, vboIDs);
        
        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(vAttrib);
        glEnableVertexAttribArray(cAttrib);
        
        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glVertexAttribPointer(vAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vboV), vboV, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glVertexAttribPointer(cAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vboC), vboC, GL_DYNAMIC_DRAW);
        
//        sCheckGLError();
        
        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        vCount = 0;
    }
    
    void Destroy()
    {
        if (vaoID)
        {
            glDeleteVertexArrays(1, &vaoID);
            glDeleteBuffers(2, vboIDs);
            vaoID = 0;
        }
        
        if (programID)
        {
            glDeleteProgram(programID);
            programID = 0;
        }
    }
    
    void Vertex(const b2Vec2& v, const b2Color& c)
    {
        if (vCount == vNumMax)
            Flush();
        
        vboV[vCount] = v;
        vboC[vCount] = c;
        ++vCount;
    }
    
    void Flush()
    {
        if (vCount == 0)
            return;
        
        glUseProgram(programID);
        
        float32 proj[16] = { 0.0f };
        cam.BuildProjectionMatrix(proj);
        
        glUniformMatrix4fv(uniProjMat, 1, GL_FALSE, proj);
        
        glBindVertexArray(vaoID);

        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(b2Vec2), vboV);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(b2Color), vboC);
        
        glDrawArrays(GL_LINES, 0, vCount);
        
//        sCheckGLError();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
        vCount = 0;
    }
    
    static const int vNumMax = 512 * 2;
    b2Vec2 vboV[vNumMax];
    b2Color vboC[vNumMax];
    
    int32 vCount;
    
    GLuint vaoID;
    GLuint vboIDs[2];
    GLuint programID;
    GLint uniProjMat;
    GLint vAttrib;
    GLint cAttrib;
};

//
struct GLRenderTriangles
{
    void Create() // Prepare for rendering
    {
        
        Program program("../Shaders/TriangleVS.glsl", "../Shaders/TriangleFS.glsl");
        programID = program.ID;
        std::cout << "Triangle Program ID: " << programID << std::endl;
        
        uniProjMat = glGetUniformLocation(programID, "uniProjMat");
        vAttrib = 0;
        cAttrib = 1;

        // Generate
        glGenVertexArrays(1, &vaoID);
        glGenBuffers(2, vboIDs);

        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(vAttrib);
        glEnableVertexAttribArray(cAttrib);

        // NOTICE: m_vertices may be null at this time, the actual vertex data will be written by glBufferSubData()
        // m_vboIds[0] : Vertex POSITION buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glVertexAttribPointer(vAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vboV), vboV, GL_DYNAMIC_DRAW);
        // m_vboIds[1] : Vertex COLOR buffer
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glVertexAttribPointer(cAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vboC), vboC, GL_DYNAMIC_DRAW);

//        sCheckGLError();

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbined VBO
        glBindVertexArray(0); // Unbined VAO

        vCount = 0;
    }

    void Destroy()
    {
        if (vaoID)
        {
            glDeleteVertexArrays(1, &vaoID);
            glDeleteBuffers(2, vboIDs);
            vaoID = 0;
        }

        if (programID)
        {
            glDeleteProgram(programID);
            programID = 0;
        }
    }
    
    // Add vertices to be drawn
    void Vertex(const b2Vec2& v, const b2Color& c)
    {
        //Flush buffer when it's full
        if (vCount == vNumMax)
            Flush();

        vboV[vCount] = v;
        vboC[vCount] = c;
        ++vCount;
    }

    void Flush() // Rendering
    {
        if (vCount == 0)
            return;
        
        glUseProgram(programID);
        
        float32 proj[16] = { 0.0f };
        cam.BuildProjectionMatrix(proj);
        
        glUniformMatrix4fv(uniProjMat, 1, GL_FALSE, proj);
        
        glBindVertexArray(vaoID);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(b2Vec2), vboV);
        
        glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(b2Color), vboC);
        
        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, vCount);
        glDisable(GL_BLEND);
        
//        sCheckGLError();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
        vCount = 0;
    }
    
    static const int vNumMax = 512 * 3;
    b2Vec2 vboV[vNumMax];
    b2Color vboC[vNumMax];

    int32 vCount;

    GLuint vaoID;
    GLuint vboIDs[2];
    GLuint programID;
    GLint uniProjMat;
    GLint vAttrib;
    GLint cAttrib;
};


class DebugDraw : public b2Draw
{
public:
    DebugDraw()
    {
        pointRender = NULL;
        lineRender = NULL;
        triangleRender = NULL;
    }

    ~DebugDraw()
    {
        b2Assert(pointRender == NULL);
        b2Assert(lineRender == NULL);
        b2Assert(triangleRender == NULL);
    }

    void Create()
    {
        pointRender = new GLRenderPoints;
        pointRender->Create();
        lineRender = new GLRenderLines;
        lineRender->Create();
        triangleRender = new GLRenderTriangles;
        triangleRender->Create();
    }
    
    void Destroy()
    {
        pointRender->Destroy();
        delete pointRender;
        pointRender = NULL;

        lineRender->Destroy();
        delete lineRender;
        lineRender = NULL;

        triangleRender->Destroy();
        delete triangleRender;
        triangleRender = NULL;
    }
    
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
    {
        b2Vec2 p1 = vertices[vertexCount - 1];
        for (int32 i = 0; i < vertexCount; ++i)
        {
            b2Vec2 p2 = vertices[i];
            lineRender->Vertex(p1, color);
            lineRender->Vertex(p2, color);
            p1 = p2;
        }
    }
    
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
    {
        b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

        for (int32 i = 1; i < vertexCount - 1; ++i)
        {
            triangleRender->Vertex(vertices[0], fillColor);
            triangleRender->Vertex(vertices[i], fillColor);
            triangleRender->Vertex(vertices[i+1], fillColor);
        }

        b2Vec2 p1 = vertices[vertexCount - 1];
        for (int32 i = 0; i < vertexCount; ++i)
        {
            b2Vec2 p2 = vertices[i];
            lineRender->Vertex(p1, color);
            lineRender->Vertex(p2, color);
            p1 = p2;
        }
    }

    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override
    {
        const float32 k_segments = 16.0f;
        const float32 k_increment = 2.0f * b2_pi / k_segments;
        float32 sinInc = sinf(k_increment);
        float32 cosInc = cosf(k_increment);
        b2Vec2 r1(1.0f, 0.0f);
        b2Vec2 v1 = center + radius * r1;
        for (int32 i = 0; i < k_segments; ++i)
        {
            // Perform rotation to avoid additional trigonometry.
            b2Vec2 r2;
            r2.x = cosInc * r1.x - sinInc * r1.y;
            r2.y = sinInc * r1.x + cosInc * r1.y;
            b2Vec2 v2 = center + radius * r2;
            lineRender->Vertex(v1, color);
            lineRender->Vertex(v2, color);
            r1 = r2;
            v1 = v2;
        }
    }
    
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override
    {
        const float32 k_segments = 16.0f;
        const float32 k_increment = 2.0f * b2_pi / k_segments;
        float32 sinInc = sinf(k_increment);
        float32 cosInc = cosf(k_increment);
        b2Vec2 v0 = center;
        b2Vec2 r1(cosInc, sinInc);
        b2Vec2 v1 = center + radius * r1;
        b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
        for (int32 i = 0; i < k_segments; ++i)
        {
            // Perform rotation to avoid additional trigonometry.
            b2Vec2 r2;
            r2.x = cosInc * r1.x - sinInc * r1.y;
            r2.y = sinInc * r1.x + cosInc * r1.y;
            b2Vec2 v2 = center + radius * r2;
            triangleRender->Vertex(v0, fillColor);
            triangleRender->Vertex(v1, fillColor);
            triangleRender->Vertex(v2, fillColor);
            r1 = r2;
            v1 = v2;
        }

        r1.Set(1.0f, 0.0f);
        v1 = center + radius * r1;
        for (int32 i = 0; i < k_segments; ++i)
        {
            b2Vec2 r2;
            r2.x = cosInc * r1.x - sinInc * r1.y;
            r2.y = sinInc * r1.x + cosInc * r1.y;
            b2Vec2 v2 = center + radius * r2;
            lineRender->Vertex(v1, color);
            lineRender->Vertex(v2, color);
            r1 = r2;
            v1 = v2;
        }

        // Draw a line fixed in the circle to animate rotation.
        b2Vec2 p = center + radius * axis;
        lineRender->Vertex(center, color);
        lineRender->Vertex(p, color);
    }
    
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override
    {
        lineRender->Vertex(p1, color);
        lineRender->Vertex(p2, color);
    }

    void DrawTransform(const b2Transform& xf) override
    {
        const float32 k_axisScale = 0.4f;
        b2Color red(1.0f, 0.0f, 0.0f);
        b2Color green(0.0f, 1.0f, 0.0f);
        b2Vec2 p1 = xf.p, p2;

        lineRender->Vertex(p1, red);
        p2 = p1 + k_axisScale * xf.q.GetXAxis();
        lineRender->Vertex(p2, red);

        lineRender->Vertex(p1, green);
        p2 = p1 + k_axisScale * xf.q.GetYAxis();
        lineRender->Vertex(p2, green);
    }
    
    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override
    {
        pointRender->Vertex(p, color, size);
    }

    void Flush()
    {
        triangleRender->Flush();
        lineRender->Flush();
        pointRender->Flush();
    }


private:
    GLRenderPoints* pointRender;
    GLRenderLines* lineRender;
    GLRenderTriangles* triangleRender;
};

DebugDraw draw;

#endif /* Display_h */
