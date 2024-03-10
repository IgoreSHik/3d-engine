#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stdio.h>

#define preset0u { -0.6f,  0.f,  0.3f,
#define preset1u {   0.f,  0.6f, 0.3f,
#define preset2u {  0.6f,  0.f,  0.3f,
#define preset3u {  0.4f, -0.6f, 0.3f,
#define preset4u { -0.4f, -0.6f, 0.3f,
#define preset0d { -0.6f,  0.f,  -0.3f,
#define preset1d {   0.f,  0.6f, -0.3f,
#define preset2d {  0.6f,  0.f,  -0.3f,
#define preset3d {  0.4f, -0.6f, -0.3f,
#define preset4d { -0.4f, -0.6f, -0.3f,

#define colorup 1.f, 0.f, 0.f }
#define colordown 1.f, 1.f, 1.f }
#define colorside0 0.7f, 0.7f, 0.7f }
#define colorside1 0.3f, 0.7f, 0.3f }
#define colorside2 0.3f, 0.3f, 0.7f }
#define colorside3 0.7f, 0.3f, 0.3f }
#define colorside4 0.3f, 0.3f, 0.3f }

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
GLboolean firstMouse = 1;

static const struct
{
    float x, y, z;
    float r, g, b;
} vertices[30] =
{
    preset0u colorup,
    preset1u colorup,
    preset2u colorup,
    preset3u colorup,
    preset4u colorup,

    preset0d colordown,
    preset1d colordown,
    preset2d colordown,
    preset3d colordown,
    preset4d colordown,

    preset0u colorside0,
    preset1u colorside0,
    preset1d colorside0,
    preset0d colorside0,

    preset1u colorside1,
    preset2u colorside1,
    preset2d colorside1,
    preset1d colorside1,

    preset2u colorside2,
    preset3u colorside2,
    preset3d colorside2,
    preset2d colorside2,

    preset3u colorside3,
    preset4u colorside3,
    preset4d colorside3,
    preset3d colorside3,

    preset0u colorside4,
    preset4u colorside4,
    preset4d colorside4,
    preset0d colorside4
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

float xKat = 20;
float zKat = 0;
float place[2] = {0,0};
double xposprev = 0;
double yposprev = 0;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (xpos<xposprev) {xposprev=xpos; xKat++;};
    if (xpos>xposprev) {xposprev=xpos; xKat--;};
    //if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) xKat = ++xKat > 18000 ? 180 : xKat;
    //if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) xKat = --xKat < 0 ? 0 : xKat;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) zKat++;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) zKat--;

    printf("x = %f, y = %f \n", xpos, ypos);
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 3));

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (glfwRawMouseMotionSupported())
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glEnable(GL_DEPTH_TEST);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4_identity(m);

        //mat4x4_rotate_X(m, m, (float) glfwGetTime());
        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_perspective(p, 1.57, width/(float)height, 1,10);
        //mat4x4_translate(p, glfwGetTime()/20, 0, 0);

        //glPushMatrix();
        {
            glfwGetCursorPos(window, &xpos, &ypos);

            if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) xKat = ++xKat > 18000 ? 180 : xKat;
            if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) xKat = --xKat < 0 ? 0 : xKat;
            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) zKat++;
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) zKat--;

            float kat = -zKat / 180 * M_PI;
            float speed = 0;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) speed = 0.1;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) speed = -0.1;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { speed = 0.1; kat -= M_PI*0.5; }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { speed = 0.1; kat += M_PI*0.5; }
            if (speed != 0)
            {
                place[0] += sin(kat) * speed;
                place[1] += cos(kat) * speed;
            }

            //glRotatef(-xKat, 1,0,0);
            mat4x4_rotate_X(m, m, (float) xKat/100);
            //glRotatef(-zKat, 0,0,1);
            mat4x4_rotate_Z(m, m, (float) zKat/100);
            //glTranslatef(-place[0], -place[1], -3);
            mat4x4_translate(p, -place[0], 0, -place[1]);
        }

        mat4x4_mul(mvp, p, m);
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
        glDrawArrays(GL_TRIANGLE_FAN, 5, 5);
        glDrawArrays(GL_QUADS, 10, 20);
        Sleep(1);
        //glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
