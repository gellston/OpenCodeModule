#include "module_mobile_segmentation_v3.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <numeric>

static void deallocator(void* data, size_t length, void* arg) {
	///free(data);
}

static void free_buffer(void* data, size_t length) {
	free(data);
}

static TF_Buffer* load_pb_file(const std::string& filename) {
	std::streampos fsize = 0;
	std::ifstream file(filename, std::ios::binary);

	// get data size
	fsize = file.tellg();
	file.seekg(0, std::ios::end);
	fsize = file.tellg() - fsize;

	// reset stream
	file.seekg(0, std::ios::beg);

	char* data = new char[fsize];
	file.read(data, fsize);

	file.close();


	TF_Buffer* graph_def = TF_NewBuffer();
	graph_def->data = data;
	graph_def->length = fsize;
	graph_def->data_deallocator = free_buffer;
	return graph_def;
}

oc::variant oc::omodule_mobile_segmentation_v3::initialize(oc::variant &inputs) {

	oc::variant outputs;

	if (this->_is_load == true) {
		std::cout << "already initialized" << std::endl;
		outputs << "already initialized";
	}

	const char * _model_path = nullptr;

	try {
		inputs >> _model_path;
		inputs >> this->_image_width;
		inputs >> this->_image_height;
		inputs >> this->_image_channel;
	}
	catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
		outputs << e.what();
		return outputs;
	}

	this->_image_size = this->_image_channel * this->_image_height * this->_image_width;

	std::cout << "image channel :" << this->_image_channel << std::endl;
	std::cout << "image width :" << this->_image_width << std::endl;
	std::cout << "image height :" << this->_image_height << std::endl;
	std::cout << "image size = " << this->_image_size << std::endl;

	TF_Buffer* graph_def = load_pb_file(_model_path);

	// allocating space for private member variables
	_status = TF_NewStatus();
	_graph = TF_NewGraph();

	// Import graph_def into graph
	TF_ImportGraphDefOptions* opts = TF_NewImportGraphDefOptions();
	TF_GraphImportGraphDef(_graph, graph_def, opts, _status);
	TF_DeleteImportGraphDefOptions(opts);
	if (TF_GetCode(_status) != TF_OK) {
		std::cerr << "ERROR: Unable to import graph " << TF_Message(_status) << "\n";
		outputs << "ERROR: Unable to import graph ";
		return outputs;
	}
	std::cout << "Successfully imported graph\n";
	TF_DeleteBuffer(graph_def);

	// creating new session variables
	_sess_opts = TF_NewSessionOptions();
	_session = TF_NewSession(_graph, _sess_opts, _status);
	if (TF_GetCode(_status) != TF_OK) {
		outputs << TF_Message(_status);
		std::cout << TF_Message(_status) << std::endl;
		return outputs;
	}

	this->_is_load = true;

	outputs << "init success";
	return outputs;
}

oc::variant oc::omodule_mobile_segmentation_v3::run(oc::variant &inputs) {
	variant outputs;

	if (this->_is_load == false) {
		std::cout << "not initialized" << std::endl;
		outputs << "not initialized";
	}

	float * _image_buffer = nullptr;
	float * _output_buffer = nullptr;
	bool    _phase = false;

	try {
		inputs >> _image_buffer;
		inputs >> _output_buffer;
		inputs >> _phase;
	}
	catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
		outputs << e.what();
		return outputs;
	}

	std::vector<TF_Output> input_vec;
	std::vector<TF_Tensor*> input_tensors;
	std::vector<TF_Output> output_vec;
	std::vector<TF_Tensor*> output_tensors(1);

	input_vec.push_back({ TF_GraphOperationByName(this->_graph, "model_custom_mobile_segmentation_v3/input"), 0 });
	input_vec.push_back({ TF_GraphOperationByName(this->_graph, "model_custom_mobile_segmentation_v3/phase"), 0 });
	output_vec.push_back({ TF_GraphOperationByName(this->_graph, "model_custom_mobile_segmentation_v3/Sigmoid"), 0 });


	int64_t in_dims[] = { 1, this->_image_size };

	input_tensors.push_back(TF_NewTensor(TF_FLOAT, in_dims, 2, _image_buffer, sizeof(float) * this->_image_size, &deallocator, 0));
	input_tensors.push_back(TF_NewTensor(TF_BOOL, nullptr, 0, &_phase, sizeof(bool), &deallocator, nullptr));


	std::cout << TF_Message(this->_status) << std::endl;
	try {
		TF_SessionRun(this->_session,
			nullptr, // Run options.
			&input_vec[0], &input_tensors[0], input_tensors.size(), // Input tensors, input tensor values, number of inputs.
			&output_vec[0], &output_tensors[0], output_tensors.size(), // Output tensors, output tensor values, number of outputs.
			nullptr, 0, // Target operations, number of targets.
			nullptr, // Run metadata.
			this->_status // Output status.
		);
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}


	TF_SessionRun(_session, nullptr,
		input_vec.data(), input_tensors.data(), input_vec.size(),
		output_vec.data(), output_tensors.data(), output_vec.size(),
		nullptr, 0, nullptr, _status);


	if (TF_GetCode(this->_status) != TF_OK) {
		std::cout << "Error run session" << std::endl;
		std::cout << TF_Message(this->_status) << std::endl;
		outputs << "Error run session";
		TF_DeleteStatus(this->_status);
		return outputs;
	}

	const auto data = static_cast<float*>(TF_TensorData(output_tensors[0]));
	memcpy(_output_buffer, data, sizeof(float) * this->_image_height * this->_image_width);
	outputs << "success";

	for (TF_Tensor * tensor : input_tensors) {
		TF_DeleteTensor(tensor);
	}

	for (TF_Tensor * tensor : output_tensors) {
		TF_DeleteTensor(tensor);
	}




	return outputs;
}

oc::variant oc::omodule_mobile_segmentation_v3::dispose() {
	variant output;

	TF_CloseSession(this->_session, this->_status);
	if (TF_GetCode(this->_status) != TF_OK) {
		std::cout << "Error close session" << std::endl;
		output << "Error close session";
		TF_DeleteStatus(this->_status);
		return output;
	}

	TF_DeleteSession(this->_session, this->_status);
	if (TF_GetCode(this->_status) != TF_OK) {
		std::cout << "Error delete session";
		output << "Error delete session";
		TF_DeleteStatus(this->_status);
		return output;
	}

	TF_DeleteGraph(this->_graph);
	TF_DeleteStatus(this->_status);

	return output;
}