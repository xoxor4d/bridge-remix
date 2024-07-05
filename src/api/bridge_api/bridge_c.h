// Bridge API header for both the x86 bridge client and the x86 game

#ifndef BRIDGE_C_H_
#define BRIDGE_C_H_

#include <windows.h>

#define BRIDGEAPI_CALL __stdcall
#define BRIDGEAPI_PTR  BRIDGEAPI_CALL

#define BRIDGE_API  __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

  typedef enum bridgeapi_ErrorCode {
    BRIDGEAPI_ERROR_CODE_SUCCESS = 0,
    BRIDGEAPI_ERROR_CODE_GENERAL_FAILURE = 1,
    // WinAPI's GetModuleHandle has failed
    BRIDGEAPI_ERROR_CODE_GET_MODULE_HANDLE_FAILURE = 2,
    BRIDGEAPI_ERROR_CODE_INVALID_ARGUMENTS = 3,
    // Couldn't find 'remixInitialize' function in the .dll
    BRIDGEAPI_ERROR_CODE_GET_PROC_ADDRESS_FAILURE = 4,
    // CreateD3D9 / RegisterD3D9Device can be called only once
    BRIDGEAPI_ERROR_CODE_ALREADY_EXISTS = 5,
    // RegisterD3D9Device requires the device that was created with IDirect3DDevice9Ex, returned by CreateD3D9
    BRIDGEAPI_ERROR_CODE_REGISTERING_NON_REMIX_D3D9_DEVICE = 6,
    // RegisterD3D9Device was not called
    BRIDGEAPI_ERROR_CODE_REMIX_DEVICE_WAS_NOT_REGISTERED = 7,
    BRIDGEAPI_ERROR_CODE_INCOMPATIBLE_VERSION = 8,
    // WinAPI's SetDllDirectory has failed
    //BRIDGEAPI_ERROR_CODE_SET_DLL_DIRECTORY_FAILURE = 9,
    // WinAPI's GetFullPathName has failed
    //BRIDGEAPI_ERROR_CODE_GET_FULL_PATH_NAME_FAILURE = 10,
    BRIDGEAPI_ERROR_CODE_NOT_INITIALIZED = 11,
    // Error codes that are encoded as HRESULT, i.e. returned from D3D9 functions.
    // Look MAKE_D3DHRESULT, but with _FACD3D=0x896, instead of D3D9's 0x876
    //BRIDGEAPI_ERROR_CODE_HRESULT_NO_REQUIRED_GPU_FEATURES = 0x88960001,
    //BRIDGEAPI_ERROR_CODE_HRESULT_DRIVER_VERSION_BELOW_MINIMUM = 0x88960002,
    //BRIDGEAPI_ERROR_CODE_HRESULT_DXVK_INSTANCE_EXTENSION_FAIL = 0x88960003,
    //BRIDGEAPI_ERROR_CODE_HRESULT_VK_CREATE_INSTANCE_FAIL = 0x88960004,
    //BRIDGEAPI_ERROR_CODE_HRESULT_VK_CREATE_DEVICE_FAIL = 0x88960005,
    //BRIDGEAPI_ERROR_CODE_HRESULT_GRAPHICS_QUEUE_FAMILY_MISSING = 0x88960006,
  } BRIDGEAPI_ErrorCode;

  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_DebugPrint)(const char* text);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_Present)(void);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_SetConfigVariable)(const char* var, const char* value);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_RegisterDevice)(void);

  typedef struct bridgeapi_Interface {
    bool initialized;
    PFN_bridgeapi_DebugPrint		    DebugPrint;
    PFN_bridgeapi_Present		        Present;
    PFN_bridgeapi_SetConfigVariable SetConfigVariable;
    PFN_bridgeapi_RegisterDevice    RegisterDevice;
  } bridgeapi_Interface;

  BRIDGE_API BRIDGEAPI_ErrorCode __cdecl bridgeapi_InitFuncs(bridgeapi_Interface* out_result);
  typedef BRIDGEAPI_ErrorCode(__cdecl* PFN_bridgeapi_InitFuncs)(bridgeapi_Interface* out_result);

  // --------
  // --------

  inline BRIDGEAPI_ErrorCode bridgeapi_initialize(bridgeapi_Interface* out_bridgeInterface) {

    PFN_bridgeapi_InitFuncs pfn_Initialize = nullptr;
  	HMODULE hModule = GetModuleHandleA("d3d9.dll");
	if (hModule) {
	  PROC func = GetProcAddress(hModule, "bridgeapi_InitFuncs");
	  if (func) {
	  	pfn_Initialize = (PFN_bridgeapi_InitFuncs)func;
	  }
	  else {
	  	return BRIDGEAPI_ERROR_CODE_GET_PROC_ADDRESS_FAILURE;
	  }
	  
	  bridgeapi_Interface bridgeInterface = { false };
	  bridgeapi_ErrorCode status = pfn_Initialize(&bridgeInterface);
	  if (status != BRIDGEAPI_ERROR_CODE_SUCCESS) {
	  	return status;
	  }

    bridgeInterface.initialized = true;
	  *out_bridgeInterface = bridgeInterface;
	  return status;
	}

  	return BRIDGEAPI_ERROR_CODE_GET_MODULE_HANDLE_FAILURE;
  }

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BRIDGE_C_H_