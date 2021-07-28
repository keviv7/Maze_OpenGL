#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <utility>
#include <algorithm>

#include <SDL.h>
#include <SDL_image.h>

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#include "shader.hpp"
#include "mazeGenerator.hpp"


int windowWidth, windowHeight, mazeSize;
bool** mazeArray;


bool checkCollisionPointReactangle(float pointX, float pointY, float wallX, float wallY)
{
    if (pointX >= wallX - 0.7f && pointX <= wallX + 0.7f &&
        pointY >= wallY - 0.7f && pointY <= wallY + 0.7f)
            return true;

    return false;
}

bool checkCollision(float positionX, float positionZ){

    int row = positionZ;
    int col = positionX;

    for(int i = max(0,row-2); i < min(mazeSize+2,row+2); i++){
        for(int j = max(0,col-2); j < min(mazeSize+2,col+2); j++){
            if(mazeArray[i][j] && checkCollisionPointReactangle(positionX, positionZ, j*1.0f, i*1.0f)){
                return true;
            }
        }
    }

    return false;
}


int main(int argc, char* argv[])
{
    windowWidth = 1000;
    windowHeight = 750;
    mazeSize = 10;

    mazeArray=generateMaze(mazeSize);


    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        cerr << "SDL init failed! Error: " << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

   
    IMG_Init(IMG_INIT_PNG);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //Set OpenGL context to OpenGL 3.3 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_Window* mainWindow = SDL_CreateWindow("Maze Runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
    if (mainWindow == nullptr)
    {
        cout << "Failed creating SDL Window!" << endl;
        return EXIT_FAILURE;
    }
    
    SDL_GLContext glContext = SDL_GL_CreateContext(mainWindow);
    if (glContext == nullptr) 
    {
        cout << "Error creating OpenGL context!" << endl;
        return EXIT_FAILURE;
    }


    if(!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        cerr << "Failed initializing GLAD!" << endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    float wallVertices[] = {
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f,             0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f,             0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,             0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f,             0.0f, 0.0f, 1.0f
    };

    unsigned int wallIndices[] = {  
        0, 1, 3, 
        1, 2, 3 
    };



    
    if (!loadShaders("shaders/vertexshader.vert","shaders/fragmentshader.frag"))
    {
        cerr << "Failed loading shaders!" << endl;
        return EXIT_FAILURE;
    }

 
    SDL_Surface* wallImage = IMG_Load("textures/wall.png");
    SDL_Surface* floorImage = IMG_Load("textures/floor.png");
    SDL_Surface* ceilingImage = IMG_Load("textures/ceiling.png");
    SDL_Surface* exitImage = IMG_Load("textures/exit.png");

    if (wallImage == nullptr || floorImage == nullptr || ceilingImage == nullptr || exitImage == nullptr){
        cerr << "Failed loading texture!" << endl;
        return EXIT_FAILURE;    
    }


    GLuint vertexBufferObject, vertexArrayObject, elementArrayBuffer;

    glGenVertexArrays(1, &vertexArrayObject); 
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementArrayBuffer);

    glBindVertexArray(vertexArrayObject); 

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(float))); 
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(float))); 
    glEnableVertexAttribArray(2);


    GLuint mazeTextures[4];
    glGenTextures(4, mazeTextures);


    glBindTexture(GL_TEXTURE_2D, mazeTextures[0]);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wallImage->w, wallImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, wallImage->pixels); //Get texture data from SDL surface
    
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, mazeTextures[1]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, floorImage->w, floorImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, floorImage->pixels); //Get texture data from SDL surface
    
    glGenerateMipmap(GL_TEXTURE_2D);

 
    glBindTexture(GL_TEXTURE_2D, mazeTextures[2]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ceilingImage->w, ceilingImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ceilingImage->pixels); //Get texture data from SDL surface
    
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, mazeTextures[3]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, exitImage->w, exitImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, exitImage->pixels); //Get texture data from SDL surface
    
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    SDL_FreeSurface(wallImage);
    SDL_FreeSurface(floorImage);
    SDL_FreeSurface(ceilingImage);
    SDL_FreeSurface(exitImage);




    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    glm::vec3 cameraPosition   = glm::vec3(getStartX()*1.0f, 0.0f, getStartY()*1.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    float lookAngle = -90.0f;

    view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);


    SDL_GL_SetSwapInterval(1);

    SDL_Event windowEvent;
    bool isRunning = true;

    float deltaTime, lastFrame, currentFrame, cameraSpeed, movementSpeed;
    lastFrame = SDL_GetTicks()*0.001f;
    deltaTime = lastFrame;

    glm::vec3 lastPosition = cameraPosition;




    while (isRunning)
    {
        while (SDL_PollEvent(&windowEvent))
        {
            if (windowEvent.type == SDL_QUIT)
                isRunning = false;
        } 

        cameraSpeed = 120.0f * deltaTime;
        movementSpeed = 2.5f * deltaTime;
        currentFrame = SDL_GetTicks()*0.001f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        
        if (state[SDL_SCANCODE_UP])
        { 
            lastPosition = cameraPosition; 

            cameraPosition.x += movementSpeed * cameraFront.x;

            if (checkCollision(cameraPosition.x, cameraPosition.z))
            {
                cameraPosition = lastPosition;
            }

            lastPosition = cameraPosition;

            cameraPosition.z += movementSpeed * cameraFront.z;

            if (checkCollision(cameraPosition.x, cameraPosition.z))
            {
                cameraPosition = lastPosition;
            }
        }

        if (state[SDL_SCANCODE_DOWN])
        { 
            lastPosition = cameraPosition;

            cameraPosition.x -= movementSpeed * cameraFront.x;

            if (checkCollision(cameraPosition.x, cameraPosition.z))
            {
                cameraPosition = lastPosition;
            }

            lastPosition = cameraPosition;

            cameraPosition.z -= movementSpeed * cameraFront.z;

            if (checkCollision(cameraPosition.x, cameraPosition.z))
            {
                cameraPosition = lastPosition;
            }
        }

        if (state[SDL_SCANCODE_LEFT])
        {
            lookAngle -= cameraSpeed;
        }
        

        if (state[SDL_SCANCODE_RIGHT])
        {
            lookAngle += cameraSpeed;
        }

        if (state[SDL_SCANCODE_ESCAPE])
        {
            isRunning = false;
        }

        if (checkCollisionPointReactangle(cameraPosition.x, cameraPosition.z, getEndX(), getEndY())){
            isRunning = false;
        }


        cameraFront.x = cos(glm::radians(lookAngle));
        cameraFront.y = sin(glm::radians(0.0f)); 
        cameraFront.z = sin(glm::radians(lookAngle));        


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glBindTexture(GL_TEXTURE_2D, mazeTextures[0]);

        useShader();
        
        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        setUniformMatrix4fv("view", view);

        setUniformMatrix4fv("projection", projection);

        setUniformVec3fv("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        setUniformVec3fv("lightVector", cameraPosition);

        glBindVertexArray(vertexArrayObject);

      
        int startRow = max(1, (int)cameraPosition.z - 15);
        int startColumn = max(1, (int)cameraPosition.x - 15);
        int endRow = min(mazeSize + 1, (int)cameraPosition.z + 15);
        int endColumn = min(mazeSize + 1, (int)cameraPosition.x + 15);


        for (int i = startRow; i < endRow; i++)
        {
            for (int j = startColumn; j < endColumn; j++)
            {
                if (mazeArray[i][j]) continue;
 
                glBindTexture(GL_TEXTURE_2D, mazeTextures[0]);

                if (mazeArray[i][j-1]) //Left
                {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(j*1.0f, 0.0f, i*1.0f)); //Move to right position
                    model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f)); //Move left a bit
                    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotate by 90 degrees around Y

                    setUniformMatrix4fv("model", model);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
                
                if (mazeArray[i][j+1]) //Right
                {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(j*1.0f, 0.0f, i*1.0f)); //Move to right position
                    model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f)); //Move right a bit
                    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotate by 90 degrees around Y

                    setUniformMatrix4fv("model", model);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
                
                if (mazeArray[i-1][j]) //Front
                {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(j*1.0f, 0.0f, i*1.0f)); //Move to right position
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f)); //Move front a bit
                    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

                    setUniformMatrix4fv("model", model);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
                
                if (mazeArray[i+1][j]) //Back
                {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(j*1.0f, 0.0f, i*1.0f)); //Move to right position
                    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f)); //Move back a bit

                    setUniformMatrix4fv("model", model);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }

                //Draw floor and ceiling
                glBindTexture(GL_TEXTURE_2D, mazeTextures[1]);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(j*1.0f, 0.0f, i*1.0f));
                model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

                setUniformMatrix4fv("model", model);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                //Ceiling
                glBindTexture(GL_TEXTURE_2D, mazeTextures[2]);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(j*1.0f, 0.0f, i*1.0f));
                model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

                setUniformMatrix4fv("model", model);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


                //Exit is visible so draw it
                if (j == getEndX() && i == getEndY())
                {
                    glBindTexture(GL_TEXTURE_2D, mazeTextures[3]);

                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(j*1.0f, 0.0f, i*1.0f));

                    char dir=getEndBorder();

                    if(dir=='L'){
                        model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
                        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    }
                    else  if(dir=='R'){
                        model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f)); 
                        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
                    }
                    else  if(dir=='U'){
                        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f)); 
                        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    }
                    else  if(dir=='D'){
                        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f));
                    }

                    setUniformMatrix4fv("model", model);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }
        }

        //Swap buffers and end drawing
        SDL_GL_SwapWindow(mainWindow);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(mainWindow);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
