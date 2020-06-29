#pragma once

#include <cstdint>

#ifdef _JADE_PLATFORM_WINDOWS
    #ifdef _JADE_DLL
        #define JADE __declspec(dllexport)
    #else 
        //#define JADE __declspec(dllimport)
        #define JADE
    #endif
#endif

/* Button actions */
#define JADE_RELEASE      0
#define JADE_PRESS        1
#define JADE_REPEAT       2
#define JADE_DOUBLE_CLICK 3

 /* The unknown key */
 #define JADE_KEY_UNKNOWN            -1
  
 /* Printable keys */
 #define JADE_KEY_NUMPAD_0           20
 #define JADE_KEY_NUMPAD_1           21
 #define JADE_KEY_NUMPAD_2           22
 #define JADE_KEY_NUMPAD_3           23
 #define JADE_KEY_NUMPAD_4           24
 #define JADE_KEY_NUMPAD_5           25
 #define JADE_KEY_NUMPAD_6           26
 #define JADE_KEY_NUMPAD_7           27
 #define JADE_KEY_NUMPAD_8           28
 #define JADE_KEY_NUMPAD_9           29
 #define JADE_KEY_SPACE              32
 #define JADE_KEY_APOSTROPHE         39  /* ' */
 #define JADE_KEY_COMMA              44  /* , */
 #define JADE_KEY_MINUS              45  /* - */
 #define JADE_KEY_PERIOD             46  /* . */
 #define JADE_KEY_SLASH              47  /* / */
 #define JADE_KEY_0                  48
 #define JADE_KEY_1                  49
 #define JADE_KEY_2                  50
 #define JADE_KEY_3                  51
 #define JADE_KEY_4                  52
 #define JADE_KEY_5                  53
 #define JADE_KEY_6                  54
 #define JADE_KEY_7                  55
 #define JADE_KEY_8                  56
 #define JADE_KEY_9                  57
 #define JADE_KEY_SEMICOLON          59  /* ; */
 #define JADE_KEY_EQUAL              61  /* = */
 #define JADE_KEY_A                  65
 #define JADE_KEY_B                  66
 #define JADE_KEY_C                  67
 #define JADE_KEY_D                  68
 #define JADE_KEY_E                  69
 #define JADE_KEY_F                  70
 #define JADE_KEY_G                  71
 #define JADE_KEY_H                  72
 #define JADE_KEY_I                  73
 #define JADE_KEY_J                  74
 #define JADE_KEY_K                  75
 #define JADE_KEY_L                  76
 #define JADE_KEY_M                  77
 #define JADE_KEY_N                  78
 #define JADE_KEY_O                  79
 #define JADE_KEY_P                  80
 #define JADE_KEY_Q                  81
 #define JADE_KEY_R                  82
 #define JADE_KEY_S                  83
 #define JADE_KEY_T                  84
 #define JADE_KEY_U                  85
 #define JADE_KEY_V                  86
 #define JADE_KEY_W                  87
 #define JADE_KEY_X                  88
 #define JADE_KEY_Y                  89
 #define JADE_KEY_Z                  90
 #define JADE_KEY_LEFT_BRACKET       91  /* [ */
 #define JADE_KEY_BACKSLASH          92  /* \ */
 #define JADE_KEY_RIGHT_BRACKET      93  /* ] */
 #define JADE_KEY_GRAVE_ACCENT       96  /* ` */
 #define JADE_KEY_WORLD_1            161 /* non-US #1 */
 #define JADE_KEY_WORLD_2            162 /* non-US #2 */
  
 /* Function keys */
 #define JADE_KEY_ESCAPE             256
 #define JADE_KEY_ENTER              257
 #define JADE_KEY_TAB                258
 #define JADE_KEY_BACKSPACE          259
 #define JADE_KEY_INSERT             260
 #define JADE_KEY_DELETE             261
 #define JADE_KEY_RIGHT              262
 #define JADE_KEY_LEFT               263
 #define JADE_KEY_DOWN               264
 #define JADE_KEY_UP                 265
 #define JADE_KEY_PAGE_UP            266
 #define JADE_KEY_PAGE_DOWN          267
 #define JADE_KEY_HOME               268
 #define JADE_KEY_END                269
 #define JADE_KEY_CAPS_LOCK          280
 #define JADE_KEY_SCROLL_LOCK        281
 #define JADE_KEY_NUM_LOCK           282
 #define JADE_KEY_PRINT_SCREEN       283
 #define JADE_KEY_PAUSE              284
 #define JADE_KEY_F1                 290
 #define JADE_KEY_F2                 291
 #define JADE_KEY_F3                 292
 #define JADE_KEY_F4                 293
 #define JADE_KEY_F5                 294
 #define JADE_KEY_F6                 295
 #define JADE_KEY_F7                 296
 #define JADE_KEY_F8                 297
 #define JADE_KEY_F9                 298
 #define JADE_KEY_F10                299
 #define JADE_KEY_F11                300
 #define JADE_KEY_F12                301
 #define JADE_KEY_F13                302
 #define JADE_KEY_F14                303
 #define JADE_KEY_F15                304
 #define JADE_KEY_F16                305
 #define JADE_KEY_F17                306
 #define JADE_KEY_F18                307
 #define JADE_KEY_F19                308
 #define JADE_KEY_F20                309
 #define JADE_KEY_F21                310
 #define JADE_KEY_F22                311
 #define JADE_KEY_F23                312
 #define JADE_KEY_F24                313
 #define JADE_KEY_F25                314
 #define JADE_KEY_KP_0               320
 #define JADE_KEY_KP_1               321
 #define JADE_KEY_KP_2               322
 #define JADE_KEY_KP_3               323
 #define JADE_KEY_KP_4               324
 #define JADE_KEY_KP_5               325
 #define JADE_KEY_KP_6               326
 #define JADE_KEY_KP_7               327
 #define JADE_KEY_KP_8               328
 #define JADE_KEY_KP_9               329
 #define JADE_KEY_KP_DECIMAL         330
 #define JADE_KEY_KP_DIVIDE          331
 #define JADE_KEY_KP_MULTIPLY        332
 #define JADE_KEY_KP_SUBTRACT        333
 #define JADE_KEY_KP_ADD             334
 #define JADE_KEY_KP_ENTER           335
 #define JADE_KEY_KP_EQUAL           336
 #define JADE_KEY_LEFT_SHIFT         340
 #define JADE_KEY_LEFT_CONTROL       341
 #define JADE_KEY_LEFT_ALT           342
 #define JADE_KEY_LEFT_SUPER         343
 #define JADE_KEY_RIGHT_SHIFT        344
 #define JADE_KEY_RIGHT_CONTROL      345
 #define JADE_KEY_RIGHT_ALT          346
 #define JADE_KEY_RIGHT_SUPER        347
 #define JADE_KEY_MENU               348
  
 #define JADE_KEY_LAST               JADE_KEY_MENU
  
 #define JADE_MOD_SHIFT           0x0001
  
 #define JADE_MOD_CONTROL         0x0002
  
 #define JADE_MOD_ALT             0x0004
  
 #define JADE_MOD_SUPER           0x0008
  
 #define JADE_MOD_CAPS_LOCK       0x0010
  
 #define JADE_MOD_NUM_LOCK        0x0020
  
 #define JADE_MOUSE_BUTTON_1         0
 #define JADE_MOUSE_BUTTON_2         1
 #define JADE_MOUSE_BUTTON_3         2
 #define JADE_MOUSE_BUTTON_4         3
 #define JADE_MOUSE_BUTTON_5         4
 #define JADE_MOUSE_BUTTON_6         5
 #define JADE_MOUSE_BUTTON_7         6
 #define JADE_MOUSE_BUTTON_8         7
 #define JADE_MOUSE_BUTTON_LAST      JADE_MOUSE_BUTTON_8
 #define JADE_MOUSE_BUTTON_LEFT      JADE_MOUSE_BUTTON_1
 #define JADE_MOUSE_BUTTON_RIGHT     JADE_MOUSE_BUTTON_2
 #define JADE_MOUSE_BUTTON_MIDDLE    JADE_MOUSE_BUTTON_3
  
 #define JADE_JOYSTICK_1             0
 #define JADE_JOYSTICK_2             1
 #define JADE_JOYSTICK_3             2
 #define JADE_JOYSTICK_4             3
 #define JADE_JOYSTICK_5             4
 #define JADE_JOYSTICK_6             5
 #define JADE_JOYSTICK_7             6
 #define JADE_JOYSTICK_8             7
 #define JADE_JOYSTICK_9             8
 #define JADE_JOYSTICK_10            9
 #define JADE_JOYSTICK_11            10
 #define JADE_JOYSTICK_12            11
 #define JADE_JOYSTICK_13            12
 #define JADE_JOYSTICK_14            13
 #define JADE_JOYSTICK_15            14
 #define JADE_JOYSTICK_16            15
 #define JADE_JOYSTICK_LAST          JADE_JOYSTICK_16
  
 #define JADE_GAMEPAD_BUTTON_A               0
 #define JADE_GAMEPAD_BUTTON_B               1
 #define JADE_GAMEPAD_BUTTON_X               2
 #define JADE_GAMEPAD_BUTTON_Y               3
 #define JADE_GAMEPAD_BUTTON_LEFT_BUMPER     4
 #define JADE_GAMEPAD_BUTTON_RIGHT_BUMPER    5
 #define JADE_GAMEPAD_BUTTON_BACK            6
 #define JADE_GAMEPAD_BUTTON_START           7
 #define JADE_GAMEPAD_BUTTON_GUIDE           8
 #define JADE_GAMEPAD_BUTTON_LEFT_THUMB      9
 #define JADE_GAMEPAD_BUTTON_RIGHT_THUMB     10
 #define JADE_GAMEPAD_BUTTON_DPAD_UP         11
 #define JADE_GAMEPAD_BUTTON_DPAD_RIGHT      12
 #define JADE_GAMEPAD_BUTTON_DPAD_DOWN       13
 #define JADE_GAMEPAD_BUTTON_DPAD_LEFT       14
 #define JADE_GAMEPAD_BUTTON_LAST            JADE_GAMEPAD_BUTTON_DPAD_LEFT
  
 #define JADE_GAMEPAD_BUTTON_CROSS       JADE_GAMEPAD_BUTTON_A
 #define JADE_GAMEPAD_BUTTON_CIRCLE      JADE_GAMEPAD_BUTTON_B
 #define JADE_GAMEPAD_BUTTON_SQUARE      JADE_GAMEPAD_BUTTON_X
 #define JADE_GAMEPAD_BUTTON_TRIANGLE    JADE_GAMEPAD_BUTTON_Y
  
 #define JADE_GAMEPAD_AXIS_LEFT_X        0
 #define JADE_GAMEPAD_AXIS_LEFT_Y        1
 #define JADE_GAMEPAD_AXIS_RIGHT_X       2
 #define JADE_GAMEPAD_AXIS_RIGHT_Y       3
 #define JADE_GAMEPAD_AXIS_LEFT_TRIGGER  4
 #define JADE_GAMEPAD_AXIS_RIGHT_TRIGGER 5
 #define JADE_GAMEPAD_AXIS_LAST          JADE_GAMEPAD_AXIS_RIGHT_TRIGGER

