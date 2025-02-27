/**
 * \file
 */
#pragma once

#include "types.h"

#include <string_view>
#include <cstdio>
#include <memory>
#include <optional>

namespace nhttp::handler {

/**
 * \brief Handler to send a body from static memory.
 *
 * This sends a complete response where the body is provided as a constant. The
 * data provided is not copied anywhere and it is assumed the data is alive for
 * the whole lifetime of the handler (usually a constant in the program). This
 * allows "embedding" small/static resources directly into the program.
 */
class SendFile {
private:
    class FileDeleter {
    public:
        void operator()(FILE *f) {
            fclose(f);
        }
    };

    std::unique_ptr<FILE, FileDeleter> file;
    ContentType content_type;
    ConnectionHandling connection_handling = ConnectionHandling::Close;
    bool can_keep_alive;
    bool headers_sent = false;
    bool etag_matches = false;
    std::optional<size_t> content_length;
    std::optional<uint32_t> etag;
    const char **extra_hdrs;

public:
    SendFile(FILE *file, const char *path, ContentType content_type, bool can_keep_alive, uint32_t if_none_match, const char **extra_hdrs = nullptr);
    Step step(std::string_view input, bool terminated_by_client, uint8_t *buffer, size_t buffer_size);
    bool want_write() const { return bool(file); }
    bool want_read() const { return false; }
};

}
