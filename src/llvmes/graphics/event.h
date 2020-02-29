#pragma once

#include <utility>

// An event system with a hierarchy, all event classes inherit from
// Event and has to provide a way to get its type. Event is a purely virtual
// class which means it cannot be instantiated itself, also called an interface

namespace llvmes::gfx {

enum class EventType {
    WindowResizeEvent,
    WindowCloseEvent,
    KeyPressEvent,
    KeyReleaseEvent,
    KeyRepeatEvent,
    KeyTypeEvent,
    MouseDownEvent,
    MouseReleaseEvent,
    MouseScrollEvent,
    MouseMoveEvent
};

class Event {
   public:
    virtual ~Event() = default;
    virtual EventType GetEventType() = 0;
};

class WindowResizeEvent : public Event {
   public:
    WindowResizeEvent(int width, int height) : m_width(width), m_height(height)
    {
    }

    EventType GetEventType() override { return EventType::WindowResizeEvent; }

    auto GetNewSize() { return std::pair{m_width, m_height}; }

   private:
    int m_width, m_height;
};

class WindowCloseEvent : public Event {
   public:
    EventType GetEventType() override { return EventType::WindowCloseEvent; }

   private:
};

class KeyPressEvent : public Event {
   public:
    explicit KeyPressEvent(int keycode) : m_keycode(keycode) {}
    EventType GetEventType() override { return EventType::KeyPressEvent; }
    int GetKeyCode() { return m_keycode; }

   private:
    int m_keycode;
};

class KeyReleaseEvent : public Event {
   public:
    explicit KeyReleaseEvent(int keycode) : m_keycode(keycode) {}
    EventType GetEventType() override { return EventType::KeyReleaseEvent; }
    int GetKeyCode() { return m_keycode; }

   private:
    int m_keycode;
};

class KeyRepeatEvent : public Event {
   public:
    explicit KeyRepeatEvent(int keycode) : m_keycode(keycode) {}
    EventType GetEventType() override { return EventType::KeyRepeatEvent; }

   private:
    int m_keycode;
};

class KeyTypeEvent : public Event {
   public:
    explicit KeyTypeEvent(int keycode) : m_keycode(keycode) {}
    EventType GetEventType() override { return EventType::KeyTypeEvent; }
    int GetKeyCode() { return m_keycode; }

   private:
    int m_keycode;
};

class MouseDownEvent : public Event {
   public:
    explicit MouseDownEvent(int code) : m_code(code) {}
    EventType GetEventType() override { return EventType::MouseDownEvent; }
    int GetButtonCode() { return m_code; }

   private:
    int m_code;
};

class MouseReleaseEvent : public Event {
   public:
    explicit MouseReleaseEvent(int code) : m_code(code) {}
    EventType GetEventType() override { return EventType::MouseReleaseEvent; }
    int GetButtonCode() { return m_code; }

   private:
    int m_code;
};

class MouseMoveEvent : public Event {
   public:
    MouseMoveEvent(float x, float y) : m_x(x), m_y(y) {}
    EventType GetEventType() override { return EventType::MouseMoveEvent; }
    auto GetPosition() { return std::pair{m_x, m_y}; }

   private:
    float m_x, m_y;
};

class MouseScrollEvent : public Event {
   public:
    MouseScrollEvent(int x_offset, int y_offset)
        : m_x_offset(x_offset), m_y_offset(y_offset)
    {
    }
    EventType GetEventType() override { return EventType::MouseScrollEvent; }
    auto GetOffset() { return std::pair{m_x_offset, m_y_offset}; }

   private:
    int m_x_offset, m_y_offset;
};

}  // namespace llvmes::gfx

/* The unknown key */
#define LLVMES_KEY_UNKNOWN -1

