
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#undef STB_TRUETYPE_IMPLEMENTATION

#include <../../include/veil/text.hpp>
#include <../../include/veil/log.hpp>

#include <fstream>

namespace veil {

Font::Font(const std::filesystem::path& path, float heightPx) {

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to open font '{}'", path.string()));

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> ttfBuffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(ttfBuffer.data()), fileSize))
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to read font '{}'", path.string()));

    m_bitmap.resize(kAtlasW * kAtlasH);
    m_charData.resize(kNumChars);

    stbtt_BakeFontBitmap(
        ttfBuffer.data(), 0, heightPx, m_bitmap.data(), kAtlasW, kAtlasH, kFirstChar, kNumChars, m_charData.data()
    );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLuint textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, 1, GL_R8, kAtlasW, kAtlasH);
    glTextureSubImage2D(textureID, 0, 0, 0, kAtlasW, kAtlasH, GL_RED, GL_UNSIGNED_BYTE, m_bitmap.data());
    
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_RED };
    glTextureParameteriv(textureID, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

    m_texture = std::make_unique<Texture>(textureID, path);
}

Font::~Font() {

    m_texture.reset();
}

Text::Text(const Font& font) : m_font(font) {

    Material fontMaterial{};
    fontMaterial.diffuse = &font.getTexture();

    m_mesh = std::make_unique<Mesh>(
        std::vector<Vertex>{},
        std::vector<unsigned int>{},
        fontMaterial
    );
}

Text::~Text() {

    m_mesh.reset();
}

void Text::setText(const std::string& str) {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(str.size() * 4);
    indices.reserve(str.size() * 6);

    float xOff = 0.0f, yOff = 0.0f;
    unsigned int vertOff = 0;

    for (char c : str) {

        if (c < Font::kFirstChar || c >= Font::kFirstChar + Font::kNumChars) 
            continue;

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(
            m_font.getCharData().data(), Font::kAtlasW, Font::kAtlasH, c - Font::kFirstChar, &xOff, &yOff, &q, 1
        );

        vertices.push_back(veil::Vertex{.position={q.x0, -q.y0, 0.0f}, .texuv={q.s0, q.t0}});
        vertices.push_back(veil::Vertex{.position={q.x1, -q.y0, 0.0f}, .texuv={q.s1, q.t0}});
        vertices.push_back(veil::Vertex{.position={q.x1, -q.y1, 0.0f}, .texuv={q.s1, q.t1}});
        vertices.push_back(veil::Vertex{.position={q.x0, -q.y1, 0.0f}, .texuv={q.s0, q.t1}});

        indices.push_back(vertOff + 0);
        indices.push_back(vertOff + 1);
        indices.push_back(vertOff + 2);
        indices.push_back(vertOff + 0);
        indices.push_back(vertOff + 2);
        indices.push_back(vertOff + 3);

        vertOff += 4;
    }

    m_mesh->setData(vertices, indices);
}

void Text::render(int mode) const {
    
    glDisable(GL_CULL_FACE);

    if (m_font.getTextureID() != 0)
        glBindTextureUnit(0, m_font.getTextureID());

    m_mesh->render(mode);

    glEnable(GL_CULL_FACE);
}

}; //namespace veil