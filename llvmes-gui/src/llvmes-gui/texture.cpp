#include "llvmes-gui/texture.h"
#include "llvmes-gui/opengl.h"

#include <iostream>

namespace llvmes {
namespace gui {

static int GetTextureWrap(TextureWrap wrap)
{
    switch (wrap) {
        case TextureWrap::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case TextureWrap::REPEAT:
            return GL_REPEAT;
        case TextureWrap::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        default:
            return GL_CLAMP_TO_EDGE;
    }
}

static int GetTextureFormat(TextureFormat format)
{
    switch (format) {
        case TextureFormat::RGBA:
            return GL_RGBA;
        case TextureFormat::RGB:
            return GL_RGB;
        case TextureFormat::LUMINANCE:
            return GL_LUMINANCE;
        case TextureFormat::LUMINANCE_ALPHA:
            return GL_LUMINANCE_ALPHA;
        default:
            return GL_RGBA;
    }
}

static int GetTextureInternalType(TextureFormat format)
{
    switch (format) {
        case TextureFormat::RGBA:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::RGB:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::RGBA32F:
            return GL_FLOAT;
        default:
            return GL_UNSIGNED_BYTE;
    }
}

Texture::Texture(int width, int height, TextureType type)
    : width(width), height(height), type(type)
{
    glGenTextures(1, &unique_id);

    if (unique_id == 0)
        throw std::runtime_error("OpenGL - Failed to create texture");

    glBindTexture(GL_TEXTURE_2D, unique_id);

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        type.filter == TextureFilter::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    type.filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetTextureWrap(type.wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetTextureWrap(type.wrap));
    glTexImage2D(GL_TEXTURE_2D, 0, GetTextureFormat(type.format), width, height, 0,
                 GetTextureFormat(type.format), GetTextureInternalType(type.format),
                 nullptr);
    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &unique_id);
}

void Texture::Bind(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, unique_id);
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetData(void* data)
{
    glBindTexture(GL_TEXTURE_2D, unique_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GetTextureFormat(type.format),
                    GetTextureInternalType(type.format), data);
}

void Texture::Resize(int width, int height)
{
    this->width = width;
    this->height = height;
    glTexImage2D(GL_TEXTURE_2D, 0, GetTextureFormat(type.format), width, height, 0,
                 GetTextureFormat(type.format), GetTextureInternalType(type.format),
                 nullptr);
}

}  // namespace gui
}  // namespace llvmes