#pragma once

#include "variant.h"

namespace oc {
	class Imodule;
	class Imodule {
	public:
		Imodule() {
			// Do not put the code here
		}

		~Imodule() {
			// Do not put the code here
		}

		virtual variant run(variant &inputs) = 0;
		virtual variant initialize(variant &inputs) = 0;
		virtual variant dispose() = 0;
	};
}
