#pragma once

#include <string>

class Texture
{
public:
    static Texture LoadFromFile(const std::string& path);
    static Texture Empty() { return Texture(0); }

    Texture() = delete;
    ~Texture();

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void Use() const;

private:
    explicit Texture(unsigned int texture) : m_texture(texture) { }

    unsigned int m_texture;
};
