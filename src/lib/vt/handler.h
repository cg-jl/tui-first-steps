#pragma once
#include <functional>
#include <vector>

namespace vt {
using byte = unsigned char;
class Handler {
   public:
    using callback_bool = std::function<bool(vt::Handler&)>;
    using callback_void = std::function<void(vt::Handler&)>;
    enum class Event {
        KEY = 0,
        DRAW,
        EXIT,
    };

   private:
    std::vector<callback_bool> m_handlers[3];
    bool m_quit = false;
    byte m_lastKey;

   public:
    void initLoop();
    void quit();
    void fireEvent(const Event& evt);
    void on(const Event& evt, callback_bool c);
    void on(const Event& evt, callback_void f);

   public:
    byte getLastKey() const;
};

inline byte Handler::getLastKey() const { return m_lastKey; }
}  // namespace vt