// ██     ██ ██ ███    ██ ██████   ██████  ██     ██ ███████
// ██     ██ ██ ████   ██ ██   ██ ██    ██ ██     ██ ██
// ██  █  ██ ██ ██ ██  ██ ██   ██ ██    ██ ██  █  ██ ███████
// ██ ███ ██ ██ ██  ██ ██ ██   ██ ██    ██ ██ ███ ██      ██
//  ███ ███  ██ ██   ████ ██████   ██████   ███ ███  ███████
//
// >>windows
_SOKOL_PRIVATE bool _sapp_win32_utf8_to_wide(const char* src, wchar_t* dst, int dst_num_bytes) {
    SOKOL_ASSERT(src && dst && (dst_num_bytes > 1));
    _sapp_clear(dst, (size_t)dst_num_bytes);
    const int dst_chars = dst_num_bytes / (int)sizeof(wchar_t);
    const int dst_needed = MultiByteToWideChar(CP_UTF8, 0, src, -1, 0, 0);
    if ((dst_needed > 0) && (dst_needed < dst_chars)) {
        MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, dst_chars);
        return true;
    }
    else {
        /* input string doesn't fit into destination buffer */
        return false;
    }
}

_SOKOL_PRIVATE void _sapp_win32_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_win32_init_keytable(void) {
    /* same as GLFW */
    _sapp.keycodes[0x00B] = SAPP_KEYCODE_0;
    _sapp.keycodes[0x002] = SAPP_KEYCODE_1;
    _sapp.keycodes[0x003] = SAPP_KEYCODE_2;
    _sapp.keycodes[0x004] = SAPP_KEYCODE_3;
    _sapp.keycodes[0x005] = SAPP_KEYCODE_4;
    _sapp.keycodes[0x006] = SAPP_KEYCODE_5;
    _sapp.keycodes[0x007] = SAPP_KEYCODE_6;
    _sapp.keycodes[0x008] = SAPP_KEYCODE_7;
    _sapp.keycodes[0x009] = SAPP_KEYCODE_8;
    _sapp.keycodes[0x00A] = SAPP_KEYCODE_9;
    _sapp.keycodes[0x01E] = SAPP_KEYCODE_A;
    _sapp.keycodes[0x030] = SAPP_KEYCODE_B;
    _sapp.keycodes[0x02E] = SAPP_KEYCODE_C;
    _sapp.keycodes[0x020] = SAPP_KEYCODE_D;
    _sapp.keycodes[0x012] = SAPP_KEYCODE_E;
    _sapp.keycodes[0x021] = SAPP_KEYCODE_F;
    _sapp.keycodes[0x022] = SAPP_KEYCODE_G;
    _sapp.keycodes[0x023] = SAPP_KEYCODE_H;
    _sapp.keycodes[0x017] = SAPP_KEYCODE_I;
    _sapp.keycodes[0x024] = SAPP_KEYCODE_J;
    _sapp.keycodes[0x025] = SAPP_KEYCODE_K;
    _sapp.keycodes[0x026] = SAPP_KEYCODE_L;
    _sapp.keycodes[0x032] = SAPP_KEYCODE_M;
    _sapp.keycodes[0x031] = SAPP_KEYCODE_N;
    _sapp.keycodes[0x018] = SAPP_KEYCODE_O;
    _sapp.keycodes[0x019] = SAPP_KEYCODE_P;
    _sapp.keycodes[0x010] = SAPP_KEYCODE_Q;
    _sapp.keycodes[0x013] = SAPP_KEYCODE_R;
    _sapp.keycodes[0x01F] = SAPP_KEYCODE_S;
    _sapp.keycodes[0x014] = SAPP_KEYCODE_T;
    _sapp.keycodes[0x016] = SAPP_KEYCODE_U;
    _sapp.keycodes[0x02F] = SAPP_KEYCODE_V;
    _sapp.keycodes[0x011] = SAPP_KEYCODE_W;
    _sapp.keycodes[0x02D] = SAPP_KEYCODE_X;
    _sapp.keycodes[0x015] = SAPP_KEYCODE_Y;
    _sapp.keycodes[0x02C] = SAPP_KEYCODE_Z;
    _sapp.keycodes[0x028] = SAPP_KEYCODE_APOSTROPHE;
    _sapp.keycodes[0x02B] = SAPP_KEYCODE_BACKSLASH;
    _sapp.keycodes[0x033] = SAPP_KEYCODE_COMMA;
    _sapp.keycodes[0x00D] = SAPP_KEYCODE_EQUAL;
    _sapp.keycodes[0x029] = SAPP_KEYCODE_GRAVE_ACCENT;
    _sapp.keycodes[0x01A] = SAPP_KEYCODE_LEFT_BRACKET;
    _sapp.keycodes[0x00C] = SAPP_KEYCODE_MINUS;
    _sapp.keycodes[0x034] = SAPP_KEYCODE_PERIOD;
    _sapp.keycodes[0x01B] = SAPP_KEYCODE_RIGHT_BRACKET;
    _sapp.keycodes[0x027] = SAPP_KEYCODE_SEMICOLON;
    _sapp.keycodes[0x035] = SAPP_KEYCODE_SLASH;
    _sapp.keycodes[0x056] = SAPP_KEYCODE_WORLD_2;
    _sapp.keycodes[0x00E] = SAPP_KEYCODE_BACKSPACE;
    _sapp.keycodes[0x153] = SAPP_KEYCODE_DELETE;
    _sapp.keycodes[0x14F] = SAPP_KEYCODE_END;
    _sapp.keycodes[0x01C] = SAPP_KEYCODE_ENTER;
    _sapp.keycodes[0x001] = SAPP_KEYCODE_ESCAPE;
    _sapp.keycodes[0x147] = SAPP_KEYCODE_HOME;
    _sapp.keycodes[0x152] = SAPP_KEYCODE_INSERT;
    _sapp.keycodes[0x15D] = SAPP_KEYCODE_MENU;
    _sapp.keycodes[0x151] = SAPP_KEYCODE_PAGE_DOWN;
    _sapp.keycodes[0x149] = SAPP_KEYCODE_PAGE_UP;
    _sapp.keycodes[0x045] = SAPP_KEYCODE_PAUSE;
    _sapp.keycodes[0x146] = SAPP_KEYCODE_PAUSE;
    _sapp.keycodes[0x039] = SAPP_KEYCODE_SPACE;
    _sapp.keycodes[0x00F] = SAPP_KEYCODE_TAB;
    _sapp.keycodes[0x03A] = SAPP_KEYCODE_CAPS_LOCK;
    _sapp.keycodes[0x145] = SAPP_KEYCODE_NUM_LOCK;
    _sapp.keycodes[0x046] = SAPP_KEYCODE_SCROLL_LOCK;
    _sapp.keycodes[0x03B] = SAPP_KEYCODE_F1;
    _sapp.keycodes[0x03C] = SAPP_KEYCODE_F2;
    _sapp.keycodes[0x03D] = SAPP_KEYCODE_F3;
    _sapp.keycodes[0x03E] = SAPP_KEYCODE_F4;
    _sapp.keycodes[0x03F] = SAPP_KEYCODE_F5;
    _sapp.keycodes[0x040] = SAPP_KEYCODE_F6;
    _sapp.keycodes[0x041] = SAPP_KEYCODE_F7;
    _sapp.keycodes[0x042] = SAPP_KEYCODE_F8;
    _sapp.keycodes[0x043] = SAPP_KEYCODE_F9;
    _sapp.keycodes[0x044] = SAPP_KEYCODE_F10;
    _sapp.keycodes[0x057] = SAPP_KEYCODE_F11;
    _sapp.keycodes[0x058] = SAPP_KEYCODE_F12;
    _sapp.keycodes[0x064] = SAPP_KEYCODE_F13;
    _sapp.keycodes[0x065] = SAPP_KEYCODE_F14;
    _sapp.keycodes[0x066] = SAPP_KEYCODE_F15;
    _sapp.keycodes[0x067] = SAPP_KEYCODE_F16;
    _sapp.keycodes[0x068] = SAPP_KEYCODE_F17;
    _sapp.keycodes[0x069] = SAPP_KEYCODE_F18;
    _sapp.keycodes[0x06A] = SAPP_KEYCODE_F19;
    _sapp.keycodes[0x06B] = SAPP_KEYCODE_F20;
    _sapp.keycodes[0x06C] = SAPP_KEYCODE_F21;
    _sapp.keycodes[0x06D] = SAPP_KEYCODE_F22;
    _sapp.keycodes[0x06E] = SAPP_KEYCODE_F23;
    _sapp.keycodes[0x076] = SAPP_KEYCODE_F24;
    _sapp.keycodes[0x038] = SAPP_KEYCODE_LEFT_ALT;
    _sapp.keycodes[0x01D] = SAPP_KEYCODE_LEFT_CONTROL;
    _sapp.keycodes[0x02A] = SAPP_KEYCODE_LEFT_SHIFT;
    _sapp.keycodes[0x15B] = SAPP_KEYCODE_LEFT_SUPER;
    _sapp.keycodes[0x137] = SAPP_KEYCODE_PRINT_SCREEN;
    _sapp.keycodes[0x138] = SAPP_KEYCODE_RIGHT_ALT;
    _sapp.keycodes[0x11D] = SAPP_KEYCODE_RIGHT_CONTROL;
    _sapp.keycodes[0x036] = SAPP_KEYCODE_RIGHT_SHIFT;
    _sapp.keycodes[0x15C] = SAPP_KEYCODE_RIGHT_SUPER;
    _sapp.keycodes[0x150] = SAPP_KEYCODE_DOWN;
    _sapp.keycodes[0x14B] = SAPP_KEYCODE_LEFT;
    _sapp.keycodes[0x14D] = SAPP_KEYCODE_RIGHT;
    _sapp.keycodes[0x148] = SAPP_KEYCODE_UP;
    _sapp.keycodes[0x052] = SAPP_KEYCODE_KP_0;
    _sapp.keycodes[0x04F] = SAPP_KEYCODE_KP_1;
    _sapp.keycodes[0x050] = SAPP_KEYCODE_KP_2;
    _sapp.keycodes[0x051] = SAPP_KEYCODE_KP_3;
    _sapp.keycodes[0x04B] = SAPP_KEYCODE_KP_4;
    _sapp.keycodes[0x04C] = SAPP_KEYCODE_KP_5;
    _sapp.keycodes[0x04D] = SAPP_KEYCODE_KP_6;
    _sapp.keycodes[0x047] = SAPP_KEYCODE_KP_7;
    _sapp.keycodes[0x048] = SAPP_KEYCODE_KP_8;
    _sapp.keycodes[0x049] = SAPP_KEYCODE_KP_9;
    _sapp.keycodes[0x04E] = SAPP_KEYCODE_KP_ADD;
    _sapp.keycodes[0x053] = SAPP_KEYCODE_KP_DECIMAL;
    _sapp.keycodes[0x135] = SAPP_KEYCODE_KP_DIVIDE;
    _sapp.keycodes[0x11C] = SAPP_KEYCODE_KP_ENTER;
    _sapp.keycodes[0x037] = SAPP_KEYCODE_KP_MULTIPLY;
    _sapp.keycodes[0x04A] = SAPP_KEYCODE_KP_SUBTRACT;
}

