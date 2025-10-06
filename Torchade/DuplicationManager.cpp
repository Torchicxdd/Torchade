#include "DuplicationManager.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;

// Sets the linker settings for us
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define DUP_EXCEPT_NOINFO(hr) DuplicationManager::HrException(__LINE__, __FILE__, (hr))
#define DUP_THROW_NOINFO(hrcall) if (FAILED(hr = (hrcall))) throw DuplicationManager::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define DUP_EXCEPT(hr) DuplicationManager::HrException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define DUP_THROW_INFO(hrcall) infoManager.Set(); if (FAILED(hr = (hrcall))) throw DUP_EXCEPT(hr)
#define DUP_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if (!v.empty()) {throw DuplicationManager::InfoException(__LINE__, __FILE__, v);}}
#else
#define DUP_EXCEPT(hr) DuplicationManager::HrException(__LINE__, __FILE__, (hr))
#define DUP_THROW_INFO(hrcall) throw DUP_THROW_NOINFO(hr)
#define DUP_THROW_INFO_ONLY(call) (call)
#endif

DuplicationManager::DuplicationManager(wrl::ComPtr<ID3D11Device> pDevice, wrl::ComPtr<IDXGIOutput1> pOutput1) {
	HRESULT hr;
	DUP_THROW_INFO(pOutput1->DuplicateOutput(pDevice.Get(), &pOutputDuplication));
}

void DuplicationManager::GetFrame(UINT timeoutMs, wrl::ComPtr<ID3D11Texture2D>& pOutTex) {
	wrl::ComPtr<IDXGIResource> pResource;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;

	HRESULT hr;

	DUP_THROW_INFO(pOutputDuplication->AcquireNextFrame(timeoutMs, &frameInfo, &pResource));

	pResource.As(&pOutTex);
}

void DuplicationManager::ReleaseFrame() {
	HRESULT hr;
	DUP_THROW_INFO(pOutputDuplication->ReleaseFrame());
}

DuplicationManager::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr) {
	// Join all info messages with newlines into single string
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}
	// Remove final newline if exists
	if (!info.empty()) {
		info.pop_back();
	}
}

const char* DuplicationManager::HrException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty()) {
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* DuplicationManager::HrException::GetType() const noexcept {
	return "Torchade Duplication Exception";
}

HRESULT DuplicationManager::HrException::GetErrorCode() const noexcept {
	return hr;
}

std::string DuplicationManager::HrException::GetErrorString() const noexcept {
	return DXGetErrorStringA(hr);
}

std::string DuplicationManager::HrException::GetErrorDescription() const noexcept {
	char buf[512];
	DXGetErrorDescriptionA(hr, buf, sizeof(buf));
	return buf;
}

DuplicationManager::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file) {
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}

	// Remove final newline if exists
	if (!info.empty()) {
		info.pop_back();
	}
}

const char* DuplicationManager::InfoException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* DuplicationManager::InfoException::GetType() const noexcept {
	return "Torchade Duplication Info Exception";
}

std::string DuplicationManager::InfoException::GetErrorInfo() const noexcept {
	return info;
}

std::string DuplicationManager::HrException::GetErrorInfo() const noexcept {
	return info;
}