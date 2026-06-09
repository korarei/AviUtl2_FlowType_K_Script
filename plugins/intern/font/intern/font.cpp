#include "../font.hpp"

#include <stdexcept>

#include <intern/string.hpp>

namespace flow::font {
IDWriteFactory5 *
DWrite::factory() {
    auto *f = instance().handle.Get();
    if (f == nullptr)
        throw std::runtime_error("Failed to load DWrite factory");

    return f;
}

void
DWrite::reset() {
    instance().handle.Reset();
}

DWrite::DWrite() { DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), &handle); }

DWrite &
DWrite::instance() {
    static DWrite inst;
    return inst;
}

void
FontData::init(IDWriteFontFileLoader *loader, const void *key, uint32_t size, uint32_t index) {
    struct Context {
        ComPtr<IDWriteFontFileStream> stream;
        void *context;
    };

    if (face != nullptr)
        return;

    ComPtr<IDWriteFontFileStream> stream;
    const void *fragment = nullptr;
    void *context = nullptr;

    try {
        if (FAILED(loader->CreateStreamFromKey(key, size, &stream)))
            throw std::runtime_error("Failed to create font file stream");

        uint64_t file_size = 0ull;

        if (FAILED(stream->GetFileSize(&file_size)) || file_size == 0)
            throw std::runtime_error("Failed to get font file size");

        if (FAILED(stream->ReadFileFragment(&fragment, 0ull, file_size, &context)) || fragment == nullptr)
            throw std::runtime_error("Failed to read font file fragment");

        auto *ctx = new Context{std::move(stream), context};
        blob = HB_Blob(hb_blob_create(
                static_cast<const char *>(fragment),
                static_cast<uint32_t>(file_size),
                HB_MEMORY_MODE_READONLY,
                ctx,
                [](void *data) {
                    auto *ctx = static_cast<Context *>(data);
                    ctx->stream->ReleaseFileFragment(ctx->context);
                    delete ctx;
                }));

        if (blob == nullptr) {
            delete ctx;
            throw std::runtime_error("Failed to create harfbuzz blob");
        }

        fragment = nullptr;
        context = nullptr;

        face = HB_Face(hb_face_create(blob.get(), index));

        if (face == nullptr)
            throw std::runtime_error("Failed to create harfbuzz face");
    } catch (...) {
        if (stream != nullptr && fragment != nullptr)
            stream->ReleaseFileFragment(context);

        reset();
        throw;
    }
}

void
FontData::reset() {
    face.reset();
    blob.reset();
}

FontCache &
FontCache::instance() {
    static FontCache inst;
    return inst;
}

void
FontCache::init(const std::filesystem::path &path) {
    const auto factory = DWrite::factory();

    ComPtr<IDWriteFontSetBuilder1> builder;
    if (FAILED(factory->CreateFontSetBuilder(&builder)))
        return;

    ComPtr<IDWriteFontSet> system;
    if (SUCCEEDED(factory->GetSystemFontSet(&system)))
        builder->AddFontSet(system.Get());

    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            const auto &file = entry.path();
            const auto ext = file.extension();
            if (ext != L".ttf" && ext != L".otf" && ext != L".ttc" && ext != L".otc")
                continue;

            ComPtr<IDWriteFontFile> font;
            if (FAILED(factory->CreateFontFileReference(file.c_str(), nullptr, &font)))
                continue;

            builder->AddFontFile(font.Get());
        }
    }

    ComPtr<IDWriteFontSet> merged;
    if (FAILED(builder->CreateFontSet(&merged)))
        return;

    fonts = merged;
}

void
FontCache::deinit() {
    fonts.Reset();
    instance().cache::Cache<FontData, std::string>::reset();
}

void
FontCache::reset() {
    instance().cache::Cache<FontData, std::string>::reset();
}