#define _sapp_d3d11_Release(self) (self)->lpVtbl->Release(self)
#define _sapp_win32_refiid(iid) &iid

#define _SAPP_SAFE_RELEASE(obj) if (obj) { _sapp_d3d11_Release(obj); obj=0; }


static const IID _sapp_IID_ID3D11Texture2D = { 0x6f15aaf2,0xd208,0x4e89, {0x9a,0xb4,0x48,0x95,0x35,0xd3,0x4f,0x9c} };
static const IID _sapp_IID_IDXGIDevice1    = { 0x77db970f,0x6276,0x48ba, {0xba,0x28,0x07,0x01,0x43,0xb4,0x39,0x2c} };
static const IID _sapp_IID_IDXGIFactory    = { 0x7b7166ec,0x21c7,0x44ae, {0xb2,0x1a,0xc9,0xae,0x32,0x1a,0xe3,0x69} };

static inline HRESULT _sapp_dxgi_GetBuffer(IDXGISwapChain* self, UINT Buffer, REFIID riid, void** ppSurface) {
    return self->lpVtbl->GetBuffer(self, Buffer, riid, ppSurface);
}

static inline HRESULT _sapp_d3d11_QueryInterface(ID3D11Device* self, REFIID riid, void** ppvObject) {
    return self->lpVtbl->QueryInterface(self, riid, ppvObject);
}

static inline HRESULT _sapp_d3d11_CreateRenderTargetView(ID3D11Device* self, ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView) {
    return self->lpVtbl->CreateRenderTargetView(self, pResource, pDesc, ppRTView);
}

static inline HRESULT _sapp_d3d11_CreateTexture2D(ID3D11Device* self, const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D) {
    return self->lpVtbl->CreateTexture2D(self, pDesc, pInitialData, ppTexture2D);
}

static inline HRESULT _sapp_d3d11_CreateDepthStencilView(ID3D11Device* self, ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView) {
    return self->lpVtbl->CreateDepthStencilView(self, pResource, pDesc, ppDepthStencilView);
}

static inline HRESULT _sapp_dxgi_ResizeBuffers(IDXGISwapChain* self, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    return self->lpVtbl->ResizeBuffers(self, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

static inline HRESULT _sapp_dxgi_Present(IDXGISwapChain* self, UINT SyncInterval, UINT Flags) {
    return self->lpVtbl->Present(self, SyncInterval, Flags);
}

static inline HRESULT _sapp_dxgi_GetFrameStatistics(IDXGISwapChain* self, DXGI_FRAME_STATISTICS* pStats) {
    return self->lpVtbl->GetFrameStatistics(self, pStats);
}

static inline HRESULT _sapp_dxgi_SetMaximumFrameLatency(IDXGIDevice1* self, UINT MaxLatency) {
    return self->lpVtbl->SetMaximumFrameLatency(self, MaxLatency);
}

static inline HRESULT _sapp_dxgi_GetAdapter(IDXGIDevice1* self, IDXGIAdapter** pAdapter) {
    return self->lpVtbl->GetAdapter(self, pAdapter);
}

static inline HRESULT _sapp_dxgi_GetParent(IDXGIObject* self, REFIID riid, void** ppParent) {
    return self->lpVtbl->GetParent(self, riid, ppParent);
}

static inline HRESULT _sapp_dxgi_MakeWindowAssociation(IDXGIFactory* self, HWND WindowHandle, UINT Flags) {
    return self->lpVtbl->MakeWindowAssociation(self, WindowHandle, Flags);
}

_SOKOL_PRIVATE void _sapp_d3d11_create_device_and_swapchain(void) {
    DXGI_SWAP_CHAIN_DESC* sc_desc = &_sapp.d3d11.swap_chain_desc;
    sc_desc->BufferDesc.Width = (UINT)_sapp.framebuffer_width;
    sc_desc->BufferDesc.Height = (UINT)_sapp.framebuffer_height;
    sc_desc->BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sc_desc->BufferDesc.RefreshRate.Numerator = 60;
    sc_desc->BufferDesc.RefreshRate.Denominator = 1;
    sc_desc->OutputWindow = _sapp.win32.hwnd;
    sc_desc->Windowed = true;
    if (_sapp.win32.is_win10_or_greater) {
        sc_desc->BufferCount = 2;
        sc_desc->SwapEffect = (DXGI_SWAP_EFFECT) _SAPP_DXGI_SWAP_EFFECT_FLIP_DISCARD;
        _sapp.d3d11.use_dxgi_frame_stats = true;
    }
    else {
        sc_desc->BufferCount = 1;
        sc_desc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        _sapp.d3d11.use_dxgi_frame_stats = false;
    }
    sc_desc->SampleDesc.Count = 1;
    sc_desc->SampleDesc.Quality = 0;
    sc_desc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    UINT create_flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    #if defined(SOKOL_DEBUG)
        create_flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif
    D3D_FEATURE_LEVEL feature_level;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        NULL,                           /* pAdapter (use default) */
        D3D_DRIVER_TYPE_HARDWARE,       /* DriverType */
        NULL,                           /* Software */
        create_flags,                   /* Flags */
        NULL,                           /* pFeatureLevels */
        0,                              /* FeatureLevels */
        D3D11_SDK_VERSION,              /* SDKVersion */
        sc_desc,                        /* pSwapChainDesc */
        &_sapp.d3d11.swap_chain,        /* ppSwapChain */
        &_sapp.d3d11.device,            /* ppDevice */
        &feature_level,                 /* pFeatureLevel */
        &_sapp.d3d11.device_context);   /* ppImmediateContext */
    _SOKOL_UNUSED(hr);
    #if defined(SOKOL_DEBUG)
    if (!SUCCEEDED(hr)) {
        // if initialization with D3D11_CREATE_DEVICE_DEBUG fails, this could be because the
        // 'D3D11 debug layer' stopped working, indicated by the error message:
        // ===
        // D3D11CreateDevice: Flags (0x2) were specified which require the D3D11 SDK Layers for Windows 10, but they are not present on the system.
        // These flags must be removed, or the Windows 10 SDK must be installed.
        // Flags include: D3D11_CREATE_DEVICE_DEBUG
        // ===
        //
        // ...just retry with the DEBUG flag switched off
        _SAPP_ERROR(WIN32_D3D11_CREATE_DEVICE_AND_SWAPCHAIN_WITH_DEBUG_FAILED);
        create_flags &= ~D3D11_CREATE_DEVICE_DEBUG;
        hr = D3D11CreateDeviceAndSwapChain(
            NULL,                           /* pAdapter (use default) */
            D3D_DRIVER_TYPE_HARDWARE,       /* DriverType */
            NULL,                           /* Software */
            create_flags,                   /* Flags */
            NULL,                           /* pFeatureLevels */
            0,                              /* FeatureLevels */
            D3D11_SDK_VERSION,              /* SDKVersion */
            sc_desc,                        /* pSwapChainDesc */
            &_sapp.d3d11.swap_chain,        /* ppSwapChain */
            &_sapp.d3d11.device,            /* ppDevice */
            &feature_level,                 /* pFeatureLevel */
            &_sapp.d3d11.device_context);   /* ppImmediateContext */
    }
    #endif
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp.d3d11.swap_chain && _sapp.d3d11.device && _sapp.d3d11.device_context);

    // minimize frame latency, disable Alt-Enter
    hr = _sapp_d3d11_QueryInterface(_sapp.d3d11.device, _sapp_win32_refiid(_sapp_IID_IDXGIDevice1), (void**)&_sapp.d3d11.dxgi_device);
    if (SUCCEEDED(hr) && _sapp.d3d11.dxgi_device) {
        _sapp_dxgi_SetMaximumFrameLatency(_sapp.d3d11.dxgi_device, 1);
        IDXGIAdapter* dxgi_adapter = 0;
        hr = _sapp_dxgi_GetAdapter(_sapp.d3d11.dxgi_device, &dxgi_adapter);
        if (SUCCEEDED(hr) && dxgi_adapter) {
            IDXGIFactory* dxgi_factory = 0;
            hr = _sapp_dxgi_GetParent((IDXGIObject*)dxgi_adapter, _sapp_win32_refiid(_sapp_IID_IDXGIFactory), (void**)&dxgi_factory);
            if (SUCCEEDED(hr)) {
                _sapp_dxgi_MakeWindowAssociation(dxgi_factory, _sapp.win32.hwnd, DXGI_MWA_NO_ALT_ENTER|DXGI_MWA_NO_PRINT_SCREEN);
                _SAPP_SAFE_RELEASE(dxgi_factory);
            }
            else {
                _SAPP_ERROR(WIN32_D3D11_GET_IDXGIFACTORY_FAILED);
            }
            _SAPP_SAFE_RELEASE(dxgi_adapter);
        }
        else {
            _SAPP_ERROR(WIN32_D3D11_GET_IDXGIADAPTER_FAILED);
        }
    }
    else {
        _SAPP_PANIC(WIN32_D3D11_QUERY_INTERFACE_IDXGIDEVICE1_FAILED);
    }
}

