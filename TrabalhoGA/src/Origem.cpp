#include "Shader.h"
#include <vector>
#include <math.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define EXIT_PROGRAM -1
int WIDTH = 640;
int HEIGHT = 480;
float incCurve = 0.001f;
unsigned int VBOPoints, VAOPoints;
unsigned int VBOSpline, VAOSpline;
unsigned int VBOInternalSpline, VAOInternalSpline;
std::vector<glm::vec3> ControlPoints;
std::vector<glm::vec3> BSplinePoints;
std::vector<glm::vec3> InternalBSpline;


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);


void SetupGeometry();
void UpdateGeometry();
void RenderGeometry();
void UpdateSpline();


int main()
{
    GLFWwindow* window;

    if (!glfwInit())
    {
        std::cout << "Erro ao iniciar a glfw" << std::endl;
        return EXIT_PROGRAM;
    }

    window = glfwCreateWindow(WIDTH, HEIGHT, "Trabalho GB - Igor Flores e Tiago Gazzola", NULL, NULL);

    if (!window)
    {
        std::cout << "Houve um erro para criar a janela GLFW" << std::endl;
        glfwTerminate();
        return EXIT_PROGRAM;
    }

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);


    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Erro ao iniciar o glew" << std::endl;
        return EXIT_PROGRAM;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;


    Shader shader("../Resources/Shaders/Basic.shader");
    shader.Bind();


    glm::mat4 projection = glm::ortho(0.0f, (float)WIDTH, 0.0f, float(HEIGHT), -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.m_RendererID, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projection));

    SetupGeometry();

    glfwSwapInterval(1);
   
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.6, 0.6, 0.6, 1.0);
                     
        RenderGeometry();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.Unbind();
    glfwTerminate();

    return 0;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float x = (float)xpos;
        float y = (float)(HEIGHT - ypos); // O Y é invertido

        ControlPoints.push_back(glm::vec3(x, y, 0.0f));
        UpdateGeometry();

        if (ControlPoints.size() >= 4)
            UpdateSpline();

        std::cout << "Pos X: " << x << " Pos Y: " << y << std::endl;
    }
}

void UpdateGeometry()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBOPoints);
    glBufferData(GL_ARRAY_BUFFER, ControlPoints.size() * sizeof(glm::vec3), ControlPoints.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBOSpline);
    glBufferData(GL_ARRAY_BUFFER, BSplinePoints.size() * sizeof(glm::vec3), BSplinePoints.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBOInternalSpline);
    glBufferData(GL_ARRAY_BUFFER, InternalBSpline.size() * sizeof(glm::vec3), InternalBSpline.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SetupGeometry()
{

    //Update Control Points
    glGenBuffers(1, &VBOPoints);
    glGenVertexArrays(1, &VAOPoints);

    glBindVertexArray(VAOPoints);

    glBindBuffer(GL_ARRAY_BUFFER, VBOPoints);
    glBufferData(GL_ARRAY_BUFFER, ControlPoints.size() * sizeof(glm::vec3), ControlPoints.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Update BSPline
    glGenBuffers(1, &VBOSpline);
    glGenVertexArrays(1, &VAOSpline);

    glBindVertexArray(VAOSpline);

    glBindBuffer(GL_ARRAY_BUFFER, VBOSpline);
    glBufferData(GL_ARRAY_BUFFER, BSplinePoints.size() * sizeof(glm::vec3), BSplinePoints.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Internal BSPline
    glGenBuffers(1, &VBOInternalSpline);
    glGenVertexArrays(1, &VAOInternalSpline);

    glBindVertexArray(VAOInternalSpline);

    glBindBuffer(GL_ARRAY_BUFFER, VBOInternalSpline);
    glBufferData(GL_ARRAY_BUFFER, InternalBSpline.size() * sizeof(glm::vec3), InternalBSpline.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderGeometry()
{
    glBindVertexArray(VAOPoints);

    if (ControlPoints.size() >= 2)
    {
        glDrawArrays(GL_LINE_STRIP, 0, ControlPoints.size());
    }
    else
    {
        glDrawArrays(GL_POINTS, 0, ControlPoints.size());
    }

    if (BSplinePoints.size() >= 4)
    {
        glBindVertexArray(VAOSpline);
        glDrawArrays(GL_LINE_STRIP, 0, BSplinePoints.size());
       
        glBindVertexArray(VAOInternalSpline);
        glDrawArrays(GL_LINE_STRIP, 0, BSplinePoints.size());
    }

    glBindVertexArray(0);
}

void UpdateSpline()
{
    BSplinePoints.clear();
    InternalBSpline.clear();

    for (int i = 0; i < ControlPoints.size(); i++)
        for (float t = 0; t <= 1.0f; t += incCurve)
        {
            float x = (((-1 * std::pow(t, 3) + 3 * std::pow(t, 2) - 3 * t + 1) * ControlPoints[i].x +
                (3 * std::pow(t, 3) - 6 * std::pow(t, 2) + 0 * t + 4) * ControlPoints[(i + 1) % ControlPoints.size()].x +
                (-3 * std::pow(t, 3) + 3 * std::pow(t, 2) + 3 * t + 1) * ControlPoints[(i + 2) % ControlPoints.size()].x +
                (1 * std::pow(t, 3) + 0 * std::pow(t, 2) + 0 * t + 0) * ControlPoints[(i + 3) % ControlPoints.size()].x) / 6);

            float y = (((-1 * std::pow(t, 3) + 3 * std::pow(t, 2) - 3 * t + 1) * ControlPoints[i].y +
                (3 * std::pow(t, 3) - 6 * std::pow(t, 2) + 0 * t + 4) * ControlPoints[(i + 1) % ControlPoints.size()].y +
                (-3 * std::pow(t, 3) + 3 * std::pow(t, 2) + 3 * t + 1) * ControlPoints[(i + 2) % ControlPoints.size()].y +
                (1 * std::pow(t, 3) + 0 * std::pow(t, 2) + 0 * t + 0) * ControlPoints[(i + 3) % ControlPoints.size()].y) / 6);

            //float z = 0.0f;
            float z = (((-1 * std::pow(t, 3) + 3 * std::pow(t, 2) - 3 * t + 1) * ControlPoints[i].z +
                (3 * std::pow(t, 3) - 6 * std::pow(t, 2) + 0 * t + 4) * ControlPoints[(i + 1) % ControlPoints.size()].z +
                (-3 * std::pow(t, 3) + 3 * std::pow(t, 2) + 3 * t + 1) * ControlPoints[(i + 2) % ControlPoints.size()].z +
                (1 * std::pow(t, 3) + 0 * std::pow(t, 2) + 0 * t + 0) * ControlPoints[(i + 3) % ControlPoints.size()].z) / 6);

            BSplinePoints.push_back(glm::vec3(x, y, z));
        }

    for (int i = 0; i < BSplinePoints.size(); i++)
    {
        float w = BSplinePoints[(i + 1) % BSplinePoints.size()].x - BSplinePoints[i].x;
        float h = BSplinePoints[(i + 1) % BSplinePoints.size()].y - BSplinePoints[i].y;
        float teta = std::atan(h / w);
        float alfa;

        if (w < 0)
            alfa = teta - (3.14159f / 2);
        else
            alfa = teta + (3.14159f / 2);

        float xSpline = std::cos(alfa) * 20 + BSplinePoints[i].x;
        float ySpline = std::sin(alfa) * 20 + BSplinePoints[i].y;
       
        InternalBSpline.push_back(glm::vec3(xSpline, ySpline, 0.0f));
    }

    UpdateGeometry();
          
}
