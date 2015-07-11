/*
 * Copyright 2011-2015 Blender Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdlib>
#include <cstdio>
#include <vector>

#include "clew.h"

using std::vector;

#define LOG(...) \
  do { \
    fprintf(stderr, __VA_ARGS__); \
    fflush(stderr); \
  } while (false)
#define opencl_assert(stmt) \
  do { \
    cl_int err = stmt; \
    if (err != CL_SUCCESS) { \
      LOG("OpenCL error: %s in %s", clewErrorString(err), #stmt); \
    } \
  } while (false)

int main(int argc, char **argv) {
  LOG("Initializing clew...\n");
  int result = clewInit();
  if (result != CLEW_SUCCESS) {
    LOG("Error initializing clew: %d\n", result);
    return EXIT_FAILURE;
  }
  LOG("clew initialization success!\n");

#define LOG_SYMBOL(sym) LOG("  " #sym ": %p\n", sym);
  LOG("Symbol table:\n");
  LOG_SYMBOL(clGetPlatformIDs)
  LOG_SYMBOL(clGetDeviceIDs)
  LOG_SYMBOL(clGetPlatformInfo)
  LOG_SYMBOL(clGetDeviceInfo)
#undef LOG_SYMBOL

  cl_uint num_platforms = 0;

  LOG("Getting number of platforms...\n");
  opencl_assert(clGetPlatformIDs(0, NULL, &num_platforms));
  LOG("Found %d platforms\n", num_platforms);
  if (num_platforms == 0) {
    LOG("No platforms found, performing early exit\n");
    return EXIT_SUCCESS;
  }

  vector<cl_platform_id> platform_ids;
  platform_ids.resize(num_platforms);

  LOG("Getting platform IDs...\n");
  opencl_assert(clGetPlatformIDs(num_platforms, &platform_ids[0], NULL));
  LOG("Getting platform IDs succeeded.\n");

  vector<cl_device_id> device_ids;
  for (cl_uint platform = 0; platform < num_platforms; ++platform) {
    LOG("Getting information for platform %d\n", platform);
    cl_uint num_devices = 0;
    LOG("Getting number of devices for the platform...\n");
    opencl_assert(clGetDeviceIDs(platform_ids[platform],
                                 CL_DEVICE_TYPE_ALL,
                                 0,
                                 NULL,
                                 &num_devices));
    LOG("Found %d devices for the platform.\n", num_devices);
    if (num_devices == 0) {
      LOG("Ignoring the platform due to lack of the devices.\n");
      continue;
    }

    device_ids.resize(num_devices);

    LOG("Getting device IDs for the platform...\n");
    opencl_assert(clGetDeviceIDs(platform_ids[platform],
                                 CL_DEVICE_TYPE_ALL,
                                 num_devices,
                                 &device_ids[0],
                                 NULL));
    LOG("Getting devices IDs succeeded.\n");

    char pname[256];
    LOG("Getting platform name...\n");
    opencl_assert(clGetPlatformInfo(platform_ids[platform],
                                    CL_PLATFORM_NAME,
                                    sizeof(pname),
                                    pname,
                                    NULL));
    printf("Platform name is: %s\n", pname);

    LOG("Getting info for each of the devices...\n");
    for (cl_uint num = 0; num < num_devices; ++num) {
      cl_device_id device_id = device_ids[num];

      cl_device_type device_type;
      LOG("Getting device type...\n");
      opencl_assert(clGetDeviceInfo(device_id,
                                    CL_DEVICE_TYPE,
                                    sizeof(cl_device_type),
                                    &device_type,
                                    NULL));
      LOG("Device type is: %lu\n", device_type);

      char name[1024] = "\0";
      LOG("Getting device %d name...\n", num);
      opencl_assert(clGetDeviceInfo(device_id,
                                    CL_DEVICE_NAME,
                                    sizeof(name),
                                    name,
                                    NULL));
      LOG("Device name is: %s\n", name);
    }
  }
  LOG("All done!\n");
  return EXIT_SUCCESS;
}