_SOKOL_PRIVATE void _sapp_d3d11_destroy_device_and_swapchain(void) {
    _SAPP_SAFE_RELEASE(_sapp.d3d11.swap_chain);
    _SAPP_SAFE_RELEASE(_sapp.d3d11.dxgi_device);
    _SAPP_SAFE_RELEASE(_sapp.d3d11.device_context);
    _SAPP_SAFE_RELEASE(_sapp.d3d11.device);
}

_SOKOL_PRIVATE void _sapp_d3d11_create_default_render_target(void) {
    SOKOL_ASSERT(0 == _sapp.d3d11.rt);
    SOKOL_ASSERT(0 == _sapp.d3d11.rtv);
    SOKOL_ASSERT(0 == _sapp.d3d11.msaa_rt);
    SOKOL_ASSERT(0 == _sapp.d3d11.msaa_rtv);
    SOKOL_ASSERT(0 == _sapp.d3d11.ds);
    SOKOL_ASSERT(0 == _sapp.d3d11.dsv);

    HRESULT hr;

    /* view for the swapchain-created framebuffer */
    hr = _sapp_dxgi_GetBuffer(_sapp.d3d11.swap_chain, 0, _sapp_win32_refiid(_sapp_IID_ID3D11Texture2D), (void**)&_sapp.d3d11.rt);
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp.d3d11.rt);
    hr = _sapp_d3d11_CreateRenderTargetView(_sapp.d3d11.device, (ID3D11Resource*)_sapp.d3d11.rt, NULL, &_sapp.d3d11.rtv);
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp.d3d11.rtv);

    /* common desc for MSAA and depth-stencil texture */
    D3D11_TEXTURE2D_DESC tex_desc;
    _sapp_clear(&tex_desc, sizeof(tex_desc));
    tex_desc.Width = (UINT)_sapp.framebuffer_width;
    tex_desc.Height = (UINT)_sapp.framebuffer_height;
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Usage = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    tex_desc.SampleDesc.Count = (UINT) _sapp.sample_count;
    tex_desc.SampleDesc.Quality = (UINT) (_sapp.sample_count > 1 ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0);

    /* create MSAA texture and view if antialiasing requested */
    if (_sapp.sample_count > 1) {
        tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        hr = _sapp_d3d11_CreateTexture2D(_sapp.d3d11.device, &tex_desc, NULL, &_sapp.d3d11.msaa_rt);
        SOKOL_ASSERT(SUCCEEDED(hr) && _sapp.d3d11.msaa_rt);
        hr = _sapp_d3d11_CreateRenderTargetView(_sapp.d3d11.device, (ID3D11Resource*)_sapp.d3d11.msaa_rt, NULL, &_sapp.d3d11.msaa_rtv);
        SOKOL_ASSERT(SUCCEEDED(hr) && _sapp.d3d11.msaa_rtv);
    }

    /* texture and view for the depth-stencil-surface */
    tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = _sapp_d3d11_CreateTexture2D(_sapp.d3d11.device, &tex_desc, NULL, &_sapp.d3d11.ds);
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp.d3d11.ds);
    hr = _sapp_d3d11_CreateDepthStencilView(_sapp.d3d11.device, (ID3D11Resource*)_sapp.d3d11.ds, NULL, &_sapp.d3d11.dsv);
    SOKOL_ASSERT(SUCCEEDED(hr) && _sapp.d3d11.dsv);
}

_SOKOL_PRIVATE void _sapp_d3d11_destroy_default_render_target(void) {
    _SAPP_SAFE_RELEASE(_sapp.d3d11.rt);
    _SAPP_SAFE_RELEASE(_sapp.d3d11.rtv);
    _SAPP_SAFE_RELEASE(_sapp.d3d11.msaa_rt);
    _SAPP_SAFE_RELEASE(_sapp.d3d11.msaa_rtv);
    _SAPP_SAFE_RELEASE(_sapp.d3d11.ds);
    _SAPP_SAFE_RELEASE(_sapp.d3d11.dsv);
}

_SOKOL_PRIVATE void _sapp_d3d11_resize_default_render_target(void) {
    if (_sapp.d3d11.swap_chain) {
        _sapp_d3d11_destroy_default_render_target();
        _sapp_dxgi_ResizeBuffers(_sapp.d3d11.swap_chain, _sapp.d3d11.swap_chain_desc.BufferCount, (UINT)_sapp.framebuffer_width, (UINT)_sapp.framebuffer_height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
        _sapp_d3d11_create_default_render_target();
    }
}

_SOKOL_PRIVATE void _sapp_d3d11_present(bool do_not_wait) {
    UINT flags = 0;
    if (_sapp.win32.is_win10_or_greater && do_not_wait) {
        /* this hack/workaround somewhat improves window-movement and -sizing
            responsiveness when rendering is controlled via WM_TIMER during window
            move and resize on NVIDIA cards on Win10 with recent drivers.
        */
        flags = DXGI_PRESENT_DO_NOT_WAIT;
    }
    _sapp_dxgi_Present(_sapp.d3d11.swap_chain, (UINT)_sapp.swap_interval, flags);
}

_SOKOL_PRIVATE bool _sapp_win32_wide_to_utf8(const wchar_t* src, char* dst, int dst_num_bytes) {
    SOKOL_ASSERT(src && dst && (dst_num_bytes > 1));
    _sapp_clear(dst, (size_t)dst_num_bytes);
    const int bytes_needed = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
    if (bytes_needed <= dst_num_bytes) {
        WideCharToMultiByte(CP_UTF8, 0, src, -1, dst, dst_num_bytes, NULL, NULL);
        return true;
    }
    else {
        return false;
    }
}

/* updates current window and framebuffer size from the window's client rect, returns true if size has changed */
_SOKOL_PRIVATE bool _sapp_win32_update_dimensions(void) {
    RECT rect;
    if (GetClientRect(_sapp.win32.hwnd, &rect)) {
        float window_width = (float)(rect.right - rect.left) / _sapp.win32.dpi.window_scale;
        float window_height = (float)(rect.bottom - rect.top) / _sapp.win32.dpi.window_scale;
        _sapp.window_width = (int)roundf(window_width);
        _sapp.window_height = (int)roundf(window_height);
        int fb_width = (int)roundf(window_width * _sapp.win32.dpi.content_scale);
        int fb_height = (int)roundf(window_height * _sapp.win32.dpi.content_scale);
        /* prevent a framebuffer size of 0 when window is minimized */
        if (0 == fb_width) {
            fb_width = 1;
        }
        if (0 == fb_height) {
            fb_height = 1;
        }
        if ((fb_width != _sapp.framebuffer_width) || (fb_height != _sapp.framebuffer_height)) {
            _sapp.framebuffer_width = fb_width;
            _sapp.framebuffer_height = fb_height;
            return true;
        }
    }
    else {
        _sapp.window_width = _sapp.window_height = 1;
        _sapp.framebuffer_width = _sapp.framebuffer_height = 1;
    }
    return false;
}

_SOKOL_PRIVATE void _sapp_win32_set_fullscreen(bool fullscreen, UINT swp_flags) {
    HMONITOR monitor = MonitorFromWindow(_sapp.win32.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO minfo;
    _sapp_clear(&minfo, sizeof(minfo));
    minfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &minfo);
    const RECT mr = minfo.rcMonitor;
    const int monitor_w = mr.right - mr.left;
    const int monitor_h = mr.bottom - mr.top;

    const DWORD win_ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD win_style;
    RECT rect = { 0, 0, 0, 0 };

    _sapp.fullscreen = fullscreen;
    if (!_sapp.fullscreen) {
        win_style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
        rect = _sapp.win32.stored_window_rect;
    }
    else {
        GetWindowRect(_sapp.win32.hwnd, &_sapp.win32.stored_window_rect);
        win_style = WS_POPUP | WS_SYSMENU | WS_VISIBLE;
        rect.left = mr.left;
        rect.top = mr.top;
        rect.right = rect.left + monitor_w;
        rect.bottom = rect.top + monitor_h;
        AdjustWindowRectEx(&rect, win_style, FALSE, win_ex_style);
    }
    const int win_w = rect.right - rect.left;
    const int win_h = rect.bottom - rect.top;
    const int win_x = rect.left;
    const int win_y = rect.top;
    SetWindowLongPtr(_sapp.win32.hwnd, GWL_STYLE, win_style);
    SetWindowPos(_sapp.win32.hwnd, HWND_TOP, win_x, win_y, win_w, win_h, swp_flags | SWP_FRAMECHANGED);
}

_SOKOL_PRIVATE void _sapp_win32_toggle_fullscreen(void) {
    _sapp_win32_set_fullscreen(!_sapp.fullscreen, SWP_SHOWWINDOW);
}

_SOKOL_PRIVATE void _sapp_win32_init_cursor(sapp_mouse_cursor cursor) {
    SOKOL_ASSERT((cursor >= 0) && (cursor < _SAPP_MOUSECURSOR_NUM));
    // NOTE: the OCR_* constants are only defined if OEMRESOURCE is defined
    // before windows.h is included, but we can't guarantee that because
    // the sokol_app.h implementation may be included with other implementations
    // in the same compilation unit
    int id = 0;
    switch (cursor) {
        case SAPP_MOUSECURSOR_ARROW:            id = 32512; break;  // OCR_NORMAL
        case SAPP_MOUSECURSOR_IBEAM:            id = 32513; break;  // OCR_IBEAM
        case SAPP_MOUSECURSOR_CROSSHAIR:        id = 32515; break;  // OCR_CROSS
        case SAPP_MOUSECURSOR_POINTING_HAND:    id = 32649; break;  // OCR_HAND
        case SAPP_MOUSECURSOR_RESIZE_EW:        id = 32644; break;  // OCR_SIZEWE
        case SAPP_MOUSECURSOR_RESIZE_NS:        id = 32645; break;  // OCR_SIZENS
        case SAPP_MOUSECURSOR_RESIZE_NWSE:      id = 32642; break;  // OCR_SIZENWSE
        case SAPP_MOUSECURSOR_RESIZE_NESW:      id = 32643; break;  // OCR_SIZENESW
        case SAPP_MOUSECURSOR_RESIZE_ALL:       id = 32646; break;  // OCR_SIZEALL
        case SAPP_MOUSECURSOR_NOT_ALLOWED:      id = 32648; break;  // OCR_NO
        default: break;
    }
    if (id != 0) {
        _sapp.win32.cursors[cursor] = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(id), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE|LR_SHARED);
    }
    // fallback: default cursor
    if (0 == _sapp.win32.cursors[cursor]) {
        // 32512 => IDC_ARROW
        _sapp.win32.cursors[cursor] = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    }
    SOKOL_ASSERT(0 != _sapp.win32.cursors[cursor]);
}

