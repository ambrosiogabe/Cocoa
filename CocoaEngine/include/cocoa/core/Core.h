#ifndef COCOA_ENGINE_CORE_H
#define COCOA_ENGINE_CORE_H

#include <cstdint>

#ifdef _WIN32
    #ifdef _COCOA_DLL
        #define COCOA __declspec(dllexport)
    #else 
        #define COCOA __declspec(dllimport)
    #endif
#else 
    #define COCOA
#endif

#ifdef _WIN32
    #ifdef _COCOA_DLL
        #define C_EXTERN extern "C"
    #else 
        #define C_EXTERN extern "C"
    #endif
#else 
    #define C_EXTERN
#endif

#ifdef _WIN32
    #ifdef _COCOA_SCRIPT_DLL
        #define COCOA_SCRIPT __declspec(dllexport)
    #else 
        #define COCOA_SCRIPT __declspec(dllimport)
    #endif
#else 
    #define COCOA_SCRIPT
#endif

#ifdef _WIN32
    #ifdef _COCOA_DLL
        #define COCOA_TEMPLATE_EXTERN
    #else 
        #define COCOA_TEMPLATE_EXTERN extern
    #endif
#else 
    #define COCOA_TEMPLATE_EXTERN
#endif

#define UPROPERTY(...) ;
#define UCLASS(...) ;
#define USTRUCT(...) ;

