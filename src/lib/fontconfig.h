#pragma once
#include <optional>
#include <string>
#include <vector>


namespace fontconfig {
struct FontFilter {
    std::optional<std::string> family, style, file;
};
struct Font {
    std::string family, style, file;
    std::string toString(const FontFilter& filter) const;
    bool operator==(const Font& other) const;
    bool operator!=(const Font& other) const;
};
class FontConfig {
    std::vector<Font> m_fonts;

   public:
    explicit FontConfig();
    std::vector<Font> filter(const FontFilter& f);
    bool hasFont(const FontFilter& f);

   private:
    void updateFontIndex();
};
}  // namespace fontconfig
