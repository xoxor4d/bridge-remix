// x86 client Bridge API implementation 

#include "bridge_api/bridge_c.h"

#include "log/log.h"
#include "util_bridgecommand.h"
#include "util_devicecommand.h"

namespace {
  void BRIDGEAPI_CALL bridgeapi_DebugPrint(const char* text) {
    ClientMessage command(Commands::Bridge_DebugMessage);
    command.send_data(1337);
    command.send_data(strlen(text), (void*) text);
  }

  void BRIDGEAPI_CALL bridgeapi_Present() {
    ClientMessage command(Commands::Api_Present);
  }

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateSphereLight(const x86::remixapi_LightInfo* info, x86::remixapi_LightInfoSphereEXT* sphere_info) {
    UID currentUID = 0;
    {
      ClientMessage command(Commands::Api_CreateSphereLight);
      currentUID = command.get_uid();

      // send LightInfo struct
      command.send_data((uint32_t) info->sType);
      command.send_data((int32_t) info->radiance.x);
      command.send_data((int32_t) info->radiance.y);
      command.send_data((int32_t) info->radiance.z);
      command.send_data((uint64_t) info->hash);

      command.send_data((uint32_t) sphere_info->sType);
      command.send_data((int32_t) sphere_info->position.x);
      command.send_data((int32_t) sphere_info->position.y);
      command.send_data((int32_t) sphere_info->position.z);
      command.send_data((int32_t) sphere_info->radius);

      //command.send_data(sizeof(x86::remixapi_LightInfo), (void*) info);
    }

    //WAIT_FOR_SERVER_RESPONSE
    {
      const uint32_t timeoutMs = GlobalOptions::getAckTimeout();
      if (Result::Success != DeviceBridge::waitForCommand(Commands::Bridge_Response, timeoutMs, nullptr, true, currentUID)) {
        Logger::err("CreateLight()" " failed with: no response from server.");
        return 0;
      }

      uint64_t res = DeviceBridge::get_data();
      DeviceBridge::pop_front();
      return res;
    }
  }

  void BRIDGEAPI_CALL bridgeapi_DestroyLight(uint64_t handle) {
    ClientMessage command(Commands::Api_DestroyLight);
    command.send_data((uint64_t) handle);
  }

  void BRIDGEAPI_CALL bridgeapi_DrawLightInstance(uint64_t handle) {
    ClientMessage command(Commands::Api_DrawLightInstance);
    command.send_data((uint64_t)handle);
  }

  void BRIDGEAPI_CALL bridgeapi_SetConfigVariable(const char* var, const char* value) {
    ClientMessage command(Commands::Api_SetConfigVariable);
    command.send_data(strlen(var), (void*) var);
    command.send_data(strlen(value), (void*) value);
  }

  void BRIDGEAPI_CALL bridgeapi_RegisterDevice() {
    ClientMessage command(Commands::Api_RegisterDevice);
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
