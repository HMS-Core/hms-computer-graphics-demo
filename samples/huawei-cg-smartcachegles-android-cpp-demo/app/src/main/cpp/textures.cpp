/* 
 *Copyright (C) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *Description: Texture.
 */
#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "textures.h"

/* Define the target of the textures before using it. */
#define GL_SMART_CACHE_TEXTURE 0x40000
#define GL_SMART_CACHE_FBO 0x80000
#define LOG_TAG "LOG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
GLuint gProgram;
GLuint gvPositionHandle;
GLuint gtexSamplerHandle;
GLuint gTexture;


const char* gVertexShader = "attribute vec4 vPosition;\n"
    "varying vec2 texCoords;\n"
    "void main() {\n"
    "texCoords = vPosition.xy + vec2(0.5, 0.5);\n"
    "gl_Position = vPosition;\n"
    "}\n";

const char* gFragmentShader = "precision mediump float;\n"
    "uniform sampler2D texSampler;\n"
    "varying vec2 texCoords;\n"
    "void main() {\n"
    "gl_FragColor = texture2D(texSampler, texCoords);\n"
    "}\n";

const GLfloat gTriangleVertices[] = {
    -0.5, 0.5,
    -0.5, -0.5,
    0.5, -0.5,
    0.5, 0.5,
};
GLuint loadShader(GLenum shaderType, const char* pSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

bool initializeGraphics(int w, int h)
{
    bool gSmartCache = false;
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (strstr(extensions, "GL_HUAWEI_smart_cache")) {
        gSmartCache = true;
    }

    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        return false;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    gtexSamplerHandle = glGetUniformLocation(gProgram, "texSampler");

    glGenTextures(1, &gTexture);
    glBindTexture(GL_TEXTURE_2D, gTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    uint32_t pixels[] = {
        0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000,
        0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF0000FF,
        0xFF00FFFF, 0xFF00FF00, 0x00FF00FF, 0xFFFFFF00, 0xFF00FFFF, 0xFF00FF00, 0x00FF00FF, 0xFFFFFF00,
        0xFF000000, 0xFFFF00FF, 0xFF00FFFF, 0xFFFFFFFF, 0xFF000000, 0xFFFF00FF, 0xFF00FFFF, 0xFFFFFFFF,
        0xFF000000, 0xFFFF00FF, 0xFF00FFFF, 0xFFFFFFFF, 0xFF000000, 0xFFFF00FF, 0xFF00FFFF, 0xFFFFFFFF,
        0xFF00FFFF, 0xFF00FF00, 0x00FF00FF, 0xFFFFFF00, 0xFF00FFFF, 0xFF00FF00, 0x00FF00FF, 0xFFFFFF00,
        0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF0000FF,
        0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000
    };
    GLenum target = GL_TEXTURE_2D;
    if (gSmartCache) {
        /* Use GL_SMART_CACHE_FBO if target is FBO. */
        target |= GL_SMART_CACHE_TEXTURE;
    }
    glTexImage2D(target, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glViewport(0, 0, w, h);
    return true;
}

void drawFrame()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gProgram);

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    glEnableVertexAttribArray(gvPositionHandle);
    glUniform1i(gtexSamplerHandle, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

extern "C" {
JNIEXPORT void JNICALL Java_com_huawei_textures_GL2TexturesLib_init(JNIEnv* env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_huawei_textures_GL2TexturesLib_step(JNIEnv* env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_huawei_textures_GL2TexturesLib_init(JNIEnv* env, jobject obj, jint width, jint height)
{
    initializeGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_huawei_textures_GL2TexturesLib_step(JNIEnv* env, jobject obj)
{
    drawFrame();
}
