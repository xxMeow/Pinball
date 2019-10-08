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

#include "../Headers/Shader.h"
//#include "../Headers/stb_image.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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
void worldRun();

/** Render functions **/
GLFWwindow *window;
int beforeRender();
int inRender(Shader shader);
int afterRender();


/** Setup vertex data **/
// Define vertices
// Each vertex includes 3 coordinates (x, y, z:depth), the middle point of space is (0.0, 0.0, 0.0)
float vertices[] = {
    // Position          // Color
     0.5f, -0.5f, 0.0f,  0.8f, 0.0f, 0.0f, // Right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.8f, 0.0f, // Left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 0.8f  // Top
};

int main(int argc, const char *argv[])
{
    /** World **/
    worldRun();
    
    
    /** Render **/
    beforeRender();
    Shader shader("../Shaders/VertexShader.glsl", "../Shaders/FragmentShader.glsl");
    inRender(shader);
    afterRender();
    
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
    
    /** MixRation : [UP] [Down] **/
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

void worldRun() {
    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    b2Body* groundBody = world.CreateBody(&groundBodyDef);

    // Define the ground box shape.
    b2PolygonShape groundBox;

    // The extents are the half-widths of the box.
    groundBox.SetAsBox(50.0f, 10.0f);

    // Add the ground fixture to the ground body.
    groundBody->CreateFixture(&groundBox, 0.0f);

    // Define the dynamic body. We set its position and call the body factory.
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 4.0f);
    b2Body* body = world.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 1.0f;

    // Override the default friction.
    fixtureDef.friction = 0.3f;

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);

    // Prepare for simulation. Typically we use a time step of 1/60 of a
    // second (60Hz) and 10 iterations. This provides a high quality simulation
    // in most game scenarios.
    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    // This is our little game loop.
    for (int32 i = 0; i < 60; ++i)
    {
        // Instruct the world to perform a single step of simulation.
        // It is generally best to keep the time step and iterations fixed.
        world.Step(timeStep, velocityIterations, positionIterations);

        // Now print the position and angle of the body.
        b2Vec2 position = body->GetPosition();
        float32 angle = body->GetAngle();

        printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
    }
}

int beforeRender() {
    /** Initialize GLFW **/
    glfwInit();
    
    /** Set basic context **/
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
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "xmxOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    // Set the context of this window as the main context of current thread
    glfwMakeContextCurrent(window);
    
    /** Callback Functions should be registered after creating window and before initializing render loop **/
    // Pass the pointer of framebuffer_size_callback to the GLFW
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Initialize GLAD : this should be done before using any openGL function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD." << std::endl;
        glfwTerminate(); // This line isn't in the official source code, but I think that it should be added here.
        return -1;
    }
    

    return 0;
}

int inRender(Shader shader) {    
    
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // Bind the VAO
    glBindVertexArray(VAO);
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData() will copy datas to the buffer which is being bound right now
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    /** glVertexAttribPointer() will get each vertex attributes from the VBO
     *  void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
     **/
    // Position Pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color Pointer
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    /** Unbind **/
    // Since glAttribArrayPointer() has registered VBO as the vertex attributes' bound vertex buffer object, now we can unbind it safely
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Similar to the VBO, the VAO can be also safely unbound now so that this VAO won't get accidentally modified by other VAO calls
    // However, modifying other VAOs required a call to glBindVertexArray() anyways so it's okay that don't do the unbinding to VAOs (but not VBOs) when it's not directly necessary
    glBindVertexArray(0);
    
    // Uncomment this call to draw in wireframe polygons
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Option: GL_FILL(default), GL_POINT, GL_LINE
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Check for events
        processInput(window);
        
        /** Other rendering operations **/
        
        // Set background clolor
        glClearColor(0.2f, 0.2f, 0.5f, 1.0f); // Set color value (R,G,B,A) - Set Status
        glClear(GL_COLOR_BUFFER_BIT); // Use the color to clear screen - Use Status
        
        /** Render triangle **/
        shader.use();
        
        glBindVertexArray(VAO);
        // In fact, we don't need to bind the VAO every render loop here since we only have a single VAO. We just do it so to keep things a bit organized
        
        // Tell OpenGL to draw 6 vertices whose indices are stored in the VBO
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements() will access the elements in the VAO which is being bound right now
        
        //glBindVertexArray(0); // Again, we don't need to unbind it right now
        
        // Swap color buffers since double buffers are applied for rendering
        /** After updating my MacOS to Mojave10.14.6, this function dose not cause flickering now **/
        glfwSwapBuffers(window);
        // Update the status of window
        glfwPollEvents();
    }
    
    // Optional: De-allocate all resources once they've outlived their purpoose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
    
    return 0;
}

int afterRender() {
    
    glfwTerminate();
    
    return 0;
}
