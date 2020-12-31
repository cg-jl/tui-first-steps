#include "handler.h"

#include "terminal.h"

namespace vt {
void Handler::initLoop() {
    vt::init();
    while (!m_quit) {
        const auto& opt = vt::readByte();
        if (opt) {
            m_lastKey = *opt;
            fireEvent(Event::KEY);
        }
        fireEvent(Event::DRAW);
        vt::flushOut();  // only flush at end of draw
    }
    fireEvent(Event::EXIT);
    vt::end();
}

void Handler::quit() { m_quit = true; }

void Handler::fireEvent(const Event& evt) {
    for (const auto& c : m_handlers[(unsigned int)evt])
        if (!c(*this)) break;
}

void Handler::on(const Event& evt, Handler::callback_bool c) {
    m_handlers[(unsigned int)evt].push_back(c);
}

void Handler::on(const Event& evt, std::function<void(Handler&)> c) {
    m_handlers[(unsigned int)evt].push_back([=](Handler& h) {
        c(h);
        return true;
    });
}

}  // namespace vt