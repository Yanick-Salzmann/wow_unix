#ifndef WOW_UNIX_DI_H
#define WOW_UNIX_DI_H

#include <boost/di.hpp>

namespace wow::utils {
    namespace internal {
        class injector_holder {
        public:
            template<class injector>
            explicit injector_holder(injector ij) : _injector(std::make_shared<model<injector> >(std::move(ij))) {
            }

        private:
            class base {
            public:
                virtual ~base() = default;
            };

            template<class injector>
            class model final : public base {
            public:
                explicit model(injector ij) : _injector(std::move(ij)) {
                }

                injector _injector;
            };

            std::shared_ptr<base> _injector{};
        };

        extern std::shared_ptr<injector_holder> _injector;
    }

    void initialize_di();

    template<typename T>
    std::shared_ptr<T> create() {
        return internal::_injector->get<std::shared_ptr<T> >();
    }
}

#endif //WOW_UNIX_DI_H