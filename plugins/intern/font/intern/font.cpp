#include "font.hpp"

#include <stdexcept>

#include <utilities.hpp>

namespace flow::font {
IDWriteFactory3 *
DWrite::get_factory() {
    auto *f = instance().factory.Get();
    if (f == nullptr)
        throw std::runtime_error("Failed to load DWrite factory");

    return f;
}

void
DWrite::reset() {
    instance().factory.Reset();
}

DWrite::DWrite() { DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), &factory); }

DWrite &
DWrite::instance() {
    static DWrite inst;
    return inst;
}

void
FontData::init(IDWriteFontFileLoader *loader, const void *key, uint32_t size, uint32_t index) {
    if (face != nullptr)
        return;

    if (FAILED(loader->CreateStreamFromKey(key, size, &stream)))
        throw std::runtime_error("Failed to create font file stream");

    uint64_t file_size = 0ull;

    if (FAILED(stream->GetFileSize(&file_size)) || file_size == 0)
        throw std::runtime_error("Failed to get font file size");

    if (FAILED(stream->ReadFileFragment(&fragment, 0ull, file_size, &context)) || fragment == nullptr)
        throw std::runtime_error("Failed to read font file fragment");

    blob = HB_Blob(hb_blob_create(
            static_cast<const char *>(fragment),
            static_cast<uint32_t>(file_size),
            HB_MEMORY_MODE_READONLY,
            nullptr,
            nullptr));
    if (blob == nullptr)
        throw std::runtime_error("Failed to create harfbuzz blob");

    face = HB_Face(hb_face_create(blob.get(), index));
    if (face == nullptr)
        throw std::runtime_error("Failed to create harfbuzz face");
}

void
FontData::reset() {
    face.reset();
    blob.reset();

    if (stream != nullptr && fragment != nullptr) {
        stream->ReleaseFileFragment(context);
        fragment = nullptr;
        context = nullptr;
    }
}

FontCache &
FontCache::instance() {
    static FontCache inst;
    return inst;
}

void
FontCache::reset() {
    instance().cache::Cache<FontData, std::string>::reset();
}

bool
FontCache::load(hb_face_t *&face, int64_t id, const std::string &name) {
    auto &self = instance();
    auto entry = self.fetch(id, name);

    if (entry->face != nullptr) {
        face = entry->face.get();
        return true;
    }

    ComPtr<IDWriteFontFace3> dw_face;
    if (!search(&dw_face, string::to_wstring(string::as_utf8(name))))
        return false;

    uint32_t count;
    if (FAILED(dw_face->GetFiles(&count, nullptr)) || count == 0u)
        throw std::runtime_error("Failed to get font files count");

    if (count != 1u)
        return false;

    ComPtr<IDWriteFontFile> file;
    if (FAILED(dw_face->GetFiles(&count, &file)))
        throw std::runtime_error("Failed to get font file");

    const void *key;
    uint32_t size;
    if (FAILED(file->GetReferenceKey(&key, &size)))
        throw std::runtime_error("Failed to get font file reference key");

    ComPtr<IDWriteFontFileLoader> loader;
    if (FAILED(file->GetLoader(&loader)))
        throw std::runtime_error("Failed to get font file loader");

    entry->init(loader.Get(), key, size, dw_face->GetIndex());

    face = entry->face.get();
    return true;
}

bool
FontCache::search(IDWriteFontFace3 **face, const std::wstring &name) {
    const auto factory = DWrite::get_factory();

    ComPtr<IDWriteFontSet> font_set;
    if (FAILED(factory->GetSystemFontSet(&font_set)))
        throw std::runtime_error("Failed to get system font set");

    DWRITE_FONT_PROPERTY prop{
            .propertyId = DWRITE_FONT_PROPERTY_ID_FULL_NAME,
            .propertyValue = name.c_str(),
            .localeName = nullptr,
    };

    ComPtr<IDWriteFontSet> matched;
    if (FAILED(font_set->GetMatchingFonts(&prop, 1u, &matched)))
        throw std::runtime_error("Failed to get matching fonts");

    if (matched->GetFontCount() == 0u) {
        prop.propertyId = DWRITE_FONT_PROPERTY_ID_FAMILY_NAME;
        if (FAILED(font_set->GetMatchingFonts(&prop, 1u, matched.ReleaseAndGetAddressOf())))
            throw std::runtime_error("Failed to get matching fonts");

        if (matched->GetFontCount() == 0u) {
            prop.propertyId = DWRITE_FONT_PROPERTY_ID_WIN32_FAMILY_NAME;
            if (FAILED(font_set->GetMatchingFonts(&prop, 1u, matched.ReleaseAndGetAddressOf())))
                throw std::runtime_error("Failed to get matching fonts");

            if (matched->GetFontCount() == 0u)
                return false;
        }
    }

    ComPtr<IDWriteFontFaceReference> ref;
    if (FAILED(matched->GetFontFaceReference(0u, &ref)))
        throw std::runtime_error("Failed to get font face reference");

    if (FAILED(ref->CreateFontFace(face)))
        throw std::runtime_error("Failed to create font face");

    return true;
}
}  // namespace flow::font
