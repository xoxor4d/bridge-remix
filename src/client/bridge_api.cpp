/*
 * Copyright (c) 2023, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "log/log.h"
#include "util_bridgecommand.h"
#include "util_devicecommand.h"
#include "bridge_api.h"


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

#define SEND_INT(MSG, T) \
  (MSG).send_data((int32_t) (T))

#define SEND_STYPE(MSG, T) \
  (MSG).send_data((uint32_t) (T))

#define SEND_U32(MSG, U32) \
  (MSG).send_data((uint32_t) (U32))

#define SEND_U64(MSG, U64) \
  (MSG).send_data(sizeof(uint64_t), &(U64))

#define PULL_DATA(SIZE, NAME) \
            uint32_t NAME##_len = DeviceBridge::get_data((void**)&(NAME)); \
            assert(NAME##_len == 0 || (SIZE) == NAME##_len)

namespace BridgeApiCL {
  bool Initialized = false;
  PFN_bridgeapi_RegisterEndSceneCallback GameCallback = nullptr;
}

namespace {
  void BRIDGEAPI_CALL bridgeapi_DebugPrint(const char* text) {
    ClientMessage command(Commands::Bridge_DebugMessage);
    command.send_data(1337);
    command.send_data(strlen(text), (void*) text);
  }

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateOpaqueMaterial(const x86::remixapi_MaterialInfo* info, const x86::remixapi_MaterialInfoOpaqueEXT* opaque_info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateOpaqueMaterial);
      currentUID = c.get_uid();

      // MaterialInfo
      SEND_STYPE(c, info->sType);
      SEND_U64(c, info->hash);
      // path albedoTexture
      // path normalTexture
      // path tangentTexture
      // path emissiveTexture
      SEND_FLOAT(c, info->emissiveIntensity);
      SEND_FLOAT3D(c, info->emissiveColorConstant);
      c.send_data((uint8_t) info->spriteSheetRow);
      c.send_data((uint8_t) info->spriteSheetCol);
      c.send_data((uint8_t) info->spriteSheetFps);
      c.send_data((uint8_t) info->filterMode);
      c.send_data((uint8_t) info->wrapModeU);
      c.send_data((uint8_t) info->wrapModeV);

      // MaterialInfoOpaqueEXT
      SEND_STYPE(c, opaque_info->sType);
      // path roughnessTexture
      // path metallicTexture
      SEND_FLOAT(c, opaque_info->anisotropy);
      SEND_FLOAT3D(c, opaque_info->albedoConstant);
      SEND_FLOAT(c, opaque_info->opacityConstant);
      SEND_FLOAT(c, opaque_info->roughnessConstant);
      SEND_FLOAT(c, opaque_info->metallicConstant);
      SEND_U32(c, opaque_info->thinFilmThickness_hasvalue);
      SEND_FLOAT(c, opaque_info->thinFilmThickness_value);
      SEND_U32(c, opaque_info->alphaIsThinFilmThickness);
      // path heightTexture;
      SEND_FLOAT(c, opaque_info->heightTextureStrength);
      // If true, InstanceInfoBlendEXT is used as a source for alpha state
      SEND_U32(c, opaque_info->useDrawCallAlphaState);
      SEND_U32(c, opaque_info->blendType_hasvalue);
      SEND_INT(c, opaque_info->blendType_value);
      SEND_U32(c, opaque_info->invertedBlend);
      SEND_INT(c, opaque_info->alphaTestType);
      c.send_data((uint8_t) opaque_info->alphaReferenceValue);
    }

    WAIT_FOR_SERVER_RESPONSE("CreateMaterial()", 0, currentUID);
    uint64_t* result = nullptr;
    PULL_DATA(sizeof(uint64_t), result);
    DeviceBridge::pop_front();
    return *result;
  }

  void BRIDGEAPI_CALL bridgeapi_DestroyMaterial(uint64_t handle) {
    ClientMessage c(Commands::Api_DestroyMaterial);
    SEND_U64(c, handle);
  }

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateTriangleMesh(const x86::remixapi_MeshInfo* info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateTriangleMesh);
      currentUID = c.get_uid();

      //Logger::debug("[BridgeApi-CL] CreateTriangleMesh ::");
      //Logger::debug("|> surface_count = " + std::to_string(info->surfaces_count));

      // MeshInfo
      SEND_STYPE(c, info->sType);
      SEND_U64(c, info->hash);

      // send each surface
      SEND_U32(c, info->surfaces_count); // send surface count before sending the surfaces
      for (uint32_t s = 0u; s < info->surfaces_count; s++) 
      {
        const auto& surf = info->surfaces_values[s];
        //Logger::debug("|> surface " + std::to_string(s));

        // send vertices of the current surface
        //Logger::debug("|>> vertex count " + std::to_string(surf.vertices_count));
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
        //Logger::debug("|>> index count " + std::to_string(surf.indices_count));
        SEND_U64(c, surf.indices_count); // send index count before indices
        for (uint64_t i = 0u; i < surf.indices_count; i++) {
          SEND_U32(c, surf.indices_values[i]);
        }

        SEND_U32(c, surf.skinning_hasvalue);

        // using remixapi_MaterialHandle is unpractical and kinda unsafe because its only 4 bytes <here> (ptr)
        // so user would have to send an actual pointer instead of the uint64_t hash val and we would have to
        // make sure to send the proper 8 bytes of the uint64_t
        //Logger::debug("[BridgeApi-CL] RemixApi::CreateTriangleMesh() sending material u32 [" + std::to_string((uint32_t) surf.material) + "]");
        //Logger::debug("[BridgeApi-CL] RemixApi::CreateTriangleMesh() sending material u64 [" + std::to_string((uint64_t) surf.material) + "]");
        SEND_U64(c, surf.material);
      }
    }

    WAIT_FOR_SERVER_RESPONSE("CreateMesh()", 0, currentUID);
    uint64_t* result = nullptr;
    PULL_DATA(sizeof(uint64_t), result);
    DeviceBridge::pop_front();
    return *result;
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
    uint64_t* result = nullptr;
    PULL_DATA(sizeof(uint64_t), result);
    DeviceBridge::pop_front();
    return *result;
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
    uint64_t* result = nullptr;
    PULL_DATA(sizeof(uint64_t), result);
    DeviceBridge::pop_front();
    return *result;
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
    uint64_t* result = nullptr;
    PULL_DATA(sizeof(uint64_t), result);
    DeviceBridge::pop_front();
    return *result;
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
    uint64_t* result = nullptr;
    PULL_DATA(sizeof(uint64_t), result);
    DeviceBridge::pop_front();
    return *result;
  }

  uint64_t BRIDGEAPI_CALL bridgeapi_CreateDistantLight(const x86::remixapi_LightInfo* info, const x86::remixapi_LightInfoDistantEXT* dist_info) {
    UID currentUID = 0;
    {
      ClientMessage c(Commands::Api_CreateDistantLight);
      currentUID = c.get_uid();

      //Logger::debug("[BridgeApi-CL] CreateDistantLight ::");
      //Logger::debug("|> info: sType[" +std::to_string(info->sType) + "]");
      //Logger::debug("|> info: hash[" + std::to_string(info->hash) + "]");
      //Logger::debug("|> ext: sType[" + std::to_string(dist_info->sType) + "]");
      //Logger::debug("|> ext: angularDiameterDegrees[" + std::to_string(dist_info->angularDiameterDegrees) + "]");

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
    uint64_t* result = nullptr;
    PULL_DATA(sizeof(uint64_t), result);
    DeviceBridge::pop_front();
    return *result;
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

  BRIDGE_API void bridgeapi_RegisterEndSceneCallback(PFN_bridgeapi_RegisterEndSceneCallback callback) {
    BridgeApiCL::GameCallback = callback;
  }

  extern "C" {
    BRIDGE_API BRIDGEAPI_ErrorCode __cdecl bridgeapi_InitFuncs(bridgeapi_Interface* out_result) {
      if (!out_result) {
        return BRIDGEAPI_ERROR_CODE_INVALID_ARGUMENTS;
      }
      auto interf = bridgeapi_Interface {};
      {
        interf.DebugPrint = bridgeapi_DebugPrint;
        interf.CreateOpaqueMaterial = bridgeapi_CreateOpaqueMaterial;
        interf.DestroyMaterial = bridgeapi_DestroyMaterial;
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
        interf.RegisterEndSceneCallback = bridgeapi_RegisterEndSceneCallback;
      }

      *out_result = interf;
      BridgeApiCL::Initialized = true;

      return BRIDGEAPI_ERROR_CODE_SUCCESS;
    }
  }
}