#define JADE_MOD_SHIFT           0x0001

#define JADE_MOD_CONTROL         0x0002
  
#define JADE_MOD_ALT             0x0004

#define JADE_MOD_SUPER           0x0008

#define JADE_MOD_CAPS_LOCK       0x0010

#define JADE_MOD_NUM_LOCK        0x0020

// // ===========================================
// // Window Hints
// // ===========================================
// #define JADE_WH_FOCUSED       0x00020001
// #define JADE_WH_ICONIFIED     0x00020002
// #define JADE_WH_RESIZABLE     0x00020003
// #define JADE_WH_VISIBLE       0x00020004
// #define JADE_WH_DECORATED     0x00020005
// #define JADE_WH_AUTO_ICONIFY  0x00020006
// #define JADE_WH_FLOATING      0x00020007
// #define JADE_WH_MAXIMIZED     0x00020008
// #define JADE_WH_CENTER_CURSOR 0x00020009
// #define JADE_WH_TRANSPARENT_FRAMEBUFFER 0x0002000A
// #define JADE_WH_HOVERED       0x0002000B
// #define JADE_WH_FOCUS_ON_SHOW 0x0002000C
// #define JADE_WH_ALLOC_CONSOLE 0x0002000D

// #define JADE_WH_RED_BITS         0x00021001
// #define JADE_WH_GREEN_BITS       0x00021002
// #define JADE_WH_BLUE_BITS        0x00021003
// #define JADE_WH_ALPHA_BITS       0x00021004
// #define JADE_WH_DEPTH_BITS       0x00021005
// #define JADE_WH_STENCIL_BITS     0x00021006
// #define JADE_WH_ACCUM_RED_BITS   0x00021007
// #define JADE_WH_ACCUM_BLUE_BITS  0x00021008
// #define JADE_WH_ACCUM_GREEN_BITS 0x00021009
// #define JADE_WH_ACCUM_ALPHA_BITS 0x0002100A

