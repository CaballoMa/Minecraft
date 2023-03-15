#pragma once

#include <openglcontext.h>
#include <memory>

class Texture
{
public:
    Texture(OpenGLContext* context);
    ~Texture();

    void create(const char *texturePath);
    void createNoise(const char *texturePath);
    void load(int texSlot );
    void bind(int texSlot);
    void loadNoise(int texSlot);

    void create3D();

    GLuint createDepthTexture(int width, int height);

private:
    OpenGLContext* context;
    GLuint m_textureHandle;
    std::shared_ptr<QImage> m_textureImage;
};
