// ██████  ██████  ██████   ██  ██     ██████   █████   ██████ ██   ██ ███████ ███    ██ ██████
// ██   ██      ██ ██   ██ ███ ███     ██   ██ ██   ██ ██      ██  ██  ██      ████   ██ ██   ██
// ██   ██  █████  ██   ██  ██  ██     ██████  ███████ ██      █████   █████   ██ ██  ██ ██   ██
// ██   ██      ██ ██   ██  ██  ██     ██   ██ ██   ██ ██      ██  ██  ██      ██  ██ ██ ██   ██
// ██████  ██████  ██████   ██  ██     ██████  ██   ██  ██████ ██   ██ ███████ ██   ████ ██████
//
// >>d3d11 backend

#define _SG_D3D11_MAX_SHADERSTAGE_SRVS (32)
#define _SG_D3D11_SHADERSTAGE_IMAGE_SRV_OFFSET (0)
#define _SG_D3D11_SHADERSTAGE_BUFFER_SRV_OFFSET (16)

#if defined(__cplusplus)
#define _sg_d3d11_AddRef(self) (self)->AddRef()
#else
#define _sg_d3d11_AddRef(self) (self)->lpVtbl->AddRef(self)
#endif

#if defined(__cplusplus)
#define _sg_d3d11_Release(self) (self)->Release()
#else
#define _sg_d3d11_Release(self) (self)->lpVtbl->Release(self)
#endif

// NOTE: This needs to be a macro since we can't use the polymorphism in C. It's called on many kinds of resources.
// NOTE: Based on microsoft docs, it's fine to call this with pData=NULL if DataSize is also zero.
#if defined(__cplusplus)
#define _sg_d3d11_SetPrivateData(self, guid, DataSize, pData) (self)->SetPrivateData(guid, DataSize, pData)
#else
#define _sg_d3d11_SetPrivateData(self, guid, DataSize, pData) (self)->lpVtbl->SetPrivateData(self, guid, DataSize, pData)
#endif

#if defined(__cplusplus)
#define _sg_win32_refguid(guid) guid
#else
#define _sg_win32_refguid(guid) &guid
#endif

static const GUID _sg_d3d11_WKPDID_D3DDebugObjectName = { 0x429b8c22,0x9188,0x4b0c, {0x87,0x42,0xac,0xb0,0xbf,0x85,0xc2,0x00} };

#if defined(SOKOL_DEBUG)
#define _sg_d3d11_setlabel(self, label) _sg_d3d11_SetPrivateData(self, _sg_win32_refguid(_sg_d3d11_WKPDID_D3DDebugObjectName), label ? (UINT)strlen(label) : 0, label)
#else
#define _sg_d3d11_setlabel(self, label)
#endif


//-- D3D11 C/C++ wrappers ------------------------------------------------------
static inline HRESULT _sg_d3d11_CheckFormatSupport(ID3D11Device* self, DXGI_FORMAT Format, UINT* pFormatSupport) {
    #if defined(__cplusplus)
        return self->CheckFormatSupport(Format, pFormatSupport);
    #else
        return self->lpVtbl->CheckFormatSupport(self, Format, pFormatSupport);
    #endif
}

static inline void _sg_d3d11_OMSetRenderTargets(ID3D11DeviceContext* self, UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView) {
    #if defined(__cplusplus)
        self->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
    #else
        self->lpVtbl->OMSetRenderTargets(self, NumViews, ppRenderTargetViews, pDepthStencilView);
    #endif
}

static inline void _sg_d3d11_RSSetState(ID3D11DeviceContext* self, ID3D11RasterizerState* pRasterizerState) {
    #if defined(__cplusplus)
        self->RSSetState(pRasterizerState);
    #else
        self->lpVtbl->RSSetState(self, pRasterizerState);
    #endif
}

static inline void _sg_d3d11_OMSetDepthStencilState(ID3D11DeviceContext* self, ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef) {
    #if defined(__cplusplus)
        self->OMSetDepthStencilState(pDepthStencilState, StencilRef);
    #else
        self->lpVtbl->OMSetDepthStencilState(self, pDepthStencilState, StencilRef);
    #endif
}

static inline void _sg_d3d11_OMSetBlendState(ID3D11DeviceContext* self, ID3D11BlendState* pBlendState, const FLOAT BlendFactor[4], UINT SampleMask) {
    #if defined(__cplusplus)
        self->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
    #else
        self->lpVtbl->OMSetBlendState(self, pBlendState, BlendFactor, SampleMask);
    #endif
}

