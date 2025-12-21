#include "web_core.h"

#include "include/cef_browser.h"
#include <filesystem>
#include <utility>
#include <gtk/gtk.h>

#include "schemes/app_scheme_handler.h"
#include "spdlog/spdlog.h"
#include "windows_virtual_keys.h"
#include "event/shell_events.h"
#include "schemes/blp_scheme_handler.h"
#include "schemes/minimap_scheme_handler.h"

namespace wow::web {
    int map_glfw_key_to_virtual_key(const int key) {
        switch (key) {
            case GLFW_KEY_LEFT_ALT:
                return VK_LMENU;
            case GLFW_KEY_RIGHT_ALT:
                return VK_RMENU;
            case GLFW_KEY_LEFT_CONTROL:
                return VK_LCONTROL;
            case GLFW_KEY_RIGHT_CONTROL:
                return VK_RCONTROL;
            case GLFW_KEY_LEFT_SHIFT:
                return VK_LSHIFT;
            case GLFW_KEY_RIGHT_SHIFT:
                return VK_RSHIFT;
            case GLFW_KEY_LEFT_SUPER:
                return VK_LWIN;
            case GLFW_KEY_RIGHT_SUPER:
                return VK_RWIN;
            case GLFW_KEY_ESCAPE:
                return VK_ESCAPE;
            case GLFW_KEY_ENTER:
                return VK_RETURN;
            case GLFW_KEY_TAB:
                return VK_TAB;
            case GLFW_KEY_BACKSPACE:
                return VK_BACK;
            case GLFW_KEY_INSERT:
                return VK_INSERT;
            case GLFW_KEY_DELETE:
                return VK_DELETE;
            case GLFW_KEY_RIGHT:
                return VK_RIGHT;
            case GLFW_KEY_LEFT:
                return VK_LEFT;
            case GLFW_KEY_DOWN:
                return VK_DOWN;
            case GLFW_KEY_UP:
                return VK_UP;
            case GLFW_KEY_PAGE_UP:
                return VK_PRIOR;
            case GLFW_KEY_PAGE_DOWN:
                return VK_NEXT;
            case GLFW_KEY_HOME:
                return VK_HOME;
            case GLFW_KEY_END:
                return VK_END;
            case GLFW_KEY_CAPS_LOCK:
                return VK_CAPITAL;
            case GLFW_KEY_SCROLL_LOCK:
                return VK_SCROLL;
            case GLFW_KEY_NUM_LOCK:
                return VK_NUMLOCK;
            case GLFW_KEY_PRINT_SCREEN:
                return VK_SNAPSHOT;
            case GLFW_KEY_PAUSE:
                return VK_PAUSE;
            case GLFW_KEY_MENU:
                return VK_APPS;
            case GLFW_KEY_F1:
                return VK_F1;
            case GLFW_KEY_F2:
                return VK_F2;
            case GLFW_KEY_F3:
                return VK_F3;
            case GLFW_KEY_F4:
                return VK_F4;
            case GLFW_KEY_F5:
                return VK_F5;
            case GLFW_KEY_F6:
                return VK_F6;
            case GLFW_KEY_F7:
                return VK_F7;
            case GLFW_KEY_F8:
                return VK_F8;
            case GLFW_KEY_F9:
                return VK_F9;
            case GLFW_KEY_F10:
                return VK_F10;
            case GLFW_KEY_F11:
                return VK_F11;
            case GLFW_KEY_F12:
                return VK_F12;
            case GLFW_KEY_F13:
                return VK_F13;
            case GLFW_KEY_F14:
                return VK_F14;
            case GLFW_KEY_F15:
                return VK_F15;
            case GLFW_KEY_F16:
                return VK_F16;
            case GLFW_KEY_F17:
                return VK_F17;
            case GLFW_KEY_F18:
                return VK_F18;
            case GLFW_KEY_F19:
                return VK_F19;
            case GLFW_KEY_F20:
                return VK_F20;
            case GLFW_KEY_F21:
                return VK_F21;
            case GLFW_KEY_F22:
                return VK_F22;
            case GLFW_KEY_F23:
                return VK_F23;
            case GLFW_KEY_F24:
                return VK_F24;
            case GLFW_KEY_KP_0:
                return VK_NUMPAD0;
            case GLFW_KEY_KP_1:
                return VK_NUMPAD1;
            case GLFW_KEY_KP_2:
                return VK_NUMPAD2;
            case GLFW_KEY_KP_3:
                return VK_NUMPAD3;
            case GLFW_KEY_KP_4:
                return VK_NUMPAD4;
            case GLFW_KEY_KP_5:
                return VK_NUMPAD5;
            case GLFW_KEY_KP_6:
                return VK_NUMPAD6;
            case GLFW_KEY_KP_7:
                return VK_NUMPAD7;
            case GLFW_KEY_KP_8:
                return VK_NUMPAD8;
            case GLFW_KEY_KP_9:
                return VK_NUMPAD9;
            case GLFW_KEY_KP_DECIMAL:
                return VK_DECIMAL;
            case GLFW_KEY_KP_DIVIDE:
                return VK_DIVIDE;
            case GLFW_KEY_KP_MULTIPLY:
                return VK_MULTIPLY;
            case GLFW_KEY_KP_SUBTRACT:
                return VK_SUBTRACT;
            case GLFW_KEY_KP_ADD:
                return VK_ADD;
            case GLFW_KEY_KP_ENTER:
                return VK_RETURN;
            case GLFW_KEY_KP_EQUAL:
                return VK_OEM_PLUS;
            case GLFW_KEY_APOSTROPHE:
                return 0xDE;
            case GLFW_KEY_COMMA:
                return VK_OEM_COMMA;
            case GLFW_KEY_MINUS:
                return VK_OEM_MINUS;
            case GLFW_KEY_PERIOD:
                return VK_OEM_PERIOD;
            case GLFW_KEY_SLASH:
                return VK_OEM_2;
            case GLFW_KEY_SEMICOLON:
                return VK_OEM_1;
            case GLFW_KEY_EQUAL:
                return VK_OEM_PLUS;
            case GLFW_KEY_LEFT_BRACKET:
                return VK_OEM_4;
            case GLFW_KEY_BACKSLASH:
                return VK_OEM_5;
            case GLFW_KEY_RIGHT_BRACKET:
                return VK_OEM_6;
            case GLFW_KEY_GRAVE_ACCENT:
                return VK_OEM_3;
            case GLFW_KEY_SPACE:
                return VK_SPACE;
            case GLFW_KEY_0:
                return '0';
            case GLFW_KEY_1:
                return '1';
            case GLFW_KEY_2:
                return '2';
            case GLFW_KEY_3:
                return '3';
            case GLFW_KEY_4:
                return '4';
            case GLFW_KEY_5:
                return '5';
            case GLFW_KEY_6:
                return '6';
            case GLFW_KEY_7:
                return '7';
            case GLFW_KEY_8:
                return '8';
            case GLFW_KEY_9:
                return '9';
            case GLFW_KEY_A:
                return 'A';
            case GLFW_KEY_B:
                return 'B';
            case GLFW_KEY_C:
                return 'C';
            case GLFW_KEY_D:
                return 'D';
            case GLFW_KEY_E:
                return 'E';
            case GLFW_KEY_F:
                return 'F';
            case GLFW_KEY_G:
                return 'G';
            case GLFW_KEY_H:
                return 'H';
            case GLFW_KEY_I:
                return 'I';
            case GLFW_KEY_J:
                return 'J';
            case GLFW_KEY_K:
                return 'K';
            case GLFW_KEY_L:
                return 'L';
            case GLFW_KEY_M:
                return 'M';
            case GLFW_KEY_N:
                return 'N';
            case GLFW_KEY_O:
                return 'O';
            case GLFW_KEY_P:
                return 'P';
            case GLFW_KEY_Q:
                return 'Q';
            case GLFW_KEY_R:
                return 'R';
            case GLFW_KEY_S:
                return 'S';
            case GLFW_KEY_T:
                return 'T';
            case GLFW_KEY_U:
                return 'U';
            case GLFW_KEY_V:
                return 'V';
            case GLFW_KEY_W:
                return 'W';
            case GLFW_KEY_X:
                return 'X';
            case GLFW_KEY_Y:
                return 'Y';
            case GLFW_KEY_Z:
                return 'Z';

            default:
                return key;
        }
    }

