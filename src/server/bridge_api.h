// Additional Bridge Server header to define Bridge API inline globals

#pragma once

#include "remix_api/remix_c.h"
#include <mutex>

namespace api {
  static inline remixapi_Interface g_remix = { 0 };
  static inline bool g_remix_initialized = false;
  static inline HMODULE g_remix_dll = NULL;

  static bool is_initialized() {
    return g_remix_initialized;
  }

  static inline IDirect3DDevice9Ex* g_device;
  static inline std::mutex g_device_mutex;
  IDirect3DDevice9Ex* get_device();
}