static inline void _sg_d3d11_IASetVertexBuffers(ID3D11DeviceContext* self, UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets) {
    #if defined(__cplusplus)
        self->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
    #else
        self->lpVtbl->IASetVertexBuffers(self, StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
    #endif
}

static inline void _sg_d3d11_IASetIndexBuffer(ID3D11DeviceContext* self, ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset) {
    #if defined(__cplusplus)
        self->IASetIndexBuffer(pIndexBuffer, Format, Offset);
    #else
        self->lpVtbl->IASetIndexBuffer(self, pIndexBuffer, Format, Offset);
    #endif
}

static inline void _sg_d3d11_IASetInputLayout(ID3D11DeviceContext* self, ID3D11InputLayout* pInputLayout) {
    #if defined(__cplusplus)
        self->IASetInputLayout(pInputLayout);
    #else
        self->lpVtbl->IASetInputLayout(self, pInputLayout);
    #endif
}

static inline void _sg_d3d11_VSSetShader(ID3D11DeviceContext* self, ID3D11VertexShader* pVertexShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) {
    #if defined(__cplusplus)
        self->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
    #else
        self->lpVtbl->VSSetShader(self, pVertexShader, ppClassInstances, NumClassInstances);
    #endif
}

static inline void _sg_d3d11_PSSetShader(ID3D11DeviceContext* self, ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstances, UINT NumClassInstances) {
    #if defined(__cplusplus)
        self->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
    #else
        self->lpVtbl->PSSetShader(self, pPixelShader, ppClassInstances, NumClassInstances);
    #endif
}

static inline void _sg_d3d11_VSSetConstantBuffers(ID3D11DeviceContext* self, UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) {
    #if defined(__cplusplus)
        self->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    #else
        self->lpVtbl->VSSetConstantBuffers(self, StartSlot, NumBuffers, ppConstantBuffers);
    #endif
}

static inline void _sg_d3d11_PSSetConstantBuffers(ID3D11DeviceContext* self, UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers) {
    #if defined(__cplusplus)
        self->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    #else
        self->lpVtbl->PSSetConstantBuffers(self, StartSlot, NumBuffers, ppConstantBuffers);
    #endif
}

static inline void _sg_d3d11_VSSetShaderResources(ID3D11DeviceContext* self, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) {
    #if defined(__cplusplus)
        self->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    #else
        self->lpVtbl->VSSetShaderResources(self, StartSlot, NumViews, ppShaderResourceViews);
    #endif
}

static inline void _sg_d3d11_PSSetShaderResources(ID3D11DeviceContext* self, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) {
    #if defined(__cplusplus)
        self->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    #else
        self->lpVtbl->PSSetShaderResources(self, StartSlot, NumViews, ppShaderResourceViews);
    #endif
}

static inline void _sg_d3d11_VSSetSamplers(ID3D11DeviceContext* self, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) {
    #if defined(__cplusplus)
        self->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    #else
        self->lpVtbl->VSSetSamplers(self, StartSlot, NumSamplers, ppSamplers);
    #endif
}

static inline void _sg_d3d11_PSSetSamplers(ID3D11DeviceContext* self, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers) {
    #if defined(__cplusplus)
        self->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    #else
        self->lpVtbl->PSSetSamplers(self, StartSlot, NumSamplers, ppSamplers);
    #endif
}

static inline HRESULT _sg_d3d11_CreateBuffer(ID3D11Device* self, const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer) {
    #if defined(__cplusplus)
        return self->CreateBuffer(pDesc, pInitialData, ppBuffer);
    #else
        return self->lpVtbl->CreateBuffer(self, pDesc, pInitialData, ppBuffer);
    #endif
}

static inline HRESULT _sg_d3d11_CreateTexture2D(ID3D11Device* self, const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D) {
    #if defined(__cplusplus)
        return self->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
    #else
        return self->lpVtbl->CreateTexture2D(self, pDesc, pInitialData, ppTexture2D);
    #endif
}

static inline HRESULT _sg_d3d11_CreateShaderResourceView(ID3D11Device* self, ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView) {
    #if defined(__cplusplus)
        return self->CreateShaderResourceView(pResource, pDesc, ppSRView);
    #else
        return self->lpVtbl->CreateShaderResourceView(self, pResource, pDesc, ppSRView);
    #endif
}

static inline void _sg_d3d11_GetResource(ID3D11View* self, ID3D11Resource** ppResource) {
    #if defined(__cplusplus)
        self->GetResource(ppResource);
    #else
        self->lpVtbl->GetResource(self, ppResource);
    #endif
}

static inline HRESULT _sg_d3d11_CreateTexture3D(ID3D11Device* self, const D3D11_TEXTURE3D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture3D** ppTexture3D) {
    #if defined(__cplusplus)
        return self->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
    #else
        return self->lpVtbl->CreateTexture3D(self, pDesc, pInitialData, ppTexture3D);
    #endif
}

static inline HRESULT _sg_d3d11_CreateSamplerState(ID3D11Device* self, const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState) {
    #if defined(__cplusplus)
        return self->CreateSamplerState(pSamplerDesc, ppSamplerState);
    #else
        return self->lpVtbl->CreateSamplerState(self, pSamplerDesc, ppSamplerState);
    #endif
}

static inline LPVOID _sg_d3d11_GetBufferPointer(ID3D10Blob* self) {
    #if defined(__cplusplus)
        return self->GetBufferPointer();
    #else
        return self->lpVtbl->GetBufferPointer(self);
    #endif
}

static inline SIZE_T _sg_d3d11_GetBufferSize(ID3D10Blob* self) {
    #if defined(__cplusplus)
        return self->GetBufferSize();
    #else
        return self->lpVtbl->GetBufferSize(self);
    #endif
}

static inline HRESULT _sg_d3d11_CreateVertexShader(ID3D11Device* self, const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11VertexShader** ppVertexShader) {
    #if defined(__cplusplus)
        return self->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
    #else
        return self->lpVtbl->CreateVertexShader(self, pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
    #endif
}

static inline HRESULT _sg_d3d11_CreatePixelShader(ID3D11Device* self, const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage* pClassLinkage, ID3D11PixelShader** ppPixelShader) {
    #if defined(__cplusplus)
        return self->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
    #else
        return self->lpVtbl->CreatePixelShader(self, pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
    #endif
}

static inline HRESULT _sg_d3d11_CreateInputLayout(ID3D11Device* self, const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout) {
    #if defined(__cplusplus)
        return self->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
    #else
        return self->lpVtbl->CreateInputLayout(self, pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
    #endif
}

static inline HRESULT _sg_d3d11_CreateRasterizerState(ID3D11Device* self, const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** ppRasterizerState) {
    #if defined(__cplusplus)
        return self->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
    #else
        return self->lpVtbl->CreateRasterizerState(self, pRasterizerDesc, ppRasterizerState);
    #endif
}

static inline HRESULT _sg_d3d11_CreateDepthStencilState(ID3D11Device* self, const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** ppDepthStencilState) {
    #if defined(__cplusplus)
        return self->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
    #else
        return self->lpVtbl->CreateDepthStencilState(self, pDepthStencilDesc, ppDepthStencilState);
    #endif
}

static inline HRESULT _sg_d3d11_CreateBlendState(ID3D11Device* self, const D3D11_BLEND_DESC* pBlendStateDesc, ID3D11BlendState** ppBlendState) {
    #if defined(__cplusplus)
        return self->CreateBlendState(pBlendStateDesc, ppBlendState);
    #else
        return self->lpVtbl->CreateBlendState(self, pBlendStateDesc, ppBlendState);
    #endif
}

static inline HRESULT _sg_d3d11_CreateRenderTargetView(ID3D11Device* self, ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView) {
    #if defined(__cplusplus)
        return self->CreateRenderTargetView(pResource, pDesc, ppRTView);
    #else
        return self->lpVtbl->CreateRenderTargetView(self, pResource, pDesc, ppRTView);
    #endif
}

static inline HRESULT _sg_d3d11_CreateDepthStencilView(ID3D11Device* self, ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView) {
    #if defined(__cplusplus)
        return self->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
    #else
        return self->lpVtbl->CreateDepthStencilView(self, pResource, pDesc, ppDepthStencilView);
    #endif
}

static inline void _sg_d3d11_RSSetViewports(ID3D11DeviceContext* self, UINT NumViewports, const D3D11_VIEWPORT* pViewports) {
    #if defined(__cplusplus)
        self->RSSetViewports(NumViewports, pViewports);
    #else
        self->lpVtbl->RSSetViewports(self, NumViewports, pViewports);
    #endif
}

static inline void _sg_d3d11_RSSetScissorRects(ID3D11DeviceContext* self, UINT NumRects, const D3D11_RECT* pRects) {
    #if defined(__cplusplus)
        self->RSSetScissorRects(NumRects, pRects);
    #else
        self->lpVtbl->RSSetScissorRects(self, NumRects, pRects);
    #endif
}

static inline void _sg_d3d11_ClearRenderTargetView(ID3D11DeviceContext* self, ID3D11RenderTargetView* pRenderTargetView, const FLOAT ColorRGBA[4]) {
    #if defined(__cplusplus)
        self->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
    #else
        self->lpVtbl->ClearRenderTargetView(self, pRenderTargetView, ColorRGBA);
    #endif
}

static inline void _sg_d3d11_ClearDepthStencilView(ID3D11DeviceContext* self, ID3D11DepthStencilView* pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil) {
    #if defined(__cplusplus)
        self->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
    #else
        self->lpVtbl->ClearDepthStencilView(self, pDepthStencilView, ClearFlags, Depth, Stencil);
    #endif
}

static inline void _sg_d3d11_ResolveSubresource(ID3D11DeviceContext* self, ID3D11Resource* pDstResource, UINT DstSubresource, ID3D11Resource* pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format) {
    #if defined(__cplusplus)
        self->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
    #else
        self->lpVtbl->ResolveSubresource(self, pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
    #endif
}

static inline void _sg_d3d11_IASetPrimitiveTopology(ID3D11DeviceContext* self, D3D11_PRIMITIVE_TOPOLOGY Topology) {
    #if defined(__cplusplus)
        self->IASetPrimitiveTopology(Topology);
    #else
        self->lpVtbl->IASetPrimitiveTopology(self, Topology);
    #endif
}

static inline void _sg_d3d11_UpdateSubresource(ID3D11DeviceContext* self, ID3D11Resource* pDstResource, UINT DstSubresource, const D3D11_BOX* pDstBox, const void* pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch) {
    #if defined(__cplusplus)
        self->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
    #else
        self->lpVtbl->UpdateSubresource(self, pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
    #endif
}

static inline void _sg_d3d11_DrawIndexed(ID3D11DeviceContext* self, UINT IndexCount, UINT StartIndexLocation, INT  BaseVertexLocation) {
    #if defined(__cplusplus)
        self->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
    #else
        self->lpVtbl->DrawIndexed(self, IndexCount, StartIndexLocation, BaseVertexLocation);
    #endif
}

static inline void _sg_d3d11_DrawIndexedInstanced(ID3D11DeviceContext* self, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) {
    #if defined(__cplusplus)
        self->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
    #else
        self->lpVtbl->DrawIndexedInstanced(self, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
    #endif
}

static inline void _sg_d3d11_Draw(ID3D11DeviceContext* self, UINT VertexCount, UINT StartVertexLocation) {
    #if defined(__cplusplus)
        self->Draw(VertexCount, StartVertexLocation);
    #else
        self->lpVtbl->Draw(self, VertexCount, StartVertexLocation);
    #endif
}

static inline void _sg_d3d11_DrawInstanced(ID3D11DeviceContext* self, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) {
    #if defined(__cplusplus)
        self->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
    #else
        self->lpVtbl->DrawInstanced(self, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
    #endif
}

static inline HRESULT _sg_d3d11_Map(ID3D11DeviceContext* self, ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource) {
    #if defined(__cplusplus)
        return self->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
    #else
        return self->lpVtbl->Map(self, pResource, Subresource, MapType, MapFlags, pMappedResource);
    #endif
}

static inline void _sg_d3d11_Unmap(ID3D11DeviceContext* self, ID3D11Resource* pResource, UINT Subresource) {
    #if defined(__cplusplus)
        self->Unmap(pResource, Subresource);
    #else
        self->lpVtbl->Unmap(self, pResource, Subresource);
    #endif
}

static inline void _sg_d3d11_ClearState(ID3D11DeviceContext* self) {
    #if defined(__cplusplus)
        self->ClearState();
    #else
        self->lpVtbl->ClearState(self);
    #endif
}

//-- enum translation functions ------------------------------------------------
_SOKOL_PRIVATE D3D11_USAGE _sg_d3d11_usage(sg_usage usg) {
    switch (usg) {
        case SG_USAGE_IMMUTABLE:
            return D3D11_USAGE_IMMUTABLE;
        case SG_USAGE_DYNAMIC:
        case SG_USAGE_STREAM:
            return D3D11_USAGE_DYNAMIC;
        default:
            SOKOL_UNREACHABLE;
            return (D3D11_USAGE) 0;
    }
}

_SOKOL_PRIVATE UINT _sg_d3d11_buffer_bind_flags(sg_buffer_type t) {
    switch (t) {
        case SG_BUFFERTYPE_VERTEXBUFFER:
            return D3D11_BIND_VERTEX_BUFFER;
        case SG_BUFFERTYPE_INDEXBUFFER:
            return D3D11_BIND_INDEX_BUFFER;
        case SG_BUFFERTYPE_STORAGEBUFFER:
            // FIXME: for compute shaders we'd want UNORDERED_ACCESS?
            return D3D11_BIND_SHADER_RESOURCE;
        default:
            SOKOL_UNREACHABLE;
            return 0;
    }
}

_SOKOL_PRIVATE UINT _sg_d3d11_buffer_misc_flags(sg_buffer_type t) {
    switch (t) {
        case SG_BUFFERTYPE_VERTEXBUFFER:
        case SG_BUFFERTYPE_INDEXBUFFER:
            return 0;
        case SG_BUFFERTYPE_STORAGEBUFFER:
            return D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        default:
            SOKOL_UNREACHABLE;
            return 0;
    }
}

_SOKOL_PRIVATE UINT _sg_d3d11_cpu_access_flags(sg_usage usg) {
    switch (usg) {
        case SG_USAGE_IMMUTABLE:
            return 0;
        case SG_USAGE_DYNAMIC:
        case SG_USAGE_STREAM:
            return D3D11_CPU_ACCESS_WRITE;
        default:
            SOKOL_UNREACHABLE;
            return 0;
    }
}

_SOKOL_PRIVATE DXGI_FORMAT _sg_d3d11_texture_pixel_format(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_R8:             return DXGI_FORMAT_R8_UNORM;
        case SG_PIXELFORMAT_R8SN:           return DXGI_FORMAT_R8_SNORM;
        case SG_PIXELFORMAT_R8UI:           return DXGI_FORMAT_R8_UINT;
        case SG_PIXELFORMAT_R8SI:           return DXGI_FORMAT_R8_SINT;
        case SG_PIXELFORMAT_R16:            return DXGI_FORMAT_R16_UNORM;
        case SG_PIXELFORMAT_R16SN:          return DXGI_FORMAT_R16_SNORM;
        case SG_PIXELFORMAT_R16UI:          return DXGI_FORMAT_R16_UINT;
        case SG_PIXELFORMAT_R16SI:          return DXGI_FORMAT_R16_SINT;
        case SG_PIXELFORMAT_R16F:           return DXGI_FORMAT_R16_FLOAT;
        case SG_PIXELFORMAT_RG8:            return DXGI_FORMAT_R8G8_UNORM;
        case SG_PIXELFORMAT_RG8SN:          return DXGI_FORMAT_R8G8_SNORM;
        case SG_PIXELFORMAT_RG8UI:          return DXGI_FORMAT_R8G8_UINT;
        case SG_PIXELFORMAT_RG8SI:          return DXGI_FORMAT_R8G8_SINT;
        case SG_PIXELFORMAT_R32UI:          return DXGI_FORMAT_R32_UINT;
        case SG_PIXELFORMAT_R32SI:          return DXGI_FORMAT_R32_SINT;
        case SG_PIXELFORMAT_R32F:           return DXGI_FORMAT_R32_FLOAT;
        case SG_PIXELFORMAT_RG16:           return DXGI_FORMAT_R16G16_UNORM;
        case SG_PIXELFORMAT_RG16SN:         return DXGI_FORMAT_R16G16_SNORM;
        case SG_PIXELFORMAT_RG16UI:         return DXGI_FORMAT_R16G16_UINT;
        case SG_PIXELFORMAT_RG16SI:         return DXGI_FORMAT_R16G16_SINT;
        case SG_PIXELFORMAT_RG16F:          return DXGI_FORMAT_R16G16_FLOAT;
        case SG_PIXELFORMAT_RGBA8:          return DXGI_FORMAT_R8G8B8A8_UNORM;
        case SG_PIXELFORMAT_SRGB8A8:        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case SG_PIXELFORMAT_RGBA8SN:        return DXGI_FORMAT_R8G8B8A8_SNORM;
        case SG_PIXELFORMAT_RGBA8UI:        return DXGI_FORMAT_R8G8B8A8_UINT;
        case SG_PIXELFORMAT_RGBA8SI:        return DXGI_FORMAT_R8G8B8A8_SINT;
        case SG_PIXELFORMAT_BGRA8:          return DXGI_FORMAT_B8G8R8A8_UNORM;
        case SG_PIXELFORMAT_RGB10A2:        return DXGI_FORMAT_R10G10B10A2_UNORM;
        case SG_PIXELFORMAT_RG11B10F:       return DXGI_FORMAT_R11G11B10_FLOAT;
        case SG_PIXELFORMAT_RGB9E5:         return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        case SG_PIXELFORMAT_RG32UI:         return DXGI_FORMAT_R32G32_UINT;
        case SG_PIXELFORMAT_RG32SI:         return DXGI_FORMAT_R32G32_SINT;
        case SG_PIXELFORMAT_RG32F:          return DXGI_FORMAT_R32G32_FLOAT;
        case SG_PIXELFORMAT_RGBA16:         return DXGI_FORMAT_R16G16B16A16_UNORM;
        case SG_PIXELFORMAT_RGBA16SN:       return DXGI_FORMAT_R16G16B16A16_SNORM;
        case SG_PIXELFORMAT_RGBA16UI:       return DXGI_FORMAT_R16G16B16A16_UINT;
        case SG_PIXELFORMAT_RGBA16SI:       return DXGI_FORMAT_R16G16B16A16_SINT;
        case SG_PIXELFORMAT_RGBA16F:        return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case SG_PIXELFORMAT_RGBA32UI:       return DXGI_FORMAT_R32G32B32A32_UINT;
        case SG_PIXELFORMAT_RGBA32SI:       return DXGI_FORMAT_R32G32B32A32_SINT;
        case SG_PIXELFORMAT_RGBA32F:        return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case SG_PIXELFORMAT_DEPTH:          return DXGI_FORMAT_R32_TYPELESS;
        case SG_PIXELFORMAT_DEPTH_STENCIL:  return DXGI_FORMAT_R24G8_TYPELESS;
        case SG_PIXELFORMAT_BC1_RGBA:       return DXGI_FORMAT_BC1_UNORM;
        case SG_PIXELFORMAT_BC2_RGBA:       return DXGI_FORMAT_BC2_UNORM;
        case SG_PIXELFORMAT_BC3_RGBA:       return DXGI_FORMAT_BC3_UNORM;
        case SG_PIXELFORMAT_BC3_SRGBA:      return DXGI_FORMAT_BC3_UNORM_SRGB;
        case SG_PIXELFORMAT_BC4_R:          return DXGI_FORMAT_BC4_UNORM;
        case SG_PIXELFORMAT_BC4_RSN:        return DXGI_FORMAT_BC4_SNORM;
        case SG_PIXELFORMAT_BC5_RG:         return DXGI_FORMAT_BC5_UNORM;
        case SG_PIXELFORMAT_BC5_RGSN:       return DXGI_FORMAT_BC5_SNORM;
        case SG_PIXELFORMAT_BC6H_RGBF:      return DXGI_FORMAT_BC6H_SF16;
        case SG_PIXELFORMAT_BC6H_RGBUF:     return DXGI_FORMAT_BC6H_UF16;
        case SG_PIXELFORMAT_BC7_RGBA:       return DXGI_FORMAT_BC7_UNORM;
        case SG_PIXELFORMAT_BC7_SRGBA:      return DXGI_FORMAT_BC7_UNORM_SRGB;
        default:                            return DXGI_FORMAT_UNKNOWN;
    };
}

_SOKOL_PRIVATE DXGI_FORMAT _sg_d3d11_srv_pixel_format(sg_pixel_format fmt) {
    if (fmt == SG_PIXELFORMAT_DEPTH) {
        return DXGI_FORMAT_R32_FLOAT;
    } else if (fmt == SG_PIXELFORMAT_DEPTH_STENCIL) {
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    } else {
        return _sg_d3d11_texture_pixel_format(fmt);
    }
}

_SOKOL_PRIVATE DXGI_FORMAT _sg_d3d11_dsv_pixel_format(sg_pixel_format fmt) {
    if (fmt == SG_PIXELFORMAT_DEPTH) {
        return DXGI_FORMAT_D32_FLOAT;
    } else if (fmt == SG_PIXELFORMAT_DEPTH_STENCIL) {
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    } else {
        return _sg_d3d11_texture_pixel_format(fmt);
    }
}

_SOKOL_PRIVATE DXGI_FORMAT _sg_d3d11_rtv_pixel_format(sg_pixel_format fmt) {
    if (fmt == SG_PIXELFORMAT_DEPTH) {
        return DXGI_FORMAT_R32_FLOAT;
    } else if (fmt == SG_PIXELFORMAT_DEPTH_STENCIL) {
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    } else {
        return _sg_d3d11_texture_pixel_format(fmt);
    }
}

_SOKOL_PRIVATE D3D11_PRIMITIVE_TOPOLOGY _sg_d3d11_primitive_topology(sg_primitive_type prim_type) {
    switch (prim_type) {
        case SG_PRIMITIVETYPE_POINTS:           return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case SG_PRIMITIVETYPE_LINES:            return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case SG_PRIMITIVETYPE_LINE_STRIP:       return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case SG_PRIMITIVETYPE_TRIANGLES:        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case SG_PRIMITIVETYPE_TRIANGLE_STRIP:   return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default: SOKOL_UNREACHABLE; return (D3D11_PRIMITIVE_TOPOLOGY) 0;
    }
}

_SOKOL_PRIVATE DXGI_FORMAT _sg_d3d11_index_format(sg_index_type index_type) {
    switch (index_type) {
        case SG_INDEXTYPE_NONE:     return DXGI_FORMAT_UNKNOWN;
        case SG_INDEXTYPE_UINT16:   return DXGI_FORMAT_R16_UINT;
        case SG_INDEXTYPE_UINT32:   return DXGI_FORMAT_R32_UINT;
        default: SOKOL_UNREACHABLE; return (DXGI_FORMAT) 0;
    }
}

_SOKOL_PRIVATE D3D11_FILTER _sg_d3d11_filter(sg_filter min_f, sg_filter mag_f, sg_filter mipmap_f, bool comparison, uint32_t max_anisotropy) {
    uint32_t d3d11_filter = 0;
    if (max_anisotropy > 1) {
        // D3D11_FILTER_ANISOTROPIC = 0x55,
        d3d11_filter |= 0x55;
    } else {
        // D3D11_FILTER_MIN_MAG_MIP_POINT = 0,
        // D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR = 0x1,
        // D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
        // D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR = 0x5,
        // D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT = 0x10,
        // D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
        // D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT = 0x14,
        // D3D11_FILTER_MIN_MAG_MIP_LINEAR = 0x15,
        if (mipmap_f == SG_FILTER_LINEAR) {
            d3d11_filter |= 0x01;
        }
        if (mag_f == SG_FILTER_LINEAR) {
            d3d11_filter |= 0x04;
        }
        if (min_f == SG_FILTER_LINEAR) {
            d3d11_filter |= 0x10;
        }
    }
    // D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
    // D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
    // D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
    // D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
    // D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
    // D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
    // D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
    // D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
    // D3D11_FILTER_COMPARISON_ANISOTROPIC = 0xd5,
    if (comparison) {
        d3d11_filter |= 0x80;
    }
    return (D3D11_FILTER)d3d11_filter;
}

_SOKOL_PRIVATE D3D11_TEXTURE_ADDRESS_MODE _sg_d3d11_address_mode(sg_wrap m) {
    switch (m) {
        case SG_WRAP_REPEAT:            return D3D11_TEXTURE_ADDRESS_WRAP;
        case SG_WRAP_CLAMP_TO_EDGE:     return D3D11_TEXTURE_ADDRESS_CLAMP;
        case SG_WRAP_CLAMP_TO_BORDER:   return D3D11_TEXTURE_ADDRESS_BORDER;
        case SG_WRAP_MIRRORED_REPEAT:   return D3D11_TEXTURE_ADDRESS_MIRROR;
        default: SOKOL_UNREACHABLE; return (D3D11_TEXTURE_ADDRESS_MODE) 0;
    }
}

_SOKOL_PRIVATE DXGI_FORMAT _sg_d3d11_vertex_format(sg_vertex_format fmt) {
    switch (fmt) {
        case SG_VERTEXFORMAT_FLOAT:     return DXGI_FORMAT_R32_FLOAT;
        case SG_VERTEXFORMAT_FLOAT2:    return DXGI_FORMAT_R32G32_FLOAT;
        case SG_VERTEXFORMAT_FLOAT3:    return DXGI_FORMAT_R32G32B32_FLOAT;
        case SG_VERTEXFORMAT_FLOAT4:    return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case SG_VERTEXFORMAT_BYTE4:     return DXGI_FORMAT_R8G8B8A8_SINT;
        case SG_VERTEXFORMAT_BYTE4N:    return DXGI_FORMAT_R8G8B8A8_SNORM;
        case SG_VERTEXFORMAT_UBYTE4:    return DXGI_FORMAT_R8G8B8A8_UINT;
        case SG_VERTEXFORMAT_UBYTE4N:   return DXGI_FORMAT_R8G8B8A8_UNORM;
        case SG_VERTEXFORMAT_SHORT2:    return DXGI_FORMAT_R16G16_SINT;
        case SG_VERTEXFORMAT_SHORT2N:   return DXGI_FORMAT_R16G16_SNORM;
        case SG_VERTEXFORMAT_USHORT2N:  return DXGI_FORMAT_R16G16_UNORM;
        case SG_VERTEXFORMAT_SHORT4:    return DXGI_FORMAT_R16G16B16A16_SINT;
        case SG_VERTEXFORMAT_SHORT4N:   return DXGI_FORMAT_R16G16B16A16_SNORM;
        case SG_VERTEXFORMAT_USHORT4N:  return DXGI_FORMAT_R16G16B16A16_UNORM;
        case SG_VERTEXFORMAT_UINT10_N2: return DXGI_FORMAT_R10G10B10A2_UNORM;
        case SG_VERTEXFORMAT_HALF2:     return DXGI_FORMAT_R16G16_FLOAT;
        case SG_VERTEXFORMAT_HALF4:     return DXGI_FORMAT_R16G16B16A16_FLOAT;
        default: SOKOL_UNREACHABLE; return (DXGI_FORMAT) 0;
    }
}

_SOKOL_PRIVATE D3D11_INPUT_CLASSIFICATION _sg_d3d11_input_classification(sg_vertex_step step) {
    switch (step) {
        case SG_VERTEXSTEP_PER_VERTEX:      return D3D11_INPUT_PER_VERTEX_DATA;
        case SG_VERTEXSTEP_PER_INSTANCE:    return D3D11_INPUT_PER_INSTANCE_DATA;
        default: SOKOL_UNREACHABLE; return (D3D11_INPUT_CLASSIFICATION) 0;
    }
}

_SOKOL_PRIVATE D3D11_CULL_MODE _sg_d3d11_cull_mode(sg_cull_mode m) {
    switch (m) {
        case SG_CULLMODE_NONE:      return D3D11_CULL_NONE;
        case SG_CULLMODE_FRONT:     return D3D11_CULL_FRONT;
        case SG_CULLMODE_BACK:      return D3D11_CULL_BACK;
        default: SOKOL_UNREACHABLE; return (D3D11_CULL_MODE) 0;
    }
}

_SOKOL_PRIVATE D3D11_COMPARISON_FUNC _sg_d3d11_compare_func(sg_compare_func f) {
    switch (f) {
        case SG_COMPAREFUNC_NEVER:          return D3D11_COMPARISON_NEVER;
        case SG_COMPAREFUNC_LESS:           return D3D11_COMPARISON_LESS;
        case SG_COMPAREFUNC_EQUAL:          return D3D11_COMPARISON_EQUAL;
        case SG_COMPAREFUNC_LESS_EQUAL:     return D3D11_COMPARISON_LESS_EQUAL;
        case SG_COMPAREFUNC_GREATER:        return D3D11_COMPARISON_GREATER;
        case SG_COMPAREFUNC_NOT_EQUAL:      return D3D11_COMPARISON_NOT_EQUAL;
        case SG_COMPAREFUNC_GREATER_EQUAL:  return D3D11_COMPARISON_GREATER_EQUAL;
        case SG_COMPAREFUNC_ALWAYS:         return D3D11_COMPARISON_ALWAYS;
        default: SOKOL_UNREACHABLE; return (D3D11_COMPARISON_FUNC) 0;
    }
}

_SOKOL_PRIVATE D3D11_STENCIL_OP _sg_d3d11_stencil_op(sg_stencil_op op) {
    switch (op) {
        case SG_STENCILOP_KEEP:         return D3D11_STENCIL_OP_KEEP;
        case SG_STENCILOP_ZERO:         return D3D11_STENCIL_OP_ZERO;
        case SG_STENCILOP_REPLACE:      return D3D11_STENCIL_OP_REPLACE;
        case SG_STENCILOP_INCR_CLAMP:   return D3D11_STENCIL_OP_INCR_SAT;
        case SG_STENCILOP_DECR_CLAMP:   return D3D11_STENCIL_OP_DECR_SAT;
        case SG_STENCILOP_INVERT:       return D3D11_STENCIL_OP_INVERT;
        case SG_STENCILOP_INCR_WRAP:    return D3D11_STENCIL_OP_INCR;
        case SG_STENCILOP_DECR_WRAP:    return D3D11_STENCIL_OP_DECR;
        default: SOKOL_UNREACHABLE; return (D3D11_STENCIL_OP) 0;
    }
}

_SOKOL_PRIVATE D3D11_BLEND _sg_d3d11_blend_factor(sg_blend_factor f) {
    switch (f) {
        case SG_BLENDFACTOR_ZERO:                   return D3D11_BLEND_ZERO;
        case SG_BLENDFACTOR_ONE:                    return D3D11_BLEND_ONE;
        case SG_BLENDFACTOR_SRC_COLOR:              return D3D11_BLEND_SRC_COLOR;
        case SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR:    return D3D11_BLEND_INV_SRC_COLOR;
        case SG_BLENDFACTOR_SRC_ALPHA:              return D3D11_BLEND_SRC_ALPHA;
        case SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA:    return D3D11_BLEND_INV_SRC_ALPHA;
        case SG_BLENDFACTOR_DST_COLOR:              return D3D11_BLEND_DEST_COLOR;
        case SG_BLENDFACTOR_ONE_MINUS_DST_COLOR:    return D3D11_BLEND_INV_DEST_COLOR;
        case SG_BLENDFACTOR_DST_ALPHA:              return D3D11_BLEND_DEST_ALPHA;
        case SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA:    return D3D11_BLEND_INV_DEST_ALPHA;
        case SG_BLENDFACTOR_SRC_ALPHA_SATURATED:    return D3D11_BLEND_SRC_ALPHA_SAT;
        case SG_BLENDFACTOR_BLEND_COLOR:            return D3D11_BLEND_BLEND_FACTOR;
        case SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR:  return D3D11_BLEND_INV_BLEND_FACTOR;
        case SG_BLENDFACTOR_BLEND_ALPHA:            return D3D11_BLEND_BLEND_FACTOR;
        case SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA:  return D3D11_BLEND_INV_BLEND_FACTOR;
        default: SOKOL_UNREACHABLE; return (D3D11_BLEND) 0;
    }
}

_SOKOL_PRIVATE D3D11_BLEND_OP _sg_d3d11_blend_op(sg_blend_op op) {
    switch (op) {
        case SG_BLENDOP_ADD:                return D3D11_BLEND_OP_ADD;
        case SG_BLENDOP_SUBTRACT:           return D3D11_BLEND_OP_SUBTRACT;
        case SG_BLENDOP_REVERSE_SUBTRACT:   return D3D11_BLEND_OP_REV_SUBTRACT;
        default: SOKOL_UNREACHABLE; return (D3D11_BLEND_OP) 0;
    }
}

_SOKOL_PRIVATE UINT8 _sg_d3d11_color_write_mask(sg_color_mask m) {
    UINT8 res = 0;
    if (m & SG_COLORMASK_R) {
        res |= D3D11_COLOR_WRITE_ENABLE_RED;
    }
    if (m & SG_COLORMASK_G) {
        res |= D3D11_COLOR_WRITE_ENABLE_GREEN;
    }
    if (m & SG_COLORMASK_B) {
        res |= D3D11_COLOR_WRITE_ENABLE_BLUE;
    }
    if (m & SG_COLORMASK_A) {
        res |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
    }
    return res;
}

_SOKOL_PRIVATE UINT _sg_d3d11_dxgi_fmt_caps(DXGI_FORMAT dxgi_fmt) {
    UINT dxgi_fmt_caps = 0;
    if (dxgi_fmt != DXGI_FORMAT_UNKNOWN) {
        HRESULT hr = _sg_d3d11_CheckFormatSupport(_sg.d3d11.dev, dxgi_fmt, &dxgi_fmt_caps);
        SOKOL_ASSERT(SUCCEEDED(hr) || (E_FAIL == hr));
        if (!SUCCEEDED(hr)) {
            dxgi_fmt_caps = 0;
        }
    }
    return dxgi_fmt_caps;
}

// see: https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-limits#resource-limits-for-feature-level-11-hardware
_SOKOL_PRIVATE void _sg_d3d11_init_caps(void) {
    _sg.backend = SG_BACKEND_D3D11;

    _sg.features.origin_top_left = true;
    _sg.features.image_clamp_to_border = true;
    _sg.features.mrt_independent_blend_state = true;
    _sg.features.mrt_independent_write_mask = true;
    _sg.features.storage_buffer = true;

    _sg.limits.max_image_size_2d = 16 * 1024;
    _sg.limits.max_image_size_cube = 16 * 1024;
    _sg.limits.max_image_size_3d = 2 * 1024;
    _sg.limits.max_image_size_array = 16 * 1024;
    _sg.limits.max_image_array_layers = 2 * 1024;
    _sg.limits.max_vertex_attrs = SG_MAX_VERTEX_ATTRIBUTES;

    // see: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_format_support
    for (int fmt = (SG_PIXELFORMAT_NONE+1); fmt < _SG_PIXELFORMAT_NUM; fmt++) {
        const UINT srv_dxgi_fmt_caps = _sg_d3d11_dxgi_fmt_caps(_sg_d3d11_srv_pixel_format((sg_pixel_format)fmt));
        const UINT rtv_dxgi_fmt_caps = _sg_d3d11_dxgi_fmt_caps(_sg_d3d11_rtv_pixel_format((sg_pixel_format)fmt));
        const UINT dsv_dxgi_fmt_caps = _sg_d3d11_dxgi_fmt_caps(_sg_d3d11_dsv_pixel_format((sg_pixel_format)fmt));
        _sg_pixelformat_info_t* info = &_sg.formats[fmt];
        const bool render = 0 != (rtv_dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_RENDER_TARGET);
        const bool depth  = 0 != (dsv_dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL);
        info->sample = 0 != (srv_dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_TEXTURE2D);
        info->filter = 0 != (srv_dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE);
        info->render = render || depth;
        info->blend  = 0 != (rtv_dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_BLENDABLE);
        info->msaa   = 0 != (rtv_dxgi_fmt_caps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET);
        info->depth  = depth;
    }
}

_SOKOL_PRIVATE void _sg_d3d11_setup_backend(const sg_desc* desc) {
    // assume _sg.d3d11 already is zero-initialized
    SOKOL_ASSERT(desc);
    SOKOL_ASSERT(desc->environment.d3d11.device);
    SOKOL_ASSERT(desc->environment.d3d11.device_context);
    _sg.d3d11.valid = true;
    _sg.d3d11.dev = (ID3D11Device*) desc->environment.d3d11.device;
    _sg.d3d11.ctx = (ID3D11DeviceContext*) desc->environment.d3d11.device_context;
    _sg_d3d11_init_caps();
}

_SOKOL_PRIVATE void _sg_d3d11_discard_backend(void) {
    SOKOL_ASSERT(_sg.d3d11.valid);
    _sg.d3d11.valid = false;
}

_SOKOL_PRIVATE void _sg_d3d11_clear_state(void) {
    // clear all the device context state, so that resource refs don't keep stuck in the d3d device context
    _sg_d3d11_ClearState(_sg.d3d11.ctx);
}

_SOKOL_PRIVATE void _sg_d3d11_reset_state_cache(void) {
    // just clear the d3d11 device context state
    _sg_d3d11_clear_state();
}

_SOKOL_PRIVATE sg_resource_state _sg_d3d11_create_buffer(_sg_buffer_t* buf, const sg_buffer_desc* desc) {
    SOKOL_ASSERT(buf && desc);
    SOKOL_ASSERT(!buf->d3d11.buf);
    const bool injected = (0 != desc->d3d11_buffer);
    if (injected) {
        buf->d3d11.buf = (ID3D11Buffer*) desc->d3d11_buffer;
        _sg_d3d11_AddRef(buf->d3d11.buf);
        // FIXME: for storage buffers also need to inject resource view
    } else {
        D3D11_BUFFER_DESC d3d11_buf_desc;
        _sg_clear(&d3d11_buf_desc, sizeof(d3d11_buf_desc));
        d3d11_buf_desc.ByteWidth = (UINT)buf->cmn.size;
        d3d11_buf_desc.Usage = _sg_d3d11_usage(buf->cmn.usage);
        d3d11_buf_desc.BindFlags = _sg_d3d11_buffer_bind_flags(buf->cmn.type);
        d3d11_buf_desc.CPUAccessFlags = _sg_d3d11_cpu_access_flags(buf->cmn.usage);
        d3d11_buf_desc.MiscFlags = _sg_d3d11_buffer_misc_flags(buf->cmn.type);
        D3D11_SUBRESOURCE_DATA* init_data_ptr = 0;
        D3D11_SUBRESOURCE_DATA init_data;
        _sg_clear(&init_data, sizeof(init_data));
        if (buf->cmn.usage == SG_USAGE_IMMUTABLE) {
            SOKOL_ASSERT(desc->data.ptr);
            init_data.pSysMem = desc->data.ptr;
            init_data_ptr = &init_data;
        }
        HRESULT hr = _sg_d3d11_CreateBuffer(_sg.d3d11.dev, &d3d11_buf_desc, init_data_ptr, &buf->d3d11.buf);
        if (!(SUCCEEDED(hr) && buf->d3d11.buf)) {
            _SG_ERROR(D3D11_CREATE_BUFFER_FAILED);
            return SG_RESOURCESTATE_FAILED;
        }

        // for storage buffers need to create a view object
        if (buf->cmn.type == SG_BUFFERTYPE_STORAGEBUFFER) {
            // FIXME: currently only shader-resource-view, in future also UAV
            // storage buffer size must be multiple of 4
            SOKOL_ASSERT(_sg_multiple_u64(buf->cmn.size, 4));
            D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_srv_desc;
            _sg_clear(&d3d11_srv_desc, sizeof(d3d11_srv_desc));
            d3d11_srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
            d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
            d3d11_srv_desc.BufferEx.FirstElement = 0;
            d3d11_srv_desc.BufferEx.NumElements = buf->cmn.size / 4;
            d3d11_srv_desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
            hr = _sg_d3d11_CreateShaderResourceView(_sg.d3d11.dev, (ID3D11Resource*)buf->d3d11.buf, &d3d11_srv_desc, &buf->d3d11.srv);
            if (!(SUCCEEDED(hr) && buf->d3d11.srv)) {
                _SG_ERROR(D3D11_CREATE_BUFFER_SRV_FAILED);
                return SG_RESOURCESTATE_FAILED;
            }
        }
        _sg_d3d11_setlabel(buf->d3d11.buf, desc->label);
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_d3d11_discard_buffer(_sg_buffer_t* buf) {
    SOKOL_ASSERT(buf);
    if (buf->d3d11.buf) {
        _sg_d3d11_Release(buf->d3d11.buf);
    }
    if (buf->d3d11.srv) {
        _sg_d3d11_Release(buf->d3d11.srv);
    }
}

_SOKOL_PRIVATE void _sg_d3d11_fill_subres_data(const _sg_image_t* img, const sg_image_data* data) {
    const int num_faces = (img->cmn.type == SG_IMAGETYPE_CUBE) ? 6:1;
    const int num_slices = (img->cmn.type == SG_IMAGETYPE_ARRAY) ? img->cmn.num_slices:1;
    int subres_index = 0;
    for (int face_index = 0; face_index < num_faces; face_index++) {
        for (int slice_index = 0; slice_index < num_slices; slice_index++) {
            for (int mip_index = 0; mip_index < img->cmn.num_mipmaps; mip_index++, subres_index++) {
                SOKOL_ASSERT(subres_index < (SG_MAX_MIPMAPS * SG_MAX_TEXTUREARRAY_LAYERS));
                D3D11_SUBRESOURCE_DATA* subres_data = &_sg.d3d11.subres_data[subres_index];
                const int mip_width = _sg_miplevel_dim(img->cmn.width, mip_index);
                const int mip_height = _sg_miplevel_dim(img->cmn.height, mip_index);
                const sg_range* subimg_data = &(data->subimage[face_index][mip_index]);
                const size_t slice_size = subimg_data->size / (size_t)num_slices;
                const size_t slice_offset = slice_size * (size_t)slice_index;
                const uint8_t* ptr = (const uint8_t*) subimg_data->ptr;
                subres_data->pSysMem = ptr + slice_offset;
                subres_data->SysMemPitch = (UINT)_sg_row_pitch(img->cmn.pixel_format, mip_width, 1);
                if (img->cmn.type == SG_IMAGETYPE_3D) {
                    // FIXME? const int mip_depth = _sg_miplevel_dim(img->depth, mip_index);
                    subres_data->SysMemSlicePitch = (UINT)_sg_surface_pitch(img->cmn.pixel_format, mip_width, mip_height, 1);
                } else {
                    subres_data->SysMemSlicePitch = 0;
                }
            }
        }
    }
}

_SOKOL_PRIVATE sg_resource_state _sg_d3d11_create_image(_sg_image_t* img, const sg_image_desc* desc) {
    SOKOL_ASSERT(img && desc);
    SOKOL_ASSERT((0 == img->d3d11.tex2d) && (0 == img->d3d11.tex3d) && (0 == img->d3d11.res) && (0 == img->d3d11.srv));
    HRESULT hr;

    const bool injected = (0 != desc->d3d11_texture);
    const bool msaa = (img->cmn.sample_count > 1);
    img->d3d11.format = _sg_d3d11_texture_pixel_format(img->cmn.pixel_format);
    if (img->d3d11.format == DXGI_FORMAT_UNKNOWN) {
        _SG_ERROR(D3D11_CREATE_2D_TEXTURE_UNSUPPORTED_PIXEL_FORMAT);
        return SG_RESOURCESTATE_FAILED;
    }

    // prepare initial content pointers
    D3D11_SUBRESOURCE_DATA* init_data = 0;
    if (!injected && (img->cmn.usage == SG_USAGE_IMMUTABLE) && !img->cmn.render_target) {
        _sg_d3d11_fill_subres_data(img, &desc->data);
        init_data = _sg.d3d11.subres_data;
    }
    if (img->cmn.type != SG_IMAGETYPE_3D) {
        // 2D-, cube- or array-texture
        // first check for injected texture and/or resource view
        if (injected) {
            img->d3d11.tex2d = (ID3D11Texture2D*) desc->d3d11_texture;
            _sg_d3d11_AddRef(img->d3d11.tex2d);
            img->d3d11.srv = (ID3D11ShaderResourceView*) desc->d3d11_shader_resource_view;
            if (img->d3d11.srv) {
                _sg_d3d11_AddRef(img->d3d11.srv);
            }
        } else {
            // if not injected, create 2D texture
            D3D11_TEXTURE2D_DESC d3d11_tex_desc;
            _sg_clear(&d3d11_tex_desc, sizeof(d3d11_tex_desc));
            d3d11_tex_desc.Width = (UINT)img->cmn.width;
            d3d11_tex_desc.Height = (UINT)img->cmn.height;
            d3d11_tex_desc.MipLevels = (UINT)img->cmn.num_mipmaps;
            switch (img->cmn.type) {
                case SG_IMAGETYPE_ARRAY:    d3d11_tex_desc.ArraySize = (UINT)img->cmn.num_slices; break;
                case SG_IMAGETYPE_CUBE:     d3d11_tex_desc.ArraySize = 6; break;
                default:                    d3d11_tex_desc.ArraySize = 1; break;
            }
            d3d11_tex_desc.Format = img->d3d11.format;
            if (img->cmn.render_target) {
                d3d11_tex_desc.Usage = D3D11_USAGE_DEFAULT;
                if (_sg_is_depth_or_depth_stencil_format(img->cmn.pixel_format)) {
                    d3d11_tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                } else {
                    d3d11_tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
                }
                if (!msaa) {
                    d3d11_tex_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
                }
                d3d11_tex_desc.CPUAccessFlags = 0;
            } else {
                d3d11_tex_desc.Usage = _sg_d3d11_usage(img->cmn.usage);
                d3d11_tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                d3d11_tex_desc.CPUAccessFlags = _sg_d3d11_cpu_access_flags(img->cmn.usage);
            }
            d3d11_tex_desc.SampleDesc.Count = (UINT)img->cmn.sample_count;
            d3d11_tex_desc.SampleDesc.Quality = (UINT) (msaa ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0);
            d3d11_tex_desc.MiscFlags = (img->cmn.type == SG_IMAGETYPE_CUBE) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

            hr = _sg_d3d11_CreateTexture2D(_sg.d3d11.dev, &d3d11_tex_desc, init_data, &img->d3d11.tex2d);
            if (!(SUCCEEDED(hr) && img->d3d11.tex2d)) {
                _SG_ERROR(D3D11_CREATE_2D_TEXTURE_FAILED);
                return SG_RESOURCESTATE_FAILED;
            }
            _sg_d3d11_setlabel(img->d3d11.tex2d, desc->label);

            // create shader-resource-view for 2D texture
            // FIXME: currently we don't support setting MSAA texture as shader resource
            if (!msaa) {
                D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_srv_desc;
                _sg_clear(&d3d11_srv_desc, sizeof(d3d11_srv_desc));
                d3d11_srv_desc.Format = _sg_d3d11_srv_pixel_format(img->cmn.pixel_format);
                switch (img->cmn.type) {
                    case SG_IMAGETYPE_2D:
                        d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                        d3d11_srv_desc.Texture2D.MipLevels = (UINT)img->cmn.num_mipmaps;
                        break;
                    case SG_IMAGETYPE_CUBE:
                        d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                        d3d11_srv_desc.TextureCube.MipLevels = (UINT)img->cmn.num_mipmaps;
                        break;
                    case SG_IMAGETYPE_ARRAY:
                        d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                        d3d11_srv_desc.Texture2DArray.MipLevels = (UINT)img->cmn.num_mipmaps;
                        d3d11_srv_desc.Texture2DArray.ArraySize = (UINT)img->cmn.num_slices;
                        break;
                    default:
                        SOKOL_UNREACHABLE; break;
                }
                hr = _sg_d3d11_CreateShaderResourceView(_sg.d3d11.dev, (ID3D11Resource*)img->d3d11.tex2d, &d3d11_srv_desc, &img->d3d11.srv);
                if (!(SUCCEEDED(hr) && img->d3d11.srv)) {
                    _SG_ERROR(D3D11_CREATE_2D_SRV_FAILED);
                    return SG_RESOURCESTATE_FAILED;
                }
                _sg_d3d11_setlabel(img->d3d11.srv, desc->label);
            }
        }
        SOKOL_ASSERT(img->d3d11.tex2d);
        img->d3d11.res = (ID3D11Resource*)img->d3d11.tex2d;
        _sg_d3d11_AddRef(img->d3d11.res);
    } else {
        // 3D texture - same procedure, first check if injected, than create non-injected
        if (injected) {
            img->d3d11.tex3d = (ID3D11Texture3D*) desc->d3d11_texture;
            _sg_d3d11_AddRef(img->d3d11.tex3d);
            img->d3d11.srv = (ID3D11ShaderResourceView*) desc->d3d11_shader_resource_view;
            if (img->d3d11.srv) {
                _sg_d3d11_AddRef(img->d3d11.srv);
            }
        } else {
            // not injected, create 3d texture
            D3D11_TEXTURE3D_DESC d3d11_tex_desc;
            _sg_clear(&d3d11_tex_desc, sizeof(d3d11_tex_desc));
            d3d11_tex_desc.Width = (UINT)img->cmn.width;
            d3d11_tex_desc.Height = (UINT)img->cmn.height;
            d3d11_tex_desc.Depth = (UINT)img->cmn.num_slices;
            d3d11_tex_desc.MipLevels = (UINT)img->cmn.num_mipmaps;
            d3d11_tex_desc.Format = img->d3d11.format;
            if (img->cmn.render_target) {
                d3d11_tex_desc.Usage = D3D11_USAGE_DEFAULT;
                d3d11_tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
                d3d11_tex_desc.CPUAccessFlags = 0;
            } else {
                d3d11_tex_desc.Usage = _sg_d3d11_usage(img->cmn.usage);
                d3d11_tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                d3d11_tex_desc.CPUAccessFlags = _sg_d3d11_cpu_access_flags(img->cmn.usage);
            }
            if (img->d3d11.format == DXGI_FORMAT_UNKNOWN) {
                _SG_ERROR(D3D11_CREATE_3D_TEXTURE_UNSUPPORTED_PIXEL_FORMAT);
                return SG_RESOURCESTATE_FAILED;
            }
            hr = _sg_d3d11_CreateTexture3D(_sg.d3d11.dev, &d3d11_tex_desc, init_data, &img->d3d11.tex3d);
            if (!(SUCCEEDED(hr) && img->d3d11.tex3d)) {
                _SG_ERROR(D3D11_CREATE_3D_TEXTURE_FAILED);
                return SG_RESOURCESTATE_FAILED;
            }
            _sg_d3d11_setlabel(img->d3d11.tex3d, desc->label);

            // create shader-resource-view for 3D texture
            if (!msaa) {
                D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_srv_desc;
                _sg_clear(&d3d11_srv_desc, sizeof(d3d11_srv_desc));
                d3d11_srv_desc.Format = _sg_d3d11_srv_pixel_format(img->cmn.pixel_format);
                d3d11_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
                d3d11_srv_desc.Texture3D.MipLevels = (UINT)img->cmn.num_mipmaps;
                hr = _sg_d3d11_CreateShaderResourceView(_sg.d3d11.dev, (ID3D11Resource*)img->d3d11.tex3d, &d3d11_srv_desc, &img->d3d11.srv);
                if (!(SUCCEEDED(hr) && img->d3d11.srv)) {
                    _SG_ERROR(D3D11_CREATE_3D_SRV_FAILED);
                    return SG_RESOURCESTATE_FAILED;
                }
                _sg_d3d11_setlabel(img->d3d11.srv, desc->label);
            }
        }
        SOKOL_ASSERT(img->d3d11.tex3d);
        img->d3d11.res = (ID3D11Resource*)img->d3d11.tex3d;
        _sg_d3d11_AddRef(img->d3d11.res);
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_d3d11_discard_image(_sg_image_t* img) {
    SOKOL_ASSERT(img);
    if (img->d3d11.tex2d) {
        _sg_d3d11_Release(img->d3d11.tex2d);
    }
    if (img->d3d11.tex3d) {
        _sg_d3d11_Release(img->d3d11.tex3d);
    }
    if (img->d3d11.res) {
        _sg_d3d11_Release(img->d3d11.res);
    }
    if (img->d3d11.srv) {
        _sg_d3d11_Release(img->d3d11.srv);
    }
}

_SOKOL_PRIVATE sg_resource_state _sg_d3d11_create_sampler(_sg_sampler_t* smp, const sg_sampler_desc* desc) {
    SOKOL_ASSERT(smp && desc);
    SOKOL_ASSERT(0 == smp->d3d11.smp);
    const bool injected = (0 != desc->d3d11_sampler);
    if (injected) {
        smp->d3d11.smp = (ID3D11SamplerState*)desc->d3d11_sampler;
        _sg_d3d11_AddRef(smp->d3d11.smp);
    } else {
        D3D11_SAMPLER_DESC d3d11_smp_desc;
        _sg_clear(&d3d11_smp_desc, sizeof(d3d11_smp_desc));
        d3d11_smp_desc.Filter = _sg_d3d11_filter(desc->min_filter, desc->mag_filter, desc->mipmap_filter, desc->compare != SG_COMPAREFUNC_NEVER, desc->max_anisotropy);
        d3d11_smp_desc.AddressU = _sg_d3d11_address_mode(desc->wrap_u);
        d3d11_smp_desc.AddressV = _sg_d3d11_address_mode(desc->wrap_v);
        d3d11_smp_desc.AddressW = _sg_d3d11_address_mode(desc->wrap_w);
        d3d11_smp_desc.MipLODBias = 0.0f; // FIXME?
        switch (desc->border_color) {
            case SG_BORDERCOLOR_TRANSPARENT_BLACK:
                // all 0.0f
                break;
            case SG_BORDERCOLOR_OPAQUE_WHITE:
                for (int i = 0; i < 4; i++) {
                    d3d11_smp_desc.BorderColor[i] = 1.0f;
                }
                break;
            default:
                // opaque black
                d3d11_smp_desc.BorderColor[3] = 1.0f;
                break;
        }
        d3d11_smp_desc.MaxAnisotropy = desc->max_anisotropy;
        d3d11_smp_desc.ComparisonFunc = _sg_d3d11_compare_func(desc->compare);
        d3d11_smp_desc.MinLOD = desc->min_lod;
        d3d11_smp_desc.MaxLOD = desc->max_lod;
        HRESULT hr = _sg_d3d11_CreateSamplerState(_sg.d3d11.dev, &d3d11_smp_desc, &smp->d3d11.smp);
        if (!(SUCCEEDED(hr) && smp->d3d11.smp)) {
            _SG_ERROR(D3D11_CREATE_SAMPLER_STATE_FAILED);
            return SG_RESOURCESTATE_FAILED;
        }
        _sg_d3d11_setlabel(smp->d3d11.smp, desc->label);
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_d3d11_discard_sampler(_sg_sampler_t* smp) {
    SOKOL_ASSERT(smp);
    if (smp->d3d11.smp) {
        _sg_d3d11_Release(smp->d3d11.smp);
    }
}

_SOKOL_PRIVATE bool _sg_d3d11_load_d3dcompiler_dll(void) {
    if ((0 == _sg.d3d11.d3dcompiler_dll) && !_sg.d3d11.d3dcompiler_dll_load_failed) {
        _sg.d3d11.d3dcompiler_dll = LoadLibraryA("d3dcompiler_47.dll");
        if (0 == _sg.d3d11.d3dcompiler_dll) {
            // don't attempt to load missing DLL in the future
            _SG_ERROR(D3D11_LOAD_D3DCOMPILER_47_DLL_FAILED);
            _sg.d3d11.d3dcompiler_dll_load_failed = true;
            return false;
        }
        // look up function pointers
        _sg.d3d11.D3DCompile_func = (pD3DCompile)(void*) GetProcAddress(_sg.d3d11.d3dcompiler_dll, "D3DCompile");
        SOKOL_ASSERT(_sg.d3d11.D3DCompile_func);
    }
    return 0 != _sg.d3d11.d3dcompiler_dll;
}

_SOKOL_PRIVATE ID3DBlob* _sg_d3d11_compile_shader(const sg_shader_stage_desc* stage_desc) {
    if (!_sg_d3d11_load_d3dcompiler_dll()) {
        return NULL;
    }
    SOKOL_ASSERT(stage_desc->d3d11_target);
    ID3DBlob* output = NULL;
    ID3DBlob* errors_or_warnings = NULL;
    HRESULT hr = _sg.d3d11.D3DCompile_func(
        stage_desc->source,             // pSrcData
        strlen(stage_desc->source),     // SrcDataSize
        NULL,                           // pSourceName
        NULL,                           // pDefines
        NULL,                           // pInclude
        stage_desc->entry ? stage_desc->entry : "main",     // pEntryPoint
        stage_desc->d3d11_target,       // pTarget
        D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_OPTIMIZATION_LEVEL3,   // Flags1
        0,          // Flags2
        &output,    // ppCode
        &errors_or_warnings);   // ppErrorMsgs
    if (FAILED(hr)) {
        _SG_ERROR(D3D11_SHADER_COMPILATION_FAILED);
    }
    if (errors_or_warnings) {
        _SG_WARN(D3D11_SHADER_COMPILATION_OUTPUT);
        _SG_LOGMSG(D3D11_SHADER_COMPILATION_OUTPUT, (LPCSTR)_sg_d3d11_GetBufferPointer(errors_or_warnings));
        _sg_d3d11_Release(errors_or_warnings); errors_or_warnings = NULL;
    }
    if (FAILED(hr)) {
        // just in case, usually output is NULL here
        if (output) {
            _sg_d3d11_Release(output);
            output = NULL;
        }
    }
    return output;
}

_SOKOL_PRIVATE sg_resource_state _sg_d3d11_create_shader(_sg_shader_t* shd, const sg_shader_desc* desc) {
    SOKOL_ASSERT(shd && desc);
    SOKOL_ASSERT(!shd->d3d11.vs && !shd->d3d11.fs && !shd->d3d11.vs_blob);
    HRESULT hr;

    // copy vertex attribute semantic names and indices
    for (int i = 0; i < SG_MAX_VERTEX_ATTRIBUTES; i++) {
        _sg_strcpy(&shd->d3d11.attrs[i].sem_name, desc->attrs[i].sem_name);
        shd->d3d11.attrs[i].sem_index = desc->attrs[i].sem_index;
    }

    // shader stage uniform blocks and image slots
    for (int stage_index = 0; stage_index < SG_NUM_SHADER_STAGES; stage_index++) {
        _sg_shader_stage_t* cmn_stage = &shd->cmn.stage[stage_index];
        _sg_d3d11_shader_stage_t* d3d11_stage = &shd->d3d11.stage[stage_index];
        for (int ub_index = 0; ub_index < cmn_stage->num_uniform_blocks; ub_index++) {
            const _sg_shader_uniform_block_t* ub = &cmn_stage->uniform_blocks[ub_index];

            // create a D3D constant buffer for each uniform block
            SOKOL_ASSERT(0 == d3d11_stage->cbufs[ub_index]);
            D3D11_BUFFER_DESC cb_desc;
            _sg_clear(&cb_desc, sizeof(cb_desc));
            cb_desc.ByteWidth = (UINT)_sg_roundup((int)ub->size, 16);
            cb_desc.Usage = D3D11_USAGE_DEFAULT;
            cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            hr = _sg_d3d11_CreateBuffer(_sg.d3d11.dev, &cb_desc, NULL, &d3d11_stage->cbufs[ub_index]);
            if (!(SUCCEEDED(hr) && d3d11_stage->cbufs[ub_index])) {
                _SG_ERROR(D3D11_CREATE_CONSTANT_BUFFER_FAILED);
                return SG_RESOURCESTATE_FAILED;
            }
            _sg_d3d11_setlabel(d3d11_stage->cbufs[ub_index], desc->label);
        }
    }

    const void* vs_ptr = 0, *fs_ptr = 0;
    SIZE_T vs_length = 0, fs_length = 0;
    ID3DBlob* vs_blob = 0, *fs_blob = 0;
    if (desc->vs.bytecode.ptr && desc->fs.bytecode.ptr) {
        // create from shader byte code
        vs_ptr = desc->vs.bytecode.ptr;
        fs_ptr = desc->fs.bytecode.ptr;
        vs_length = desc->vs.bytecode.size;
        fs_length = desc->fs.bytecode.size;
    } else {
        // compile from shader source code
        vs_blob = _sg_d3d11_compile_shader(&desc->vs);
        fs_blob = _sg_d3d11_compile_shader(&desc->fs);
        if (vs_blob && fs_blob) {
            vs_ptr = _sg_d3d11_GetBufferPointer(vs_blob);
            vs_length = _sg_d3d11_GetBufferSize(vs_blob);
            fs_ptr = _sg_d3d11_GetBufferPointer(fs_blob);
            fs_length = _sg_d3d11_GetBufferSize(fs_blob);
        }
    }
    sg_resource_state result = SG_RESOURCESTATE_FAILED;
    if (vs_ptr && fs_ptr && (vs_length > 0) && (fs_length > 0)) {
        // create the D3D vertex- and pixel-shader objects
        hr = _sg_d3d11_CreateVertexShader(_sg.d3d11.dev, vs_ptr, vs_length, NULL, &shd->d3d11.vs);
        bool vs_succeeded = SUCCEEDED(hr) && shd->d3d11.vs;
        hr = _sg_d3d11_CreatePixelShader(_sg.d3d11.dev, fs_ptr, fs_length, NULL, &shd->d3d11.fs);
        bool fs_succeeded = SUCCEEDED(hr) && shd->d3d11.fs;

        // need to store the vertex shader byte code, this is needed later in sg_create_pipeline
        if (vs_succeeded && fs_succeeded) {
            shd->d3d11.vs_blob_length = vs_length;
            shd->d3d11.vs_blob = _sg_malloc((size_t)vs_length);
            SOKOL_ASSERT(shd->d3d11.vs_blob);
            memcpy(shd->d3d11.vs_blob, vs_ptr, vs_length);
            result = SG_RESOURCESTATE_VALID;
            _sg_d3d11_setlabel(shd->d3d11.vs, desc->label);
            _sg_d3d11_setlabel(shd->d3d11.fs, desc->label);
        }
    }
    if (vs_blob) {
        _sg_d3d11_Release(vs_blob); vs_blob = 0;
    }
    if (fs_blob) {
        _sg_d3d11_Release(fs_blob); fs_blob = 0;
    }
    return result;
}

_SOKOL_PRIVATE void _sg_d3d11_discard_shader(_sg_shader_t* shd) {
    SOKOL_ASSERT(shd);
    if (shd->d3d11.vs) {
        _sg_d3d11_Release(shd->d3d11.vs);
    }
    if (shd->d3d11.fs) {
        _sg_d3d11_Release(shd->d3d11.fs);
    }
    if (shd->d3d11.vs_blob) {
        _sg_free(shd->d3d11.vs_blob);
    }
    for (int stage_index = 0; stage_index < SG_NUM_SHADER_STAGES; stage_index++) {
        _sg_shader_stage_t* cmn_stage = &shd->cmn.stage[stage_index];
        _sg_d3d11_shader_stage_t* d3d11_stage = &shd->d3d11.stage[stage_index];
        for (int ub_index = 0; ub_index < cmn_stage->num_uniform_blocks; ub_index++) {
            if (d3d11_stage->cbufs[ub_index]) {
                _sg_d3d11_Release(d3d11_stage->cbufs[ub_index]);
            }
        }
    }
}

_SOKOL_PRIVATE sg_resource_state _sg_d3d11_create_pipeline(_sg_pipeline_t* pip, _sg_shader_t* shd, const sg_pipeline_desc* desc) {
    SOKOL_ASSERT(pip && shd && desc);
    SOKOL_ASSERT(desc->shader.id == shd->slot.id);
    SOKOL_ASSERT(shd->slot.state == SG_RESOURCESTATE_VALID);
    SOKOL_ASSERT(shd->d3d11.vs_blob && shd->d3d11.vs_blob_length > 0);
    SOKOL_ASSERT(!pip->d3d11.il && !pip->d3d11.rs && !pip->d3d11.dss && !pip->d3d11.bs);

    pip->shader = shd;
    pip->d3d11.index_format = _sg_d3d11_index_format(pip->cmn.index_type);
    pip->d3d11.topology = _sg_d3d11_primitive_topology(desc->primitive_type);
    pip->d3d11.stencil_ref = desc->stencil.ref;

    // create input layout object
    HRESULT hr;
    D3D11_INPUT_ELEMENT_DESC d3d11_comps[SG_MAX_VERTEX_ATTRIBUTES];
    _sg_clear(d3d11_comps, sizeof(d3d11_comps));
    int attr_index = 0;
    for (; attr_index < SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        const sg_vertex_attr_state* a_state = &desc->layout.attrs[attr_index];
        if (a_state->format == SG_VERTEXFORMAT_INVALID) {
            break;
        }
        SOKOL_ASSERT(a_state->buffer_index < SG_MAX_VERTEX_BUFFERS);
        const sg_vertex_buffer_layout_state* l_state = &desc->layout.buffers[a_state->buffer_index];
        const sg_vertex_step step_func = l_state->step_func;
        const int step_rate = l_state->step_rate;
        D3D11_INPUT_ELEMENT_DESC* d3d11_comp = &d3d11_comps[attr_index];
        d3d11_comp->SemanticName = _sg_strptr(&shd->d3d11.attrs[attr_index].sem_name);
        d3d11_comp->SemanticIndex = (UINT)shd->d3d11.attrs[attr_index].sem_index;
        d3d11_comp->Format = _sg_d3d11_vertex_format(a_state->format);
        d3d11_comp->InputSlot = (UINT)a_state->buffer_index;
        d3d11_comp->AlignedByteOffset = (UINT)a_state->offset;
        d3d11_comp->InputSlotClass = _sg_d3d11_input_classification(step_func);
        if (SG_VERTEXSTEP_PER_INSTANCE == step_func) {
            d3d11_comp->InstanceDataStepRate = (UINT)step_rate;
            pip->cmn.use_instanced_draw = true;
        }
        pip->cmn.vertex_buffer_layout_active[a_state->buffer_index] = true;
    }
    for (int layout_index = 0; layout_index < SG_MAX_VERTEX_BUFFERS; layout_index++) {
        if (pip->cmn.vertex_buffer_layout_active[layout_index]) {
            const sg_vertex_buffer_layout_state* l_state = &desc->layout.buffers[layout_index];
            SOKOL_ASSERT(l_state->stride > 0);
            pip->d3d11.vb_strides[layout_index] = (UINT)l_state->stride;
        } else {
            pip->d3d11.vb_strides[layout_index] = 0;
        }
    }
    if (attr_index > 0) {
        hr = _sg_d3d11_CreateInputLayout(_sg.d3d11.dev,
            d3d11_comps,                // pInputElementDesc
            (UINT)attr_index,           // NumElements
            shd->d3d11.vs_blob,         // pShaderByteCodeWithInputSignature
            shd->d3d11.vs_blob_length,  // BytecodeLength
            &pip->d3d11.il);
        if (!(SUCCEEDED(hr) && pip->d3d11.il)) {
            _SG_ERROR(D3D11_CREATE_INPUT_LAYOUT_FAILED);
            return SG_RESOURCESTATE_FAILED;
        }
        _sg_d3d11_setlabel(pip->d3d11.il, desc->label);
    }

    // create rasterizer state
    D3D11_RASTERIZER_DESC rs_desc;
    _sg_clear(&rs_desc, sizeof(rs_desc));
    rs_desc.FillMode = D3D11_FILL_SOLID;
    rs_desc.CullMode = _sg_d3d11_cull_mode(desc->cull_mode);
    rs_desc.FrontCounterClockwise = desc->face_winding == SG_FACEWINDING_CCW;
    rs_desc.DepthBias = (INT) pip->cmn.depth.bias;
    rs_desc.DepthBiasClamp = pip->cmn.depth.bias_clamp;
    rs_desc.SlopeScaledDepthBias = pip->cmn.depth.bias_slope_scale;
    rs_desc.DepthClipEnable = TRUE;
    rs_desc.ScissorEnable = TRUE;
    rs_desc.MultisampleEnable = desc->sample_count > 1;
    rs_desc.AntialiasedLineEnable = FALSE;
    hr = _sg_d3d11_CreateRasterizerState(_sg.d3d11.dev, &rs_desc, &pip->d3d11.rs);
    if (!(SUCCEEDED(hr) && pip->d3d11.rs)) {
        _SG_ERROR(D3D11_CREATE_RASTERIZER_STATE_FAILED);
        return SG_RESOURCESTATE_FAILED;
    }
    _sg_d3d11_setlabel(pip->d3d11.rs, desc->label);

    // create depth-stencil state
    D3D11_DEPTH_STENCIL_DESC dss_desc;
    _sg_clear(&dss_desc, sizeof(dss_desc));
    dss_desc.DepthEnable = TRUE;
    dss_desc.DepthWriteMask = desc->depth.write_enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    dss_desc.DepthFunc = _sg_d3d11_compare_func(desc->depth.compare);
    dss_desc.StencilEnable = desc->stencil.enabled;
    dss_desc.StencilReadMask = desc->stencil.read_mask;
    dss_desc.StencilWriteMask = desc->stencil.write_mask;
    const sg_stencil_face_state* sf = &desc->stencil.front;
    dss_desc.FrontFace.StencilFailOp = _sg_d3d11_stencil_op(sf->fail_op);
    dss_desc.FrontFace.StencilDepthFailOp = _sg_d3d11_stencil_op(sf->depth_fail_op);
    dss_desc.FrontFace.StencilPassOp = _sg_d3d11_stencil_op(sf->pass_op);
    dss_desc.FrontFace.StencilFunc = _sg_d3d11_compare_func(sf->compare);
    const sg_stencil_face_state* sb = &desc->stencil.back;
    dss_desc.BackFace.StencilFailOp = _sg_d3d11_stencil_op(sb->fail_op);
    dss_desc.BackFace.StencilDepthFailOp = _sg_d3d11_stencil_op(sb->depth_fail_op);
    dss_desc.BackFace.StencilPassOp = _sg_d3d11_stencil_op(sb->pass_op);
    dss_desc.BackFace.StencilFunc = _sg_d3d11_compare_func(sb->compare);
    hr = _sg_d3d11_CreateDepthStencilState(_sg.d3d11.dev, &dss_desc, &pip->d3d11.dss);
    if (!(SUCCEEDED(hr) && pip->d3d11.dss)) {
        _SG_ERROR(D3D11_CREATE_DEPTH_STENCIL_STATE_FAILED);
        return SG_RESOURCESTATE_FAILED;
    }
    _sg_d3d11_setlabel(pip->d3d11.dss, desc->label);

    // create blend state
    D3D11_BLEND_DESC bs_desc;
    _sg_clear(&bs_desc, sizeof(bs_desc));
    bs_desc.AlphaToCoverageEnable = desc->alpha_to_coverage_enabled;
    bs_desc.IndependentBlendEnable = TRUE;
    {
        int i = 0;
        for (i = 0; i < desc->color_count; i++) {
            const sg_blend_state* src = &desc->colors[i].blend;
            D3D11_RENDER_TARGET_BLEND_DESC* dst = &bs_desc.RenderTarget[i];
            dst->BlendEnable = src->enabled;
            dst->SrcBlend = _sg_d3d11_blend_factor(src->src_factor_rgb);
            dst->DestBlend = _sg_d3d11_blend_factor(src->dst_factor_rgb);
            dst->BlendOp = _sg_d3d11_blend_op(src->op_rgb);
            dst->SrcBlendAlpha = _sg_d3d11_blend_factor(src->src_factor_alpha);
            dst->DestBlendAlpha = _sg_d3d11_blend_factor(src->dst_factor_alpha);
            dst->BlendOpAlpha = _sg_d3d11_blend_op(src->op_alpha);
            dst->RenderTargetWriteMask = _sg_d3d11_color_write_mask(desc->colors[i].write_mask);
        }
        for (; i < 8; i++) {
            D3D11_RENDER_TARGET_BLEND_DESC* dst = &bs_desc.RenderTarget[i];
            dst->BlendEnable = FALSE;
            dst->SrcBlend = dst->SrcBlendAlpha = D3D11_BLEND_ONE;
            dst->DestBlend = dst->DestBlendAlpha = D3D11_BLEND_ZERO;
            dst->BlendOp = dst->BlendOpAlpha = D3D11_BLEND_OP_ADD;
            dst->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }
    }
    hr = _sg_d3d11_CreateBlendState(_sg.d3d11.dev, &bs_desc, &pip->d3d11.bs);
    if (!(SUCCEEDED(hr) && pip->d3d11.bs)) {
        _SG_ERROR(D3D11_CREATE_BLEND_STATE_FAILED);
        return SG_RESOURCESTATE_FAILED;
    }
    _sg_d3d11_setlabel(pip->d3d11.bs, desc->label);
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_d3d11_discard_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    if (pip == _sg.d3d11.cur_pipeline) {
        _sg.d3d11.cur_pipeline = 0;
        _sg.d3d11.cur_pipeline_id.id = SG_INVALID_ID;
    }
    if (pip->d3d11.il) {
        _sg_d3d11_Release(pip->d3d11.il);
    }
    if (pip->d3d11.rs) {
        _sg_d3d11_Release(pip->d3d11.rs);
    }
    if (pip->d3d11.dss) {
        _sg_d3d11_Release(pip->d3d11.dss);
    }
    if (pip->d3d11.bs) {
        _sg_d3d11_Release(pip->d3d11.bs);
    }
}

_SOKOL_PRIVATE sg_resource_state _sg_d3d11_create_attachments(_sg_attachments_t* atts, _sg_image_t** color_images, _sg_image_t** resolve_images, _sg_image_t* ds_img, const sg_attachments_desc* desc) {
    SOKOL_ASSERT(atts && desc);
    SOKOL_ASSERT(color_images && resolve_images);
    SOKOL_ASSERT(_sg.d3d11.dev);

    // copy image pointers
    for (int i = 0; i < atts->cmn.num_colors; i++) {
        const sg_attachment_desc* color_desc = &desc->colors[i];
        _SOKOL_UNUSED(color_desc);
        SOKOL_ASSERT(color_desc->image.id != SG_INVALID_ID);
        SOKOL_ASSERT(0 == atts->d3d11.colors[i].image);
        SOKOL_ASSERT(color_images[i] && (color_images[i]->slot.id == color_desc->image.id));
        SOKOL_ASSERT(_sg_is_valid_rendertarget_color_format(color_images[i]->cmn.pixel_format));
        atts->d3d11.colors[i].image = color_images[i];

        const sg_attachment_desc* resolve_desc = &desc->resolves[i];
        if (resolve_desc->image.id != SG_INVALID_ID) {
            SOKOL_ASSERT(0 == atts->d3d11.resolves[i].image);
            SOKOL_ASSERT(resolve_images[i] && (resolve_images[i]->slot.id == resolve_desc->image.id));
            SOKOL_ASSERT(color_images[i] && (color_images[i]->cmn.pixel_format == resolve_images[i]->cmn.pixel_format));
            atts->d3d11.resolves[i].image = resolve_images[i];
        }
    }
    SOKOL_ASSERT(0 == atts->d3d11.depth_stencil.image);
    const sg_attachment_desc* ds_desc = &desc->depth_stencil;
    if (ds_desc->image.id != SG_INVALID_ID) {
        SOKOL_ASSERT(ds_img && (ds_img->slot.id == ds_desc->image.id));
        SOKOL_ASSERT(_sg_is_valid_rendertarget_depth_format(ds_img->cmn.pixel_format));
        atts->d3d11.depth_stencil.image = ds_img;
    }

    // create render-target views
    for (int i = 0; i < atts->cmn.num_colors; i++) {
        const _sg_attachment_common_t* cmn_color_att = &atts->cmn.colors[i];
        const _sg_image_t* color_img = color_images[i];
        SOKOL_ASSERT(0 == atts->d3d11.colors[i].view.rtv);
        const bool msaa = color_img->cmn.sample_count > 1;
        D3D11_RENDER_TARGET_VIEW_DESC d3d11_rtv_desc;
        _sg_clear(&d3d11_rtv_desc, sizeof(d3d11_rtv_desc));
        d3d11_rtv_desc.Format = _sg_d3d11_rtv_pixel_format(color_img->cmn.pixel_format);
        if (color_img->cmn.type == SG_IMAGETYPE_2D) {
            if (msaa) {
                d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
            } else {
                d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                d3d11_rtv_desc.Texture2D.MipSlice = (UINT)cmn_color_att->mip_level;
            }
        } else if ((color_img->cmn.type == SG_IMAGETYPE_CUBE) || (color_img->cmn.type == SG_IMAGETYPE_ARRAY)) {
            if (msaa) {
                d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                d3d11_rtv_desc.Texture2DMSArray.FirstArraySlice = (UINT)cmn_color_att->slice;
                d3d11_rtv_desc.Texture2DMSArray.ArraySize = 1;
            } else {
                d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                d3d11_rtv_desc.Texture2DArray.MipSlice = (UINT)cmn_color_att->mip_level;
                d3d11_rtv_desc.Texture2DArray.FirstArraySlice = (UINT)cmn_color_att->slice;
                d3d11_rtv_desc.Texture2DArray.ArraySize = 1;
            }
        } else {
            SOKOL_ASSERT(color_img->cmn.type == SG_IMAGETYPE_3D);
            SOKOL_ASSERT(!msaa);
            d3d11_rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
            d3d11_rtv_desc.Texture3D.MipSlice = (UINT)cmn_color_att->mip_level;
            d3d11_rtv_desc.Texture3D.FirstWSlice = (UINT)cmn_color_att->slice;
            d3d11_rtv_desc.Texture3D.WSize = 1;
        }
        SOKOL_ASSERT(color_img->d3d11.res);
        HRESULT hr = _sg_d3d11_CreateRenderTargetView(_sg.d3d11.dev, color_img->d3d11.res, &d3d11_rtv_desc, &atts->d3d11.colors[i].view.rtv);
        if (!(SUCCEEDED(hr) && atts->d3d11.colors[i].view.rtv)) {
            _SG_ERROR(D3D11_CREATE_RTV_FAILED);
            return SG_RESOURCESTATE_FAILED;
        }
        _sg_d3d11_setlabel(atts->d3d11.colors[i].view.rtv, desc->label);
    }
    SOKOL_ASSERT(0 == atts->d3d11.depth_stencil.view.dsv);
    if (ds_desc->image.id != SG_INVALID_ID) {
        const _sg_attachment_common_t* cmn_ds_att = &atts->cmn.depth_stencil;
        const bool msaa = ds_img->cmn.sample_count > 1;
        D3D11_DEPTH_STENCIL_VIEW_DESC d3d11_dsv_desc;
        _sg_clear(&d3d11_dsv_desc, sizeof(d3d11_dsv_desc));
        d3d11_dsv_desc.Format = _sg_d3d11_dsv_pixel_format(ds_img->cmn.pixel_format);
        SOKOL_ASSERT(ds_img && ds_img->cmn.type != SG_IMAGETYPE_3D);
        if (ds_img->cmn.type == SG_IMAGETYPE_2D) {
            if (msaa) {
                d3d11_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            } else {
                d3d11_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                d3d11_dsv_desc.Texture2D.MipSlice = (UINT)cmn_ds_att->mip_level;
            }
        } else if ((ds_img->cmn.type == SG_IMAGETYPE_CUBE) || (ds_img->cmn.type == SG_IMAGETYPE_ARRAY)) {
            if (msaa) {
                d3d11_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                d3d11_dsv_desc.Texture2DMSArray.FirstArraySlice = (UINT)cmn_ds_att->slice;
                d3d11_dsv_desc.Texture2DMSArray.ArraySize = 1;
            } else {
                d3d11_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                d3d11_dsv_desc.Texture2DArray.MipSlice = (UINT)cmn_ds_att->mip_level;
                d3d11_dsv_desc.Texture2DArray.FirstArraySlice = (UINT)cmn_ds_att->slice;
                d3d11_dsv_desc.Texture2DArray.ArraySize = 1;
            }
        }
        SOKOL_ASSERT(ds_img->d3d11.res);
        HRESULT hr = _sg_d3d11_CreateDepthStencilView(_sg.d3d11.dev, ds_img->d3d11.res, &d3d11_dsv_desc, &atts->d3d11.depth_stencil.view.dsv);
        if (!(SUCCEEDED(hr) && atts->d3d11.depth_stencil.view.dsv)) {
            _SG_ERROR(D3D11_CREATE_DSV_FAILED);
            return SG_RESOURCESTATE_FAILED;
        }
        _sg_d3d11_setlabel(atts->d3d11.depth_stencil.view.dsv, desc->label);
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_d3d11_discard_attachments(_sg_attachments_t* atts) {
    SOKOL_ASSERT(atts);
    for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
        if (atts->d3d11.colors[i].view.rtv) {
            _sg_d3d11_Release(atts->d3d11.colors[i].view.rtv);
        }
        if (atts->d3d11.resolves[i].view.rtv) {
            _sg_d3d11_Release(atts->d3d11.resolves[i].view.rtv);
        }
    }
    if (atts->d3d11.depth_stencil.view.dsv) {
        _sg_d3d11_Release(atts->d3d11.depth_stencil.view.dsv);
    }
}

_SOKOL_PRIVATE _sg_image_t* _sg_d3d11_attachments_color_image(const _sg_attachments_t* atts, int index) {
    SOKOL_ASSERT(atts && (index >= 0) && (index < SG_MAX_COLOR_ATTACHMENTS));
    return atts->d3d11.colors[index].image;
}

_SOKOL_PRIVATE _sg_image_t* _sg_d3d11_attachments_resolve_image(const _sg_attachments_t* atts, int index) {
    SOKOL_ASSERT(atts && (index >= 0) && (index < SG_MAX_COLOR_ATTACHMENTS));
    return atts->d3d11.resolves[index].image;
}

_SOKOL_PRIVATE _sg_image_t* _sg_d3d11_attachments_ds_image(const _sg_attachments_t* atts) {
    SOKOL_ASSERT(atts);
    return atts->d3d11.depth_stencil.image;
}

_SOKOL_PRIVATE void _sg_d3d11_begin_pass(const sg_pass* pass) {
    SOKOL_ASSERT(pass);

    const _sg_attachments_t* atts = _sg.cur_pass.atts;
    const sg_swapchain* swapchain = &pass->swapchain;
    const sg_pass_action* action = &pass->action;

    int num_rtvs = 0;
    ID3D11RenderTargetView* rtvs[SG_MAX_COLOR_ATTACHMENTS] = { 0 };
    ID3D11DepthStencilView* dsv = 0;
    _sg.d3d11.cur_pass.render_view = 0;
    _sg.d3d11.cur_pass.resolve_view = 0;
    if (atts) {
        num_rtvs = atts->cmn.num_colors;
        for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
            rtvs[i] = atts->d3d11.colors[i].view.rtv;
        }
        dsv = atts->d3d11.depth_stencil.view.dsv;
    } else {
        // NOTE: depth-stencil-view is optional
        SOKOL_ASSERT(swapchain->d3d11.render_view);
        num_rtvs = 1;
        rtvs[0] = (ID3D11RenderTargetView*) swapchain->d3d11.render_view;
        dsv = (ID3D11DepthStencilView*) swapchain->d3d11.depth_stencil_view;
        _sg.d3d11.cur_pass.render_view = (ID3D11RenderTargetView*) swapchain->d3d11.render_view;
        _sg.d3d11.cur_pass.resolve_view = (ID3D11RenderTargetView*) swapchain->d3d11.resolve_view;
    }
    // apply the render-target- and depth-stencil-views
    _sg_d3d11_OMSetRenderTargets(_sg.d3d11.ctx, SG_MAX_COLOR_ATTACHMENTS, rtvs, dsv);
    _sg_stats_add(d3d11.pass.num_om_set_render_targets, 1);

    // set viewport and scissor rect to cover whole screen
    D3D11_VIEWPORT vp;
    _sg_clear(&vp, sizeof(vp));
    vp.Width = (FLOAT) _sg.cur_pass.width;
    vp.Height = (FLOAT) _sg.cur_pass.height;
    vp.MaxDepth = 1.0f;
    _sg_d3d11_RSSetViewports(_sg.d3d11.ctx, 1, &vp);
    D3D11_RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = _sg.cur_pass.width;
    rect.bottom = _sg.cur_pass.height;
    _sg_d3d11_RSSetScissorRects(_sg.d3d11.ctx, 1, &rect);

    // perform clear action
    for (int i = 0; i < num_rtvs; i++) {
        if (action->colors[i].load_action == SG_LOADACTION_CLEAR) {
            _sg_d3d11_ClearRenderTargetView(_sg.d3d11.ctx, rtvs[i], (float*)&action->colors[i].clear_value);
            _sg_stats_add(d3d11.pass.num_clear_render_target_view, 1);
        }
    }
    UINT ds_flags = 0;
    if (action->depth.load_action == SG_LOADACTION_CLEAR) {
        ds_flags |= D3D11_CLEAR_DEPTH;
    }
    if (action->stencil.load_action == SG_LOADACTION_CLEAR) {
        ds_flags |= D3D11_CLEAR_STENCIL;
    }
    if ((0 != ds_flags) && dsv) {
        _sg_d3d11_ClearDepthStencilView(_sg.d3d11.ctx, dsv, ds_flags, action->depth.clear_value, action->stencil.clear_value);
        _sg_stats_add(d3d11.pass.num_clear_depth_stencil_view, 1);
    }
}

// D3D11CalcSubresource only exists for C++
_SOKOL_PRIVATE UINT _sg_d3d11_calcsubresource(UINT mip_slice, UINT array_slice, UINT mip_levels) {
    return mip_slice + array_slice * mip_levels;
}

_SOKOL_PRIVATE void _sg_d3d11_end_pass(void) {
    SOKOL_ASSERT(_sg.d3d11.ctx);

    // need to resolve MSAA render attachments into texture?
    if (_sg.cur_pass.atts_id.id != SG_INVALID_ID) {
        // ...for offscreen pass...
        SOKOL_ASSERT(_sg.cur_pass.atts && _sg.cur_pass.atts->slot.id == _sg.cur_pass.atts_id.id);
        for (int i = 0; i < _sg.cur_pass.atts->cmn.num_colors; i++) {
            const _sg_image_t* resolve_img = _sg.cur_pass.atts->d3d11.resolves[i].image;
            if (resolve_img) {
                const _sg_image_t* color_img = _sg.cur_pass.atts->d3d11.colors[i].image;
                const _sg_attachment_common_t* cmn_color_att = &_sg.cur_pass.atts->cmn.colors[i];
                const _sg_attachment_common_t* cmn_resolve_att = &_sg.cur_pass.atts->cmn.resolves[i];
                SOKOL_ASSERT(resolve_img->slot.id == cmn_resolve_att->image_id.id);
                SOKOL_ASSERT(color_img && (color_img->slot.id == cmn_color_att->image_id.id));
                SOKOL_ASSERT(color_img->cmn.sample_count > 1);
                SOKOL_ASSERT(resolve_img->cmn.sample_count == 1);
                const UINT src_subres = _sg_d3d11_calcsubresource(
                    (UINT)cmn_color_att->mip_level,
                    (UINT)cmn_color_att->slice,
                    (UINT)color_img->cmn.num_mipmaps);
                const UINT dst_subres = _sg_d3d11_calcsubresource(
                    (UINT)cmn_resolve_att->mip_level,
                    (UINT)cmn_resolve_att->slice,
                    (UINT)resolve_img->cmn.num_mipmaps);
                _sg_d3d11_ResolveSubresource(_sg.d3d11.ctx,
                    resolve_img->d3d11.res,
                    dst_subres,
                    color_img->d3d11.res,
                    src_subres,
                    color_img->d3d11.format);
                _sg_stats_add(d3d11.pass.num_resolve_subresource, 1);
            }
        }
    } else {
        // ...for swapchain pass...
        if (_sg.d3d11.cur_pass.resolve_view) {
            SOKOL_ASSERT(_sg.d3d11.cur_pass.render_view);
            SOKOL_ASSERT(_sg.cur_pass.swapchain.sample_count > 1);
            SOKOL_ASSERT(_sg.cur_pass.swapchain.color_fmt > SG_PIXELFORMAT_NONE);
            ID3D11Resource* d3d11_render_res = 0;
            ID3D11Resource* d3d11_resolve_res = 0;
            _sg_d3d11_GetResource((ID3D11View*)_sg.d3d11.cur_pass.render_view, &d3d11_render_res);
            _sg_d3d11_GetResource((ID3D11View*)_sg.d3d11.cur_pass.resolve_view, &d3d11_resolve_res);
            SOKOL_ASSERT(d3d11_render_res);
            SOKOL_ASSERT(d3d11_resolve_res);
            const sg_pixel_format color_fmt = _sg.cur_pass.swapchain.color_fmt;
            _sg_d3d11_ResolveSubresource(_sg.d3d11.ctx, d3d11_resolve_res, 0, d3d11_render_res, 0, _sg_d3d11_rtv_pixel_format(color_fmt));
            _sg_d3d11_Release(d3d11_render_res);
            _sg_d3d11_Release(d3d11_resolve_res);
            _sg_stats_add(d3d11.pass.num_resolve_subresource, 1);
        }
    }
    _sg.d3d11.cur_pass.render_view = 0;
    _sg.d3d11.cur_pass.resolve_view = 0;
    _sg.d3d11.cur_pipeline = 0;
    _sg.d3d11.cur_pipeline_id.id = SG_INVALID_ID;
    _sg_d3d11_clear_state();
}

_SOKOL_PRIVATE void _sg_d3d11_apply_viewport(int x, int y, int w, int h, bool origin_top_left) {
    SOKOL_ASSERT(_sg.d3d11.ctx);
    D3D11_VIEWPORT vp;
    vp.TopLeftX = (FLOAT) x;
    vp.TopLeftY = (FLOAT) (origin_top_left ? y : (_sg.cur_pass.height - (y + h)));
    vp.Width = (FLOAT) w;
    vp.Height = (FLOAT) h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    _sg_d3d11_RSSetViewports(_sg.d3d11.ctx, 1, &vp);
}

_SOKOL_PRIVATE void _sg_d3d11_apply_scissor_rect(int x, int y, int w, int h, bool origin_top_left) {
    SOKOL_ASSERT(_sg.d3d11.ctx);
    D3D11_RECT rect;
    rect.left = x;
    rect.top = (origin_top_left ? y : (_sg.cur_pass.height - (y + h)));
    rect.right = x + w;
    rect.bottom = origin_top_left ? (y + h) : (_sg.cur_pass.height - y);
    _sg_d3d11_RSSetScissorRects(_sg.d3d11.ctx, 1, &rect);
}

_SOKOL_PRIVATE void _sg_d3d11_apply_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    SOKOL_ASSERT(pip->shader && (pip->cmn.shader_id.id == pip->shader->slot.id));
    SOKOL_ASSERT(_sg.d3d11.ctx);
    SOKOL_ASSERT(pip->d3d11.rs && pip->d3d11.bs && pip->d3d11.dss);

    _sg.d3d11.cur_pipeline = pip;
    _sg.d3d11.cur_pipeline_id.id = pip->slot.id;
    _sg.d3d11.use_indexed_draw = (pip->d3d11.index_format != DXGI_FORMAT_UNKNOWN);
    _sg.d3d11.use_instanced_draw = pip->cmn.use_instanced_draw;

    _sg_d3d11_RSSetState(_sg.d3d11.ctx, pip->d3d11.rs);
    _sg_d3d11_OMSetDepthStencilState(_sg.d3d11.ctx, pip->d3d11.dss, pip->d3d11.stencil_ref);
    _sg_d3d11_OMSetBlendState(_sg.d3d11.ctx, pip->d3d11.bs, (float*)&pip->cmn.blend_color, 0xFFFFFFFF);
    _sg_d3d11_IASetPrimitiveTopology(_sg.d3d11.ctx, pip->d3d11.topology);
    _sg_d3d11_IASetInputLayout(_sg.d3d11.ctx, pip->d3d11.il);
    _sg_d3d11_VSSetShader(_sg.d3d11.ctx, pip->shader->d3d11.vs, NULL, 0);
    _sg_d3d11_VSSetConstantBuffers(_sg.d3d11.ctx, 0, SG_MAX_SHADERSTAGE_UBS, pip->shader->d3d11.stage[SG_SHADERSTAGE_VS].cbufs);
    _sg_d3d11_PSSetShader(_sg.d3d11.ctx, pip->shader->d3d11.fs, NULL, 0);
    _sg_d3d11_PSSetConstantBuffers(_sg.d3d11.ctx, 0, SG_MAX_SHADERSTAGE_UBS, pip->shader->d3d11.stage[SG_SHADERSTAGE_FS].cbufs);
    _sg_stats_add(d3d11.pipeline.num_rs_set_state, 1);
    _sg_stats_add(d3d11.pipeline.num_om_set_depth_stencil_state, 1);
    _sg_stats_add(d3d11.pipeline.num_om_set_blend_state, 1);
    _sg_stats_add(d3d11.pipeline.num_ia_set_primitive_topology, 1);
    _sg_stats_add(d3d11.pipeline.num_ia_set_input_layout, 1);
    _sg_stats_add(d3d11.pipeline.num_vs_set_shader, 1);
    _sg_stats_add(d3d11.pipeline.num_vs_set_constant_buffers, 1);
    _sg_stats_add(d3d11.pipeline.num_ps_set_shader, 1);
    _sg_stats_add(d3d11.pipeline.num_ps_set_constant_buffers, 1);
}

_SOKOL_PRIVATE bool _sg_d3d11_apply_bindings(_sg_bindings_t* bnd) {
    SOKOL_ASSERT(bnd);
    SOKOL_ASSERT(bnd->pip);
    SOKOL_ASSERT(_sg.d3d11.ctx);

    // gather all the D3D11 resources into arrays
    ID3D11Buffer* d3d11_ib = bnd->ib ? bnd->ib->d3d11.buf : 0;
    ID3D11Buffer* d3d11_vbs[SG_MAX_VERTEX_BUFFERS] = {0};
    UINT d3d11_vb_offsets[SG_MAX_VERTEX_BUFFERS] = {0};
    ID3D11ShaderResourceView* d3d11_vs_srvs[_SG_D3D11_MAX_SHADERSTAGE_SRVS] = {0};
    ID3D11ShaderResourceView* d3d11_fs_srvs[_SG_D3D11_MAX_SHADERSTAGE_SRVS] = {0};
    ID3D11SamplerState* d3d11_vs_smps[SG_MAX_SHADERSTAGE_SAMPLERS] = {0};
    ID3D11SamplerState* d3d11_fs_smps[SG_MAX_SHADERSTAGE_SAMPLERS] = {0};
    for (int i = 0; i < bnd->num_vbs; i++) {
        SOKOL_ASSERT(bnd->vbs[i]->d3d11.buf);
        d3d11_vbs[i] = bnd->vbs[i]->d3d11.buf;
        d3d11_vb_offsets[i] = (UINT)bnd->vb_offsets[i];
    }
    for (int i = 0; i < bnd->num_vs_imgs; i++) {
        SOKOL_ASSERT(bnd->vs_imgs[i]->d3d11.srv);
        d3d11_vs_srvs[_SG_D3D11_SHADERSTAGE_IMAGE_SRV_OFFSET + i] = bnd->vs_imgs[i]->d3d11.srv;
    }
    for (int i = 0; i < bnd->num_vs_sbufs; i++) {
        SOKOL_ASSERT(bnd->vs_sbufs[i]->d3d11.srv);
        d3d11_vs_srvs[_SG_D3D11_SHADERSTAGE_BUFFER_SRV_OFFSET + i] = bnd->vs_sbufs[i]->d3d11.srv;
    }
    for (int i = 0; i < bnd->num_fs_imgs; i++) {
        SOKOL_ASSERT(bnd->fs_imgs[i]->d3d11.srv);
        d3d11_fs_srvs[_SG_D3D11_SHADERSTAGE_IMAGE_SRV_OFFSET + i] = bnd->fs_imgs[i]->d3d11.srv;
    }
    for (int i = 0; i < bnd->num_fs_sbufs; i++) {
        SOKOL_ASSERT(bnd->fs_sbufs[i]->d3d11.srv);
        d3d11_fs_srvs[_SG_D3D11_SHADERSTAGE_BUFFER_SRV_OFFSET + i] = bnd->fs_sbufs[i]->d3d11.srv;
    }
    for (int i = 0; i < bnd->num_vs_smps; i++) {
        SOKOL_ASSERT(bnd->vs_smps[i]->d3d11.smp);
        d3d11_vs_smps[i] = bnd->vs_smps[i]->d3d11.smp;
    }
    for (int i = 0; i < bnd->num_fs_smps; i++) {
        SOKOL_ASSERT(bnd->fs_smps[i]->d3d11.smp);
        d3d11_fs_smps[i] = bnd->fs_smps[i]->d3d11.smp;
    }
    _sg_d3d11_IASetVertexBuffers(_sg.d3d11.ctx, 0, SG_MAX_VERTEX_BUFFERS, d3d11_vbs, bnd->pip->d3d11.vb_strides, d3d11_vb_offsets);
    _sg_d3d11_IASetIndexBuffer(_sg.d3d11.ctx, d3d11_ib, bnd->pip->d3d11.index_format, (UINT)bnd->ib_offset);
    _sg_d3d11_VSSetShaderResources(_sg.d3d11.ctx, 0, _SG_D3D11_MAX_SHADERSTAGE_SRVS, d3d11_vs_srvs);
    _sg_d3d11_PSSetShaderResources(_sg.d3d11.ctx, 0, _SG_D3D11_MAX_SHADERSTAGE_SRVS, d3d11_fs_srvs);
    _sg_d3d11_VSSetSamplers(_sg.d3d11.ctx, 0, SG_MAX_SHADERSTAGE_SAMPLERS, d3d11_vs_smps);
    _sg_d3d11_PSSetSamplers(_sg.d3d11.ctx, 0, SG_MAX_SHADERSTAGE_SAMPLERS, d3d11_fs_smps);
    _sg_stats_add(d3d11.bindings.num_ia_set_vertex_buffers, 1);
    _sg_stats_add(d3d11.bindings.num_ia_set_index_buffer, 1);
    _sg_stats_add(d3d11.bindings.num_vs_set_shader_resources, 1);
    _sg_stats_add(d3d11.bindings.num_ps_set_shader_resources, 1);
    _sg_stats_add(d3d11.bindings.num_vs_set_samplers, 1);
    _sg_stats_add(d3d11.bindings.num_ps_set_samplers, 1);
    return true;
}

_SOKOL_PRIVATE void _sg_d3d11_apply_uniforms(sg_shader_stage stage_index, int ub_index, const sg_range* data) {
    SOKOL_ASSERT(_sg.d3d11.ctx);
    SOKOL_ASSERT(_sg.d3d11.cur_pipeline && _sg.d3d11.cur_pipeline->slot.id == _sg.d3d11.cur_pipeline_id.id);
    SOKOL_ASSERT(_sg.d3d11.cur_pipeline->shader && _sg.d3d11.cur_pipeline->shader->slot.id == _sg.d3d11.cur_pipeline->cmn.shader_id.id);
    SOKOL_ASSERT(ub_index < _sg.d3d11.cur_pipeline->shader->cmn.stage[stage_index].num_uniform_blocks);
    SOKOL_ASSERT(data->size == _sg.d3d11.cur_pipeline->shader->cmn.stage[stage_index].uniform_blocks[ub_index].size);
    ID3D11Buffer* cb = _sg.d3d11.cur_pipeline->shader->d3d11.stage[stage_index].cbufs[ub_index];
    SOKOL_ASSERT(cb);
    _sg_d3d11_UpdateSubresource(_sg.d3d11.ctx, (ID3D11Resource*)cb, 0, NULL, data->ptr, 0, 0);
    _sg_stats_add(d3d11.uniforms.num_update_subresource, 1);
}

_SOKOL_PRIVATE void _sg_d3d11_draw(int base_element, int num_elements, int num_instances) {
    const bool use_instanced_draw = (num_instances > 1) || (_sg.d3d11.use_instanced_draw);
    if (_sg.d3d11.use_indexed_draw) {
        if (use_instanced_draw) {
            _sg_d3d11_DrawIndexedInstanced(_sg.d3d11.ctx, (UINT)num_elements, (UINT)num_instances, (UINT)base_element, 0, 0);
            _sg_stats_add(d3d11.draw.num_draw_indexed_instanced, 1);
        } else {
            _sg_d3d11_DrawIndexed(_sg.d3d11.ctx, (UINT)num_elements, (UINT)base_element, 0);
            _sg_stats_add(d3d11.draw.num_draw_indexed, 1);
        }
    } else {
        if (use_instanced_draw) {
            _sg_d3d11_DrawInstanced(_sg.d3d11.ctx, (UINT)num_elements, (UINT)num_instances, (UINT)base_element, 0);
            _sg_stats_add(d3d11.draw.num_draw_instanced, 1);
        } else {
            _sg_d3d11_Draw(_sg.d3d11.ctx, (UINT)num_elements, (UINT)base_element);
            _sg_stats_add(d3d11.draw.num_draw, 1);
        }
    }
}

_SOKOL_PRIVATE void _sg_d3d11_commit(void) {
    // empty
}

_SOKOL_PRIVATE void _sg_d3d11_update_buffer(_sg_buffer_t* buf, const sg_range* data) {
    SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
    SOKOL_ASSERT(_sg.d3d11.ctx);
    SOKOL_ASSERT(buf->d3d11.buf);
    D3D11_MAPPED_SUBRESOURCE d3d11_msr;
    HRESULT hr = _sg_d3d11_Map(_sg.d3d11.ctx, (ID3D11Resource*)buf->d3d11.buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11_msr);
    _sg_stats_add(d3d11.num_map, 1);
    if (SUCCEEDED(hr)) {
        memcpy(d3d11_msr.pData, data->ptr, data->size);
        _sg_d3d11_Unmap(_sg.d3d11.ctx, (ID3D11Resource*)buf->d3d11.buf, 0);
        _sg_stats_add(d3d11.num_unmap, 1);
    } else {
        _SG_ERROR(D3D11_MAP_FOR_UPDATE_BUFFER_FAILED);
    }
}

_SOKOL_PRIVATE void _sg_d3d11_append_buffer(_sg_buffer_t* buf, const sg_range* data, bool new_frame) {
    SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
    SOKOL_ASSERT(_sg.d3d11.ctx);
    SOKOL_ASSERT(buf->d3d11.buf);
    D3D11_MAP map_type = new_frame ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;
    D3D11_MAPPED_SUBRESOURCE d3d11_msr;
    HRESULT hr = _sg_d3d11_Map(_sg.d3d11.ctx, (ID3D11Resource*)buf->d3d11.buf, 0, map_type, 0, &d3d11_msr);
    _sg_stats_add(d3d11.num_map, 1);
    if (SUCCEEDED(hr)) {
        uint8_t* dst_ptr = (uint8_t*)d3d11_msr.pData + buf->cmn.append_pos;
        memcpy(dst_ptr, data->ptr, data->size);
        _sg_d3d11_Unmap(_sg.d3d11.ctx, (ID3D11Resource*)buf->d3d11.buf, 0);
        _sg_stats_add(d3d11.num_unmap, 1);
    } else {
        _SG_ERROR(D3D11_MAP_FOR_APPEND_BUFFER_FAILED);
    }
}

_SOKOL_PRIVATE void _sg_d3d11_update_image(_sg_image_t* img, const sg_image_data* data) {
    SOKOL_ASSERT(img && data);
    SOKOL_ASSERT(_sg.d3d11.ctx);
    SOKOL_ASSERT(img->d3d11.res);
    const int num_faces = (img->cmn.type == SG_IMAGETYPE_CUBE) ? 6:1;
    const int num_slices = (img->cmn.type == SG_IMAGETYPE_ARRAY) ? img->cmn.num_slices:1;
    UINT subres_index = 0;
    HRESULT hr;
    D3D11_MAPPED_SUBRESOURCE d3d11_msr;
    for (int face_index = 0; face_index < num_faces; face_index++) {
        for (int slice_index = 0; slice_index < num_slices; slice_index++) {
            for (int mip_index = 0; mip_index < img->cmn.num_mipmaps; mip_index++, subres_index++) {
                SOKOL_ASSERT(subres_index < (SG_MAX_MIPMAPS * SG_MAX_TEXTUREARRAY_LAYERS));
                const int mip_width = _sg_miplevel_dim(img->cmn.width, mip_index);
                const int mip_height = _sg_miplevel_dim(img->cmn.height, mip_index);
                const int src_pitch = _sg_row_pitch(img->cmn.pixel_format, mip_width, 1);
                const sg_range* subimg_data = &(data->subimage[face_index][mip_index]);
                const size_t slice_size = subimg_data->size / (size_t)num_slices;
                const size_t slice_offset = slice_size * (size_t)slice_index;
                const uint8_t* slice_ptr = ((const uint8_t*)subimg_data->ptr) + slice_offset;
                hr = _sg_d3d11_Map(_sg.d3d11.ctx, img->d3d11.res, subres_index, D3D11_MAP_WRITE_DISCARD, 0, &d3d11_msr);
                _sg_stats_add(d3d11.num_map, 1);
                if (SUCCEEDED(hr)) {
                    // FIXME: need to handle difference in depth-pitch for 3D textures as well!
                    if (src_pitch == (int)d3d11_msr.RowPitch) {
                        memcpy(d3d11_msr.pData, slice_ptr, slice_size);
                    } else {
                        SOKOL_ASSERT(src_pitch < (int)d3d11_msr.RowPitch);
                        const uint8_t* src_ptr = slice_ptr;
                        uint8_t* dst_ptr = (uint8_t*) d3d11_msr.pData;
                        for (int row_index = 0; row_index < mip_height; row_index++) {
                            memcpy(dst_ptr, src_ptr, (size_t)src_pitch);
                            src_ptr += src_pitch;
                            dst_ptr += d3d11_msr.RowPitch;
                        }
                    }
                    _sg_d3d11_Unmap(_sg.d3d11.ctx, img->d3d11.res, subres_index);
                    _sg_stats_add(d3d11.num_unmap, 1);
                } else {
                    _SG_ERROR(D3D11_MAP_FOR_UPDATE_IMAGE_FAILED);
                }
            }
        }
    }
}

