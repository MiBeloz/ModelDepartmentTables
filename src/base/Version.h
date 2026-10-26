#ifndef VERSION_H
#define VERSION_H

#define MDT_VERSION_MAJOR 0
#define MDT_VERSION_MINOR 1
#define MDT_VERSION_BUGFIX 0
#define MDT_VERSION_STATUS "" // "", "-rc1", "-beta2", ...

// Numeric form for #if comparisons: 0.1.0 -> 100
#define MDT_VERSION_CODE (MDT_VERSION_MAJOR * 10000 + MDT_VERSION_MINOR * 100 + MDT_VERSION_BUGFIX)

#define MDT_STRINGIFY_IMPL(x) #x
#define MDT_STRINGIFY(x) MDT_STRINGIFY_IMPL(x)

#define MDT_VERSION_STRING                                                                   \
    MDT_STRINGIFY(MDT_VERSION_MAJOR.MDT_VERSION_MINOR.MDT_VERSION_BUGFIX) MDT_VERSION_STATUS

#define MDT_VERSION_DISPLAY "v" MDT_VERSION_STRING

#endif // VERSION_H
