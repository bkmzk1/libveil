
#pragma once 

#include <vector>
#include <span>

#include <stb/stb_truetype.h>
#include <glad/glad.h>

#include "assets.hpp"
#include "model.hpp"

namespace veil {

class Font {
    public:
        static constexpr int kAtlasW = 512;
        static constexpr int kAtlasH = 512;
        static constexpr unsigned char kFirstChar = 32;
        static constexpr size_t kNumChars = 96;

        Font() = delete;
        Font(const std::filesystem::path& path, float heightPx);

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        ~Font();

        std::span<const unsigned char> getBitmap() const { return m_bitmap; }
        std::span<const stbtt_bakedchar> getCharData() const { return m_charData; }
        GLuint getTextureID() const { if (m_texture) return m_texture->id; else return 0; }

    private:
        std::vector<unsigned char> m_bitmap;
        std::vector<stbtt_bakedchar> m_charData;
        std::unique_ptr<Texture> m_texture;

}; //class Font

class Text {
    public:
        Text(const Font& font);

        Text(const Text&) = delete;
        Text& operator=(const Text&) = delete;

        ~Text();

        void setText(const std::string& str);
        void render(int mode) const;

        inline const Mesh* const getMesh() const { return m_mesh.get(); }
        inline std::span<const Mesh> getMeshesRead() const { return m_mesh->getMeshesRead(); }

    private:
        const Font& m_font;
        std::unique_ptr<Mesh> m_mesh;
}; //class Text

}; //namespace vel