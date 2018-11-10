#include "expoter.h"
#include "module_darknet19_animal_classification.h"


extern "C" __declspec(dllexport) oc::Imodule * CreateInstance() {
	oc::Imodule * module = (oc::Imodule *)new oc::omodule_darknet19_animal_classification();
	return module;
}



extern "C" __declspec(dllexport) void DeleteInstance(oc::Imodule * _instance) {

	if (_instance != nullptr) {
		oc::omodule_darknet19_animal_classification * module = (oc::omodule_darknet19_animal_classification *)_instance;
		delete module;
	}

}