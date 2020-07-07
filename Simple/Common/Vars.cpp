#include "stdafx.h"

#include <new>

#include "ProudNetClient.h"
#include "Vars.h"

using namespace std;
using namespace Proud;

// Protocol version that you define.
// Your server app and client app must have the same value below.
PNGUID guid = { 0x3ae33249, 0xecc6, 0x4980, { 0xbc, 0x5d, 0x7b, 0xa, 0x99, 0x9c, 0x7, 0x39 } };
Guid g_Version = Guid(guid);

// TCP listening port number.
int g_ServerPort = 33334;
