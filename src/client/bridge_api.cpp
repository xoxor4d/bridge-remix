// x86 client Bridge API implementation 

#include "bridge_api/bridge_c.h"

#include "log/log.h"
#include "util_devicecommand.h"

namespace {
  void BRIDGEAPI_CALL bridgeapi_DebugPrint(const char* text) {
    //Logger::info(std::string("[API-CL] sending DebugPrint: ") + std::string(text));
    ClientMessage command(Commands::Bridge_DebugMessage);
    command.send_data(1337);
    command.send_data(strlen(text), (void*) text);
  }

  void BRIDGEAPI_CALL bridgeapi_Present() {
    ClientMessage command(Commands::Api_Present);
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
        interf.SetConfigVariable = bridgeapi_SetConfigVariable;
        interf.RegisterDevice = bridgeapi_RegisterDevice;
      }
      *out_result = interf;
      return BRIDGEAPI_ERROR_CODE_SUCCESS;
    }
  }
}
