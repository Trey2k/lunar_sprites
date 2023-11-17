#ifndef API_H
#define API_H

#if defined(_MSC_VER)
#define LS_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define LS_EXPORT __attribute__((visibility("default")))
#else // _MSC_VER
#define LS_EXPORT
#endif // _MSC_VER

#endif // API_H