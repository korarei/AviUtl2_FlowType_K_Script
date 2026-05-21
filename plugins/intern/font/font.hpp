#pragma once

#include <string>

#include <dwrite_3.h>
#include <wrl/client.h>

#include <hb.h>

#include <cache.hpp>

namespace flow::font {
enum ID : int64_t {
    TextSplit = -100ll,
};

struct HB_FontDeleter {
    void operator()(hb_font_t *font) const { hb_font_destroy(font); }
};

struct HB_BufferDeleter {
    void operator()(hb_buffer_t *buf) const { hb_buffer_destroy(buf); }
};

using HB_Font = std::unique_ptr<hb_font_t, HB_FontDeleter>;
using HB_Buffer = std::unique_ptr<hb_buffer_t, HB_BufferDeleter>;

class DWrite {
public:
    [[nodiscard]] static IDWriteFactory3 *get_factory();
    static void reset();

private:
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<IDWriteFactory3> factory;

    ~DWrite() { reset(); }
    DWrite();

    DWrite(const DWrite &) = delete;
    DWrite &operator=(const DWrite &) = delete;
    DWrite(DWrite &&) = delete;
    DWrite &operator=(DWrite &&) = delete;

    [[nodiscard]] static DWrite &instance();
};

class FontData {
public:
    struct HB_FaceDeleter {
        void operator()(hb_face_t *face) const { hb_face_destroy(face); }
    };

    using HB_Face = std::unique_ptr<hb_face_t, HB_FaceDeleter>;

    HB_Face face;

    FontData() = default;
    ~FontData() { reset(); }

    FontData(const FontData &) = delete;
    FontData &operator=(const FontData &) = delete;
    FontData(FontData &&) = delete;
    FontData &operator=(FontData &&) = delete;

    void init(IDWriteFontFileLoader *loader, const void *key, uint32_t size, uint32_t index);
    void reset();

private:
    struct HB_BlobDeleter {
        void operator()(hb_blob_t *blob) const { hb_blob_destroy(blob); }
    };

    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
    using HB_Blob = std::unique_ptr<hb_blob_t, HB_BlobDeleter>;

    ComPtr<IDWriteFontFileStream> stream = nullptr;
    const void *fragment = nullptr;
    void *context = nullptr;
    HB_Blob blob;
};

class FontCache : private cache::Cache<FontData, std::string> {
public:
    [[nodiscard]] static bool load(hb_face_t *&face, int64_t id, const std::string &name);
    static void reset();

private:
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ~FontCache() { reset(); }
    FontCache() = default;

    FontCache(const FontCache &) = delete;
    FontCache &operator=(const FontCache &) = delete;
    FontCache(FontCache &&) = delete;
    FontCache &operator=(FontCache &&) = delete;

    [[nodiscard]] static FontCache &instance();
    [[nodiscard]] static bool search(IDWriteFontFace3 **face, const std::wstring &name);
};
}  // namespace flow::font
