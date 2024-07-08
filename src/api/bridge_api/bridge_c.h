// Bridge API header for both the x86 bridge client and the x86 game

#ifndef BRIDGE_C_H_
#define BRIDGE_C_H_

#include <cstdint>
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

  // -----------------------------------------------------------
  // <- remix api types directly grabbed from the remix_c header

  typedef enum remixapi_StructType {
    REMIXAPI_STRUCT_TYPE_NONE = 0,
    REMIXAPI_STRUCT_TYPE_INITIALIZE_LIBRARY_INFO = 1,
    REMIXAPI_STRUCT_TYPE_MATERIAL_INFO = 2,
    REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_PORTAL_EXT = 3,
    REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_TRANSLUCENT_EXT = 4,
    REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_OPAQUE_EXT = 5,
    REMIXAPI_STRUCT_TYPE_LIGHT_INFO = 6,
    REMIXAPI_STRUCT_TYPE_LIGHT_INFO_DISTANT_EXT = 7,
    REMIXAPI_STRUCT_TYPE_LIGHT_INFO_CYLINDER_EXT = 8,
    REMIXAPI_STRUCT_TYPE_LIGHT_INFO_DISK_EXT = 9,
    REMIXAPI_STRUCT_TYPE_LIGHT_INFO_RECT_EXT = 10,
    REMIXAPI_STRUCT_TYPE_LIGHT_INFO_SPHERE_EXT = 11,
    REMIXAPI_STRUCT_TYPE_MESH_INFO = 12,
    REMIXAPI_STRUCT_TYPE_INSTANCE_INFO = 13,
    REMIXAPI_STRUCT_TYPE_INSTANCE_INFO_BONE_TRANSFORMS_EXT = 14,
    REMIXAPI_STRUCT_TYPE_INSTANCE_INFO_BLEND_EXT = 15,
    REMIXAPI_STRUCT_TYPE_CAMERA_INFO = 16,
    REMIXAPI_STRUCT_TYPE_CAMERA_INFO_PARAMETERIZED_EXT = 17,
    REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_OPAQUE_SUBSURFACE_EXT = 18,
    REMIXAPI_STRUCT_TYPE_INSTANCE_INFO_OBJECT_PICKING_EXT = 19,
    REMIXAPI_STRUCT_TYPE_LIGHT_INFO_DOME_EXT = 20,
    REMIXAPI_STRUCT_TYPE_LIGHT_INFO_USD_EXT = 21,
    REMIXAPI_STRUCT_TYPE_STARTUP_INFO = 22,
    REMIXAPI_STRUCT_TYPE_PRESENT_INFO = 23,
    // NOTE: if adding a new struct, register it in 'rtx_remix_specialization.inl'
  } remixapi_StructType;

  namespace x86
  {
    typedef uint32_t remixapi_Bool;

    typedef struct remixapi_Rect2D {
      int32_t left;
      int32_t top;
      int32_t right;
      int32_t bottom;
    } remixapi_Rect2D;

    typedef struct remixapi_Float2D {
      float x;
      float y;
    } remixapi_Float2D;

    typedef struct remixapi_Float3D {
      float x;
      float y;
      float z;
    } remixapi_Float3D;

    typedef struct remixapi_Float4D {
      float x;
      float y;
      float z;
      float w;
    } remixapi_Float4D;

    typedef struct remixapi_Transform {
      float matrix[3][4];
    } remixapi_Transform;


    typedef struct remixapi_MaterialHandle_T* remixapi_MaterialHandle;
    typedef struct remixapi_MeshHandle_T* remixapi_MeshHandle;
    typedef struct remixapi_LightHandle_T* remixapi_LightHandle;

    typedef const wchar_t* remixapi_Path;

    typedef struct remixapi_MaterialInfoOpaqueEXT {
      remixapi_StructType sType;
      //void* pNext;
      remixapi_Path       roughnessTexture;
      remixapi_Path       metallicTexture;
      float               anisotropy;
      remixapi_Float3D    albedoConstant;
      float               opacityConstant;
      float               roughnessConstant;
      float               metallicConstant;
      remixapi_Bool       thinFilmThickness_hasvalue;
      float               thinFilmThickness_value;
      remixapi_Bool       alphaIsThinFilmThickness;
      remixapi_Path       heightTexture;
      float               heightTextureStrength;
      // If true, InstanceInfoBlendEXT is used as a source for alpha state
      remixapi_Bool       useDrawCallAlphaState;
      remixapi_Bool       blendType_hasvalue;
      int                 blendType_value;
      remixapi_Bool       invertedBlend;
      int                 alphaTestType;
      uint8_t             alphaReferenceValue;
    } remixapi_MaterialInfoOpaqueEXT;

    typedef struct remixapi_MaterialInfo {
      remixapi_StructType sType;
      //void* pNext;
      uint64_t            hash;
      remixapi_Path       albedoTexture;
      remixapi_Path       normalTexture;
      remixapi_Path       tangentTexture;
      remixapi_Path       emissiveTexture;
      float               emissiveIntensity;
      remixapi_Float3D    emissiveColorConstant;
      uint8_t             spriteSheetRow;
      uint8_t             spriteSheetCol;
      uint8_t             spriteSheetFps;
      uint8_t             filterMode;
      uint8_t             wrapModeU;
      uint8_t             wrapModeV;
    } remixapi_MaterialInfo;


    typedef struct remixapi_HardcodedVertex {
      float    position[3];
      float    normal[3];
      float    texcoord[2];
      uint32_t color;
      uint32_t _pad0;
      uint32_t _pad1;
      uint32_t _pad2;
      uint32_t _pad3;
      uint32_t _pad4;
      uint32_t _pad5;
      uint32_t _pad6;
    } remixapi_HardcodedVertex;

    /*typedef struct remixapi_MeshInfoSkinning {
      uint32_t                        bonesPerVertex;
      // Each tuple of 'bonesPerVertex' float-s defines a vertex.
      // I.e. the size must be (bonesPerVertex * vertexCount).
      const float*                    blendWeights_values;
      uint32_t                        blendWeights_count;
      // Each tuple of 'bonesPerVertex' uint32_t-s defines a vertex.
      // I.e. the size must be (bonesPerVertex * vertexCount).
      const uint32_t*                 blendIndices_values;
      uint32_t                        blendIndices_count;
    } remixapi_MeshInfoSkinning;*/

    typedef struct remixapi_MeshInfoSurfaceTriangles {
      const remixapi_HardcodedVertex* vertices_values;
      uint64_t                        vertices_count;
      const uint32_t*                 indices_values;
      uint64_t                        indices_count;
      remixapi_Bool                   skinning_hasvalue;
      //remixapi_MeshInfoSkinning       skinning_value; // # TODO
      remixapi_MaterialHandle         material;
    } remixapi_MeshInfoSurfaceTriangles;

    typedef struct remixapi_MeshInfo {
      remixapi_StructType                      sType;
      //void* pNext;
      uint64_t                                 hash;
      const remixapi_MeshInfoSurfaceTriangles* surfaces_values;
      uint32_t                                 surfaces_count;
    } remixapi_MeshInfo;


    typedef struct remixapi_LightInfoLightShaping {
      // The direction the Light Shaping is pointing in. Must be normalized.
      remixapi_Float3D               direction;
      float                          coneAngleDegrees;
      float                          coneSoftness;
      float                          focusExponent;
    } remixapi_LightInfoLightShaping;

    typedef struct remixapi_LightInfoSphereEXT {
      remixapi_StructType            sType;
      //void* pNext;
      remixapi_Float3D               position;
      float                          radius;
      remixapi_Bool                  shaping_hasvalue;
      remixapi_LightInfoLightShaping shaping_value;
    } remixapi_LightInfoSphereEXT;

    typedef struct remixapi_LightInfoRectEXT {
      remixapi_StructType            sType;
      //void* pNext;
      remixapi_Float3D               position;
      // The X axis of the Rect Light. Must be normalized and orthogonal to the Y and direction axes.
      remixapi_Float3D               xAxis;
      float                          xSize;
      // The Y axis of the Rect Light. Must be normalized and orthogonal to the X and direction axes.
      remixapi_Float3D               yAxis;
      float                          ySize;
      // The direction the Rect Light is pointing in, should match the Shaping direction if present.
      // Must be normalized and orthogonal to the X and Y axes.
      remixapi_Float3D               direction;
      remixapi_Bool                  shaping_hasvalue;
      remixapi_LightInfoLightShaping shaping_value;
    } remixapi_LightInfoRectEXT;

    typedef struct remixapi_LightInfoDiskEXT {
      remixapi_StructType            sType;
      //void* pNext;
      remixapi_Float3D               position;
      // The X axis of the Disk Light. Must be normalized and orthogonal to the Y and direction axes.
      remixapi_Float3D               xAxis;
      float                          xRadius;
      // The Y axis of the Disk Light. Must be normalized and orthogonal to the X and direction axes.
      remixapi_Float3D               yAxis;
      float                          yRadius;
      // The direction the Disk Light is pointing in, should match the Shaping direction if present
      // Must be normalized and orthogonal to the X and Y axes.
      remixapi_Float3D               direction;
      remixapi_Bool                  shaping_hasvalue;
      remixapi_LightInfoLightShaping shaping_value;
    } remixapi_LightInfoDiskEXT;

    typedef struct remixapi_LightInfoCylinderEXT {
      remixapi_StructType            sType;
      //void* pNext;
      remixapi_Float3D               position;
      float                          radius;
      // The "center" axis of the Cylinder Light. Must be normalized.
      remixapi_Float3D               axis;
      float                          axisLength;
    } remixapi_LightInfoCylinderEXT;

    typedef struct remixapi_LightInfoDistantEXT {
      remixapi_StructType             sType;
      //void* pNext;
      // The direction the Distant Light is pointing in. Must be normalized.
      remixapi_Float3D                direction;
      float                           angularDiameterDegrees;
    } remixapi_LightInfoDistantEXT;

    typedef struct remixapi_LightInfo {
      remixapi_StructType             sType;
      //void* pNext;
      uint64_t                        hash;
      remixapi_Float3D                radiance;
    } remixapi_LightInfo;
  }

  // -----------------------------------------------------------
  // remix api types end ->


  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_DebugPrint)(const char* text);
  typedef uint64_t(BRIDGEAPI_PTR* PFN_bridgeapi_CreateOpaqueMaterial)(const x86::remixapi_MaterialInfo* info, const x86::remixapi_MaterialInfoOpaqueEXT* opaque_info);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_DestroyMaterial)(uint64_t handle);
  typedef uint64_t(BRIDGEAPI_PTR* PFN_bridgeapi_CreateTriangleMesh)(const x86::remixapi_MeshInfo* info);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_DestroyMesh)(uint64_t handle);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_DrawMeshInstance)(uint64_t handle, const x86::remixapi_Transform* t, x86::remixapi_Bool double_sided);
  typedef uint64_t(BRIDGEAPI_PTR* PFN_bridgeapi_CreateSphereLight)(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoSphereEXT* sphere_info);
  typedef uint64_t(BRIDGEAPI_PTR* PFN_bridgeapi_CreateRectLight)(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoRectEXT* rect_info);
  typedef uint64_t(BRIDGEAPI_PTR* PFN_bridgeapi_CreateDiskLight)(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoDiskEXT* disk_info);
  typedef uint64_t(BRIDGEAPI_PTR* PFN_bridgeapi_CreateCylinderLight)(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoCylinderEXT* cylinder_info);
  typedef uint64_t(BRIDGEAPI_PTR* PFN_bridgeapi_CreateDistantLight)(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoDistantEXT* dist_info);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_DestroyLight)(uint64_t handle);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_DrawLightInstance)(uint64_t handle);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_SetConfigVariable)(const char* var, const char* value);
  typedef void(BRIDGEAPI_PTR* PFN_bridgeapi_RegisterDevice)(void);

  typedef struct bridgeapi_Interface {
    bool initialized;
    PFN_bridgeapi_DebugPrint           DebugPrint;            // const char* text
    PFN_bridgeapi_CreateOpaqueMaterial CreateOpaqueMaterial;  // x86::remixapi_MaterialInfo* info
    PFN_bridgeapi_DestroyMaterial      DestroyMaterial;       // uint64_t handle
    PFN_bridgeapi_CreateTriangleMesh   CreateTriangleMesh;    // x86::remixapi_MeshInfo* info
    PFN_bridgeapi_DestroyMesh          DestroyMesh;           // uint64_t handle
    PFN_bridgeapi_DrawMeshInstance     DrawMeshInstance;      // uint64_t handle --- x86::remixapi_Transform* t --- x86::remixapi_Bool double_sided
    PFN_bridgeapi_CreateSphereLight    CreateSphereLight;     // x86::remixapi_LightInfo* info --- x86::remixapi_LightInfoSphereEXT* sphere_info
    PFN_bridgeapi_CreateRectLight      CreateRectLight;       // x86::remixapi_LightInfo* info --- x86::remixapi_LightInfoRectEXT* rect_info
    PFN_bridgeapi_CreateDiskLight      CreateDiskLight;       // x86::remixapi_LightInfo* info --- x86::remixapi_LightInfoDiskEXT* disk_info
    PFN_bridgeapi_CreateCylinderLight  CreateCylinderLight;   // x86::remixapi_LightInfo* info --- x86::remixapi_LightInfoCylinderEXT* cylinder_info
    PFN_bridgeapi_CreateDistantLight   CreateDistantLight;    // x86::remixapi_LightInfo* info --- x86::remixapi_LightInfoDistantEXT* dist_info
    PFN_bridgeapi_DestroyLight         DestroyLight;          // uint64_t handle
    PFN_bridgeapi_DrawLightInstance    DrawLightInstance;     // uint64_t handle
    PFN_bridgeapi_SetConfigVariable    SetConfigVariable;     // const char* var --- const char* value
    PFN_bridgeapi_RegisterDevice       RegisterDevice;        // void
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
	  	pfn_Initialize = (PFN_bridgeapi_InitFuncs) func;
	  }
	  else {
	  	return BRIDGEAPI_ERROR_CODE_GET_PROC_ADDRESS_FAILURE;
	  }
	  
	  bridgeapi_Interface bridgeInterface = { 0 };
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