#pragma once

#include <iostream>

#include <vector>

#include "omacro.h"

namespace oc {

	class element;
	class element {

	private:

		friend class variant;
		const char * ti;
		unsigned char data[8];

		element() {
			ti = nullptr;
		}

		template<typename T>element(T _input) {
			T _temp = _input;
			ti = typeid(_input).name();
			memset(data, 0, sizeof(T));
			memcpy(data, &_temp, sizeof(T));
		}

	public:

		template<typename T> T get() {
			const char * _name = typeid(T).name();
			if (strcmp(ti, _name) != 0) {
				OCERROR(INVALID_PARSING_TYPE);
				OCERRORWRITE(INVALID_PARSING_TYPE, "Invalid parsing type");
				throw(std::runtime_error(INVALID_PARSING_TYPE));
			}
				

			T value;
			memcpy(&value, data, sizeof(T));
			return value;
		}

		const char * tostring() {

			return ti;
		}
	};

	class variant;
	class variant {
	private:

		std::vector<element> elements;
		int index = 0;

	public:

		variant() {
			index = 0;
			elements.clear();
		}

		template<typename T> variant & operator = (T _input) {
			index = 0;
			elements.clear();
			elements.push_back(element(_input));
			return *this;
		}

		template<typename T> variant & operator , (T _input) {
			elements.push_back(element(_input));
			return *this;
		}

		template<typename T> variant & operator << (T _input) {
			elements.push_back(element(_input));
			return *this;
		}

		template<typename T> variant & operator >> (T &_input) {
			if (elements.size() == 0) {
				OCERROR(NO_ELEMENT);
				OCERRORWRITE(NO_ELEMENT, "No elements");
				throw std::runtime_error(NO_ELEMENT);
			}
			if (elements.size() == index) {
				OCERROR(EOF_E);
				OCERRORWRITE(EOF_E, "End of elements, Need to reset index");
				throw std::runtime_error(EOF_E);
			}
			T value = elements[index++].get<T>();

			_input = value;
			return *this;
		}

		element & operator [](int _index) {
			if (_index >= elements.size()) {
				OCERROR(EXCEED_INDEX);
				OCERRORWRITE(EXCEED_INDEX, "Exceed index");
				throw std::runtime_error(EXCEED_INDEX);
			}
			return elements[_index];
		}

		void reset() {
			index = 0;
		}

		unsigned long long size() {
			return elements.size();
		}

		const char * tostring(int _index) {
			if (_index >= elements.size()) {
				OCERROR(EXCEED_INDEX);
				OCERRORWRITE(EXCEED_INDEX, "Exceed index");
				throw std::runtime_error(EXCEED_INDEX);
			}
			return elements[_index].tostring();
		}

	};

}
