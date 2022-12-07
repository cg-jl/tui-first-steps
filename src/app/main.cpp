#include <fontconfig.h>
#include <vt/handler.h>
#include <vt/input.h>
#include <vt/terminal.h>

#include <cctype>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

int main() {
  vt::Handler h;

  fontconfig::FontConfig fonts;

  std::vector<std::string> filtered;

  std::size_t selected = 0;

  const vt::byte fg = 33;
  const vt::byte bg = 0;

  fontconfig::FontFilter currentFilter;

  enum class Mode {
    FILTER = 0,
    LIST = 1,
  };

  Mode mode = Mode::FILTER; // input filter

  const auto &size = vt::getSize();
  const auto &initialPos = vt::getCursorPos();

  const auto width = size.x;
  const auto height = size.y;

  std::vector<char> filter;

  const auto distance = height - initialPos.y;

  auto updateFilter = [&]() {
    currentFilter.family = "";
    for (const char &c : filter)
      *currentFilter.family += c;
    filtered.clear();
    for (const auto &f : fonts.filter(currentFilter)) {
      filtered.push_back(f.toString(currentFilter));
    }
    selected = 0;
  };
  updateFilter();

  h.on(
      vt::Handler::Event::DRAW,
      (vt::Handler::callback_bool)[](vt::Handler & handler)->bool {
        vt::cursor(false);
        return true;
      });

  // quit handler
  h.on(
      vt::Handler::Event::KEY,
      (vt::Handler::callback_bool)[&](vt::Handler & handler)->bool {
        vt::byte key = handler.getLastKey();
        if (key == vt::input::control('q')) {
          handler.quit();
          return false;
        }
        return true;
      });

  // switch mode handler
  h.on(
      vt::Handler::Event::KEY,
      (vt::Handler::callback_bool)[&](vt::Handler & handler) {
        vt::byte key = handler.getLastKey();
        if (key == '\t') {
          mode = mode == Mode::FILTER ? Mode::LIST : Mode::FILTER;
          return false;
        }
        return true; // fallthrough
      });

  // filter mode
  h.on(
      vt::Handler::Event::KEY,
      (vt::Handler::callback_bool)[&](vt::Handler & handler)->bool {
        if (mode != Mode::FILTER)
          return true;
        char key = (char)handler.getLastKey();

        if (key == vt::input::control('q')) {
          handler.quit();
        }
        if (key == 0x7f && filter.size() > 0) {
          filter.pop_back();
          updateFilter();
        }

        if (std::isprint(key)) {
          filter.push_back(key);
          updateFilter();
        }
        return false;
      });

  h.on(vt::Handler::Event::DRAW, [&](vt::Handler &handler) {
    vt::moveTo(initialPos);
    vt::eraseToEnd();
    for (const char &c : filter)
      vt::putc(c);
    vt::move(vt::Direction::RIGHT, 1);
    vt::lineDown();
  });

  // filter
  h.on(vt::Handler::Event::KEY, [&](vt::Handler &handler) {
    if (mode != Mode::LIST)
      return;
    vt::byte key = handler.getLastKey();
    if (key == 'j') {
      if (selected < filtered.size())
        ++selected;
    }
    if (key == 'k') {
      if (selected > 0)
        --selected;
    }
    if (key == 'G') {
      selected = filtered.size() - 1;
    }
    if (key == 'H')
      selected = 0;
    if (key == '\r')
      handler.quit(); // selected
  });

  const auto &drawSelected = [&]() {
    vt::clearColor();
    vt::bold();
    vt::setBg(fg);
    if (filtered[selected].c_str())
      vt::puts(filtered[selected].c_str());
    vt::repeat(' ', width - filtered[selected].size());
    vt::clearColor();
  };

  h.on(
      vt::Handler::Event::DRAW,
      (vt::Handler::callback_bool)[&](vt::Handler & handler) mutable->bool {
        if (filtered.size() == 0)
          return true;
        vt::cursor(false);
        vt::putc('\r');
        vt::clearColor();
        vt::eraseToEnd();

        if (selected <= distance) {
          vt::clearColor();
          vt::setFg(fg);
          for (std::size_t i = 0; i < selected; ++i) {
            vt::puts(filtered[i].c_str());
            vt::lineDown();
          }

          drawSelected();
          vt::lineDown();

          vt::clearColor();
          vt::setFg(fg);
          for (auto it = filtered.begin() + selected + 1;
               it != filtered.begin() + distance; ++it) {
            vt::puts(it->c_str());
            vt::lineDown();
          }
          return true;
        } else {
          vt::moveTo(1, height);
          drawSelected();
          vt::lineUp();

          // pos is 1-based
          vt::clearColor();
          vt::setFg(fg);
          for (auto it = filtered.begin() + selected;
               it != filtered.begin() + selected - distance; --it) {
            vt::puts(it->c_str());
            vt::lineUp();
          }
          return false;
        }
      });

  h.on(vt::Handler::Event::EXIT, [&](vt::Handler &handler) {
    vt::clearColor();
    vt::cursor(true);
    vt::moveTo(initialPos);
    vt::eraseToEnd();
    vt::flushOut();
  });
  // draw cursor on filter if it is seen
  h.on(vt::Handler::Event::DRAW, [&](vt::Handler &handler) {
    vt::moveTo(filter.size() % width, initialPos.y + filter.size() / width);
    vt::cursor(true);
  });

  h.initLoop();

  std::printf("Selected: %s\n", filtered[selected].c_str());

  return 0;
}