_SOKOL_PRIVATE void _sapp_win32_init_cursors(void) {
    for (int i = 0; i < _SAPP_MOUSECURSOR_NUM; i++) {
        _sapp_win32_init_cursor((sapp_mouse_cursor)i);
    }
}

_SOKOL_PRIVATE bool _sapp_win32_cursor_in_content_area(void) {
    POINT pos;
    if (!GetCursorPos(&pos)) {
        return false;
    }
    if (WindowFromPoint(pos) != _sapp.win32.hwnd) {
        return false;
    }
    RECT area;
    GetClientRect(_sapp.win32.hwnd, &area);
    ClientToScreen(_sapp.win32.hwnd, (POINT*)&area.left);
    ClientToScreen(_sapp.win32.hwnd, (POINT*)&area.right);
    return PtInRect(&area, pos) == TRUE;
}

_SOKOL_PRIVATE void _sapp_win32_update_cursor(sapp_mouse_cursor cursor, bool shown, bool skip_area_test) {
    // NOTE: when called from WM_SETCURSOR, the area test would be redundant
    if (!skip_area_test) {
        if (!_sapp_win32_cursor_in_content_area()) {
            return;
        }
    }
    if (!shown) {
        SetCursor(NULL);
    }
    else {
        SOKOL_ASSERT((cursor >= 0) && (cursor < _SAPP_MOUSECURSOR_NUM));
        SOKOL_ASSERT(0 != _sapp.win32.cursors[cursor]);
        SetCursor(_sapp.win32.cursors[cursor]);
    }
}

_SOKOL_PRIVATE void _sapp_win32_capture_mouse(uint8_t btn_mask) {
    if (0 == _sapp.win32.mouse_capture_mask) {
        SetCapture(_sapp.win32.hwnd);
    }
    _sapp.win32.mouse_capture_mask |= btn_mask;
}

_SOKOL_PRIVATE void _sapp_win32_release_mouse(uint8_t btn_mask) {
    if (0 != _sapp.win32.mouse_capture_mask) {
        _sapp.win32.mouse_capture_mask &= ~btn_mask;
        if (0 == _sapp.win32.mouse_capture_mask) {
            ReleaseCapture();
        }
    }
}

_SOKOL_PRIVATE void _sapp_win32_lock_mouse(bool lock) {
    if (lock == _sapp.mouse.locked) {
        return;
    }
    _sapp.mouse.dx = 0.0f;
    _sapp.mouse.dy = 0.0f;
    _sapp.mouse.locked = lock;
    _sapp_win32_release_mouse(0xFF);
    if (_sapp.mouse.locked) {
        /* store the current mouse position, so it can be restored when unlocked */
        POINT pos;
        BOOL res = GetCursorPos(&pos);
        SOKOL_ASSERT(res); _SOKOL_UNUSED(res);
        _sapp.win32.mouse_locked_x = pos.x;
        _sapp.win32.mouse_locked_y = pos.y;

        /* while the mouse is locked, make the mouse cursor invisible and
           confine the mouse movement to a small rectangle inside our window
           (so that we don't miss any mouse up events)
        */
        RECT client_rect = {
            _sapp.win32.mouse_locked_x,
            _sapp.win32.mouse_locked_y,
            _sapp.win32.mouse_locked_x,
            _sapp.win32.mouse_locked_y
        };
        ClipCursor(&client_rect);

        /* make the mouse cursor invisible, this will stack with sapp_show_mouse() */
        ShowCursor(FALSE);

        /* enable raw input for mouse, starts sending WM_INPUT messages to WinProc (see GLFW) */
        const RAWINPUTDEVICE rid = {
            0x01,   // usUsagePage: HID_USAGE_PAGE_GENERIC
            0x02,   // usUsage: HID_USAGE_GENERIC_MOUSE
            0,      // dwFlags
            _sapp.win32.hwnd    // hwndTarget
        };
        if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
            _SAPP_ERROR(WIN32_REGISTER_RAW_INPUT_DEVICES_FAILED_MOUSE_LOCK);
        }
        /* in case the raw mouse device only supports absolute position reporting,
           we need to skip the dx/dy compution for the first WM_INPUT event
        */
        _sapp.win32.raw_input_mousepos_valid = false;
    }
    else {
        /* disable raw input for mouse */
        const RAWINPUTDEVICE rid = { 0x01, 0x02, RIDEV_REMOVE, NULL };
        if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
            _SAPP_ERROR(WIN32_REGISTER_RAW_INPUT_DEVICES_FAILED_MOUSE_UNLOCK);
        }

        /* let the mouse roam freely again */
        ClipCursor(NULL);
        ShowCursor(TRUE);

        /* restore the 'pre-locked' mouse position */
        BOOL res = SetCursorPos(_sapp.win32.mouse_locked_x, _sapp.win32.mouse_locked_y);
        SOKOL_ASSERT(res); _SOKOL_UNUSED(res);
    }
}

_SOKOL_PRIVATE bool _sapp_win32_update_monitor(void) {
    const HMONITOR cur_monitor = MonitorFromWindow(_sapp.win32.hwnd, MONITOR_DEFAULTTONULL);
    if (cur_monitor != _sapp.win32.hmonitor) {
        _sapp.win32.hmonitor = cur_monitor;
        return true;
    }
    else {
        return false;
    }
}

_SOKOL_PRIVATE uint32_t _sapp_win32_mods(void) {
    uint32_t mods = 0;
    if (GetKeyState(VK_SHIFT) & (1<<15)) {
        mods |= SAPP_MODIFIER_SHIFT;
    }
    if (GetKeyState(VK_CONTROL) & (1<<15)) {
        mods |= SAPP_MODIFIER_CTRL;
    }
    if (GetKeyState(VK_MENU) & (1<<15)) {
        mods |= SAPP_MODIFIER_ALT;
    }
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & (1<<15)) {
        mods |= SAPP_MODIFIER_SUPER;
    }
    const bool swapped = (TRUE == GetSystemMetrics(SM_SWAPBUTTON));
    if (GetAsyncKeyState(VK_LBUTTON)) {
        mods |= swapped ? SAPP_MODIFIER_RMB : SAPP_MODIFIER_LMB;
    }
    if (GetAsyncKeyState(VK_RBUTTON)) {
        mods |= swapped ? SAPP_MODIFIER_LMB : SAPP_MODIFIER_RMB;
    }
    if (GetAsyncKeyState(VK_MBUTTON)) {
        mods |= SAPP_MODIFIER_MMB;
    }
    return mods;
}

