#include "minimap_scheme_handler.h"

#include "spdlog/spdlog.h"

namespace wow::web::schemes {
    bool minimap_scheme_handler_factory::resource_handler::Open(CefRefPtr<CefRequest> request, bool &handle_request,
                                                                CefRefPtr<CefCallback> callback) {
        SPDLOG_INFO("Request received: {}", request->GetURL().ToString());

        return true;
    }

    void minimap_scheme_handler_factory::resource_handler::GetResponseHeaders(CefRefPtr<CefResponse> response,
        int64_t &response_length, CefString &redirectUrl) {
        response->SetStatus(404);
    }

    bool minimap_scheme_handler_factory::resource_handler::Skip(int64_t bytes_to_skip, int64_t &bytes_skipped,
        CefRefPtr<CefResourceSkipCallback> callback) {
        bytes_skipped = 0;
        return false;
    }

    bool minimap_scheme_handler_factory::resource_handler::Read(void *data_out, int bytes_to_read, int &bytes_read,
        CefRefPtr<CefResourceReadCallback> callback) {
        bytes_read = 0;
        return false;
    }

    void minimap_scheme_handler_factory::resource_handler::Cancel() {

    }
}
