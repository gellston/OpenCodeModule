
#include "module.h"
#include "image.h"


oc::variant oc::omodule::initialize(oc::variant &inputs) {


	std::cout << "Initialized !! Test" << std::endl;



	oc::variant outputs;
	return outputs;
}

oc::variant oc::omodule::run(oc::variant &inputs){


	std::cout << "run !! Test" << std::endl;

	variant outputs;
	return outputs;
}

oc::variant oc::omodule::dispose() {

	std::cout << "dispose !! Test" << std::endl;

	variant outputs;
	return outputs;
}