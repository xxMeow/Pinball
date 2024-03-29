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
float worldWidthHalf = 130.0f;
float worldHeightHalf = 20.0f;
float thicknessHalf = 0.3f;
b2Vec2 gravity(0.0f, -1.0f);
b2World world(gravity);
b2Body* ball;
b2Vec2 posToDown(b2Vec2 offset, b2Vec2 size);
b2Vec2 posToUp(b2Vec2 offset, b2Vec2 size);
void genesis();

/** Simulation settings **/
unsigned int stepCount = 0;
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
    
    cam.center.Set(ball->GetWorldCenter().x, 0);
    /** Prevent the bug that the ball moves too fast while going through a narrow way **/
    
    
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
        
        b2Vec2 ballPos = ball->GetWorldCenter();
        printf(" %d : %.2f %.2f\n", stepCount, ballPos.x, ballPos.y);
        
        // Reset balls velocity
        b2ContactEdge* c = ball->GetContactList();
        if (c != NULL) { // Collision
            printf("ContactAt: %d (%.2f, %.2f)\n", stepCount, ball->GetLinearVelocity().x, ball->GetLinearVelocity().y);
            if (c->other->GetFixtureList()->GetRestitution() != 0.0f && ballPos.y > c->other->GetWorldCenter().y) {
                ball->SetLinearVelocity(b2Vec2(ball->GetLinearVelocity().x/2, 4.0f));
            }
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
        b2Vec2 pos = posToDown(b2Vec2(10.0f, 4.0f), b2Vec2(0.5f, 0.5f)); // Origin
//        b2Vec2 pos = posToDown(b2Vec2(31.0f, 34.0f), b2Vec2(0.5f, 0.5f)); // Channel
//        b2Vec2 pos = posToDown(b2Vec2(91.0f, 34.0f), b2Vec2(0.5f, 0.5f)); // Poor left
//        b2Vec2 pos = posToDown(b2Vec2(121.0f, 34.0f), b2Vec2(0.5f, 0.5f)); // Poor
//        b2Vec2 pos = posToDown(b2Vec2(171.0f, 34.0f), b2Vec2(0.5f, 0.5f)); // Poor Right
//        b2Vec2 pos = posToDown(b2Vec2(181.0f, 5.0f), b2Vec2(0.5f, 0.5f)); // Poor Right Bottom
//        b2Vec2 pos = posToDown(b2Vec2(202.0f, 30.0f), b2Vec2(0.5f, 0.5f)); // 2nd Poor
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
        // Down !! The only border with non-zero restitution
        b2BodyDef dBorderDef;
        dBorderDef.position.Set(0.0f, -(worldHeightHalf-thicknessHalf));
        b2Body* dBorder = world.CreateBody(&dBorderDef);
        b2FixtureDef dBorderFixDef;
        dBorderFixDef.shape = &longBorderShape;
        dBorderFixDef.restitution = 1.0f;
        dBorder->CreateFixture(&dBorderFixDef);
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
    /** Area.1 **/ // Stairs
    {
        int n = 4;
        int disDiff = 7.0f;
        int heightDiff = 2.5f;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2FixtureDef fixs[n];
        b2Body* boxes[n];
        b2Vec2 sizeHalf(2.0f, 3.0f);
        float posX = 20.0f;
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(b2Vec2(posX, 0.0f), sizeHalf);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizeHalf.x, sizeHalf.y);
            fixs[i].shape = &shapes[i];
            fixs[i].restitution = 1.0f;
            boxes[i]->CreateFixture(&fixs[i]);
            
            posX += disDiff;
            sizeHalf.y += heightDiff;
        }
    }
    /** Area.2 **/
    {
        int n = 3;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2FixtureDef fixs[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(0.0f, 26.0f), b2Vec2(7.0f, 20.0f), b2Vec2(22.0f, 22.0f)};
        b2Vec2 sizes[] = {b2Vec2(3.0f, thicknessHalf*2), b2Vec2(6.0f, thicknessHalf*2), b2Vec2(7.0f, thicknessHalf*2)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            fixs[i].shape = &shapes[i];
            fixs[i].restitution = 1.0f;
            boxes[i]->CreateFixture(&fixs[i]);
        }
    }
    
    /** Area.3-1 **/ // Narrow channel
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
    
    /** Area.3-2 **/ // Narrow channel
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
    
    /** Area.4 **/ // Little boxes
    {
        int n = 10;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {
            b2Vec2(83.0f, 0.0f), b2Vec2(84.0f, 0.0f), b2Vec2(85.0f, 0.0f), b2Vec2(86.0f, 0.0f),
            b2Vec2(83.5f, 1.0f), b2Vec2(84.5f, 1.0f), b2Vec2(85.5f, 1.0f),
            b2Vec2(84.0f, 2.0f), b2Vec2(85.0f, 2.0f),
            b2Vec2(84.5f, 3.0f)
        };
        b2Vec2 size(0.5f, 0.5f);
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], size);
            defs[i].type = b2_dynamicBody; // TODO: ? neccessery?
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(size.x, size.y);
            boxes[i]->CreateFixture(&shapes[i], 0.1f);
        }
    }
    
    /** Area.5 **/ // Boxes at left side of poor
    {
        int n = 4;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2FixtureDef fixs[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(85.0f, 13.8f), b2Vec2(85.0f, 12.6f), b2Vec2(108.0f, 5.0f), b2Vec2(96.0f, 20.0f)};
        b2Vec2 sizes[] = {b2Vec2(thicknessHalf*2, 12.5f), b2Vec2(10.0f, thicknessHalf*2), b2Vec2(4.0f, thicknessHalf*2), b2Vec2(10.0f, thicknessHalf*2)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            fixs[i].shape = &shapes[i];
            fixs[i].restitution = 1.0f;
            boxes[i]->CreateFixture(&fixs[i]);
        }
    }
    
    /** Area.6 **/ // Poor
    {
        int n = 3;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(116.0f, 0.0f), b2Vec2(117.2f, 0.0f), b2Vec2(147.2f, 0.0f)};
        b2Vec2 sizes[] = {b2Vec2(thicknessHalf*2, 12.5f), b2Vec2(15.0f, thicknessHalf*10), b2Vec2(thicknessHalf*10, 10.0f)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            boxes[i]->CreateFixture(&shapes[i], 0.0f);
        }
    }
    
    /** Area.7 **/ // Boxes in poor
    {
        int n = 2;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(127.0f, 25.0f), b2Vec2(138.0f, 25.0f)};
        b2Vec2 sizes[] = {b2Vec2(4.0f, 4.0f), b2Vec2(3.0f, 3.0f)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].type = b2_dynamicBody;
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            boxes[i]->CreateFixture(&shapes[i], 0.5f);
        }
    }
    
    /** Area.8-1 **/ // Upper boxes at right side of poor
    {
        int n = 7;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(160.0f, 21.8f), b2Vec2(160.0f, 14.8f), b2Vec2(168.0f, 29.0f), b2Vec2(190.0f, 14.8f), b2Vec2(169.0f, 16.0f), b2Vec2(180.0f, 24.0f), b2Vec2(191.2f, 10.0f)};
        b2Vec2 sizes[] = {b2Vec2(thicknessHalf*2, 8.5f), b2Vec2(15.0f, thicknessHalf*2), b2Vec2(3.0f, thicknessHalf*2), b2Vec2(thicknessHalf*2, 12.0f), b2Vec2(6.0f, thicknessHalf*10), b2Vec2(thicknessHalf*8, 5.0f), b2Vec2(thicknessHalf*10, 8.0f)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            boxes[i]->CreateFixture(&shapes[i], 0.0f);
        }
    }
    
    /** Area.8-2 **/ // Lower boxes at right side of poor
    {
        int n = 4;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2FixtureDef fixs[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(160.0f, 6.8f), b2Vec2(167.0f, 0.0f), b2Vec2(174.0f, 6.8f), b2Vec2(181.0f, 0.0f)};
        b2Vec2 size(thicknessHalf*2, 4.0f);
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], size);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(size.x, size.y);
            fixs[i].shape = &shapes[i];
            fixs[i].restitution = 1.0f;
            boxes[i]->CreateFixture(&fixs[i]);
        }
    }
    
    /** Area.9 **/ // 2nd Poor
    {
        int n = 2;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(202.0f, 0.0f), b2Vec2(252.8f, 0.0f)};
        b2Vec2 sizes[] = {b2Vec2(thicknessHalf*10, worldHeightHalf*0.85), b2Vec2(thicknessHalf*10, worldHeightHalf/2)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            boxes[i]->CreateFixture(&shapes[i], 0.0f);
        }
    }
    
    /** Pulley **/
    {
        b2Vec2 boxOffset1(197.7f, 0.0f);
        b2Vec2 boxOffset2(209.0f, 27.0f);
        b2Vec2 boxSize1(2.0f, thicknessHalf);
        b2Vec2 boxSize2(2.0f, 2.0f);
        b2Vec2 boxPos1 = posToDown(boxOffset1, boxSize1);
        b2Vec2 boxPos2 = posToDown(boxOffset2, boxSize2);
        b2BodyDef boxDef1, boxDef2;
        boxDef1.type = b2_dynamicBody;
        boxDef1.fixedRotation = true;
        boxDef2.type = b2_dynamicBody;
        boxDef2.fixedRotation = true;
        boxDef1.position.Set(boxPos1.x, boxPos1.y);
        boxDef2.position.Set(boxPos2.x, boxPos2.y);
        b2PolygonShape boxShape1, boxShape2;
        boxShape1.SetAsBox(boxSize1.x, boxSize1.y);
        boxShape2.SetAsBox(boxSize2.x, boxSize2.y);
        b2Body* box1;
        b2FixtureDef boxFixDef1;
        boxFixDef1.shape = &boxShape1;
        boxFixDef1.density = 1.0f;
        boxFixDef1.restitution = 1.0f;
        box1 = world.CreateBody(&boxDef1);
        box1->CreateFixture(&boxFixDef1);
        b2Body* box2;
        box2 = world.CreateBody(&boxDef2);
        box2->CreateFixture(&boxShape2, 2.0f);
        
        b2Vec2 ancPos1(boxPos1.x, worldHeightHalf-2.0f);
        b2Vec2 ancPos2(boxPos2.x, worldHeightHalf-2.0f);
        float32 ratio = 1.0f;
        b2PulleyJointDef jointDef;
        jointDef.Initialize(box1, box2, ancPos1, ancPos2, boxPos1, boxPos2, ratio);
        b2PulleyJoint* joint;
        joint = (b2PulleyJoint*)world.CreateJoint(&jointDef);
        
        /** Boxes on left **/
        {
            b2BodyDef def;
            b2PolygonShape shape;
            b2Body* box;
            b2Vec2 offset(197.5f, 7.0f);
            b2Vec2 size(0.2f, 2.0f);
            
            b2Vec2 pos = posToDown(offset, size);
            def.type = b2_dynamicBody;
            def.position.Set(pos.x, pos.y);
            box = world.CreateBody(&def);
            shape.SetAsBox(size.x, size.y);
            box->CreateFixture(&shape, 20.0f);
        }
    }

    /** Area.10 **/ // Boxes upon the 2nd poor
    {
        int n = 2;
        b2BodyDef defs[n];
        b2PolygonShape shapes[n];
        b2FixtureDef fixs[n];
        b2Body* boxes[n];
        b2Vec2 offsets[] = {b2Vec2(215.0f, worldHeightHalf+2.0f), b2Vec2(235.0f, worldHeightHalf+4.0f)};
        b2Vec2 sizes[] = {b2Vec2(6.0f, thicknessHalf*2), b2Vec2(4.0f, thicknessHalf*2)};
        for (int i = 0; i < n; i ++) {
            b2Vec2 pos = posToDown(offsets[i], sizes[i]);
            defs[i].position.Set(pos.x, pos.y);
            boxes[i] = world.CreateBody(&defs[i]);
            shapes[i].SetAsBox(sizes[i].x, sizes[i].y);
            fixs[i].shape = &shapes[i];
            fixs[i].restitution = 1.0f;
            boxes[i]->CreateFixture(&fixs[i]);
        }
    }
}
