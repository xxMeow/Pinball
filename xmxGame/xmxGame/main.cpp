//
//  main.cpp
//  xmxGame
//
//  Created by XMX on 2019/09/9.
//  Copyright © 2019 XMX. All rights reserved.
//

#include "Box2D/Box2D.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
//#include <cmath>

#include "../Headers/Program.h"
#include "../Headers/Display.h"
//#include "../Headers/stb_image.h"

/** Callback functions **/
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

/** Box2D world **/
float worldWidthHalf = 100.0f;
float worldHeightHalf = 20.0f;
float thicknessHalf = 0.3f;
b2Vec2 gravity(0.0f, -1.0f);
b2World world(gravity);
b2Body* ball;
b2Vec2 posToDown(b2Vec2 offset, b2Vec2 size);
b2Vec2 posToUp(b2Vec2 offset, b2Vec2 size);
void genesis();

/** Simulation settings **/
int stepCount = 0;
float32 timeStep = 1.0f / 60.0f; //TODO: Return to 60Hz
int32 velocityIterations = 6;
int32 positionIterations = 2;
uint32 flags = 0;
bool flagDrawShapes = true;
bool flagDrawJoints = true;


/** Render functions **/
GLFWwindow *window;


int main(int argc, const char *argv[])
{
    /** Setup world **/
    genesis();
    
    /** Prepare for rendering **/
    // Initialize GLFW
    glfwInit();
    // Set OpenGL version number as 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Use the core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // MacOS is forward compatible
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    /** Create a GLFW window **/
    window = glfwCreateWindow(cam.width, cam.height, "xmxOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    // Set the context of this window as the main context of current thread
    glfwMakeContextCurrent(window);
    
    // Initialize GLAD : this should be done before using any openGL function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD." << std::endl;
        glfwTerminate(); // This line isn't in the official source code, but I think that it should be added here.
        return -1;
    }
    
    
    /** Callback Functions **/
    // Callback functions should be registered after creating window and before initializing render loop
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    draw.Create();
    
    /** ---------------------------------- Simulation & Rendering ---------------------------------- **/
    
    // Uncomment this call to draw in wireframe polygons
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Option: GL_FILL(default), GL_POINT, GL_LINE
    
    flags += flagDrawShapes * b2Draw::e_shapeBit;
    flags += flagDrawJoints * b2Draw::e_jointBit;
    draw.SetFlags(flags);
    world.SetAllowSleeping(true);
    world.SetWarmStarting(true);
    world.SetContinuousPhysics(true);
    world.SetSubStepping(true);
    
    b2Vec2 myPoint(0.0f, 0.0f);
    b2Color myColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    /** For logging **/
    b2Body *boxPtr = world.GetBodyList();\
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        /** Check for events **/
        processInput(window);
        
        /** Set background clolor **/
        glClearColor(0.2f, 0.2f, 0.5f, 1.0f); // Set color value (R,G,B,A) - Set Status
        glClear(GL_COLOR_BUFFER_BIT); // Use the color to clear screen - Use Status
        
        /** Simulation **/
        world.Step(timeStep, velocityIterations, positionIterations);
        stepCount ++;
        printf(" >> %d >> %4.2f %4.2f\n", stepCount, boxPtr->GetPosition().x, boxPtr->GetPosition().y);
        
        // Reset balls velocity
        b2ContactEdge* c = ball->GetContactList();
        if (c != NULL && ball->GetWorldCenter().y > c->other->GetWorldCenter().y) {
            ball->SetLinearVelocity(b2Vec2(0.0f, 4.0f));
        }
        
        /** Move camera **/
        cam.center.Set(ball->GetWorldCenter().x, 0);
        
        
        /** Prepare buffer **/
        world.SetDebugDraw(&draw);
        world.DrawDebugData();
        
        
        /** Display **/
        draw.Flush();
        glfwSwapBuffers(window);
        glfwPollEvents(); // Update the status of window
    }
    






    /** ---------------------------------- Simulation & Rendering ---------------------------------- **/
    
    draw.Destroy();
    glfwTerminate();
    
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // Reset the size of glViewport
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    /** Window : [ESC] **/
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { // If key did not get pressed it will return GLFW_RELEASE
        glfwSetWindowShouldClose(window, true);
    }
    
    /** Control **/
    float forceX = 0.3f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        ball->ApplyForce(b2Vec2(-forceX, 0.0f), ball->GetWorldCenter(), true);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        ball->ApplyForce(b2Vec2(forceX, 0.0f), ball->GetWorldCenter(), true);
    }
    /*
    float forceY = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        ball->ApplyForce(b2Vec2(0.0f, forceY), ball->GetWorldCenter(), true);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        ball->ApplyForce(b2Vec2(0.0f, -forceY), ball->GetWorldCenter(), true);
    }
    b2Vec2 v(0.0f, 0.5f);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        ball->SetAwake(true);
        ball->SetLinearVelocity(v);
    }
     */
}

