#pragma once

#define CINTERFACE
#include <string>
#include <Windows.h>

#include "Imodule.h"
#include "omacro.h"

typedef oc::Imodule * (__stdcall * CreateInstance)();
typedef void(__stdcall * DeleteInstance)(oc::Imodule *);

namespace oc {

	class oloader;
	class oloader : Imodule{

	private:

		HMODULE instance;
		Imodule * _ocmodule;
		CreateInstance _createInstance;
		DeleteInstance _deleteInstance;


	public:

		enum MODULE_TYPE {
			__DLL,
			__MODULE,
		};

		oloader() :instance(nullptr),
			_ocmodule(nullptr),
			_createInstance(nullptr),
			_deleteInstance(nullptr) {};


		oloader(std::wstring _path, MODULE_TYPE _type) :instance(nullptr), 
														_ocmodule(nullptr), 
														_createInstance(nullptr),
														_deleteInstance(nullptr){
			try {
				this->load(_path, _type);
			}
			catch (std::exception e) {
				throw e;
			}
		}

		void load(std::wstring _path, MODULE_TYPE _type) {
			switch (_type)
			{
				case oloader::__DLL:{

					this->instance = LoadLibraryW(_path.c_str());

					if (this->instance == nullptr) {
						OCERROR(INSTANCE_NOT_CREATED);
						OCERRORWRITE(INSTANCE_NOT_CREATED, "Instance is not created");
						throw std::runtime_error(INSTANCE_NOT_CREATED);
					}

					this->_createInstance = (CreateInstance)GetProcAddress(this->instance, "CreateInstance");

					if (this->_createInstance == nullptr) {
						OCERROR(INSTANCE_FUNCTION_NOT_CREATED);
						OCERRORWRITE(INSTANCE_FUNCTION_NOT_CREATED, "Instance function is not created");
						throw std::runtime_error(INSTANCE_FUNCTION_NOT_CREATED);
					}
					_ocmodule = this->_createInstance();
					if (_ocmodule == nullptr) throw std::runtime_error("Module is not created");

					break;
				}
			}
		}

		~oloader() {
			if (this->_ocmodule != nullptr && this->instance != nullptr) {
				this->_deleteInstance = (DeleteInstance)GetProcAddress(this->instance, "DeleteInstance");
				this->_deleteInstance(this->_ocmodule);
			}
			if(this->instance != nullptr) 
				FreeLibrary(this->instance);
		}

		/// Composite Pattrn
		variant run(variant &inputs) override {
			return this->_ocmodule->run(inputs);
		}

		variant initialize(variant &inputs) override {
			return this->_ocmodule->initialize(inputs);
		}

		variant dispose()override {
			return this->_ocmodule->dispose();
		}
	};

}

