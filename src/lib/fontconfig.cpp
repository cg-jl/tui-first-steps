#include "fontconfig.h"

#include <fontconfig/fontconfig.h>
#include <stdlib.h>  // free()

#include <sstream>

#include "searching.h"

namespace fontconfig {

FontConfig::FontConfig() { updateFontIndex(); }

void FontConfig::updateFontIndex() {
    m_fonts.clear();

    // fetch config
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcPatternCreate();
    FcObjectSet* os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FILE, nullptr);

    FcFontSet* fs = FcFontList(config, pat, os);

    if (fs) {
        m_fonts.reserve(fs->nfont);
        for (int i = 0; i < fs->nfont; ++i) {
            FcPattern* font = fs->fonts[i];
            FcChar8 *file, *style, *family;
            if (FcPatternGetString(font, FC_FAMILY, 0, &family) ==
                    FcResultMatch &&
                FcPatternGetString(font, FC_STYLE, 0, &style) ==
                    FcResultMatch &&
                FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
                m_fonts.push_back(Font{
                    std::string((const char*)family),
                    std::string((const char*)style),
                    std::string((const char*)file),
                });
            }
        }
        free(fs);
    }
}

std::vector<Font> FontConfig::filter(const FontFilter& filter) {
    std::vector<Font> fonts;
    fonts.reserve(m_fonts.size());

    for (const auto& f : m_fonts) {
        if (filter.family && fuzzyFind(*filter.family, f.family) ||
            filter.style && fuzzyFind(*filter.style, f.style) ||
            filter.file && fuzzyFind(*filter.file, f.file))
            fonts.push_back(f);
    }
    return fonts;
}

// iterators2::Iterator<Font, iterators2::backend::Filter<Font>>
// FontConfig::filter(const FontFilter& filter) {
//     return iterators2::Iterator<Font, iterators2::backend::Filter<Font>>(
//                m_fonts)
//         .filter([&](const Font& f) {
//             if (filter.family && fuzzyFind(*filter.family, f.family))
//                 return true;
//             if (filter.style && fuzzyFind(*filter.style, f.style)) return
//             true; if (filter.file && fuzzyFind(*filter.file, f.file)) return
//             true; if (filter.language && fuzzyFind(*filter.language,
//             f.language))
//                 return true;

//             return false;
//         });
// }

std::string cut(const std::string& src, unsigned int nChars = 10) {
    if (src.size() <= nChars) return src;
    return src.substr(0, nChars) + "...";
}

std::string Font::toString(const FontFilter& filter) const {
    std::ostringstream ss;
    if (filter.family) ss << family << ' ';
    if (filter.file) ss << cut(file, 11) << ' ';
    if (filter.style) ss << style << ' ';
    return ss.str();
}

bool Font::operator==(const Font& other) const {
    return (fuzzyFind(family, other.family) && fuzzyFind(style, other.style) &&
            fuzzyFind(file, other.file));
}

bool Font::operator!=(const Font& other) const { return !(*this == other); }

}  // namespace fontconfig