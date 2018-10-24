#include "expoter.h"
#include "module.h"


extern "C" __declspec(dllexport) oc::Imodule * CreateInstance() {
	oc::Imodule * module = (oc::Imodule *)new oc::omodule();
	return module;
}



extern "C" __declspec(dllexport) void DeleteInstance(oc::Imodule * _instance) {

	if (_instance != nullptr) {
		oc::omodule * module = (oc::omodule *)_instance;
		delete module;
	}

}