HB_Font
FontCache::load(int64_t id, const std::string &name, bool is_bold, bool is_italic) {
    const uint8_t flag = (static_cast<uint8_t>(is_bold) << 1) | static_cast<uint8_t>(is_italic);
    auto entry = instance().fetch(id, name + char('0' + flag));

    if (entry->face == nullptr) {
        ComPtr<IDWriteFontFace3> face;
        if (!search(&face, string::to_wstring(string::as_utf8(name)), is_bold, is_italic))
            return nullptr;

        uint32_t count;
        if (FAILED(face->GetFiles(&count, nullptr)) || count == 0u)
            throw std::runtime_error("Failed to get font files count");

        if (count != 1u)
            return nullptr;

        ComPtr<IDWriteFontFile> file;
        if (FAILED(face->GetFiles(&count, &file)))
            throw std::runtime_error("Failed to get font file");

        const void *key;
        uint32_t size;
        if (FAILED(file->GetReferenceKey(&key, &size)))
            throw std::runtime_error("Failed to get font file reference key");

        ComPtr<IDWriteFontFileLoader> loader;
        if (FAILED(file->GetLoader(&loader)))
            throw std::runtime_error("Failed to get font file loader");

        entry->init(loader.Get(), key, size, face->GetIndex());
    }

    return HB_Font(hb_font_create(entry->face.get()));
}

bool
FontCache::search(IDWriteFontFace3 **face, const std::wstring &name, bool is_bold, bool is_italic) {
    if (fonts == nullptr) {
        const auto factory = DWrite::factory();
        if (FAILED(factory->GetSystemFontSet(fonts.ReleaseAndGetAddressOf())))
            throw std::runtime_error("Failed to get system font set");
    }

    ComPtr<IDWriteFontSet> matched;

    auto create_face = [&]() {
        ComPtr<IDWriteFontFaceReference> ref;
        if (FAILED(matched->GetFontFaceReference(0u, &ref)))
            throw std::runtime_error("Failed to get font face reference");

        if (FAILED(ref->CreateFontFace(face)))
            throw std::runtime_error("Failed to create font face");
    };

    DWRITE_FONT_PROPERTY prop = {
            .propertyId = DWRITE_FONT_PROPERTY_ID_FULL_NAME,
            .propertyValue = name.c_str(),
            .localeName = nullptr,
    };

    if (FAILED(fonts->GetMatchingFonts(&prop, 1u, matched.ReleaseAndGetAddressOf())))
        throw std::runtime_error("Failed to get matching fonts");

    if (matched->GetFontCount() > 0u) {
        create_face();
        return true;
    }

    DWRITE_FONT_PROPERTY props[] = {
            {
                    .propertyId = DWRITE_FONT_PROPERTY_ID_FAMILY_NAME,
                    .propertyValue = name.c_str(),
                    .localeName = nullptr,
            },
            {
                    .propertyId = DWRITE_FONT_PROPERTY_ID_WEIGHT,
                    .propertyValue = is_bold ? L"700" : L"400",
                    .localeName = nullptr,
            },
            {
                    .propertyId = DWRITE_FONT_PROPERTY_ID_STYLE,
                    .propertyValue = is_italic ? L"2" : L"0",
                    .localeName = nullptr,
            },
    };

    for (uint32_t i = 3u; i > 0u; --i) {
        if (FAILED(fonts->GetMatchingFonts(props, i, matched.ReleaseAndGetAddressOf())))
            throw std::runtime_error("Failed to get matching fonts");

        if (matched->GetFontCount() > 0u) {
            create_face();
            return true;
        }
    }

    props[0].propertyId = DWRITE_FONT_PROPERTY_ID_WIN32_FAMILY_NAME;

    for (uint32_t i = 3u; i > 0u; --i) {
        if (FAILED(fonts->GetMatchingFonts(props, i, matched.ReleaseAndGetAddressOf())))
            throw std::runtime_error("Failed to get matching fonts");

        if (matched->GetFontCount() > 0u) {
            create_face();
            return true;
        }
    }

    return false;
}
}  // namespace flow::font