// #define JADE_WH_AUX_BUFFERS   0x0002100B
// #define JADE_WH_STEREO        0x0002100C
// #define JADE_WH_SAMPLES       0x0002100D
// #define JADE_WH_SRGB_CAPABLE  0x0002100E
// #define JADE_WH_REFRESH_RATE  0x0002100F
// #define JADE_WH_DOUBLEBUFFER  0x00021010

// #define JADE_WH_CLIENT_API            0x00022001
// #define JADE_WH_CONTEXT_VERSION_MAJOR 0x00022002
// #define JADE_WH_CONTEXT_VERSION_MINOR 0x00022003
// #define JADE_WH_CONTEXT_REVISION      0x00022004
// #define JADE_WH_CONTEXT_ROBUSTNESS    0x00022005
// #define JADE_WH_OPENGL_FORWARD_COMPAT 0x00022006
// #define JADE_WH_OPENGL_DEBUG_CONTEXT  0x00022007
// #define JADE_WH_OPENGL_PROFILE        0x00022008
// #define JADE_WH_CONTEXT_RELEASE_BEHAVIOR 0x00022009
// #define JADE_WH_CONTEXT_NO_ERROR      0x0002200A
// #define JADE_WH_CONTEXT_CREATION_API  0x0002200B
// #define JADE_WH_SCALE_TO_MONITOR      0x0002200C


namespace Jade {
    // // ===========================================
    // // Function pointer typedefs
    // // ===========================================
    // // Forward declare Window so we don't get circular dependency
    // class Window;
    // typedef void (*KeyCallbackFnPt) (Window* window, int key, int scancode, int action, int mods);
    // typedef void (*CursorCallbackFnPt) (Window* window, double xpos, double ypos);
    // typedef void (*MouseButtonCallbackFnPt) (Window* window, int button, int action, int mods);
    // typedef void (*ScrollCallbackFnPt) (Window* window, double xoffset, double yoffset);
    // typedef void (*ResizeCallbackFnPt) (Window* window, int width, int height);
    // typedef void (*WindowCloseCallbackFnPt) (Window* window);
    // typedef void (*CharCallbackFnPt) (Window* window, unsigned int keycode); // TODO: Implement this!
    // //typedef void (*MonitorCallbackFnPt) (Monitor* monitor, int); // TODO: Implement this!!


    // ===========================================
    // Fixed-size primitive types
    // ===========================================
    typedef int8_t  int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;

    typedef uint8_t  uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;
}