_SOKOL_PRIVATE void _sapp_win32_mouse_update(LPARAM lParam) {
    if (!_sapp.mouse.locked) {
        const float new_x  = (float)GET_X_LPARAM(lParam) * _sapp.win32.dpi.mouse_scale;
        const float new_y = (float)GET_Y_LPARAM(lParam) * _sapp.win32.dpi.mouse_scale;
        if (_sapp.mouse.pos_valid) {
            // don't update dx/dy in the very first event
            _sapp.mouse.dx = new_x - _sapp.mouse.x;
            _sapp.mouse.dy = new_y - _sapp.mouse.y;
        }
        _sapp.mouse.x = new_x;
        _sapp.mouse.y = new_y;
        _sapp.mouse.pos_valid = true;
    }
}

_SOKOL_PRIVATE void _sapp_win32_mouse_event(sapp_event_type type, sapp_mousebutton btn) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.modifiers = _sapp_win32_mods();
        _sapp.event.mouse_button = btn;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_win32_scroll_event(float x, float y) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(SAPP_EVENTTYPE_MOUSE_SCROLL);
        _sapp.event.modifiers = _sapp_win32_mods();
        _sapp.event.scroll_x = -x / 30.0f;
        _sapp.event.scroll_y = y / 30.0f;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_win32_key_event(sapp_event_type type, int vk, bool repeat) {
    if (_sapp_events_enabled() && (vk < SAPP_MAX_KEYCODES)) {
        _sapp_init_event(type);
        _sapp.event.modifiers = _sapp_win32_mods();
        _sapp.event.key_code = _sapp.keycodes[vk];
        _sapp.event.key_repeat = repeat;
        _sapp_call_event(&_sapp.event);
        /* check if a CLIPBOARD_PASTED event must be sent too */
        if (_sapp.clipboard.enabled &&
            (type == SAPP_EVENTTYPE_KEY_DOWN) &&
            (_sapp.event.modifiers == SAPP_MODIFIER_CTRL) &&
            (_sapp.event.key_code == SAPP_KEYCODE_V))
        {
            _sapp_init_event(SAPP_EVENTTYPE_CLIPBOARD_PASTED);
            _sapp_call_event(&_sapp.event);
        }
    }
}