    void web_core::on_paint(const int32_t width, const int32_t height, const void *data) {
        std::lock_guard lock{_image_lock};
        const auto ptr = static_cast<const uint8_t *>(data);
        _image_data.assign(ptr, ptr + width * height * 4);
        _width = width;
        _height = height;
        _is_dirty = true;
    }

    web_core::web_core(
        gl::window_ptr window,
        event::event_manager_ptr event_manager
    ) : _event_manager(std::move(event_manager)),
        _window(std::move(window)) {
        if (!_window) {
            throw std::runtime_error("Window is null");
        }
        _texture = gl::make_texture();
        _mesh = gl::mesh::create_ui_quad();
        _texture_uniform = _mesh->program()->uniform_location("ui_texture");

        _mesh->texture(_texture_uniform, _texture)
                .blend(gl::blend_mode::alpha);
    }

    void web_core::initialize(int argc, char *argv[]) {
        _task = std::packaged_task<bool()>([argc, argv, this] {
            SPDLOG_INFO("Initializing CEF"); // NOLINT(bugprone-lambda-function-name)
            CefMainArgs args{};
            args.argc = argc;
            args.argv = argv;

            CefSettings settings{};
            CefString(&settings.browser_subprocess_path) = canonical(std::filesystem::absolute("./wow_unix_browser"));
            CefString(&settings.locales_dir_path) = canonical(std::filesystem::absolute("./locales"));
            CefString(&settings.resources_dir_path) = canonical(std::filesystem::absolute("./"));
            CefString(&settings.cache_path) = canonical(std::filesystem::absolute("./cache"));

            settings.no_sandbox = true;
            settings.windowless_rendering_enabled = true;
            settings.remote_debugging_port = 9222;
            CefString(&settings.locale) = "de_CH";
            CefString(&settings.accept_language_list) = "de-CH";

            _application = new web_application();
            _client = new web_client(_window, shared_from_this());

            if (!CefInitialize(args, settings, _application, nullptr)) {
                SPDLOG_ERROR("Could not initialize CEF"); // NOLINT(bugprone-lambda-function-name)
                return false;
            }

            event::initialize_shell_events(_event_manager);

            CefRegisterSchemeHandlerFactory("app", "", new schemes::app_scheme_handler_factory());
            CefRegisterSchemeHandlerFactory("blp", "", new schemes::blp_scheme_handler_factory());
            CefRegisterSchemeHandlerFactory("minimap", "", new schemes::minimap_scheme_handler_factory());

            CefWindowInfo window_info{};
            window_info.SetAsWindowless(_window->handle());

            CefBrowserSettings browser_settings{};
            browser_settings.windowless_frame_rate = 60;
            browser_settings.background_color = CefColorSetARGB(0, 255, 255, 255);
            browser_settings.javascript_access_clipboard = STATE_ENABLED;
            browser_settings.local_storage = STATE_ENABLED;

            CefBrowserHost::CreateBrowser(window_info, _client, "app://localhost/index.html", browser_settings, nullptr,
                                          nullptr);

            return true;
        });

        _message_loop = std::thread([this]() {
            _task();
            CefRunMessageLoop();
        });

        if (!_task.get_future().get()) {
            throw std::runtime_error("Could not initialize CEF");
        }

        _window->add_mouse_button_callback([this](const int button, const int action, const int mods) {
            cef_mouse_button_type_t button_type;
            switch (button) {
                case GLFW_MOUSE_BUTTON_LEFT:
                    button_type = MBT_LEFT;
                    break;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    button_type = MBT_RIGHT;
                    break;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    button_type = MBT_MIDDLE;
                    break;
                default:
                    button_type = MBT_LEFT;
                    break;
            }

            if (const auto browser = _client->browser()) {
                CefMouseEvent event{};
                event.x = static_cast<int>(_mouse_x);
                event.y = static_cast<int>(_mouse_y);
                event.modifiers = calculate_modifiers();

                if (action == GLFW_PRESS) {
                    browser->GetHost()->SetFocus(true);
                    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
                    const auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
                    const auto cancelPreviousClick =
                            std::abs(_last_click_x - event.x) > 36.0f / 2 ||
                            std::abs(_last_click_y - event.y) > 36.0f / 2 ||
                            currentTime - _last_click_time > 600;

                    if (cancelPreviousClick) {
                        _last_click_count = 0;
                        _last_click_x = 0;
                        _last_click_y = 0;
                        _last_click_time = 0;
                    }

                    if (!cancelPreviousClick && _last_click_button == button) {
                        ++_last_click_count;
                    } else {
                        _last_click_count = 1;
                        _last_click_x = event.x;
                        _last_click_y = event.y;
                    }

                    _last_click_time = currentTime;
                    _last_click_button = button;

                    browser->GetHost()->SendMouseClickEvent(event, button_type, false, _last_click_count);
                } else if (action == GLFW_RELEASE) {
                    browser->GetHost()->SendMouseClickEvent(event, button_type, true, 1);
                }
            }
        });

        _window->add_mouse_move_callback([this](const double x, const double y) {
            _mouse_x = x;
            _mouse_y = y;

            if (const auto browser = _client->browser()) {
                CefMouseEvent event{};
                event.x = static_cast<int>(x);
                event.y = static_cast<int>(y);
                event.modifiers = calculate_modifiers();
                browser->GetHost()->SendMouseMoveEvent(event, false);
            }
        });

        _window->add_char_callback([this](const unsigned int codepoint) {
            if (const auto browser = _client->browser()) {
                CefKeyEvent event{};
                event.type = KEYEVENT_CHAR;
                event.character = codepoint;
                event.unmodified_character = codepoint;
                event.modifiers = calculate_modifiers();
                browser->GetHost()->SendKeyEvent(event);
            }
        });

        _window->add_key_callback([this](const int key, const int scancode, const int action, const int mods) {
            if (const auto browser = _client->browser()) {
                CefKeyEvent event{};
                event.type = action == GLFW_PRESS ? KEYEVENT_KEYDOWN : KEYEVENT_KEYUP;
                event.windows_key_code = map_glfw_key_to_virtual_key(key);
                event.native_key_code = scancode;
                event.modifiers = calculate_modifiers();
                browser->GetHost()->SendKeyEvent(event);
            }
        });

        _window->add_scroll_callback([this](const double x, const double y) {
            if (const auto browser = _client->browser()) {
                CefMouseEvent event{};
                event.x = static_cast<int>(_mouse_x);
                event.y = static_cast<int>(_mouse_y);
                event.modifiers = calculate_modifiers();
                browser->GetHost()->SendMouseWheelEvent(event, static_cast<int>(x * 120), static_cast<int>(y * 120));
            }
        });

        _window->add_resize_callback([this](const int width, const int height) {
            if (const auto browser = _client->browser()) {
                browser->GetHost()->WasResized();
                browser->GetHost()->NotifyMoveOrResizeStarted();
            }
        });
    }

