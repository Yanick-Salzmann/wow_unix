#ifndef WOW_UNIX_BLP_SCHEME_HANDLER_H
#define WOW_UNIX_BLP_SCHEME_HANDLER_H

#include "include/cef_scheme.h"

namespace wow::web::schemes {
    class blp_scheme_handler_factory final : public CefSchemeHandlerFactory {
        IMPLEMENT_REFCOUNTING(blp_scheme_handler_factory);

        class resource_handler final : public CefResourceHandler {
            IMPLEMENT_REFCOUNTING(resource_handler);

            std::vector<uint8_t> _data{};
            size_t _offset = 0;
            bool _found = false;

        public:
            bool Open(CefRefPtr<CefRequest> request, bool &handle_request, CefRefPtr<CefCallback> callback) override;

            void GetResponseHeaders(CefRefPtr<CefResponse> response, int64_t &response_length,
                                    CefString &redirectUrl) override;

            bool Skip(int64_t bytes_to_skip, int64_t &bytes_skipped,
                      CefRefPtr<CefResourceSkipCallback> callback) override;

            bool Read(void *data_out, int bytes_to_read, int &bytes_read,
                      CefRefPtr<CefResourceReadCallback> callback) override;

            bool ReadResponse(void *data_out, int bytes_to_read, int &bytes_read,
                              CefRefPtr<CefCallback> callback) override {
                return Read(data_out, bytes_to_read, bytes_read, nullptr);
            }

            bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override {
                bool handle_request = false;
                return Open(request, handle_request, callback);
            }

            void Cancel() override;
        };

    public:
        CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                             const CefString &scheme_name, CefRefPtr<CefRequest> request) override {
            return new resource_handler();
        }
    };
}

#endif //WOW_UNIX_BLP_SCHEME_HANDLER_H