b2Vec2 posToDown(b2Vec2 offset, b2Vec2 sizeHalf)
{
    b2Vec2 pos(0.0f, 0.0f);
    pos.x = -(worldWidthHalf - 2 * thicknessHalf) + offset.x + sizeHalf.x;
    pos.y = -(worldHeightHalf - 2 * thicknessHalf) + offset.y + sizeHalf.y;
    return pos;
}
b2Vec2 posToUp(b2Vec2 offset, b2Vec2 sizeHalf)
{
    b2Vec2 pos(0.0f, 0.0f);
    pos.x = -(worldWidthHalf - 2 * thicknessHalf) + offset.x + sizeHalf.x;
    pos.y = worldHeightHalf - 2 * thicknessHalf - offset.y - sizeHalf.y;
    return pos;
}
/*
b2Vec2 posToLeft(b2Vec2 offset, b2Vec2 sizeHalf)
{
    b2Vec2 pos(0.0f, 0.0f);
    pos.x = -(worldWidthHalf - 2 * thicknessHalf) + offset.x + sizeHalf.x;
    pos.y = -(worldHeightHalf - 2 * thicknessHalf) + offset.y + sizeHalf.y;
    return pos;
}
b2Vec2 posToRight(b2Vec2 offset, b2Vec2 sizeHalf)
{
    b2Vec2 pos(0.0f, 0.0f);
    pos.x = -(worldWidthHalf - 2 * thicknessHalf) + offset.x + sizeHalf.x;
    pos.y = -(worldHeightHalf - 2 * thicknessHalf) + offset.y + sizeHalf.y;
    return pos;
}
*/

void genesis()
{
    /** Ball **/
    {
        b2BodyDef def;
        def.type = b2_dynamicBody;
//        b2Vec2 pos = posToDown(b2Vec2(10.0f, 4.0f), b2Vec2(0.5f, 0.5f));
        b2Vec2 pos = posToDown(b2Vec2(20.0f, 14.0f), b2Vec2(0.5f, 0.5f));
        def.position.Set(pos.x, pos.y);
        // 2. Create body by def
        ball = world.CreateBody(&def);
        // 3. Set shape
        b2CircleShape shape;
        shape.m_p.Set(2.0f, 2.0f);
        shape.m_radius = 0.5f;
        // 4.1. Define fixture
        b2FixtureDef fixDef;
        fixDef.shape = &shape;
        fixDef.density = 1.0f; // Set the box density to be non-zero, so it will be dynamic.
        fixDef.restitution = 1.0f;
        // 4.2. Add fixture
        ball->CreateFixture(&fixDef);
    }
    
    /** Borders :  x **/
    {
        // Up & Down
        b2PolygonShape longBorderShape;
        longBorderShape.SetAsBox(worldWidthHalf, thicknessHalf);
        // Up
        b2BodyDef uBorderDef;
        uBorderDef.position.Set(0.0f, worldHeightHalf-thicknessHalf);
        b2Body* uBorder = world.CreateBody(&uBorderDef);
        uBorder->CreateFixture(&longBorderShape, 0.0f);
        // Down
        b2BodyDef dBorderDef;
        dBorderDef.position.Set(0.0f, -(worldHeightHalf-thicknessHalf));
        b2Body* dBorder = world.CreateBody(&dBorderDef);
        dBorder->CreateFixture(&longBorderShape, 0.0f);
        // Left & Right
        b2PolygonShape shortBorderShape;
        shortBorderShape.SetAsBox(thicknessHalf, worldHeightHalf);
        // Left
        b2BodyDef lBorderDef;
        lBorderDef.position.Set(-(worldWidthHalf-thicknessHalf), 0.0f);
        b2Body* lBorder = world.CreateBody(&lBorderDef);
        lBorder->CreateFixture(&shortBorderShape, 0.0f);
        // Right
        b2BodyDef rBorderDef;
        rBorderDef.position.Set(worldWidthHalf-thicknessHalf, 0.0f);
        b2Body* rBorder = world.CreateBody(&rBorderDef);
        rBorder->CreateFixture(&shortBorderShape, 0.0f);
    }

    /** ------------------------------------------------------------------------------------------- **/
    /** Area.1 **/
    {
        int n = 4;
        int disDiff = 7.0f;
        int heightDiff = 2.5f;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2Body* boxes[n];
        b2Vec2 sizeHalf(2.0f, 3.0f);
        float posX = 20.0f;
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(b2Vec2(posX, 0.0f), sizeHalf);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizeHalf.x, sizeHalf.y);
            boxes[i]->CreateFixture(&shapes[i], 0.0f);
            
            posX += disDiff;
            sizeHalf.y += heightDiff;
        }
        
    }
    /** Area.2 **/
    {
        int n = 3;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(0.0f, 26.0f), b2Vec2(7.0f, 20.0f), b2Vec2(22.0f, 22.0f)};
        b2Vec2 sizes[] = {b2Vec2(3.0f, thicknessHalf*2), b2Vec2(6.0f, thicknessHalf*2), b2Vec2(7.0f, thicknessHalf*2)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            boxes[i]->CreateFixture(&shapes[i], 0.0f);
        }
    }
    
    /** Area.3-1 **/
    {
        b2BodyDef def;
        b2Vec2 offset(7.0f, 33.0f);
        b2Vec2 size(19.5f, thicknessHalf*2);
        b2Vec2 pos = posToDown(offset, size);
        def.position.Set(pos.x, pos.y);
        b2PolygonShape shape;
        shape.SetAsBox(size.x, size.y);
        b2Body* box = world.CreateBody(&def);
        box->CreateFixture(&shape, 0.0f);
    }
    
    /** Area.3-2 **/
    {
        int n = 2;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(40.0f, 16.0f), b2Vec2(40.0f, 21.0f)};
        b2Vec2 sizes[] = {b2Vec2(23.0f, thicknessHalf*2), b2Vec2(24.0f, thicknessHalf*2)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].position.Set(pos.x, pos.y);
            defs[i].angle = -0.25f * b2_pi;
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            boxes[i]->CreateFixture(&shapes[i], 0.0f);
        }
    }
    
}
