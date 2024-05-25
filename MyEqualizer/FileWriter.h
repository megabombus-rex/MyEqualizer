#pragma once
#include <string>
#include <iostream>
#include <sstream>

namespace MyEq {
	template <typename T>
	class FileWriter {
	private:
		std::string fileToWrite;

	public:
		FileWriter();
		FileWriter(std::string fileToWrite);

		int WriteToFile(T* buffer, size_t bufferSize);
	};

	template<typename T>
	inline FileWriter<T>::FileWriter()
	{
		this->fileToWrite = "UNKNOWN";
	}

	template<typename T>
	inline FileWriter<T>::FileWriter(std::string fileToWrite)
	{
		this->fileToWrite = fileToWrite;
	}

	template<typename T>
	inline int FileWriter<T>::WriteToFile(T* buffer, size_t bufferSize)
	{
		static_assert(sizeof(T*) == sizeof(uint8_t*));
		try {
			std::ofstream fp;
			fp.open(this->fileToWrite, std::ios::out, std::ios::binary);
			fp.write((char*)buffer, bufferSize);
		}
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
		}

		return 0;
	}

}