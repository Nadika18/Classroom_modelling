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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Classroom Modelling", NULL, NULL);
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
        //position              //texture coordinates
        0.0f, -6.0f, 0.0f,     0.0f, 0.0f, //bottom left
        0.0f, -6.0f, 10.0f,    0.0f, 1.0f, // bottom right
        10.0f, -6.0f, 0.0f,    1.0f, 0.0f , // top left 
        10.0f, -6.0f, 10.0f ,   1.0f, 1.0f // top right
    };

    unsigned int floorindex[] = {
        0,1,2,1,2,3
    };

    unsigned int VBO[7], VAO[7], EBO[7];
    
    glGenVertexArrays(1, &VAO[0]);
    glGenBuffers(1, &VBO[0]);
    glGenBuffers(1, &EBO[0]);
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorvertexCoordinate), floorvertexCoordinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorindex), floorindex, GL_STATIC_DRAW);
     // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
      // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    // -------------------------
    unsigned int floortexture;
    glGenTextures(1, &floortexture); 
    //binding texture
    glBindTexture(GL_TEXTURE_2D, floortexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(FileSystem::getPath("assets/floor/floors.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //east wall
    float eastwallvertexCoordinate[] = {
        //position          //texture
        40.0f, 0.0f, 0.0f,  0.0f, 0.0f, //bottom left
        40.0f, 0.0f, 5.0f,  1.0f, 0.0f, // bottom right
        40.0f, 5.0f, 0.0f,   0.0f, 1.0f , // top left
        40.0f, 5.0f, 5.0f,  1.0f, 1.0f // top right
    };

    unsigned int eastwallindex[] = {
        0,1,2,1,2,3
    };
    
    glGenVertexArrays(1, &VAO[1]);
    glGenBuffers(1, &VBO[1]);
    glGenBuffers(1, &EBO[1]);
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(eastwallvertexCoordinate), eastwallvertexCoordinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eastwallindex), eastwallindex, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

      // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    // -------------------------
    unsigned int walltexture;
    glGenTextures(1, &walltexture); 
    //binding texture
    glBindTexture(GL_TEXTURE_2D, walltexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
   
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *walldata = stbi_load(FileSystem::getPath("assets/walls/pingYellow.png").c_str(), &width, &height, &nrChannels, 0);
    if (walldata)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, walldata);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(walldata);

     
    // //west wall
    // float westwallvertexCoordinate[] = {
    //     //position          //texture
    //     -20.0f, 0.0f, 0.0f,  0.0f, 0.0f, //bottom left
    //     -20.0f, 0.0f, 5.0f,  1.0f, 0.0f, // bottom right
    //     -20.0f, 5.0f, 0.0f,   0.0f, 1.0f , // top left
    //    -20.0f, 5.0f, 5.0f,  1.0f, 1.0f // top right
    // };

    // unsigned int westwallindex[] = {
    //     0,1,2,1,2,3
    // };
    
    // glGenVertexArrays(1, &VAO[5]);
    // glGenBuffers(1, &VBO[5]);
    // glGenBuffers(1, &EBO[5]);
    // glBindVertexArray(VAO[5]);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(westwallvertexCoordinate), westwallvertexCoordinate, GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[5]);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(westwallindex), westwallindex, GL_STATIC_DRAW);
    
    // //position attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    
    // //binding texture
    // glBindTexture(GL_TEXTURE_2D, walltexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // // texture coord attribute
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // glBindBuffer(GL_ARRAY_BUFFER, 0); 
    // glBindVertexArray(0); 
    

    //board wall
    float boardwallvertexCoordinate[] = {
        0.0f, 0.0f, 60.0f,   0.0f, 0.0f, //bottom left
        0.0f, 5.0f, 60.0f,   0.0f, 1.0f, // bottom right
        5.0f, 0.0f, 60.0f,     1.0f, 0.0f , // top left
        5.0f, 5.0f, 60.0f,   1.0f, 1.0f // top right
    };

    unsigned int boardwallindex[] = {
        0,1,2,1,2,3
    };
    
    glGenVertexArrays(1, &VAO[2]);
    glGenBuffers(1, &VBO[2]);
    glGenBuffers(1, &EBO[2]);
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardwallvertexCoordinate), boardwallvertexCoordinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boardwallindex), boardwallindex, GL_STATIC_DRAW);
    
    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


     //binding texture
    glBindTexture(GL_TEXTURE_2D, walltexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    
   

    //opposite wall
    float oppositeboardwallvertexCoordinate[] = {
        0.0f, 0.0f, -35.0f,   0.0f, 0.0f, //bottom left
        0.0f, 5.0f, -35.0f,   0.0f, 1.0f, // bottom right
        5.0f, 0.0f, -35.0f,     1.0f, 0.0f , // top left
        5.0f, 5.0f, -35.0f,   1.0f, 1.0f // top right
    };

    unsigned int oppositeboardwallindex[] = {
        0,1,2,1,2,3
    };
    
    glGenVertexArrays(1, &VAO[3]);
    glGenBuffers(1, &VBO[3]);
    glGenBuffers(1, &EBO[3]);
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(oppositeboardwallvertexCoordinate), oppositeboardwallvertexCoordinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(oppositeboardwallindex), oppositeboardwallindex, GL_STATIC_DRAW);
    
    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    //binding texture
    glBindTexture(GL_TEXTURE_2D, walltexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    

    //ceiling
    float ceilingvertexCoordinate[] = {
       //position              //texture coordinates
        0.0f, 14.0f, 0.0f,     0.0f, 0.0f, //bottom left
        0.0f, 14.0f, 10.0f,    0.0f, 1.0f, // bottom right
        10.0f, 14.0f, 0.0f,    1.0f, 0.0f , // top left 
        10.0f, 14.0f, 10.0f ,   1.0f, 1.0f // top right
    };

    unsigned int ceilingindex[] = {
        0,1,2,1,2,3
    };
    
    glGenVertexArrays(1, &VAO[4]);
    glGenBuffers(1, &VBO[4]);
    glGenBuffers(1, &EBO[4]);
    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ceilingvertexCoordinate), ceilingvertexCoordinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ceilingindex), ceilingindex, GL_STATIC_DRAW);
    
    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
      // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    // -------------------------
    unsigned int ceilingtexture;
    glGenTextures(1, &ceilingtexture); 
    //binding texture
    glBindTexture(GL_TEXTURE_2D, ceilingtexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
   
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *walldata1 = stbi_load(FileSystem::getPath("assets/ceiling/white.png").c_str(), &width, &height, &nrChannels, 0);
    if (walldata1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, walldata1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(walldata1);
   //yeha 


   //board ko
    float bvertex[] = {
       //position of board              //texture coordinates
        0.0f, 0.0f, 58.0f,   0.0f, 0.0f, //bottom left
        0.0f, 5.0f, 58.0f,   0.0f, 1.0f, // bottom right
        5.0f, 0.0f, 58.0f,     1.0f, 0.0f , // top left
        5.0f, 5.0f, 58.0f,   1.0f, 1.0f // top right
    };

    unsigned int bindex[] = {
        0,1,2,1,2,3
    };
    
    glGenVertexArrays(1, &VAO[6]);
    glGenBuffers(1, &VBO[6]);
    glGenBuffers(1, &EBO[6]);
    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bvertex), bvertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[6]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bindex), bindex, GL_STATIC_DRAW);
    
    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
      // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    // -------------------------
    unsigned int btexture;
    glGenTextures(1, &btexture); 
    //binding texture
    glBindTexture(GL_TEXTURE_2D, btexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
   
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *walldata2 = stbi_load(FileSystem::getPath("assets/board/board.png").c_str(), &width, &height, &nrChannels, 0);
    if (walldata2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, walldata2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(walldata2);










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
        // glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

         // bind Texture
        
       
        planeShader.use();
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", view);


        //floor
        for ( int k = -7 ; k < 11 ; ++k){ //z-cordinate for floor 
            for(int j = -4 ; j < 7 ; ++j){  //x-cordinate for floor 
            //floor
                glBindVertexArray(VAO[0]);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, floortexture);
                model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f * j, 0.0f, 5.0f * k));
                planeShader.setMat4("model", model);
               
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                //ceiling
                glBindVertexArray(VAO[4]);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, ceilingtexture);
                model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f * j, 0.0f, 5.0f * k));
                planeShader.setMat4("model", model);
               
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            } 
        }


        //eastwall
        for ( int k = -7; k < 12 ; ++k){ //z-cordinate 
            for(int j = -1 ; j < 3 ; ++j){  //y for east
               glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, walltexture);
                model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f+5.0f*j, 5.0f * k));
                planeShader.setMat4("model", model);
                glBindVertexArray(VAO[1]);
                //space for window
                if((j>-1 && j<2 ) && ((k> -6 && k<-1) || (k>4 && k<9) ))
                    continue;
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                // //west wall
                // glActiveTexture(GL_TEXTURE0);
                // glBindTexture(GL_TEXTURE_2D, walltexture);
                // model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f+5.0f*j, 5.0f * k));
                // planeShader.setMat4("model", model);
                // glBindVertexArray(VAO[5]);
                // if((j>=-1 && j< 2) && (k>4 && k<8) )
                //     continue;
                // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        //boardwall x-y
        for ( int k = -1; k < 3 ; ++k){ //y for board
            for(int j = -4 ; j < 8; ++j){  //x for board
               glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, walltexture);
                model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f*j, -1.0f+5.0f*k, 0.0f));
                planeShader.setMat4("model", model);
                glBindVertexArray(VAO[2]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                //opposite wall
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, walltexture);
                model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f*j, -1.0f+5.0f*k, 0.0f));
                planeShader.setMat4("model", model);
                glBindVertexArray(VAO[3]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
//board
            for ( int k = 0; k < 2 ; ++k){ //y for board
            for(int j = -1 ; j < 6; ++j){  //x for board
               glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, btexture);
                model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f*j, -1.0f+5.0f*k, 0.0f));
                planeShader.setMat4("model", model);
                glBindVertexArray(VAO[6]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                
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
        model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 1.0f, 26.0f));
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