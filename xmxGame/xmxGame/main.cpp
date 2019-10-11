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
b2Vec2 gravity(0.0f, -2.5f);
b2World world(gravity);
b2Body* ball;
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
    b2Body *boxPtr = world.GetBodyList();
    b2Body *groundPtr = boxPtr->GetNext();
    
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
        printf(" >> %d ---------------------\n", stepCount);
        printf("B: %4.2f %4.2f\n", boxPtr->GetPosition().x, boxPtr->GetPosition().y);
        printf("G: %4.2f %4.2f\n", groundPtr->GetPosition().x, groundPtr->GetPosition().y);
        
        
        /** Move amera **/
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
    float xForce = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        ball->ApplyForce(b2Vec2(-xForce, 0.0f), ball->GetWorldCenter(), true);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        ball->ApplyForce(b2Vec2(xForce, 0.0f), ball->GetWorldCenter(), true);
    }
}



void genesis()
{
    /** Ball **/
    // 1. Define body
    b2BodyDef ballDef;
    ballDef.type = b2_dynamicBody;
    ballDef.position.Set(-20.0f, 10.0f);
    // 2. Create body by def
    ball = world.CreateBody(&ballDef);
    // 3. Set shape
    b2CircleShape ballShape;
    ballShape.m_p.Set(2.0f, 2.0f);
    ballShape.m_radius = 1.0f;
    // 4.1. Define fixture
    b2FixtureDef ballFixDef;
    ballFixDef.shape = &ballShape;
    ballFixDef.density = 0.5f; // Set the box density to be non-zero, so it will be dynamic.
//    ballFixDef.friction = 0.0f; // Override the default friction.
    ballFixDef.restitution = 1.0f;
    // 4.2. Add fixture
    ball->CreateFixture(&ballFixDef);
    
    /** Borders **/
    // Up & Down
    b2PolygonShape longBorderShape;
    longBorderShape.SetAsBox(50.0f, 0.5f);
    b2FixtureDef longBorderFixDef;
    longBorderFixDef.shape = &longBorderShape;
    longBorderFixDef.friction = 0.2f;
    // Up
    b2BodyDef uBorderDef;
    uBorderDef.position.Set(0.0f, 19.5f);
    b2Body* uBorder = world.CreateBody(&uBorderDef);
    uBorder->CreateFixture(&longBorderFixDef);
    // Down
    b2BodyDef dBorderDef;
    dBorderDef.position.Set(0.0f, -19.5f);
    b2Body* dBorder = world.CreateBody(&dBorderDef);
    dBorder->CreateFixture(&longBorderFixDef);
    // Left & Right
    b2PolygonShape shortBorderShape;
    shortBorderShape.SetAsBox(0.5f, 19.0f);
    b2FixtureDef shortBorderFixDef;
    shortBorderFixDef.shape = &shortBorderShape;
    shortBorderFixDef.friction = 0.2f;
    // Left
    b2BodyDef lBorderDef;
    lBorderDef.position.Set(-49.5f, 0.0f);
    b2Body* lBorder = world.CreateBody(&lBorderDef);
    lBorder->CreateFixture(&shortBorderFixDef);
    // Right
    b2BodyDef rBorderDef;
    rBorderDef.position.Set(49.5f, 0.0f);
    b2Body* rBorder = world.CreateBody(&rBorderDef);
    rBorder->CreateFixture(&shortBorderFixDef);
    

    /** Box **/
    // 1. Define body
    b2BodyDef boxDef;
    boxDef.type = b2_dynamicBody;
    boxDef.position.Set(-30.0f, 11.0f);
    // 2. Create body by def
    b2Body* box = world.CreateBody(&boxDef);
    // 3. Set shape
    b2PolygonShape boxShape;
    boxShape.SetAsBox(3.0f, 3.0f);
    // 4.1. Define fixture
    b2FixtureDef boxFixDef;
    boxFixDef.shape = &boxShape;
    boxFixDef.density = 1.0f; // Set the box density to be non-zero, so it will be dynamic.
    boxFixDef.friction = 0.1f; // Override the default friction.
    // 4.2. Add fixture
    box->CreateFixture(&boxFixDef);
    
}