/* Button actions */
#define COCOA_RELEASE      0
#define COCOA_PRESS        1
#define COCOA_REPEAT       2
#define COCOA_DOUBLE_CLICK 3

 /* The unknown key */
 #define COCOA_KEY_UNKNOWN            -1
  
 /* Printable keys */
 #define COCOA_KEY_NUMPAD_0           20
 #define COCOA_KEY_NUMPAD_1           21
 #define COCOA_KEY_NUMPAD_2           22
 #define COCOA_KEY_NUMPAD_3           23
 #define COCOA_KEY_NUMPAD_4           24
 #define COCOA_KEY_NUMPAD_5           25
 #define COCOA_KEY_NUMPAD_6           26
 #define COCOA_KEY_NUMPAD_7           27
 #define COCOA_KEY_NUMPAD_8           28
 #define COCOA_KEY_NUMPAD_9           29
 #define COCOA_KEY_SPACE              32
 #define COCOA_KEY_APOSTROPHE         39  /* ' */
 #define COCOA_KEY_COMMA              44  /* , */
 #define COCOA_KEY_MINUS              45  /* - */
 #define COCOA_KEY_PERIOD             46  /* . */
 #define COCOA_KEY_SLASH              47  /* / */
 #define COCOA_KEY_0                  48
 #define COCOA_KEY_1                  49
 #define COCOA_KEY_2                  50
 #define COCOA_KEY_3                  51
 #define COCOA_KEY_4                  52
 #define COCOA_KEY_5                  53
 #define COCOA_KEY_6                  54
 #define COCOA_KEY_7                  55
 #define COCOA_KEY_8                  56
 #define COCOA_KEY_9                  57
 #define COCOA_KEY_SEMICOLON          59  /* ; */
 #define COCOA_KEY_EQUAL              61  /* = */
 #define COCOA_KEY_A                  65
 #define COCOA_KEY_B                  66
 #define COCOA_KEY_C                  67
 #define COCOA_KEY_D                  68
 #define COCOA_KEY_E                  69
 #define COCOA_KEY_F                  70
 #define COCOA_KEY_G                  71
 #define COCOA_KEY_H                  72
 #define COCOA_KEY_I                  73
 #define COCOA_KEY_J                  74
 #define COCOA_KEY_K                  75
 #define COCOA_KEY_L                  76
 #define COCOA_KEY_M                  77
 #define COCOA_KEY_N                  78
 #define COCOA_KEY_O                  79
 #define COCOA_KEY_P                  80
 #define COCOA_KEY_Q                  81
 #define COCOA_KEY_R                  82
 #define COCOA_KEY_S                  83
 #define COCOA_KEY_T                  84
 #define COCOA_KEY_U                  85
 #define COCOA_KEY_V                  86
 #define COCOA_KEY_W                  87
 #define COCOA_KEY_X                  88
 #define COCOA_KEY_Y                  89
 #define COCOA_KEY_Z                  90
 #define COCOA_KEY_LEFT_BRACKET       91  /* [ */
 #define COCOA_KEY_BACKSLASH          92  /* \ */
 #define COCOA_KEY_RIGHT_BRACKET      93  /* ] */
 #define COCOA_KEY_GRAVE_ACCENT       96  /* ` */
 #define COCOA_KEY_WORLD_1            161 /* non-US #1 */
 #define COCOA_KEY_WORLD_2            162 /* non-US #2 */
  
 /* Function keys */
 #define COCOA_KEY_ESCAPE             256
 #define COCOA_KEY_ENTER              257
 #define COCOA_KEY_TAB                258
 #define COCOA_KEY_BACKSPACE          259
 #define COCOA_KEY_INSERT             260
 #define COCOA_KEY_DELETE             261
 #define COCOA_KEY_RIGHT              262
 #define COCOA_KEY_LEFT               263
 #define COCOA_KEY_DOWN               264
 #define COCOA_KEY_UP                 265
 #define COCOA_KEY_PAGE_UP            266
 #define COCOA_KEY_PAGE_DOWN          267
 #define COCOA_KEY_HOME               268
 #define COCOA_KEY_END                269
 #define COCOA_KEY_CAPS_LOCK          280
 #define COCOA_KEY_SCROLL_LOCK        281
 #define COCOA_KEY_NUM_LOCK           282
 #define COCOA_KEY_PRINT_SCREEN       283
 #define COCOA_KEY_PAUSE              284
 #define COCOA_KEY_F1                 290
 #define COCOA_KEY_F2                 291
 #define COCOA_KEY_F3                 292
 #define COCOA_KEY_F4                 293
 #define COCOA_KEY_F5                 294
 #define COCOA_KEY_F6                 295
 #define COCOA_KEY_F7                 296
 #define COCOA_KEY_F8                 297
 #define COCOA_KEY_F9                 298
 #define COCOA_KEY_F10                299
 #define COCOA_KEY_F11                300
 #define COCOA_KEY_F12                301
 #define COCOA_KEY_F13                302
 #define COCOA_KEY_F14                303
 #define COCOA_KEY_F15                304
 #define COCOA_KEY_F16                305
 #define COCOA_KEY_F17                306
 #define COCOA_KEY_F18                307
 #define COCOA_KEY_F19                308
 #define COCOA_KEY_F20                309
 #define COCOA_KEY_F21                310
 #define COCOA_KEY_F22                311
 #define COCOA_KEY_F23                312
 #define COCOA_KEY_F24                313
 #define COCOA_KEY_F25                314
 #define COCOA_KEY_KP_0               320
 #define COCOA_KEY_KP_1               321
 #define COCOA_KEY_KP_2               322
 #define COCOA_KEY_KP_3               323
 #define COCOA_KEY_KP_4               324
 #define COCOA_KEY_KP_5               325
 #define COCOA_KEY_KP_6               326
 #define COCOA_KEY_KP_7               327
 #define COCOA_KEY_KP_8               328
 #define COCOA_KEY_KP_9               329
 #define COCOA_KEY_KP_DECIMAL         330
 #define COCOA_KEY_KP_DIVIDE          331
 #define COCOA_KEY_KP_MULTIPLY        332
 #define COCOA_KEY_KP_SUBTRACT        333
 #define COCOA_KEY_KP_ADD             334
 #define COCOA_KEY_KP_ENTER           335
 #define COCOA_KEY_KP_EQUAL           336
 #define COCOA_KEY_LEFT_SHIFT         340
 #define COCOA_KEY_LEFT_CONTROL       341
 #define COCOA_KEY_LEFT_ALT           342
 #define COCOA_KEY_LEFT_SUPER         343
 #define COCOA_KEY_RIGHT_SHIFT        344
 #define COCOA_KEY_RIGHT_CONTROL      345
 #define COCOA_KEY_RIGHT_ALT          346
 #define COCOA_KEY_RIGHT_SUPER        347
 #define COCOA_KEY_MENU               348
  
 #define COCOA_KEY_LAST               COCOA_KEY_MENU
  
 #define COCOA_MOD_SHIFT           0x0001
  
 #define COCOA_MOD_CONTROL         0x0002
  
 #define COCOA_MOD_ALT             0x0004
  
 #define COCOA_MOD_SUPER           0x0008
  
 #define COCOA_MOD_CAPS_LOCK       0x0010
  
 #define COCOA_MOD_NUM_LOCK        0x0020
  
 #define COCOA_MOUSE_BUTTON_1         0
 #define COCOA_MOUSE_BUTTON_2         1
 #define COCOA_MOUSE_BUTTON_3         2
 #define COCOA_MOUSE_BUTTON_4         3
 #define COCOA_MOUSE_BUTTON_5         4
 #define COCOA_MOUSE_BUTTON_6         5
 #define COCOA_MOUSE_BUTTON_7         6
 #define COCOA_MOUSE_BUTTON_8         7
 #define COCOA_MOUSE_BUTTON_LAST      COCOA_MOUSE_BUTTON_8
 #define COCOA_MOUSE_BUTTON_LEFT      COCOA_MOUSE_BUTTON_1
 #define COCOA_MOUSE_BUTTON_RIGHT     COCOA_MOUSE_BUTTON_2
 #define COCOA_MOUSE_BUTTON_MIDDLE    COCOA_MOUSE_BUTTON_3
  
 #define COCOA_JOYSTICK_1             0
 #define COCOA_JOYSTICK_2             1
 #define COCOA_JOYSTICK_3             2
 #define COCOA_JOYSTICK_4             3
 #define COCOA_JOYSTICK_5             4
 #define COCOA_JOYSTICK_6             5
 #define COCOA_JOYSTICK_7             6
 #define COCOA_JOYSTICK_8             7
 #define COCOA_JOYSTICK_9             8
 #define COCOA_JOYSTICK_10            9
 #define COCOA_JOYSTICK_11            10
 #define COCOA_JOYSTICK_12            11
 #define COCOA_JOYSTICK_13            12
 #define COCOA_JOYSTICK_14            13
 #define COCOA_JOYSTICK_15            14
 #define COCOA_JOYSTICK_16            15
 #define COCOA_JOYSTICK_LAST          JADE_JOYSTICK_16
  
 #define COCOA_GAMEPAD_BUTTON_A               0
 #define COCOA_GAMEPAD_BUTTON_B               1
 #define COCOA_GAMEPAD_BUTTON_X               2
 #define COCOA_GAMEPAD_BUTTON_Y               3
 #define COCOA_GAMEPAD_BUTTON_LEFT_BUMPER     4
 #define COCOA_GAMEPAD_BUTTON_RIGHT_BUMPER    5
 #define COCOA_GAMEPAD_BUTTON_BACK            6
 #define COCOA_GAMEPAD_BUTTON_START           7
 #define COCOA_GAMEPAD_BUTTON_GUIDE           8
 #define COCOA_GAMEPAD_BUTTON_LEFT_THUMB      9
 #define COCOA_GAMEPAD_BUTTON_RIGHT_THUMB     10
 #define COCOA_GAMEPAD_BUTTON_DPAD_UP         11
 #define COCOA_GAMEPAD_BUTTON_DPAD_RIGHT      12
 #define COCOA_GAMEPAD_BUTTON_DPAD_DOWN       13
 #define COCOA_GAMEPAD_BUTTON_DPAD_LEFT       14
 #define COCOA_GAMEPAD_BUTTON_LAST            COCOA_GAMEPAD_BUTTON_DPAD_LEFT
  
 #define COCOA_GAMEPAD_BUTTON_CROSS       COCOA_GAMEPAD_BUTTON_A
 #define COCOA_GAMEPAD_BUTTON_CIRCLE      COCOA_GAMEPAD_BUTTON_B
 #define COCOA_GAMEPAD_BUTTON_SQUARE      COCOA_GAMEPAD_BUTTON_X
 #define COCOA_GAMEPAD_BUTTON_TRIANGLE    COCOA_GAMEPAD_BUTTON_Y
  
 #define COCOA_GAMEPAD_AXIS_LEFT_X        0
 #define COCOA_GAMEPAD_AXIS_LEFT_Y        1
 #define COCOA_GAMEPAD_AXIS_RIGHT_X       2
 #define COCOA_GAMEPAD_AXIS_RIGHT_Y       3
 #define COCOA_GAMEPAD_AXIS_LEFT_TRIGGER  4
 #define COCOA_GAMEPAD_AXIS_RIGHT_TRIGGER 5
 #define COCOA_GAMEPAD_AXIS_LAST          COCOA_GAMEPAD_AXIS_RIGHT_TRIGGER

#define COCOA_MOD_SHIFT           0x0001

#define COCOA_MOD_CONTROL         0x0002
  
#define COCOA_MOD_ALT             0x0004

#define COCOA_MOD_SUPER           0x0008

#define COCOA_MOD_CAPS_LOCK       0x0010

#define COCOA_MOD_NUM_LOCK        0x0020

namespace Cocoa
{
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

#endif
