#include "llvmes-gui/buffers.h"
#include "llvmes-gui/log.h"
#include "llvmes-gui/opengl.h"

#include <iostream>

namespace llvmes {
namespace gui {

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
{
    glGenBuffers(1, &unique_id);

    if (unique_id == 0)
        throw std::runtime_error("OpenGL - Failed to create IndexBuffer");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, unique_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data,
                 GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &unique_id);
}

void IndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, unique_id);
}

void IndexBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

VertexBuffer::VertexBuffer() : type(BufferUsage::NONE)
{
    glGenBuffers(1, &unique_id);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &unique_id);
}

void VertexBuffer::SetType(BufferUsage usage)
{
    type = usage;
}

void VertexBuffer::Load(std::size_t size, const void* data)
{
    LLVMES_ASSERT(type != BufferUsage::NONE, "Use 'SetType' before calling 'Load'");
    buffer_size = size;
    glBindBuffer(GL_ARRAY_BUFFER, unique_id);

    if (type == BufferUsage::DYNAMIC)
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

    else if (type == BufferUsage::STATIC)
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void* VertexBuffer::GetInternalPointer()
{
    return glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size,
                            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

void VertexBuffer::ReleasePointer()
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void VertexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, unique_id);
}

void VertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &unique_id);
    if (unique_id == 0)
        throw std::runtime_error("OpenGL - Failed to create Vertex Array");
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &unique_id);
    for (VertexBuffer* vbo : buffers)
        delete vbo;
}

void VertexArray::AddBuffer(VertexBuffer* vb, const VertexBufferLayout& layout)
{
    Bind();
    vb->Bind();
    const auto& elements = layout.GetElements();
    std::size_t offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        glVertexAttribPointer(i, element.count, element.type, element.normalized,
                              layout.GetStride(), (const void*)offset);
        glEnableVertexAttribArray(i);
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }

    buffers.push_back(vb);
    Unbind();
}

void VertexArray::Bind()
{
    glBindVertexArray(unique_id);
}

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

}  // namespace gui
}  // namespace llvmes