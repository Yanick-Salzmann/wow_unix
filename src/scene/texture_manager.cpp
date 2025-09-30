#include "texture_manager.h"

#include "spdlog/spdlog.h"

#include "utils/string_utils.h"

namespace wow::scene {
    void texture_manager::unload_texture(const gl::texture *texture, const std::string &texture_name) const {
        if (texture != nullptr) {
            const auto native = texture->native();
            if (native) {
                _dispatcher->dispatch([native] {
                    glDeleteTextures(1, &native);
                });
            }
            delete texture;
        }
    }

    texture_manager::texture_manager(gpu_dispatcher_ptr dispatcher) : _dispatcher(std::move(dispatcher)) {
    }

    gl::texture_ptr texture_manager::load(const std::string &path) {
        {
            std::lock_guard lock(_texture_lock);

            const auto name = utils::to_lower(path);
            if (const auto it = _texture_map.find(name); it != _texture_map.end()) {
                if (const auto tex = it->second.lock()) {
                    return tex;
                }
            }

            const auto texture = std::shared_ptr<gl::texture>(new gl::texture(), [this, name](const gl::texture *ptr) {
                {
                    std::lock_guard inner_lock(_texture_lock);
                    _texture_map.erase(name);
                }

                unload_texture(ptr, name);
            });

            _texture_map[name] = texture;
            return texture;
        }
    }
}
