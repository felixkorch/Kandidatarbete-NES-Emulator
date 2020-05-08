#include "llvmes-gui/draw.h"
#include "llvmes-gui/application.h"
#include "llvmes-gui/opengl.h"

#include "glm/gtc/matrix_transform.hpp"

namespace llvmes {
namespace gui {

static constexpr std::size_t MAX_RECTANGLES = 10000;
static constexpr std::size_t VERTEX_SIZE = sizeof(VertexData);
static constexpr std::size_t RECTANGLE_SIZE = (4 * VERTEX_SIZE);
static constexpr std::size_t BUFFER_SIZE = RECTANGLE_SIZE * MAX_RECTANGLES;
static constexpr std::size_t INDICES_COUNT = (6 * MAX_RECTANGLES);

const char* default_shader = R"(
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;
layout(location = 3) in float tid;

out DATA
{
	vec3 position;
	vec2 uv;
	float tid;
	vec4 color;
} vs_out;

uniform mat4 u_projection;

void main() {
	gl_Position = u_projection * vec4(position, 1.0);
	vs_out.position = position;
	vs_out.uv = uv;
	vs_out.tid = tid;
	vs_out.color = color;
}

#shader fragment
#version 330 core

const int MAX_TEXTURES = 10;

uniform sampler2D u_sampler[MAX_TEXTURES];

in DATA
{
	vec3 position;
	vec2 uv;
	float tid;
	vec4 color;
} fs_in;

layout (location = 0) out vec4 color;

void main() {
    vec4 tex_color = fs_in.color;

    // Has a texture
    if(fs_in.tid > 0) {
        int tid = int(fs_in.tid - 0.5);
        switch(tid) {
            case 0:
                tex_color = fs_in.color * texture(u_sampler[0], fs_in.uv);
                break;
            case 1:
                tex_color = fs_in.color * texture(u_sampler[1], fs_in.uv);
                break;
            case 2:
                tex_color = fs_in.color * texture(u_sampler[2], fs_in.uv);
                break;
            case 3:
                tex_color = fs_in.color * texture(u_sampler[3], fs_in.uv);
                break;
            case 4:
                tex_color = fs_in.color * texture(u_sampler[4], fs_in.uv);
                break;
            case 5:
                tex_color = fs_in.color * texture(u_sampler[5], fs_in.uv);
                break;
            case 6:
                tex_color = fs_in.color * texture(u_sampler[6], fs_in.uv);
                break;
            case 7:
                tex_color = fs_in.color * texture(u_sampler[7], fs_in.uv);
                break;
            case 8:
                tex_color = fs_in.color * texture(u_sampler[8], fs_in.uv);
                break;
            case 9:
                tex_color = fs_in.color * texture(u_sampler[9], fs_in.uv);
                break;
            default:
                tex_color = fs_in.color * texture(u_sampler[0], fs_in.uv);
                break;
        }
    }
	color = tex_color;
}
)";

const char* gles3_shader = R"(
#shader vertex
#version 300 es

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;
layout(location = 3) in float tid;

vec3 f_position;
vec2 f_uv;
float f_tid;
vec4 f_color;

uniform mat4 u_projection;

void main() {
	gl_Position = u_projection * vec4(position, 1.0);
	f_position = position;
	f_uv = uv;
	f_tid = tid;
	f_color = color;
}

#shader fragment
#version 300 es

precision mediump float;

const int MAX_TEXTURES = 10;

uniform sampler2D u_sampler[MAX_TEXTURES];

vec3 f_position;
vec2 f_uv;
float f_tid;
vec4 f_color;

layout (location = 0) out vec4 color;

void main() {
    vec4 tex_color = f_color;

    // Has a texture
    if(f_tid > 0.0) {
        int tid = int(f_tid - 0.5);
        switch(tid) {
            case 0:
                tex_color = f_color * texture(u_sampler[0], f_uv);
                break;
            case 1:
                tex_color = f_color * texture(u_sampler[1], f_uv);
                break;
            case 2:
                tex_color = f_color * texture(u_sampler[2], f_uv);
                break;
            case 3:
                tex_color = f_color * texture(u_sampler[3], f_uv);
                break;
            case 4:
                tex_color = f_color * texture(u_sampler[4], f_uv);
                break;
            case 5:
                tex_color = f_color * texture(u_sampler[5], f_uv);
                break;
            case 6:
                tex_color = f_color * texture(u_sampler[6], f_uv);
                break;
            case 7:
                tex_color = f_color * texture(u_sampler[7], f_uv);
                break;
            case 8:
                tex_color = f_color * texture(u_sampler[8], f_uv);
                break;
            case 9:
                tex_color = f_color * texture(u_sampler[9], f_uv);
                break;
            default:
                tex_color = f_color * texture(u_sampler[0], f_uv);
                break;
        }
    }
	color = tex_color;
}
)";

