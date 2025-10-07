#include "texture_manager.h"

#include "io/blp/blp_file.h"
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

    texture_manager::texture_manager(
        io::mpq_manager_ptr mpq_manager,
        gpu_dispatcher_ptr dispatcher
    ) : _dispatcher(std::move(dispatcher)), _mpq_manager(std::move(mpq_manager)) {
    }

    gl::texture_ptr texture_manager::load(const std::string &path) {
        std::shared_ptr<gl::texture> texture;
        {
            std::lock_guard lock(_texture_lock);

            const auto name = utils::to_lower(path);
            if (const auto it = _texture_map.find(name); it != _texture_map.end()) {
                if (const auto tex = it->second.lock()) {
                    return tex;
                }
            }

            texture = std::shared_ptr<gl::texture>(new gl::texture(), [this, name](const gl::texture *ptr) {
                {
                    std::lock_guard inner_lock(_texture_lock);
                    _texture_map.erase(name);
                }

                unload_texture(ptr, name);
            });

            _texture_map[name] = texture;
        }

        if (auto file = _mpq_manager->open(path)) {
            auto blp = std::make_shared<io::blp::blp_file>(file);
            _dispatcher->dispatch([blp, texture] {
                texture->load_blp(blp);
                texture->filtering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
                texture->wrap(GL_REPEAT, GL_REPEAT);
            });
        }

        return texture;
    }
}
