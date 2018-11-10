#include "expoter.h"
#include "module_mobile_segmentation_v3.h"

extern "C" __declspec(dllexport) oc::Imodule * CreateInstance() {
	oc::Imodule * module = (oc::Imodule *)new oc::omodule_mobile_segmentation_v3();
	return module;
}



extern "C" __declspec(dllexport) void DeleteInstance(oc::Imodule * _instance) {

	if (_instance != nullptr) {
		oc::omodule_mobile_segmentation_v3 * module = (oc::omodule_mobile_segmentation_v3 *)_instance;
		delete module;
	}

}