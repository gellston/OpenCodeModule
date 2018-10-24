#pragma once

#include "Imodule.h"


extern "C" __declspec(dllexport) oc::Imodule * CreateInstance();
extern "C" __declspec(dllexport) void DeleteInstance(oc::Imodule * _instance);