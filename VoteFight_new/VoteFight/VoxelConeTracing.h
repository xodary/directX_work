#pragma once

class DXRSRenderTarget
{
public:
	DXRSRenderTarget(ID3D12Device* device, DXRS::DescriptorHeapManager* descriptorManger, int width, int height, DXGI_FORMAT aFormat, D3D12_RESOURCE_FLAGS flags, LPCWSTR name, int depth = -1, int mips = 1, D3D12_RESOURCE_STATES defaultState = D3D12_RESOURCE_STATE_RENDER_TARGET);
	~DXRSRenderTarget();

	ID3D12Resource* GetResource() { return mRenderTarget.Get(); }

	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }
	int GetDepth() { return mDepth; }
	void TransitionTo(std::vector<CD3DX12_RESOURCE_BARRIER>& barriers, ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES stateAfter);
	D3D12_RESOURCE_STATES GetCurrentState() { return mCurrentResourceState; }

	DXRS::DescriptorHandle& GetRTV(int mip = 0)
	{
		return mDescriptorRTVMipsHandles[mip];
	}

	DXRS::DescriptorHandle& GetSRV()
	{
		return mDescriptorSRV;
	}

	DXRS::DescriptorHandle& GetUAV(int mip = 0)
	{
		return mDescriptorUAVMipsHandles[mip];
	}

private:

	int mWidth, mHeight, mDepth;
	DXGI_FORMAT mFormat;
	D3D12_RESOURCE_STATES mCurrentResourceState;

	//DXRS::DescriptorHandle mDescriptorUAV;
	DXRS::DescriptorHandle mDescriptorSRV;
	//DXRS::DescriptorHandle mDescriptorRTV;
	ComPtr<ID3D12Resource> mRenderTarget;

	std::vector<DXRS::DescriptorHandle> mDescriptorUAVMipsHandles;
	std::vector<DXRS::DescriptorHandle> mDescriptorRTVMipsHandles;
};

class DXRSExampleGIScene 
{
	enum RenderQueue {
		GRAPHICS_QUEUE,
		COMPUTE_QUEUE
	};

public:
	DXRSExampleGIScene() {}
	~DXRSExampleGIScene() {}

	void DXRSExampleGIScene::InitVoxelConeTracing(ID3D12Device* device, DXRS::DescriptorHeapManager* descriptorManager);
	void DXRSExampleGIScene::RenderVoxelConeTracing(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DXRS::GPUDescriptorHeap* gpuDescriptorHeap, RenderQueue aQueue, bool useAsyncCompute);

		// Voxel Cone Tracing
	RootSignature mVCTVoxelizationRS;
	RootSignature mVCTMainRS;
	RootSignature mVCTMainRS_Compute;
	RootSignature mVCTMainUpsampleAndBlurRS;
	RootSignature mVCTAnisoMipmappingPrepareRS;
	RootSignature mVCTAnisoMipmappingMainRS;
	GraphicsPSO mVCTVoxelizationPSO;
	GraphicsPSO mVCTMainPSO;
	ComputePSO mVCTMainPSO_Compute;
	ComputePSO mVCTAnisoMipmappingPreparePSO;
	ComputePSO mVCTAnisoMipmappingMainPSO;
	ComputePSO mVCTMainUpsampleAndBlurPSO;
	RootSignature mVCTVoxelizationDebugRS;
	GraphicsPSO mVCTVoxelizationDebugPSO;
	DXRSRenderTarget* mVCTVoxelization3DRT = nullptr;
	DXRSRenderTarget* mVCTVoxelization3DRT_CopyForAsync = nullptr;
	DXRSRenderTarget* mVCTVoxelizationDebugRT = nullptr;
	DXRSRenderTarget* mVCTMainRT = nullptr;
	DXRSRenderTarget* mVCTMainUpsampleAndBlurRT = nullptr;
	DXRSRenderTarget* mVCTAnisoMipmappinPrepare3DRTs[6] = { nullptr };
	DXRSRenderTarget* mVCTAnisoMipmappinMain3DRTs[6] = { nullptr };
	__declspec(align(16)) struct VCTVoxelizationCBData
	{
		XMMATRIX WorldVoxelCube;
		XMMATRIX ViewProjection;
		XMMATRIX ShadowViewProjection;
		float WorldVoxelScale;
	};
	__declspec(align(16)) struct VCTAnisoMipmappingCBData
	{
		int MipDimension;
		int MipLevel;
	};
	__declspec(align(16)) struct VCTMainCBData
	{
		XMFLOAT4 CameraPos;
		XMFLOAT2 UpsampleRatio;
		float IndirectDiffuseStrength;
		float IndirectSpecularStrength;
		float MaxConeTraceDistance;
		float AOFalloff;
		float SamplingFactor;
		float VoxelSampleOffset;
	};
	DXRSBuffer* mVCTVoxelizationCB = nullptr;
	DXRSBuffer* mVCTAnisoMipmappingCB = nullptr;
	DXRSBuffer* mVCTMainCB = nullptr;
	std::vector<DXRSBuffer*> mVCTAnisoMipmappingMainCB;
	bool mVCTRenderDebug = false;
	float mWorldVoxelScale = VCT_SCENE_VOLUME_SIZE * 0.5f;
	float mVCTIndirectDiffuseStrength = 1.0f;
	float mVCTIndirectSpecularStrength = 1.0f;
	float mVCTMaxConeTraceDistance = 100.0f;
	float mVCTAoFalloff = 2.0f;
	float mVCTSamplingFactor = 0.5f;
	float mVCTVoxelSampleOffset = 0.0f;
	float mVCTRTRatio = 0.5f; // from MAX_SCREEN_WIDTH/HEIGHT
	bool mVCTUseMainCompute = true;
	bool mVCTMainRTUseUpsampleAndBlur = true;
	float mVCTGIPower = 1.0f;
};