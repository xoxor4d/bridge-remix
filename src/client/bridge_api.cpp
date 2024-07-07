// x86 client Bridge API implementation 

#include "bridge_api/bridge_c.h"

#include "log/log.h"
#include "util_bridgecommand.h"
#include "util_devicecommand.h"

#define SEND_FLOAT(MSG, V) \
  (MSG).send_data((uint32_t) *(uint32_t*)(&(V)))

#define SEND_FLOAT2D(MSG, V) \
  SEND_FLOAT(MSG, (V).x); \
  SEND_FLOAT(MSG, (V).y)

#define SEND_FLOAT3D(MSG, V) \
  SEND_FLOAT(MSG, (V).x); \
  SEND_FLOAT(MSG, (V).y); \
  SEND_FLOAT(MSG, (V).z)

#define SEND_FLOAT4D(MSG, V) \
  SEND_FLOAT(MSG, (V).x); \
  SEND_FLOAT(MSG, (V).y); \
  SEND_FLOAT(MSG, (V).z); \
  SEND_FLOAT(MSG, (V).w)

#define SEND_STYPE(MSG, T) \
  (MSG).send_data((uint32_t) (T))

#define SEND_U32(MSG, U32) \
  (MSG).send_data((uint32_t) (U32))

#define SEND_U64(MSG, U64) \
  (MSG).send_data((uint64_t) (U64))

namespace {
  void BRIDGEAPI_CALL bridgeapi_DebugPrint(const char* text) {
    ClientMessage command(Commands::Bridge_DebugMessage);
    command.send_data(1337);
    command.send_data(strlen(text), (void*) text);
  }

  void BRIDGEAPI_CALL bridgeapi_Present() {
    ClientMessage command(Commands::Api_Present);
  }

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateSphereLight(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoSphereEXT* sphere_info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateSphereLight);
      currentUID = c.get_uid();

      // LightInfo
      SEND_STYPE(c, info->sType);
      SEND_U64(c, info->hash);
      SEND_FLOAT3D(c, info->radiance);

      // LightInfoSphereEXT
      SEND_STYPE(c, sphere_info->sType);
      SEND_FLOAT3D(c, sphere_info->position);
      SEND_FLOAT(c, sphere_info->radius);
      SEND_U32(c, sphere_info->shaping_hasvalue);

      if (sphere_info->shaping_hasvalue) {
        SEND_FLOAT3D(c, sphere_info->shaping_value.direction);
        SEND_FLOAT(c, sphere_info->shaping_value.coneAngleDegrees);
        SEND_FLOAT(c, sphere_info->shaping_value.coneSoftness);
        SEND_FLOAT(c, sphere_info->shaping_value.focusExponent);
      }
    }

    WAIT_FOR_SERVER_RESPONSE("CreateLight()", 0, currentUID);
    uint64_t result = DeviceBridge::get_data();
    DeviceBridge::pop_front();
    return result;
  }

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateRectLight(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoRectEXT* rect_info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateRectLight);
      currentUID = c.get_uid();

      // LightInfo
      SEND_STYPE(c, info->sType);
      SEND_U64(c, info->hash);
      SEND_FLOAT3D(c, info->radiance);

      // LightInfoRectEXT
      SEND_STYPE(c, rect_info->sType);
      SEND_FLOAT3D(c, rect_info->position);
      SEND_FLOAT3D(c, rect_info->xAxis);
      SEND_FLOAT(c, rect_info->xSize);
      SEND_FLOAT3D(c, rect_info->yAxis);
      SEND_FLOAT(c, rect_info->ySize);
      SEND_FLOAT3D(c, rect_info->direction);
      SEND_U32(c, rect_info->shaping_hasvalue);

      if (rect_info->shaping_hasvalue) {
        SEND_FLOAT3D(c, rect_info->shaping_value.direction);
        SEND_FLOAT(c, rect_info->shaping_value.coneAngleDegrees);
        SEND_FLOAT(c, rect_info->shaping_value.coneSoftness);
        SEND_FLOAT(c, rect_info->shaping_value.focusExponent);
      }
    }

    WAIT_FOR_SERVER_RESPONSE("CreateLight()", 0, currentUID);
    uint64_t result = DeviceBridge::get_data();
    DeviceBridge::pop_front();
    return result;
  }

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateDiscLight(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoDiskEXT* disk_info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateDiskLight);
      currentUID = c.get_uid();

      // LightInfo
      SEND_STYPE(c, info->sType);
      SEND_U64(c, info->hash);
      SEND_FLOAT3D(c, info->radiance);

      // LightInfoDiskEXT
      SEND_STYPE(c, disk_info->sType);
      SEND_FLOAT3D(c, disk_info->position);
      SEND_FLOAT3D(c, disk_info->xAxis);
      SEND_FLOAT(c, disk_info->xRadius);
      SEND_FLOAT3D(c, disk_info->yAxis);
      SEND_FLOAT(c, disk_info->yRadius);
      SEND_FLOAT3D(c, disk_info->direction);
      SEND_U32(c, disk_info->shaping_hasvalue);

      if (disk_info->shaping_hasvalue) {
        SEND_FLOAT3D(c, disk_info->shaping_value.direction);
        SEND_FLOAT(c, disk_info->shaping_value.coneAngleDegrees);
        SEND_FLOAT(c, disk_info->shaping_value.coneSoftness);
        SEND_FLOAT(c, disk_info->shaping_value.focusExponent);
      }
    }

    WAIT_FOR_SERVER_RESPONSE("CreateLight()", 0, currentUID);
    uint64_t result = DeviceBridge::get_data();
    DeviceBridge::pop_front();
    return result;
  }

  void BRIDGEAPI_CALL bridgeapi_DestroyLight(uint64_t handle) {
    ClientMessage c(Commands::Api_DestroyLight);
    c.send_data((uint64_t) handle);
  }

  void BRIDGEAPI_CALL bridgeapi_DrawLightInstance(uint64_t handle) {
    ClientMessage c(Commands::Api_DrawLightInstance);
    c.send_data((uint64_t)handle);
  }

  void BRIDGEAPI_CALL bridgeapi_SetConfigVariable(const char* var, const char* value) {
    ClientMessage command(Commands::Api_SetConfigVariable);
    command.send_data(strlen(var), (void*) var);
    command.send_data(strlen(value), (void*) value);
  }

  void BRIDGEAPI_CALL bridgeapi_RegisterDevice() {
    ClientMessage c(Commands::Api_RegisterDevice);
  }

  extern "C" {
    BRIDGE_API BRIDGEAPI_ErrorCode __cdecl bridgeapi_InitFuncs(bridgeapi_Interface* out_result) {
      if (!out_result) {
        return BRIDGEAPI_ERROR_CODE_INVALID_ARGUMENTS;
      }
      auto interf = bridgeapi_Interface {};
      {
        interf.DebugPrint = bridgeapi_DebugPrint;
        interf.Present = bridgeapi_Present;
        interf.CreateSphereLight = bridgeapi_CreateSphereLight;
        interf.CreateRectLight = bridgeapi_CreateRectLight;
        interf.CreateDiskLight = bridgeapi_CreateDiscLight;
        interf.DestroyLight = bridgeapi_DestroyLight;
        interf.DrawLightInstance = bridgeapi_DrawLightInstance;
        interf.SetConfigVariable = bridgeapi_SetConfigVariable;
        interf.RegisterDevice = bridgeapi_RegisterDevice;
      }
      *out_result = interf;
      return BRIDGEAPI_ERROR_CODE_SUCCESS;
    }
  }
}