    void web_core::shutdown() {
        _client.reset();
        _application.reset();

        class ShutdownTask : public CefTask {
            IMPLEMENT_REFCOUNTING(ShutdownTask);

        public:
            ShutdownTask() = default;

            ~ShutdownTask() override = default;

            void Execute() override {
                CefQuitMessageLoop();
            }
        };

        CefPostTask(TID_UI, new ShutdownTask());
        _message_loop.join();
        CefShutdown();
        spdlog::info("Shutdown complete");
    }

    void web_core::render() {
        /*if (const auto browser = _client->browser()) {
            browser->GetHost()->SendExternalBeginFrame();
        }*/

        update_texture();
        if (_has_loaded) {
            _mesh->draw();
        }
    }

    void web_core::update_texture() {
        if (!_is_dirty) {
            return;
        }

        std::lock_guard lock{_image_lock};
        _texture->bgra_image(_width, _height, _image_data.data());
        _is_dirty = false;
        _has_loaded = true;
    }

    int web_core::calculate_modifiers() const {
        int32_t modifiers = 0;
        if (_window->is_key_pressed(GLFW_KEY_LEFT_ALT) || _window->is_key_pressed(GLFW_KEY_RIGHT_ALT)) {
            modifiers |= EVENTFLAG_ALT_DOWN;
        }

        if (_window->is_key_pressed(GLFW_KEY_LEFT_CONTROL) || _window->is_key_pressed(GLFW_KEY_RIGHT_CONTROL)) {
            modifiers |= EVENTFLAG_CONTROL_DOWN;
        }

        if (_window->is_key_pressed(GLFW_KEY_LEFT_SHIFT) || _window->is_key_pressed(GLFW_KEY_RIGHT_SHIFT)) {
            modifiers |= EVENTFLAG_SHIFT_DOWN;
        }

        if (_window->is_key_pressed(GLFW_KEY_LEFT_SUPER)) {
            modifiers |= EVENTFLAG_COMMAND_DOWN;
        }

        if (_window->is_key_pressed(GLFW_KEY_CAPS_LOCK)) {
            modifiers |= EVENTFLAG_CAPS_LOCK_ON;
        }

        if (_window->is_key_pressed(GLFW_KEY_NUM_LOCK)) {
            modifiers |= EVENTFLAG_NUM_LOCK_ON;
        }

        if (_window->is_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT)) {
            modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
        }

        if (_window->is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
        }

        if (_window->is_mouse_button_pressed(GLFW_MOUSE_BUTTON_MIDDLE)) {
            modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
        }

        return modifiers;
    }
}
