#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <shader_s.h>
#include "model.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem.h>
#include <camera.h>
#include <vector>
#include <string>
#include <iostream>
#include "vertexBuffer.hpp"
#include "indexBuffer.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(-10.0f, 10.0f, -10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader(FileSystem::getPath("shaders/main.vs"), FileSystem::getPath("shaders/main.fs"));
    Model chairtable(FileSystem::getPath("assets/Chair_table/Table_Chair_Nadika_map.obj"));

    Model teachertable(FileSystem::getPath("assets/Teachers_table/Table_Chair_Nadika.obj"));

    float chair_x_offset = 10.0f;
    float chair_z_offset = 5.0f;

    Shader planeShader(FileSystem::getPath("shaders/plane.vs"), FileSystem::getPath("shaders/plane.fs"));
    float floorvertexCoordinate[] = {
        0.0f, -6.0f, 0.0f,
        0.0f, -6.0f, 10.0f,
        10.0f, -6.0f, 0.0f,
        10.0f, -6.0f, 10.0f
    };

    unsigned int floorindex[] = {
        0,1,2,1,2,3
    };

    unsigned int VBO[4], VAO[4], EBO[4];
    {
    glGenVertexArrays(1, &VAO[0]);
    glGenBuffers(1, &VBO[0]);
    glGenBuffers(1, &EBO[0]);
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorvertexCoordinate), floorvertexCoordinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorindex), floorindex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    }
    float eastwallvertexCoordinate[] = {
        40.0f, 0.0f, 0.0f,
        40.0f, 0.0f, 10.0f,
        40.0f, 10.0f, 0.0f,
        40.0f, 10.0f, 10.0f
    };

    unsigned int eastwallindex[] = {
        0,1,2,1,2,3
    };
    {
    glGenVertexArrays(1, &VAO[1]);
    glGenBuffers(1, &VBO[1]);
    glGenBuffers(1, &EBO[1]);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(eastwallvertexCoordinate), eastwallvertexCoordinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eastwallindex), eastwallindex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    }

    float westwallvertexCoordinate[] = {
        0.0f, 0.0f, 40.0f,
        0.0f, 10.0f, 40.0f,
        10.0f, 0.0f, 40.0f,
        10.0f, 10.0f, 40.0f
    };

    unsigned int westwallindex[] = {
        0,1,2,1,2,3
    };
    {
    glGenVertexArrays(1, &VAO[2]);
    glGenBuffers(1, &VBO[2]);
    glGenBuffers(1, &EBO[2]);
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(westwallvertexCoordinate), westwallvertexCoordinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(westwallindex), westwallindex, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    }
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        planeShader.use();
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", view);

        for ( int k = -9 ; k < 9 ; ++k){
            for(int j = -9 ; j < 9 ; ++j){
                model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f * j, 0.0f, 5.0f * k));
                planeShader.setMat4("model", model);
                glBindVertexArray(VAO[0]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                if(k == 0)
                if(j == 1 || j == 0 || j == -1)
                continue;
                model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f*j, 5.0f * k));
                planeShader.setMat4("model", model);
                glBindVertexArray(VAO[1]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                // model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f * j, 0.0f, 5.0f * k));
                // planeShader.setMat4("model", model);
                // glBindVertexArray(VAO[2]);
                // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
        

    

        lightingShader.use();

        // view/projection transformations

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // // world transformation
        // glm::mat4 model = glm::mat4(1.0f);
        // lightingShader.setMat4("model", model);


        // chairtable.Draw(lightingShader);

        for ( int k = -3 ; k < 3 ; ++k){
            for(int j = -1 ; j < 2 ; ++j){
                model = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f * j + chair_x_offset, 0.0f, 10.0f * k + chair_z_offset));
                lightingShader.setMat4("model", model);
                        chairtable.Draw(lightingShader);
            }
        }
        model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 26.0f));
        lightingShader.setMat4("model", model);

        teachertable.Draw(lightingShader);
      


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