static glm::vec2 default_uv[] = {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1),
                                 glm::vec2(0, 1)};

VertexArray* Draw::vao = nullptr;
IndexBuffer* Draw::ibo = nullptr;
Shader* Draw::shader = nullptr;
VertexData* Draw::data = nullptr;
unsigned int Draw::index_count = 0;
std::unordered_map<Texture*, unsigned int> Draw::textures;
Texture* Draw::current_texture = nullptr;
glm::vec4 Draw::current_color = glm::vec4(1);

void Draw::Init()
{
    vao = new VertexArray;
    VertexBuffer* vbo = new VertexBuffer;
    vbo->SetType(BufferUsage::DYNAMIC);
    vbo->Load(BUFFER_SIZE);

    VertexBufferLayout layout;
    layout.Push<float>(3);  // Position
    layout.Push<float>(4);  // Color
    layout.Push<float>(2);  // UV-Coords (Texture coordinates)
    layout.Push<float>(1);  // TID (Texture ID)

    vao->AddBuffer(vbo, layout);

    std::vector<unsigned int> indices(INDICES_COUNT);

    int offset = 0;
    for (int i = 0; i < INDICES_COUNT; i += 6) {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
    }

    ibo = new IndexBuffer(indices.data(), INDICES_COUNT);
#ifdef LLVMES_PLATFORM_WEB
    shader = new Shader(gles3_shader);
#else
    shader = new Shader(default_shader);
#endif
    index_count = 0;
}

void Draw::DeInit()
{
    delete vao;
    delete ibo;
    delete shader;
}

void Draw::Begin()
{
    vao->Bind();
    data = (VertexData*)vao->GetBuffer().GetInternalPointer();
}

void Draw::End()
{
    Application& app = Application::Get();
    int width = app.GetWindow().GetWidth();
    int height = app.GetWindow().GetHeight();

    vao->GetBuffer().ReleasePointer();
    shader->Bind();
    shader->SetUniformMat4f(
        "u_projection", glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f) *
                            glm::translate(glm::mat4(1.0f), glm::vec3()));

    for (auto& pair : textures)
        pair.first->Bind(pair.second);

    vao->Bind();
    ibo->Bind();
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);

    textures.clear();
    index_count = 0;
}

void Draw::Rectangle(float x, float y, float w, float h)
{
    const glm::vec3 v1 = glm::vec3(x, y, 1);
    const glm::vec3 v2 = glm::vec3(x + w, y, 1);
    const glm::vec3 v3 = glm::vec3(x + w, y + h, 1);
    const glm::vec3 v4 = glm::vec3(x, y + h, 1);

    float slot = current_texture == nullptr ? 0 : textures[current_texture];

    *data++ = {v1, current_color, default_uv[0], slot};
    *data++ = {v2, current_color, default_uv[1], slot};
    *data++ = {v3, current_color, default_uv[2], slot};
    *data++ = {v4, current_color, default_uv[3], slot};

    index_count += 6;
}

void Draw::Rectangle(const glm::vec2& pos, const glm::vec2& size)
{
    Rectangle(pos.x, pos.y, size.x, size.y);
}

void Draw::UseColor(float r, float g, float b, float a)
{
    current_color = glm::vec4(r, g, b, a);
    current_texture = nullptr;
}

void Draw::UseColor(const glm::vec4& color)
{
    current_color = color;
    current_texture = nullptr;
}

void Draw::UseTexture(Texture* texture)
{
    current_texture = texture;
    textures[texture] = textures.size() + 1;
}

}  // namespace gui
}  // namespace llvmes