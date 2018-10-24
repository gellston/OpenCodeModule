#pragma once

#include "Imodule.h"


namespace oc {
	class omodule : oc::Imodule {

	private:

		variant variables;

	public:

		omodule() {

		}

		~omodule() {

		}

		variant run(variant &inputs) override;
		variant initialize(variant &inputs) override;
		variant dispose() override;

	};
}