_SOKOL_PRIVATE void _sapp_win32_char_event(uint32_t c, bool repeat) {
    if (_sapp_events_enabled() && (c >= 32)) {
        _sapp_init_event(SAPP_EVENTTYPE_CHAR);
        _sapp.event.modifiers = _sapp_win32_mods();
        _sapp.event.char_code = c;
        _sapp.event.key_repeat = repeat;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_win32_dpi_changed(HWND hWnd, LPRECT proposed_win_rect) {
    /* called on WM_DPICHANGED, which will only be sent to the application
        if sapp_desc.high_dpi is true and the Windows version is recent enough
        to support DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    */
    SOKOL_ASSERT(_sapp.desc.high_dpi);
    HINSTANCE user32 = LoadLibraryA("user32.dll");
    if (!user32) {
        return;
    }
    typedef UINT(WINAPI * GETDPIFORWINDOW_T)(HWND hwnd);
    GETDPIFORWINDOW_T fn_getdpiforwindow = (GETDPIFORWINDOW_T)(void*)GetProcAddress(user32, "GetDpiForWindow");
    if (fn_getdpiforwindow) {
        UINT dpix = fn_getdpiforwindow(_sapp.win32.hwnd);
        // NOTE: for high-dpi apps, mouse_scale remains one
        _sapp.win32.dpi.window_scale = (float)dpix / 96.0f;
        _sapp.win32.dpi.content_scale = _sapp.win32.dpi.window_scale;
        _sapp.dpi_scale = _sapp.win32.dpi.window_scale;
        SetWindowPos(hWnd, 0,
            proposed_win_rect->left,
            proposed_win_rect->top,
            proposed_win_rect->right - proposed_win_rect->left,
            proposed_win_rect->bottom - proposed_win_rect->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
    FreeLibrary(user32);
}

_SOKOL_PRIVATE void _sapp_win32_files_dropped(HDROP hdrop) {
    if (!_sapp.drop.enabled) {
        return;
    }
    _sapp_clear_drop_buffer();
    bool drop_failed = false;
    const int count = (int) DragQueryFileW(hdrop, 0xffffffff, NULL, 0);
    _sapp.drop.num_files = (count > _sapp.drop.max_files) ? _sapp.drop.max_files : count;
    for (UINT i = 0;  i < (UINT)_sapp.drop.num_files;  i++) {
        const UINT num_chars = DragQueryFileW(hdrop, i, NULL, 0) + 1;
        WCHAR* buffer = (WCHAR*) _sapp_malloc_clear(num_chars * sizeof(WCHAR));
        DragQueryFileW(hdrop, i, buffer, num_chars);
        if (!_sapp_win32_wide_to_utf8(buffer, _sapp_dropped_file_path_ptr((int)i), _sapp.drop.max_path_length)) {
            _SAPP_ERROR(DROPPED_FILE_PATH_TOO_LONG);
            drop_failed = true;
        }
        _sapp_free(buffer);
    }
    DragFinish(hdrop);
    if (!drop_failed) {
        if (_sapp_events_enabled()) {
            _sapp_init_event(SAPP_EVENTTYPE_FILES_DROPPED);
            _sapp.event.modifiers = _sapp_win32_mods();
            _sapp_call_event(&_sapp.event);
        }
    }
    else {
        _sapp_clear_drop_buffer();
        _sapp.drop.num_files = 0;
    }
}

_SOKOL_PRIVATE void _sapp_win32_timing_measure(void) {
    // on D3D11, use the more precise DXGI timestamp
    if (_sapp.d3d11.use_dxgi_frame_stats) {
        DXGI_FRAME_STATISTICS dxgi_stats;
        _sapp_clear(&dxgi_stats, sizeof(dxgi_stats));
        HRESULT hr = _sapp_dxgi_GetFrameStatistics(_sapp.d3d11.swap_chain, &dxgi_stats);
        if (SUCCEEDED(hr)) {
            if (dxgi_stats.SyncRefreshCount != _sapp.d3d11.sync_refresh_count) {
                if ((_sapp.d3d11.sync_refresh_count + 1) != dxgi_stats.SyncRefreshCount) {
                    _sapp_timing_discontinuity(&_sapp.timing);
                }
                _sapp.d3d11.sync_refresh_count = dxgi_stats.SyncRefreshCount;
                LARGE_INTEGER qpc = dxgi_stats.SyncQPCTime;
                const uint64_t now = (uint64_t)_sapp_int64_muldiv(qpc.QuadPart - _sapp.timing.timestamp.win.start.QuadPart, 1000000000, _sapp.timing.timestamp.win.freq.QuadPart);
                _sapp_timing_external(&_sapp.timing, (double)now / 1000000000.0);
            }
            return;
        }
    }
    // fallback if swap model isn't "flip-discard" or GetFrameStatistics failed for another reason
    _sapp_timing_measure(&_sapp.timing);
    #if defined(SOKOL_NOAPI)
        _sapp_timing_measure(&_sapp.timing);
    #endif
}

_SOKOL_PRIVATE LRESULT CALLBACK _sapp_win32_wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!_sapp.win32.in_create_window) {
        switch (uMsg) {
            case WM_CLOSE:
                /* only give user a chance to intervene when sapp_quit() wasn't already called */
                if (!_sapp.quit_ordered) {
                    /* if window should be closed and event handling is enabled, give user code
                        a change to intervene via sapp_cancel_quit()
                    */
                    _sapp.quit_requested = true;
                    _sapp_win32_app_event(SAPP_EVENTTYPE_QUIT_REQUESTED);
                    /* if user code hasn't intervened, quit the app */
                    if (_sapp.quit_requested) {
                        _sapp.quit_ordered = true;
                    }
                }
                if (_sapp.quit_ordered) {
                    PostQuitMessage(0);
                }
                return 0;
            case WM_SYSCOMMAND:
                switch (wParam & 0xFFF0) {
                    case SC_SCREENSAVE:
                    case SC_MONITORPOWER:
                        if (_sapp.fullscreen) {
                            /* disable screen saver and blanking in fullscreen mode */
                            return 0;
                        }
                        break;
                    case SC_KEYMENU:
                        /* user trying to access menu via ALT */
                        return 0;
                }
                break;
            case WM_ERASEBKGND:
                return 1;
            case WM_SIZE:
                {
                    const bool iconified = wParam == SIZE_MINIMIZED;
                    if (iconified != _sapp.win32.iconified) {
                        _sapp.win32.iconified = iconified;
                        if (iconified) {
                            _sapp_win32_app_event(SAPP_EVENTTYPE_ICONIFIED);
                        }
                        else {
                            _sapp_win32_app_event(SAPP_EVENTTYPE_RESTORED);
                        }
                    }
                }
                break;
            case WM_SETFOCUS:
                _sapp_win32_app_event(SAPP_EVENTTYPE_FOCUSED);
                break;
            case WM_KILLFOCUS:
                /* if focus is lost for any reason, and we're in mouse locked mode, disable mouse lock */
                if (_sapp.mouse.locked) {
                    _sapp_win32_lock_mouse(false);
                }
                _sapp_win32_app_event(SAPP_EVENTTYPE_UNFOCUSED);
                break;
            case WM_SETCURSOR:
                if (LOWORD(lParam) == HTCLIENT) {
                    _sapp_win32_update_cursor(_sapp.mouse.current_cursor, _sapp.mouse.shown, true);
                    return TRUE;
                }
                break;
            case WM_DPICHANGED:
            {
                /* Update window's DPI and size if its moved to another monitor with a different DPI
                   Only sent if DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 is used.
                */
                _sapp_win32_dpi_changed(hWnd, (LPRECT)lParam);
                break;
            }
            case WM_LBUTTONDOWN:
                _sapp_win32_mouse_update(lParam);
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_LEFT);
                _sapp_win32_capture_mouse(1<<SAPP_MOUSEBUTTON_LEFT);
                break;
            case WM_RBUTTONDOWN:
                _sapp_win32_mouse_update(lParam);
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_RIGHT);
                _sapp_win32_capture_mouse(1<<SAPP_MOUSEBUTTON_RIGHT);
                break;
            case WM_MBUTTONDOWN:
                _sapp_win32_mouse_update(lParam);
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_MIDDLE);
                _sapp_win32_capture_mouse(1<<SAPP_MOUSEBUTTON_MIDDLE);
                break;
            case WM_LBUTTONUP:
                _sapp_win32_mouse_update(lParam);
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_LEFT);
                _sapp_win32_release_mouse(1<<SAPP_MOUSEBUTTON_LEFT);
                break;
            case WM_RBUTTONUP:
                _sapp_win32_mouse_update(lParam);
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_RIGHT);
                _sapp_win32_release_mouse(1<<SAPP_MOUSEBUTTON_RIGHT);
                break;
            case WM_MBUTTONUP:
                _sapp_win32_mouse_update(lParam);
                _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_MIDDLE);
                _sapp_win32_release_mouse(1<<SAPP_MOUSEBUTTON_MIDDLE);
                break;
            case WM_MOUSEMOVE:
                if (!_sapp.mouse.locked) {
                    _sapp_win32_mouse_update(lParam);
                    if (!_sapp.win32.mouse_tracked) {
                        _sapp.win32.mouse_tracked = true;
                        TRACKMOUSEEVENT tme;
                        _sapp_clear(&tme, sizeof(tme));
                        tme.cbSize = sizeof(tme);
                        tme.dwFlags = TME_LEAVE;
                        tme.hwndTrack = _sapp.win32.hwnd;
                        TrackMouseEvent(&tme);
                        _sapp.mouse.dx = 0.0f;
                        _sapp.mouse.dy = 0.0f;
                        _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_ENTER, SAPP_MOUSEBUTTON_INVALID);
                    }
                    _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID);
                }
                break;
            case WM_INPUT:
                /* raw mouse input during mouse-lock */
                if (_sapp.mouse.locked) {
                    HRAWINPUT ri = (HRAWINPUT) lParam;
                    UINT size = sizeof(_sapp.win32.raw_input_data);
                    // see: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getrawinputdata
                    if ((UINT)-1 == GetRawInputData(ri, RID_INPUT, &_sapp.win32.raw_input_data, &size, sizeof(RAWINPUTHEADER))) {
                        _SAPP_ERROR(WIN32_GET_RAW_INPUT_DATA_FAILED);
                        break;
                    }
                    const RAWINPUT* raw_mouse_data = (const RAWINPUT*) &_sapp.win32.raw_input_data;
                    if (raw_mouse_data->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
                        /* mouse only reports absolute position
                           NOTE: This code is untested and will most likely behave wrong in Remote Desktop sessions.
                           (such remote desktop sessions are setting the MOUSE_MOVE_ABSOLUTE flag).
                           See: https://github.com/floooh/sokol/issues/806 and
                           https://github.com/microsoft/DirectXTK/commit/ef56b63f3739381e451f7a5a5bd2c9779d2a7555)
                        */
                        LONG new_x = raw_mouse_data->data.mouse.lLastX;
                        LONG new_y = raw_mouse_data->data.mouse.lLastY;
                        if (_sapp.win32.raw_input_mousepos_valid) {
                            _sapp.mouse.dx = (float) (new_x - _sapp.win32.raw_input_mousepos_x);
                            _sapp.mouse.dy = (float) (new_y - _sapp.win32.raw_input_mousepos_y);
                        }
                        _sapp.win32.raw_input_mousepos_x = new_x;
                        _sapp.win32.raw_input_mousepos_y = new_y;
                        _sapp.win32.raw_input_mousepos_valid = true;
                    }
                    else {
                        /* mouse reports movement delta (this seems to be the common case) */
                        _sapp.mouse.dx = (float) raw_mouse_data->data.mouse.lLastX;
                        _sapp.mouse.dy = (float) raw_mouse_data->data.mouse.lLastY;
                    }
                    _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID);
                }
                break;

            case WM_MOUSELEAVE:
                if (!_sapp.mouse.locked) {
                    _sapp.mouse.dx = 0.0f;
                    _sapp.mouse.dy = 0.0f;
                    _sapp.win32.mouse_tracked = false;
                    _sapp_win32_mouse_event(SAPP_EVENTTYPE_MOUSE_LEAVE, SAPP_MOUSEBUTTON_INVALID);
                }
                break;
            case WM_MOUSEWHEEL:
                _sapp_win32_scroll_event(0.0f, (float)((SHORT)HIWORD(wParam)));
                break;
            case WM_MOUSEHWHEEL:
                _sapp_win32_scroll_event((float)((SHORT)HIWORD(wParam)), 0.0f);
                break;
            case WM_CHAR:
                _sapp_win32_char_event((uint32_t)wParam, !!(lParam&0x40000000));
                break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                _sapp_win32_key_event(SAPP_EVENTTYPE_KEY_DOWN, (int)(HIWORD(lParam)&0x1FF), !!(lParam&0x40000000));
                break;
            case WM_KEYUP:
            case WM_SYSKEYUP:
                _sapp_win32_key_event(SAPP_EVENTTYPE_KEY_UP, (int)(HIWORD(lParam)&0x1FF), false);
                break;
            case WM_ENTERSIZEMOVE:
                SetTimer(_sapp.win32.hwnd, 1, USER_TIMER_MINIMUM, NULL);
                break;
            case WM_EXITSIZEMOVE:
                KillTimer(_sapp.win32.hwnd, 1);
                break;
            case WM_TIMER:
                _sapp_win32_timing_measure();
                _sapp_frame();
                // present with DXGI_PRESENT_DO_NOT_WAIT
                _sapp_d3d11_present(true);
                /* NOTE: resizing the swap-chain during resize leads to a substantial
                   memory spike (hundreds of megabytes for a few seconds).

                if (_sapp_win32_update_dimensions()) {
                    #if defined(SOKOL_D3D11)
                    _sapp_d3d11_resize_default_render_target();
                    #endif
                    _sapp_win32_app_event(SAPP_EVENTTYPE_RESIZED);
                }
                */
                break;
            case WM_NCLBUTTONDOWN:
                /* workaround for half-second pause when starting to move window
                    see: https://gamedev.net/forums/topic/672094-keeping-things-moving-during-win32-moveresize-events/5254386/
                */
                if (SendMessage(_sapp.win32.hwnd, WM_NCHITTEST, wParam, lParam) == HTCAPTION) {
                    POINT point;
                    GetCursorPos(&point);
                    ScreenToClient(_sapp.win32.hwnd, &point);
                    PostMessage(_sapp.win32.hwnd, WM_MOUSEMOVE, 0, ((uint32_t)point.x)|(((uint32_t)point.y) << 16));
                }
                break;
            case WM_DROPFILES:
                _sapp_win32_files_dropped((HDROP)wParam);
                break;
            case WM_DISPLAYCHANGE:
                // refresh rate might have changed
                _sapp_timing_reset(&_sapp.timing);
                break;

            default:
                break;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

