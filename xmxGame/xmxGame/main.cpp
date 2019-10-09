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

//#define WINDOW_WIDTH 800
//#define WINDOW_HEIGHT 600

/** Camera setting **/
glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

/** Callback functions **/
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

/** Box2D world **/
b2Vec2 gravity(0.0f, -10.0f);
b2World world(gravity);
void genesis();

/** Simulation settings **/
int stepCount = 0;
float32 timeStep = 1.0f / 60.0f;
int32 velocityIterations = 6;
int32 positionIterations = 2;

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
    
    
    b2Vec2 myPoint(0.0f, 0.0f);
    b2Color myColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    b2Body *boxPtr = world.GetBodyList();
    b2Body *groundPtr = boxPtr->GetNext();
    
    b2Fixture *groundFixPtr = groundPtr->GetFixtureList();
    b2Fixture *boxFixPtr = boxPtr->GetFixtureList();
    b2PolygonShape *groundShape = (b2PolygonShape*)groundFixPtr->GetShape();
    b2PolygonShape *boxShape = (b2PolygonShape*)boxFixPtr->GetShape();
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
        
        /** Prepare buffer **/
        
        draw.DrawPolygon(&(groundShape->GetVertex(0)), groundShape->GetVertexCount(), myColor);
        draw.DrawPolygon(&(boxShape->GetVertex(0)), boxShape->GetVertexCount(), myColor);
        
        
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
    
    /** MixRation : [UP] [DOWN] **/
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        ;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        ;
    }
    
    /** Camera : [W] [S] [A] [D] **/
    float camSpeed = 0.005f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camPosition += camSpeed * camFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camPosition -= camSpeed * camFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camPosition -= glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camPosition += glm::normalize(glm::cross(camFront, camUp)) * camSpeed;
    }
}



void genesis()
{
    /** Ground **/
    // 1. Define body
    b2BodyDef groundDef;
    groundDef.position.Set(0.0f, -10.0f);
    // 2. Create body by def
    b2Body* ground = world.CreateBody(&groundDef);
    // 3. Set shape
    b2PolygonShape groundShape;
    groundShape.SetAsBox(20.0f, 2.0f); // The extents are the half-widths of the box.
    // 4. Add fixture
    ground->CreateFixture(&groundShape, 0.0f); // Add the ground fixture to the ground body.

    /** Box **/
    // 1. Define body
    b2BodyDef boxDef;
    boxDef.type = b2_dynamicBody;
    boxDef.position.Set(0.0f, 40.0f);
    // 2. Create body by def
    b2Body* box = world.CreateBody(&boxDef);
    // 3. Set shape
    b2PolygonShape boxShape;
    boxShape.SetAsBox(3.0f, 3.0f);
    // 4.1. Define fixture
    b2FixtureDef boxFxtDef;
    boxFxtDef.shape = &boxShape;
    boxFxtDef.density = 1.0f; // Set the box density to be non-zero, so it will be dynamic.
    boxFxtDef.friction = 0.9f; // Override the default friction.
    // 4.2. Add fixture
    box->CreateFixture(&boxFxtDef);
}
