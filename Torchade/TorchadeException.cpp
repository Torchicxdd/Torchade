#include "TorchadeException.h"
#include <sstream>

TorchadeException::TorchadeException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* TorchadeException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* TorchadeException::GetType() const noexcept
{
	return "Torchade Exception";
}

int TorchadeException::GetLine() const noexcept
{
	return line;
}

const std::string& TorchadeException::GetFile() const noexcept
{
	return file;
}

std::string TorchadeException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}