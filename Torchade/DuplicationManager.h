#pragma once
#include "framework.h"
#include "TorchadeException.h"
#include <d3d11.h>
#include <wrl.h>
#include "DxgiInfoManager.h"

class DuplicationManager {
public:
	class Exception : public TorchadeException {
		using TorchadeException::TorchadeException;
	};
	class HrException : public Exception {
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception {
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
public:
	DuplicationManager(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<IDXGIOutput1> pOutput1);
	DuplicationManager(const DuplicationManager&) = delete;
	DuplicationManager& operator=(const DuplicationManager&) = delete;
	~DuplicationManager() = default;
	void GetFrame(UINT timeoutMs, Microsoft::WRL::ComPtr<ID3D11Texture2D>& pOutTex);
	void ReleaseFrame();
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	Microsoft::WRL::ComPtr<IDXGIOutputDuplication> pOutputDuplication;
};