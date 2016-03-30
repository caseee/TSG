/*
 * TeamSpeak 3 demo plugin
 *
 * Copyright (c) 2008-2015 TeamSpeak Systems GmbH
 */

#ifdef _WIN32
#pragma warning (disable : 4100)  /* Disable Unreferenced parameter warning */
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "plugin.h"

#include "LogitechGkeyLib.h"
#pragma comment(lib, "LogitechGkeyLib.lib")

void __cdecl GkeySDKCallback(GkeyCode gkeyCode, wchar_t* gkeyOrButtonString, void* /*pContext*/)
{


}

static struct TS3Functions ts3Functions;

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); (dest)[destSize-1] = '\0'; }
#endif

#define PLUGIN_API_VERSION 20

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

static char* pluginID = NULL;

#ifdef _WIN32
/* Helper function to convert wchar_T to Utf-8 encoded strings on Windows */
static int wcharToUtf8(const wchar_t* str, char** result) {
   int outlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, 0, 0);
   *result = (char*)malloc(outlen);
   if (WideCharToMultiByte(CP_UTF8, 0, str, -1, *result, outlen, 0, 0) == 0) {
      *result = NULL;
      return -1;
   }
   return 0;
}
#endif

/*********************************** Required functions ************************************/
/*
 * If any of these required functions is not implemented, TS3 will refuse to load the plugin
 */

 /* Unique name identifying this plugin */
const char* ts3plugin_name() {
#ifdef _WIN32
   /* TeamSpeak expects UTF-8 encoded characters. Following demonstrates a possibility how to convert UTF-16 wchar_t into UTF-8. */
   static char* result = NULL;  /* Static variable so it's allocated only once */
   if (!result) {
      const wchar_t* name = L"TSG - Logitech G-Key Teamspeak plugin";
      if (wcharToUtf8(name, &result) == -1) {  /* Convert name into UTF-8 encoded result */
         result = "TSG - Logitech G-Key Teamspeak plugin";  /* Conversion failed, fallback here */
      }
   }
   return result;
#else
   return "Test Plugin";
#endif
}

/* Plugin version */
const char* ts3plugin_version() {
   return "0.1";
}

/* Plugin API version. Must be the same as the clients API major version, else the plugin fails to load. */
int ts3plugin_apiVersion() {
   return PLUGIN_API_VERSION;
}

/* Plugin author */
const char* ts3plugin_author() {
   /* If you want to use wchar_t, see ts3plugin_name() on how to use */
   return "Marco Casella";
}

/* Plugin description */
const char* ts3plugin_description() {
   /* If you want to use wchar_t, see ts3plugin_name() on how to use */
   return "This plugin allow to use logitech g-key with teamspeak 3.";
}

/* Set TeamSpeak 3 callback functions */
void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
   ts3Functions = funcs;
}

int ts3plugin_init() {
   logiGkeyCBContext gkeyContext;
   ZeroMemory(&gkeyContext, sizeof(gkeyContext));
   gkeyContext.gkeyCallBack = (logiGkeyCB)GkeySDKCallback;
   gkeyContext.gkeyContext = NULL;

   LogiGkeyInit(&gkeyContext);
   
   return 0;  
}


void ts3plugin_shutdown() {
   LogiGkeyShutdown();
}