_SOKOL_PRIVATE void _sapp_win32_create_window(void) {
    WNDCLASSW wndclassw;
    _sapp_clear(&wndclassw, sizeof(wndclassw));
    wndclassw.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclassw.lpfnWndProc = (WNDPROC) _sapp_win32_wndproc;
    wndclassw.hInstance = GetModuleHandleW(NULL);
    wndclassw.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclassw.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wndclassw.lpszClassName = L"SOKOLAPP";
    RegisterClassW(&wndclassw);

    /* NOTE: regardless whether fullscreen is requested or not, a regular
       windowed-mode window will always be created first (however in hidden
       mode, so that no windowed-mode window pops up before the fullscreen window)
    */
    const DWORD win_ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    RECT rect = { 0, 0, 0, 0 };
    DWORD win_style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    rect.right = (int) ((float)_sapp.window_width * _sapp.win32.dpi.window_scale);
    rect.bottom = (int) ((float)_sapp.window_height * _sapp.win32.dpi.window_scale);
    const bool use_default_width = 0 == _sapp.window_width;
    const bool use_default_height = 0 == _sapp.window_height;
    AdjustWindowRectEx(&rect, win_style, FALSE, win_ex_style);
    const int win_width = rect.right - rect.left;
    const int win_height = rect.bottom - rect.top;
    _sapp.win32.in_create_window = true;
    _sapp.win32.hwnd = CreateWindowExW(
        win_ex_style,               // dwExStyle
        L"SOKOLAPP",                // lpClassName
        _sapp.window_title_wide,    // lpWindowName
        win_style,                  // dwStyle
        CW_USEDEFAULT,              // X
        SW_HIDE,                    // Y (NOTE: CW_USEDEFAULT is not used for position here, but internally calls ShowWindow!
        use_default_width ? CW_USEDEFAULT : win_width, // nWidth
        use_default_height ? CW_USEDEFAULT : win_height, // nHeight (NOTE: if width is CW_USEDEFAULT, height is actually ignored)
        NULL,                       // hWndParent
        NULL,                       // hMenu
        GetModuleHandle(NULL),      // hInstance
        NULL);                      // lParam
    _sapp.win32.in_create_window = false;
    _sapp.win32.dc = GetDC(_sapp.win32.hwnd);
    _sapp.win32.hmonitor = MonitorFromWindow(_sapp.win32.hwnd, MONITOR_DEFAULTTONULL);
    SOKOL_ASSERT(_sapp.win32.dc);

    /* this will get the actual windowed-mode window size, if fullscreen
       is requested, the set_fullscreen function will then capture the
       current window rectangle, which then might be used later to
       restore the window position when switching back to windowed
    */
    _sapp_win32_update_dimensions();
    if (_sapp.fullscreen) {
        _sapp_win32_set_fullscreen(_sapp.fullscreen, SWP_HIDEWINDOW);
        _sapp_win32_update_dimensions();
    }
    ShowWindow(_sapp.win32.hwnd, SW_SHOW);
    DragAcceptFiles(_sapp.win32.hwnd, 1);
}

_SOKOL_PRIVATE void _sapp_win32_destroy_window(void) {
    DestroyWindow(_sapp.win32.hwnd); _sapp.win32.hwnd = 0;
    UnregisterClassW(L"SOKOLAPP", GetModuleHandleW(NULL));
}

_SOKOL_PRIVATE void _sapp_win32_destroy_icons(void) {
    if (_sapp.win32.big_icon) {
        DestroyIcon(_sapp.win32.big_icon);
        _sapp.win32.big_icon = 0;
    }
    if (_sapp.win32.small_icon) {
        DestroyIcon(_sapp.win32.small_icon);
        _sapp.win32.small_icon = 0;
    }
}

_SOKOL_PRIVATE void _sapp_win32_init_console(void) {
    if (_sapp.desc.win32_console_create || _sapp.desc.win32_console_attach) {
        BOOL con_valid = FALSE;
        if (_sapp.desc.win32_console_create) {
            con_valid = AllocConsole();
        }
        else if (_sapp.desc.win32_console_attach) {
            con_valid = AttachConsole(ATTACH_PARENT_PROCESS);
        }
        if (con_valid) {
            FILE* res_fp = 0;
            errno_t err;
            err = freopen_s(&res_fp, "CON", "w", stdout);
            (void)err;
            err = freopen_s(&res_fp, "CON", "w", stderr);
            (void)err;
        }
    }
    if (_sapp.desc.win32_console_utf8) {
        _sapp.win32.orig_codepage = GetConsoleOutputCP();
        SetConsoleOutputCP(CP_UTF8);
    }
}

_SOKOL_PRIVATE void _sapp_win32_restore_console(void) {
    if (_sapp.desc.win32_console_utf8) {
        SetConsoleOutputCP(_sapp.win32.orig_codepage);
    }
}

