#ifndef WOW_UNIX_TEXTURE_MANAGER_H
#define WOW_UNIX_TEXTURE_MANAGER_H

#include <memory>
#include <unordered_map>

#include "gpu_dispatcher.h"
#include "gl/texture.h"

namespace wow::scene {
    class texture_manager {
        std::mutex _texture_lock{};
        std::unordered_map<std::string, std::weak_ptr<gl::texture>> _texture_map{};

        gpu_dispatcher_ptr _dispatcher{};

        void unload_texture(const gl::texture *texture, const std::string& texture_name) const;

    public:
        texture_manager(gpu_dispatcher_ptr dispatcher);

        gl::texture_ptr load(const std::string& path);
    };

    using texture_manager_ptr = std::shared_ptr<texture_manager>;
}

#endif //WOW_UNIX_TEXTURE_MANAGER_H