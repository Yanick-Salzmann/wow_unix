#ifndef WOW_UNIX_BINDABLE_TEXTURE_H
#define WOW_UNIX_BINDABLE_TEXTURE_H

#include <memory>

namespace wow::gl {
    class bindable_texture {
    public:
        virtual ~bindable_texture() = default;
        virtual void bind() = 0;
    };

    using bindable_texture_ptr = std::shared_ptr<bindable_texture>;
}

#endif //WOW_UNIX_BINDABLE_TEXTURE_H