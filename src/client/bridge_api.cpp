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

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateTriangleMesh(const x86::remixapi_MeshInfo* info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateTriangleMesh);
      currentUID = c.get_uid();

      //Logger::info("bridgeapi_CreateTriangleMesh::");
      //Logger::info("|> surface_count = " + std::to_string(info->surfaces_count));

      // remixapi_MeshInfo
      SEND_STYPE(c, info->sType);
      SEND_U64(c, info->hash);

      // send each surface
      SEND_U32(c, info->surfaces_count); // send surface count before sending the surfaces
      for (uint32_t s = 0u; s < info->surfaces_count; s++) 
      {
        const auto& surf = info->surfaces_values[s];
        //Logger::info("|> surface " + std::to_string(s));

        // send vertices of the current surface
        //Logger::info("|>> vertex count " + std::to_string(surf.vertices_count));
        SEND_U64(c, surf.vertices_count); // send vertex count before vertices
        for (uint64_t v = 0u; v < surf.vertices_count; v++)
        {
          const auto& vert = surf.vertices_values[v];
          SEND_FLOAT(c, vert.position[0]);
          SEND_FLOAT(c, vert.position[1]);
          SEND_FLOAT(c, vert.position[2]);

          SEND_FLOAT(c, vert.normal[0]);
          SEND_FLOAT(c, vert.normal[1]);
          SEND_FLOAT(c, vert.normal[2]);

          SEND_FLOAT(c, vert.texcoord[0]);
          SEND_FLOAT(c, vert.texcoord[1]);

          SEND_U32(c, vert.color);
        }

        // send indices of the current surface
        //Logger::info("|>> index count " + std::to_string(surf.indices_count));
        SEND_U64(c, surf.indices_count); // send index count before indices
        for (uint64_t i = 0u; i < surf.indices_count; i++) {
          SEND_U32(c, surf.indices_values[i]);
        }

        SEND_U32(c, surf.skinning_hasvalue);
        SEND_U64(c, (uint64_t) surf.material); // remixapi_MaterialHandle material
      }
    }

    WAIT_FOR_SERVER_RESPONSE("CreateMesh()", 0, currentUID);
    uint64_t result = DeviceBridge::get_data();
    DeviceBridge::pop_front();
    return result;
  }

  void BRIDGEAPI_CALL bridgeapi_DestroyMesh(uint64_t handle) {
    ClientMessage c(Commands::Api_DestroyMesh);
    SEND_U64(c, handle);
  }

  void BRIDGEAPI_CALL bridgeapi_DrawMeshInstance(uint64_t handle, const x86::remixapi_Transform* t, x86::remixapi_Bool double_sided) {
    ClientMessage c(Commands::Api_DrawMeshInstance);
    SEND_U64(c, handle);
    SEND_FLOAT(c, t->matrix[0][0]); SEND_FLOAT(c, t->matrix[0][1]); SEND_FLOAT(c, t->matrix[0][2]); SEND_FLOAT(c, t->matrix[0][3]);
    SEND_FLOAT(c, t->matrix[1][0]); SEND_FLOAT(c, t->matrix[1][1]); SEND_FLOAT(c, t->matrix[1][2]); SEND_FLOAT(c, t->matrix[1][3]);
    SEND_FLOAT(c, t->matrix[2][0]); SEND_FLOAT(c, t->matrix[2][1]); SEND_FLOAT(c, t->matrix[2][2]); SEND_FLOAT(c, t->matrix[2][3]);
    SEND_U32(c, double_sided);
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

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateCylinderLight(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoCylinderEXT* cylinder_info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateCylinderLight);
      currentUID = c.get_uid();

      // LightInfo
      SEND_STYPE(c, info->sType);
      SEND_U64(c, info->hash);
      SEND_FLOAT3D(c, info->radiance);

      // LightInfoCylinderEXT
      SEND_STYPE(c, cylinder_info->sType);
      SEND_FLOAT3D(c, cylinder_info->position);
      SEND_FLOAT(c, cylinder_info->radius);
      SEND_FLOAT3D(c, cylinder_info->axis);
      SEND_FLOAT(c, cylinder_info->axisLength);
    }

    WAIT_FOR_SERVER_RESPONSE("CreateLight()", 0, currentUID);
    uint64_t result = DeviceBridge::get_data();
    DeviceBridge::pop_front();
    return result;
  }

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateDistantLight(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoDistantEXT* dist_info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateDistantLight);
      currentUID = c.get_uid();

      // LightInfo
      SEND_STYPE(c, info->sType);
      SEND_U64(c, info->hash);
      SEND_FLOAT3D(c, info->radiance);

      // LightInfoDistantEXT
      SEND_STYPE(c, dist_info->sType);
      SEND_FLOAT3D(c, dist_info->direction);
      SEND_FLOAT(c, dist_info->angularDiameterDegrees);
    }

    WAIT_FOR_SERVER_RESPONSE("CreateLight()", 0, currentUID);
    uint64_t result = DeviceBridge::get_data();
    DeviceBridge::pop_front();
    return result;
  }

  void BRIDGEAPI_CALL bridgeapi_DestroyLight(uint64_t handle) {
    ClientMessage c(Commands::Api_DestroyLight);
    SEND_U64(c, handle);
  }

  void BRIDGEAPI_CALL bridgeapi_DrawLightInstance(uint64_t handle) {
    ClientMessage c(Commands::Api_DrawLightInstance);
    SEND_U64(c, handle);
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
        interf.CreateTriangleMesh = bridgeapi_CreateTriangleMesh;
        interf.DestroyMesh = bridgeapi_DestroyMesh;
        interf.DrawMeshInstance = bridgeapi_DrawMeshInstance;
        interf.CreateSphereLight = bridgeapi_CreateSphereLight;
        interf.CreateRectLight = bridgeapi_CreateRectLight;
        interf.CreateDiskLight = bridgeapi_CreateDiscLight;
        interf.CreateCylinderLight = bridgeapi_CreateCylinderLight;
        interf.CreateDistantLight = bridgeapi_CreateDistantLight;
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