_SOKOL_PRIVATE void _sapp_win32_init_dpi(void) {

    DECLARE_HANDLE(DPI_AWARENESS_CONTEXT_T);
    typedef BOOL(WINAPI * SETPROCESSDPIAWARE_T)(void);
    typedef bool (WINAPI * SETPROCESSDPIAWARENESSCONTEXT_T)(DPI_AWARENESS_CONTEXT_T); // since Windows 10, version 1703
    typedef HRESULT(WINAPI * SETPROCESSDPIAWARENESS_T)(PROCESS_DPI_AWARENESS);
    typedef HRESULT(WINAPI * GETDPIFORMONITOR_T)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

    SETPROCESSDPIAWARE_T fn_setprocessdpiaware = 0;
    SETPROCESSDPIAWARENESS_T fn_setprocessdpiawareness = 0;
    GETDPIFORMONITOR_T fn_getdpiformonitor = 0;
    SETPROCESSDPIAWARENESSCONTEXT_T fn_setprocessdpiawarenesscontext =0;

    HINSTANCE user32 = LoadLibraryA("user32.dll");
    if (user32) {
        fn_setprocessdpiaware = (SETPROCESSDPIAWARE_T)(void*) GetProcAddress(user32, "SetProcessDPIAware");
        fn_setprocessdpiawarenesscontext = (SETPROCESSDPIAWARENESSCONTEXT_T)(void*) GetProcAddress(user32, "SetProcessDpiAwarenessContext");
    }
    HINSTANCE shcore = LoadLibraryA("shcore.dll");
    if (shcore) {
        fn_setprocessdpiawareness = (SETPROCESSDPIAWARENESS_T)(void*) GetProcAddress(shcore, "SetProcessDpiAwareness");
        fn_getdpiformonitor = (GETDPIFORMONITOR_T)(void*) GetProcAddress(shcore, "GetDpiForMonitor");
    }
    /*
        NOTE on SetProcessDpiAware() vs SetProcessDpiAwareness() vs SetProcessDpiAwarenessContext():

        These are different attempts to get DPI handling on Windows right, from oldest
        to newest. SetProcessDpiAwarenessContext() is required for the new
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 method.
    */
    if (fn_setprocessdpiawareness) {
        if (_sapp.desc.high_dpi) {
            /* app requests HighDPI rendering, first try the Win10 Creator Update per-monitor-dpi awareness,
               if that fails, fall back to system-dpi-awareness
            */
            _sapp.win32.dpi.aware = true;
            DPI_AWARENESS_CONTEXT_T per_monitor_aware_v2 = (DPI_AWARENESS_CONTEXT_T)-4;
            if (!(fn_setprocessdpiawarenesscontext && fn_setprocessdpiawarenesscontext(per_monitor_aware_v2))) {
                // fallback to system-dpi-aware
                fn_setprocessdpiawareness(PROCESS_SYSTEM_DPI_AWARE);
            }
        }
        else {
            /* if the app didn't request HighDPI rendering, let Windows do the upscaling */
            _sapp.win32.dpi.aware = false;
            fn_setprocessdpiawareness(PROCESS_DPI_UNAWARE);
        }
    }
    else if (fn_setprocessdpiaware) {
        // fallback for Windows 7
        _sapp.win32.dpi.aware = true;
        fn_setprocessdpiaware();
    }
    /* get dpi scale factor for main monitor */
    if (fn_getdpiformonitor && _sapp.win32.dpi.aware) {
        POINT pt = { 1, 1 };
        HMONITOR hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        UINT dpix, dpiy;
        HRESULT hr = fn_getdpiformonitor(hm, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
        _SOKOL_UNUSED(hr);
        SOKOL_ASSERT(SUCCEEDED(hr));
        /* clamp window scale to an integer factor */
        _sapp.win32.dpi.window_scale = (float)dpix / 96.0f;
    }
    else {
        _sapp.win32.dpi.window_scale = 1.0f;
    }
    if (_sapp.desc.high_dpi) {
        _sapp.win32.dpi.content_scale = _sapp.win32.dpi.window_scale;
        _sapp.win32.dpi.mouse_scale = 1.0f;
    }
    else {
        _sapp.win32.dpi.content_scale = 1.0f;
        _sapp.win32.dpi.mouse_scale = 1.0f / _sapp.win32.dpi.window_scale;
    }
    _sapp.dpi_scale = _sapp.win32.dpi.content_scale;
    if (user32) {
        FreeLibrary(user32);
    }
    if (shcore) {
        FreeLibrary(shcore);
    }
}

_SOKOL_PRIVATE bool _sapp_win32_set_clipboard_string(const char* str) {
    SOKOL_ASSERT(str);
    SOKOL_ASSERT(_sapp.win32.hwnd);
    SOKOL_ASSERT(_sapp.clipboard.enabled && (_sapp.clipboard.buf_size > 0));

    if (!OpenClipboard(_sapp.win32.hwnd)) {
        return false;
    }

    HANDLE object = 0;
    wchar_t* wchar_buf = 0;

    const SIZE_T wchar_buf_size = (SIZE_T)_sapp.clipboard.buf_size * sizeof(wchar_t);
    object = GlobalAlloc(GMEM_MOVEABLE, wchar_buf_size);
    if (NULL == object) {
        goto error;
    }
    wchar_buf = (wchar_t*) GlobalLock(object);
    if (NULL == wchar_buf) {
        goto error;
    }
    if (!_sapp_win32_utf8_to_wide(str, wchar_buf, (int)wchar_buf_size)) {
        goto error;
    }
    GlobalUnlock(object);
    wchar_buf = 0;
    EmptyClipboard();
    // NOTE: when successful, SetClipboardData() takes ownership of memory object!
    if (NULL == SetClipboardData(CF_UNICODETEXT, object)) {
        goto error;
    }
    CloseClipboard();
    return true;

error:
    if (wchar_buf) {
        GlobalUnlock(object);
    }
    if (object) {
        GlobalFree(object);
    }
    CloseClipboard();
    return false;
}

_SOKOL_PRIVATE const char* _sapp_win32_get_clipboard_string(void) {
    SOKOL_ASSERT(_sapp.clipboard.enabled && _sapp.clipboard.buffer);
    SOKOL_ASSERT(_sapp.win32.hwnd);
    if (!OpenClipboard(_sapp.win32.hwnd)) {
        /* silently ignore any errors and just return the current
           content of the local clipboard buffer
        */
        return _sapp.clipboard.buffer;
    }
    HANDLE object = GetClipboardData(CF_UNICODETEXT);
    if (!object) {
        CloseClipboard();
        return _sapp.clipboard.buffer;
    }
    const wchar_t* wchar_buf = (const wchar_t*) GlobalLock(object);
    if (!wchar_buf) {
        CloseClipboard();
        return _sapp.clipboard.buffer;
    }
    if (!_sapp_win32_wide_to_utf8(wchar_buf, _sapp.clipboard.buffer, _sapp.clipboard.buf_size)) {
        _SAPP_ERROR(CLIPBOARD_STRING_TOO_BIG);
    }
    GlobalUnlock(object);
    CloseClipboard();
    return _sapp.clipboard.buffer;
}

_SOKOL_PRIVATE void _sapp_win32_update_window_title(void) {
    _sapp_win32_utf8_to_wide(_sapp.window_title, _sapp.window_title_wide, sizeof(_sapp.window_title_wide));
    SetWindowTextW(_sapp.win32.hwnd, _sapp.window_title_wide);
}

_SOKOL_PRIVATE HICON _sapp_win32_create_icon_from_image(const sapp_image_desc* desc) {
    BITMAPV5HEADER bi;
    _sapp_clear(&bi, sizeof(bi));
    bi.bV5Size = sizeof(bi);
    bi.bV5Width = desc->width;
    bi.bV5Height = -desc->height;   // NOTE the '-' here to indicate that origin is top-left
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;

    uint8_t* target = 0;
    const uint8_t* source = (const uint8_t*)desc->pixels.ptr;

    HDC dc = GetDC(NULL);
    HBITMAP color = CreateDIBSection(dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&target, NULL, (DWORD)0);
    ReleaseDC(NULL, dc);
    if (0 == color) {
        return NULL;
    }
    SOKOL_ASSERT(target);

    HBITMAP mask = CreateBitmap(desc->width, desc->height, 1, 1, NULL);
    if (0 == mask) {
        DeleteObject(color);
        return NULL;
    }

    for (int i = 0; i < (desc->width*desc->height); i++) {
        target[0] = source[2];
        target[1] = source[1];
        target[2] = source[0];
        target[3] = source[3];
        target += 4;
        source += 4;
    }

    ICONINFO icon_info;
    _sapp_clear(&icon_info, sizeof(icon_info));
    icon_info.fIcon = true;
    icon_info.xHotspot = 0;
    icon_info.yHotspot = 0;
    icon_info.hbmMask = mask;
    icon_info.hbmColor = color;
    HICON icon_handle = CreateIconIndirect(&icon_info);
    DeleteObject(color);
    DeleteObject(mask);

    return icon_handle;
}

_SOKOL_PRIVATE void _sapp_win32_set_icon(const sapp_icon_desc* icon_desc, int num_images) {
    SOKOL_ASSERT((num_images > 0) && (num_images <= SAPP_MAX_ICONIMAGES));

    int big_img_index = _sapp_image_bestmatch(icon_desc->images, num_images, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
    int sml_img_index = _sapp_image_bestmatch(icon_desc->images, num_images, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
    HICON big_icon = _sapp_win32_create_icon_from_image(&icon_desc->images[big_img_index]);
    HICON sml_icon = _sapp_win32_create_icon_from_image(&icon_desc->images[sml_img_index]);

    // if icon creation or lookup has failed for some reason, leave the currently set icon untouched
    if (0 != big_icon) {
        SendMessage(_sapp.win32.hwnd, WM_SETICON, ICON_BIG, (LPARAM) big_icon);
        if (0 != _sapp.win32.big_icon) {
            DestroyIcon(_sapp.win32.big_icon);
        }
        _sapp.win32.big_icon = big_icon;
    }
    if (0 != sml_icon) {
        SendMessage(_sapp.win32.hwnd, WM_SETICON, ICON_SMALL, (LPARAM) sml_icon);
        if (0 != _sapp.win32.small_icon) {
            DestroyIcon(_sapp.win32.small_icon);
        }
        _sapp.win32.small_icon = sml_icon;
    }
}

/* don't laugh, but this seems to be the easiest and most robust
   way to check if we're running on Win10

   From: https://github.com/videolan/vlc/blob/232fb13b0d6110c4d1b683cde24cf9a7f2c5c2ea/modules/video_output/win32/d3d11_swapchain.c#L263
*/
_SOKOL_PRIVATE bool _sapp_win32_is_win10_or_greater(void) {
    HMODULE h = GetModuleHandleW(L"kernel32.dll");
    if (NULL != h) {
        return (NULL != GetProcAddress(h, "GetSystemCpuSetInformation"));
    }
    else {
        return false;
    }
}

_SOKOL_PRIVATE void _sapp_win32_run(const sapp_desc* desc) {
    _sapp_init_state(desc);
    _sapp_win32_init_console();
    _sapp.win32.is_win10_or_greater = _sapp_win32_is_win10_or_greater();
    _sapp_win32_init_keytable();
    _sapp_win32_utf8_to_wide(_sapp.window_title, _sapp.window_title_wide, sizeof(_sapp.window_title_wide));
    _sapp_win32_init_dpi();
    _sapp_win32_init_cursors();
    _sapp_win32_create_window();
    sapp_set_icon(&desc->icon);
    _sapp_d3d11_create_device_and_swapchain();
    _sapp_d3d11_create_default_render_target();
    _sapp.valid = true;

    bool done = false;
    while (!(done || _sapp.quit_ordered)) {
        _sapp_win32_timing_measure();
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message) {
                done = true;
                continue;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        _sapp_frame();
        _sapp_d3d11_present(false);
        if (IsIconic(_sapp.win32.hwnd)) {
            Sleep((DWORD)(16 * _sapp.swap_interval));
        }
        /* check for window resized, this cannot happen in WM_SIZE as it explodes memory usage */
        if (_sapp_win32_update_dimensions()) {
            _sapp_d3d11_resize_default_render_target();
            _sapp_win32_app_event(SAPP_EVENTTYPE_RESIZED);
        }
        /* check if the window monitor has changed, need to reset timing because
           the new monitor might have a different refresh rate
        */
        if (_sapp_win32_update_monitor()) {
            _sapp_timing_reset(&_sapp.timing);
        }
        if (_sapp.quit_requested) {
            PostMessage(_sapp.win32.hwnd, WM_CLOSE, 0, 0);
        }
    }
    _sapp_call_cleanup();

    _sapp_d3d11_destroy_default_render_target();
    _sapp_d3d11_destroy_device_and_swapchain();
    _sapp_win32_destroy_window();
    _sapp_win32_destroy_icons();
    _sapp_win32_restore_console();
    _sapp_discard_state();
}

_SOKOL_PRIVATE char** _sapp_win32_command_line_to_utf8_argv(LPWSTR w_command_line, int* o_argc) {
    int argc = 0;
    char** argv = 0;
    char* args;

    LPWSTR* w_argv = CommandLineToArgvW(w_command_line, &argc);
    if (w_argv == NULL) {
        // FIXME: chicken egg problem, can't report errors before sokol_main() is called!
    } else {
        size_t size = wcslen(w_command_line) * 4;
        argv = (char**) _sapp_malloc_clear(((size_t)argc + 1) * sizeof(char*) + size);
        SOKOL_ASSERT(argv);
        args = (char*) &argv[argc + 1];
        int n;
        for (int i = 0; i < argc; ++i) {
            n = WideCharToMultiByte(CP_UTF8, 0, w_argv[i], -1, args, (int)size, NULL, NULL);
            if (n == 0) {
                // FIXME: chicken egg problem, can't report errors before sokol_main() is called!
                break;
            }
            argv[i] = args;
            size -= (size_t)n;
            args += n;
        }
        LocalFree(w_argv);
    }
    *o_argc = argc;
    return argv;
}

#if !defined(SOKOL_NO_ENTRY)
#if defined(SOKOL_WIN32_FORCE_MAIN)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_win32_run(&desc);
    return 0;
}
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    _SOKOL_UNUSED(hInstance);
    _SOKOL_UNUSED(hPrevInstance);
    _SOKOL_UNUSED(lpCmdLine);
    _SOKOL_UNUSED(nCmdShow);
    int argc_utf8 = 0;
    char** argv_utf8 = _sapp_win32_command_line_to_utf8_argv(GetCommandLineW(), &argc_utf8);
    sapp_desc desc = sokol_main(argc_utf8, argv_utf8);
    _sapp_win32_run(&desc);
    _sapp_free(argv_utf8);
    return 0;
}
#endif /* SOKOL_WIN32_FORCE_MAIN */
#endif /* SOKOL_NO_ENTRY */

#ifdef _MSC_VER
    #pragma warning(pop)
#endif
