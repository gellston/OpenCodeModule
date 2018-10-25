#pragma once

#include "Imodule.h"

#include "tf_utils.hpp"

namespace oc {
	class omodule : oc::Imodule {

	private:
		/// local variable
		bool _is_load;
		
		int _image_width;
		int _image_height;
		int _image_channel;
		long _image_size;

		TF_Graph* _graph;
		TF_Status* _status; // used to hold all statuses about TF operations
		TF_SessionOptions* _sess_opts;
		TF_Session * _session;

		TF_Output input_op;
		TF_Output phase_op;
		TF_Output output_op;

	public:

		omodule() {
			this->_is_load = false;
			this->_image_width = 0;
			this->_image_height = 0;
			this->_image_channel = 0;
			this->_image_size = 0;

			this->_graph = nullptr;
			this->_status = nullptr;
			this->_sess_opts = nullptr;
			this->_session = nullptr;
		}

		~omodule() {

		}

		variant run(variant &inputs) override;
		variant initialize(variant &inputs) override;
		variant dispose() override;

	};
}