/* Printable keys */
#define LLVMES_KEY_SPACE 32
#define LLVMES_KEY_APOSTROPHE 39 /* ' */
#define LLVMES_KEY_COMMA 44      /* , */
#define LLVMES_KEY_MINUS 45      /* - */
#define LLVMES_KEY_PERIOD 46     /* . */
#define LLVMES_KEY_SLASH 47      /* / */
#define LLVMES_KEY_0 48
#define LLVMES_KEY_1 49
#define LLVMES_KEY_2 50
#define LLVMES_KEY_3 51
#define LLVMES_KEY_4 52
#define LLVMES_KEY_5 53
#define LLVMES_KEY_6 54
#define LLVMES_KEY_7 55
#define LLVMES_KEY_8 56
#define LLVMES_KEY_9 57
#define LLVMES_KEY_SEMICOLON 59 /* ; */
#define LLVMES_KEY_EQUAL 61     /* = */
#define LLVMES_KEY_A 65
#define LLVMES_KEY_B 66
#define LLVMES_KEY_C 67
#define LLVMES_KEY_D 68
#define LLVMES_KEY_E 69
#define LLVMES_KEY_F 70
#define LLVMES_KEY_G 71
#define LLVMES_KEY_H 72
#define LLVMES_KEY_I 73
#define LLVMES_KEY_J 74
#define LLVMES_KEY_K 75
#define LLVMES_KEY_L 76
#define LLVMES_KEY_M 77
#define LLVMES_KEY_N 78
#define LLVMES_KEY_O 79
#define LLVMES_KEY_P 80
#define LLVMES_KEY_Q 81
#define LLVMES_KEY_R 82
#define LLVMES_KEY_S 83
#define LLVMES_KEY_T 84
#define LLVMES_KEY_U 85
#define LLVMES_KEY_V 86
#define LLVMES_KEY_W 87
#define LLVMES_KEY_X 88
#define LLVMES_KEY_Y 89
#define LLVMES_KEY_Z 90
#define LLVMES_KEY_LEFT_BRACKET 91  /* [ */
#define LLVMES_KEY_BACKSLASH 92     /* \ */
#define LLVMES_KEY_RIGHT_BRACKET 93 /* ] */
#define LLVMES_KEY_GRAVE_ACCENT 96  /* ` */
#define LLVMES_KEY_WORLD_1 161      /* non-US #1 */
#define LLVMES_KEY_WORLD_2 162      /* non-US #2 */

/* Function keys */
#define LLVMES_KEY_ESCAPE 256
#define LLVMES_KEY_ENTER 257
#define LLVMES_KEY_TAB 258
#define LLVMES_KEY_BACKSPACE 259
#define LLVMES_KEY_INSERT 260
#define LLVMES_KEY_DELETE 261
#define LLVMES_KEY_RIGHT 262
#define LLVMES_KEY_LEFT 263
#define LLVMES_KEY_DOWN 264
#define LLVMES_KEY_UP 265
#define LLVMES_KEY_PAGE_UP 266
#define LLVMES_KEY_PAGE_DOWN 267
#define LLVMES_KEY_HOME 268
#define LLVMES_KEY_END 269
#define LLVMES_KEY_CAPS_LOCK 280
#define LLVMES_KEY_SCROLL_LOCK 281
#define LLVMES_KEY_NUM_LOCK 282
#define LLVMES_KEY_PRINT_SCREEN 283
#define LLVMES_KEY_PAUSE 284
#define LLVMES_KEY_F1 290
#define LLVMES_KEY_F2 291
#define LLVMES_KEY_F3 292
#define LLVMES_KEY_F4 293
#define LLVMES_KEY_F5 294
#define LLVMES_KEY_F6 295
#define LLVMES_KEY_F7 296
#define LLVMES_KEY_F8 297
#define LLVMES_KEY_F9 298
#define LLVMES_KEY_F10 299
#define LLVMES_KEY_F11 300
#define LLVMES_KEY_F12 301
#define LLVMES_KEY_F13 302
#define LLVMES_KEY_F14 303
#define LLVMES_KEY_F15 304
#define LLVMES_KEY_F16 305
#define LLVMES_KEY_F17 306
#define LLVMES_KEY_F18 307
#define LLVMES_KEY_F19 308
#define LLVMES_KEY_F20 309
#define LLVMES_KEY_F21 310
#define LLVMES_KEY_F22 311
#define LLVMES_KEY_F23 312
#define LLVMES_KEY_F24 313
#define LLVMES_KEY_F25 314
#define LLVMES_KEY_KP_0 320
#define LLVMES_KEY_KP_1 321
#define LLVMES_KEY_KP_2 322
#define LLVMES_KEY_KP_3 323
#define LLVMES_KEY_KP_4 324
#define LLVMES_KEY_KP_5 325
#define LLVMES_KEY_KP_6 326
#define LLVMES_KEY_KP_7 327
#define LLVMES_KEY_KP_8 328
#define LLVMES_KEY_KP_9 329
#define LLVMES_KEY_KP_DECIMAL 330
#define LLVMES_KEY_KP_DIVIDE 331
#define LLVMES_KEY_KP_MULTIPLY 332
#define LLVMES_KEY_KP_SUBTRACT 333
#define LLVMES_KEY_KP_ADD 334
#define LLVMES_KEY_KP_ENTER 335
#define LLVMES_KEY_KP_EQUAL 336
#define LLVMES_KEY_LEFT_SHIFT 340
#define LLVMES_KEY_LEFT_CONTROL 341
#define LLVMES_KEY_LEFT_ALT 342
#define LLVMES_KEY_LEFT_SUPER 343
#define LLVMES_KEY_RIGHT_SHIFT 344
#define LLVMES_KEY_RIGHT_CONTROL 345
#define LLVMES_KEY_RIGHT_ALT 346
#define LLVMES_KEY_RIGHT_SUPER 347
#define LLVMES_KEY_MENU 348