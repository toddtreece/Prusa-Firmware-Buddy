/**
 * \file
 *
 * Support for uploading gcode.
 */
#pragma once

#include "req_parser.h"
#include "status_page.h"
#include "types.h"
#include "upload_state.h"

#include <memory>
#include <optional>

struct Uploader;

namespace nhttp::printer {

/**
 * \brief A handler to accept incoming gcodes.
 *
 * This will take a POST request with valid form containing gcode and store it
 * onto the USB drive.
 */
class GcodeUpload final : private UploadHooks {
public:
    typedef void UploadedNotify(const char *name, bool start_print);

private:
    UploadState uploader;
    UploadedNotify *uploaded_notify;
    size_t size_rest;
    GcodeUpload(UploadState uploader, size_t size_rest);
    class FileDeleter {
    public:
        void operator()(FILE *f);
    };

    using FilePtr = std::unique_ptr<FILE, FileDeleter>;
    FilePtr tmp_upload_file;
    // A way how to reconstruct the name of the temporary file.
    size_t file_idx;

    virtual Result data(std::string_view data) override;
    virtual Result finish(const char *final_filename, bool start_print) override;

    void delete_file();
    GcodeUpload(UploadState uploader, size_t length, size_t upload_idx, FilePtr file, UploadedNotify *uploaded);

public:
    bool want_read() const { return size_rest > 0; }
    bool want_write() const { return false; }
    handler::Step step(std::string_view input, bool terminated_by_client, uint8_t *output, size_t output_size);
    using UploadResult = std::variant<handler::StatusPage, GcodeUpload>;
    static UploadResult start(const handler::RequestParser &parser, UploadedNotify *uploaded);
    GcodeUpload(const GcodeUpload &other) = delete;
    GcodeUpload(GcodeUpload &&other) = default;
    GcodeUpload &operator=(const GcodeUpload &other) = delete;
    GcodeUpload &operator=(GcodeUpload &&other) = default;
    ~GcodeUpload();
};

}
