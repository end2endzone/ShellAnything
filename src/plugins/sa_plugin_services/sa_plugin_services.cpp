/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include <cstdio>
#include "shellanything/sa_logging.h"
#include "shellanything/sa_error.h"
#include "shellanything/sa_plugin.h"
#include "shellanything/sa_selection_context.h"
#include "shellanything/sa_properties.h"
#include <string>
#include <Windows.h>

#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

static const char* PLUGIN_NAME_IDENTIFIER = "sa_plugin_services";

const char* get_service_status(const char* name)
{
  SC_HANDLE hService, hSCManager;
  SERVICE_STATUS_PROCESS ssp;
  DWORD dwBytesNeeded;
  BOOL result;

  static const char* EMPTY_STATUS = "";

  hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
  if (!hSCManager) {
    return EMPTY_STATUS;
  }

  hService = OpenService(hSCManager, name, SERVICE_QUERY_STATUS);
  if (!hService) {
    CloseServiceHandle(hSCManager);
    return EMPTY_STATUS;
  }

  result = QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO,
    reinterpret_cast<LPBYTE>(&ssp), sizeof(SERVICE_STATUS_PROCESS),
    &dwBytesNeeded);
  if (result == 0) {
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return EMPTY_STATUS;
  }

  CloseServiceHandle(hService);
  CloseServiceHandle(hSCManager);

  const char* sStatus = NULL;
  switch (ssp.dwCurrentState) {
  case SERVICE_CONTINUE_PENDING:    sStatus ="continuing"; break;
  case SERVICE_PAUSE_PENDING:       sStatus ="pausing";    break;
  case SERVICE_PAUSED:              sStatus ="paused";     break;
  case SERVICE_RUNNING:             sStatus ="running";    break;
  case SERVICE_START_PENDING:       sStatus ="starting";   break;
  case SERVICE_STOP_PENDING:        sStatus ="stopping";   break;
  case SERVICE_STOPPED:             sStatus ="stopped";    break;
  default:
    sStatus = "unknown";
  };

  return sStatus;
}

void replace_spaces(char * buffer, size_t buffer_size)
{
  if (buffer == NULL || buffer_size == 0)
    return;
  
  while (buffer[0] != '\0')
  {
    if (buffer[0] == ' ')
      buffer[0] = '_';
    buffer++;
  }
}

void process_service_name(const char* service_name)
{
  if (service_name == NULL || service_name[0] == '\0')
    return;

  // get the service status
  const char* status = get_service_status(service_name);

  // build a property name specific to this name
  static const size_t PROPERTY_NAME_LENGTH = 2048;
  char property_name[PROPERTY_NAME_LENGTH];
  memset(property_name, 0, PROPERTY_NAME_LENGTH);
  strcat(property_name, PLUGIN_NAME_IDENTIFIER);
  strcat(property_name, ".");
  strcat(property_name, service_name);
  strcat(property_name, ".");
  strcat(property_name, "status");

  // make sure there is no space in the property name
  replace_spaces(property_name, PROPERTY_NAME_LENGTH);

  // set the output property value
  sa_properties_set(property_name, status);
}

void sa_plugin_services_update_callback()
{
  // Get the property with the services names
  static const char* property_name = "sa_plugin_services.names";
  static const char property_separator = ';';

  if (!sa_properties_exists(property_name))
    return;

  static const size_t NAMES_LENGTH = 2048;
  char names[NAMES_LENGTH];
  int property_length = 0;
  sa_error_t result = sa_properties_get_buffer(property_name, &property_length, names, NAMES_LENGTH);
  if (result != SA_ERROR_SUCCESS)
  {
    const char * error_desc = sa_error_get_error_description(result);
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed getting property '%s'. Error: %s.", property_name, error_desc);
    return;
  }

  // for each name in names
  static const size_t SERVICE_NAME_LENGTH = 512;
  char name[SERVICE_NAME_LENGTH];
  memset(name, 0, SERVICE_NAME_LENGTH);
  size_t names_length = strlen(names);
  size_t name_length = 0;
  for (size_t i = 0; i < names_length; i++)
  {
    char c = names[i];
    if (c == property_separator || c == '\0')
    {
      if (name[0] != '\0')
      {
        process_service_name(name);
        memset(name, 0, SERVICE_NAME_LENGTH);
        name_length = 0;
      }
    }
    else
    {
      if (name_length + 1 < SERVICE_NAME_LENGTH)
      {
        // append next character to the name string
        char tmp[2];
        tmp[0] = c;
        tmp[1] = '\0';
        strcat(name, tmp);
        name_length++;
      }
    }
  }
  if (name[0] != '\0')
  {
    process_service_name(name);
    memset(name, 0, SERVICE_NAME_LENGTH);
    name_length = 0;
  }
}

EXPORT_API sa_error_t sa_plugin_initialize(sa_version_info_t* version)
{
  if (version->major == 0 && version->minor < 8) // this plugin is designed for version 0.8.0 and over
    return SA_ERROR_NOT_SUPPORTED;
  return SA_ERROR_SUCCESS;
}

EXPORT_API sa_error_t sa_plugin_register()
{
  // register update callback function.
  sa_plugin_config_update_func update_func = &sa_plugin_services_update_callback;
  sa_error_t result = sa_plugin_register_config_update(update_func);
  if (result != SA_ERROR_SUCCESS)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, PLUGIN_NAME_IDENTIFIER, "Failed registering update callback function.");
    return result;
  }

  return SA_ERROR_SUCCESS;
}

void test()
{
#define ASSERT(expression) if (!(expression)) { printf("The assertion '%s' at line %d has failed.\n", #expression, __LINE__); exit(1); }

  printf("%s()\n", __FUNCTION__);

  bool a = false;
  ASSERT(a == true);

  printf("All tests in function '%s' has passed!\n", __FUNCTION__);
#undef ASSERT
}

#ifdef __cplusplus
}
#endif
