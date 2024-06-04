const std = @import("std");

pub const c = @cImport({
    @cInclude("webgpu.h");
    @cInclude("wgpu.h");
});

const log = std.log.scoped(.gpu);
const do_debug_log = std.log.logEnabled(.debug, .gpu);

pub const TextureUsage = enum(u32) {
    None = 0x00000000,
    CopySrc = 0x00000001,
    CopyDst = 0x00000002,
    TextureBinding = 0x00000004,
    StorageBinding = 0x00000008,
    RenderAttachment = 0x00000010,
};

pub const PresentMode = enum(u32) {
    Fifo = 0x00000000,
    FifoRelaxed = 0x00000001,
    Immediate = 0x00000002,
    Mailbox = 0x00000003,
};

pub const ColorWriteMask = enum(u32) {
    None = 0x00000000,
    Red = 0x00000001,
    Green = 0x00000002,
    Blue = 0x00000004,
    Alpha = 0x00000008,
    All = 0x00000001 | 0x00000002 | 0x00000004 | 0x00000008,
};

pub const PrimitiveTopology = enum(u32) {
    PointList = 0x00000000,
    LineList = 0x00000001,
    LineStrip = 0x00000002,
    TriangleList = 0x00000003,
    TriangleStrip = 0x00000004,
};

pub const LoadOp = enum(u32) {
    Clear = 1,
    Load = 2,
};

pub const StoreOp = enum(u32) {
    Store = 1,
    Discard = 2,
};

pub const DepthSliceUndefined = c.WGPU_DEPTH_SLICE_UNDEFINED;

pub const Instance = struct {
    ptr: c.WGPUInstance,

    pub fn init() !@This() {
        if (do_debug_log) c.wgpuSetLogLevel(c.WGPULogLevel_Trace);
        c.wgpuSetLogCallback(logCallback, null);

        var instance_desc = c.WGPUInstanceExtras{
            .chain = .{
                .sType = c.WGPUSType_InstanceExtras,
            },
            .flags = if (do_debug_log) c.WGPUInstanceFlag_Debug else 0,
        };
        return .{ .ptr = c.wgpuCreateInstance(&.{
            .nextInChain = @ptrCast(&instance_desc),
        }) orelse return error.InstanceFail };
    }

    pub fn deinit(self: @This()) void {
        c.wgpuInstanceRelease(self.ptr);
    }

    pub fn requestAdapter(self: @This(), options: *const c.WGPURequestAdapterOptions) !Adapter {
        var adapter: Adapter = .{ .ptr = null };
        c.wgpuInstanceRequestAdapter(self.ptr, options, handleRequestAdapter, &adapter);
        if (adapter.ptr == null) return error.FailedAdapter;
        return adapter;
    }
};

pub const Surface = struct {
    pub const Config = c.WGPUSurfaceConfiguration;

    pub const Capabilities = extern struct {
        nextInChain: *c.WGPUChainedStructOut,
        formatCount: usize,
        formats: [*c]const c.WGPUTextureFormat,
        presentModeCount: usize,
        presentModes: [*c]const c.WGPUPresentMode,
        alphaModeCount: usize,
        alphaModes: [*c]const c.WGPUCompositeAlphaMode,

        pub fn deinit(self: @This()) void {
            c.wgpuSurfaceCapabilitiesFreeMembers(@bitCast(self));
        }
    };

    ptr: c.WGPUSurface,
    pub fn deinit(self: @This()) void {
        c.wgpuSurfaceRelease(self.ptr);
    }

    pub fn getCapabilities(self: @This(), adapter: Adapter) Capabilities {
        var surface_capabilities: Capabilities = undefined;
        c.wgpuSurfaceGetCapabilities(self.ptr, adapter.ptr, @ptrCast(&surface_capabilities));
        return surface_capabilities;
    }

    pub fn getCurrentTexture(self: @This()) !Texture {
        var maybe_texture: c.WGPUSurfaceTexture = undefined;
        c.wgpuSurfaceGetCurrentTexture(self.ptr, &maybe_texture);

        switch (maybe_texture.status) {
            c.WGPUSurfaceGetCurrentTextureStatus_Success => {
                return .{ .ptr = maybe_texture.texture };
            },
            c.WGPUSurfaceGetCurrentTextureStatus_Timeout => return error.TextureTimeout,
            c.WGPUSurfaceGetCurrentTextureStatus_Outdated => return error.TextureOutdated,
            c.WGPUSurfaceGetCurrentTextureStatus_Lost => return error.TextureLost,
            c.WGPUSurfaceGetCurrentTextureStatus_OutOfMemory => return error.TextureOOM,
            c.WGPUSurfaceGetCurrentTextureStatus_DeviceLost => return error.TextureDeviceLost,
            else => unreachable,
        }
    }

    pub fn present(self: @This()) void {
        c.wgpuSurfacePresent(self.ptr);
    }

    pub fn configure(self: @This(), config: *const Config) void {
        c.wgpuSurfaceConfigure(self.ptr, config);
    }
};

pub const Texture = struct {
    pub const View = struct {
        ptr: c.WGPUTextureView,

        pub fn deinit(self: @This()) void {
            c.wgpuTextureViewRelease(self.ptr);
        }
    };

    ptr: c.WGPUTexture,

    pub fn deinit(self: @This()) void {
        c.wgpuTextureRelease(self.ptr);
    }

    pub fn createView(self: @This(), options: ?*const c.WGPUTextureViewDescriptor) !View {
        return .{ .ptr = c.wgpuTextureCreateView(self.ptr, options) orelse return error.ViewFailed };
    }
};

pub const Device = struct {
    ptr: c.WGPUDevice,

    pub fn getQueue(self: @This()) !Queue {
        return .{ .ptr = c.wgpuDeviceGetQueue(self.ptr) orelse return error.QueueFail };
    }

    pub fn deinit(self: @This()) void {
        c.wgpuDeviceRelease(self.ptr);
    }

    pub fn createShaderModule(self: @This(), name: [:0]const u8, src: ShaderModule.Src) !ShaderModule {
        const chain = switch (src) {
            .wgsl => |buf| c.WGPUShaderModuleWGSLDescriptor{
                .chain = .{
                    .sType = c.WGPUSType_ShaderModuleWGSLDescriptor,
                },
                .code = buf,
            },
        };
        return .{ .ptr = c.wgpuDeviceCreateShaderModule(self.ptr, &.{
            .label = name,
            .nextInChain = @ptrCast(&chain),
        }) orelse return error.WgpuShader };
    }

    pub fn createPipelineLayout(self: @This(), options: ?*const c.WGPUPipelineLayoutDescriptor) !PipelineLayout {
        return .{ .ptr = c.wgpuDeviceCreatePipelineLayout(
            self.ptr,
            options,
        ) orelse return error.WgpuPipelineLayout };
    }

    pub fn createRenderPipeline(self: @This(), options: *const c.WGPURenderPipelineDescriptor) !RenderPipeline {
        return .{ .ptr = c.wgpuDeviceCreateRenderPipeline(
            self.ptr,
            options,
        ) orelse return error.RenderPipelineFailed };
    }

    pub fn createCommandEncoder(self: @This(), options: ?*const c.WGPUCommandEncoderDescriptor) !CommandEncoder {
        return .{ .ptr = c.wgpuDeviceCreateCommandEncoder(self.ptr, options) orelse return error.WgpuCommandEncoder };
    }
};

pub const CommandEncoder = struct {
    ptr: c.WGPUCommandEncoder,

    pub fn deinit(self: @This()) void {
        c.wgpuCommandEncoderRelease(self.ptr);
    }

    pub fn beginRenderPass(self: @This(), options: *const c.WGPURenderPassDescriptor) !RenderPassEncoder {
        return .{ .ptr = c.wgpuCommandEncoderBeginRenderPass(self.ptr, options) orelse return error.RenderPassEncoderFail };
    }

    pub fn finish(self: @This(), options: ?*const c.WGPUCommandBufferDescriptor) !CommandBuffer {
        return .{ .ptr = c.wgpuCommandEncoderFinish(self.ptr, options) orelse return error.CommandBufferFail };
    }
};

pub const CommandBuffer = struct {
    ptr: c.WGPUCommandBuffer,
    pub fn deinit(self: @This()) void {
        c.wgpuCommandBufferRelease(self.ptr);
    }
};

pub const RenderPassEncoder = struct {
    ptr: c.WGPURenderPassEncoder,

    pub fn deinit(self: @This()) void {
        c.wgpuRenderPassEncoderRelease(self.ptr);
    }

    pub fn setPipeline(self: @This(), pipeline: RenderPipeline) void {
        c.wgpuRenderPassEncoderSetPipeline(self.ptr, pipeline.ptr);
    }

    const DrawArgs = struct {
        vertex_count: u32 = 0,
        instance_count: u32 = 1,
        first_vertex: u32 = 0,
        first_instance: u32 = 0,
    };
    pub fn draw(self: @This(), opts: DrawArgs) void {
        c.wgpuRenderPassEncoderDraw(
            self.ptr,
            opts.vertex_count,
            opts.instance_count,
            opts.first_vertex,
            opts.first_instance,
        );
    }

    pub fn end(self: @This()) void {
        c.wgpuRenderPassEncoderEnd(self.ptr);
    }
};

pub const Queue = struct {
    ptr: c.WGPUQueue,
    pub fn deinit(self: @This()) void {
        c.wgpuQueueRelease(self.ptr);
    }

    pub fn submit(self: @This(), count: u32, command: CommandBuffer) void {
        c.wgpuQueueSubmit(self.ptr, count, &command.ptr);
    }
};

pub const ShaderModule = struct {
    const Src = union(enum) {
        wgsl: [:0]const u8,
    };
    ptr: c.WGPUShaderModule,

    pub fn deinit(self: @This()) void {
        c.wgpuShaderModuleRelease(self.ptr);
    }
};

pub const PipelineLayout = struct {
    ptr: c.WGPUPipelineLayout,
    pub fn deinit(self: @This()) void {
        c.wgpuPipelineLayoutRelease(self.ptr);
    }
};

pub const RenderPipeline = struct {
    ptr: c.WGPURenderPipeline,

    pub fn deinit(self: @This()) void {
        c.wgpuRenderPipelineRelease(self.ptr);
    }
};

pub const Adapter = struct {
    ptr: c.WGPUAdapter,

    pub fn deinit(self: @This()) void {
        c.wgpuAdapterRelease(self.ptr);
    }

    pub fn requestDevice(self: @This(), maybe_options: ?*c.WGPUDeviceDescriptor) !Device {
        var device: Device = .{ .ptr = null };

        const default_options: c.WGPUDeviceDescriptor = .{
            .requiredLimits = &.{
                .limits = defaultLimits(),
            },
        };

        const options = if (maybe_options) |opt| opt else &default_options;
        c.wgpuAdapterRequestDevice(
            self.ptr,
            options,
            handleRequestDevice,
            &device,
        );
        if (device.ptr == null) return error.DeviceFailed;
        return device;
    }

    pub fn defaultLimits() c.WGPULimits {
        return .{
            .maxTextureDimension1D = 8192,
            .maxTextureDimension2D = 8192,
            .maxTextureDimension3D = 2048,
            .maxTextureArrayLayers = 256,
            .maxBindGroups = 4,
            .maxBindingsPerBindGroup = 1000,
            .maxDynamicUniformBuffersPerPipelineLayout = 8,
            .maxDynamicStorageBuffersPerPipelineLayout = 4,
            .maxSampledTexturesPerShaderStage = 16,
            .maxSamplersPerShaderStage = 16,
            .maxStorageBuffersPerShaderStage = 8,
            .maxStorageTexturesPerShaderStage = 4,
            .maxUniformBuffersPerShaderStage = 12,
            .maxUniformBufferBindingSize = 64 << 10, // (64 KiB)
            .maxStorageBufferBindingSize = 128 << 20, // (128 MiB)
            .minUniformBufferOffsetAlignment = 256,
            .minStorageBufferOffsetAlignment = 256,
            .maxVertexBuffers = 8,
            .maxBufferSize = 256 << 20, // (256 MiB)
            .maxVertexAttributes = 16,
            .maxVertexBufferArrayStride = 2048,
            .maxInterStageShaderComponents = 60,
            .maxColorAttachments = 8,
            .maxColorAttachmentBytesPerSample = 32,
            .maxComputeWorkgroupStorageSize = 16384,
            .maxComputeInvocationsPerWorkgroup = 256,
            .maxComputeWorkgroupSizeX = 256,
            .maxComputeWorkgroupSizeY = 256,
            .maxComputeWorkgroupSizeZ = 64,
            .maxComputeWorkgroupsPerDimension = 65535,

            // not specified in the rust code
            // .maxBindGroupsPlusVertexBuffers
            // .maxInterStageShaderVariables

            // not listed in the c code
            // max_push_constant_size: 0,
            // max_non_sampler_bindings: 1_000_000,
        };
    }
};

fn handleRequestAdapter(
    status: c.WGPURequestAdapterStatus,
    adapter: c.WGPUAdapter,
    message: [*c]const u8,
    userdata: ?*anyopaque,
) callconv(.C) void {
    var ctx: *Adapter = @ptrCast(@alignCast(userdata));
    if (status == c.WGPURequestAdapterStatus_Success) {
        ctx.ptr = adapter;
    } else {
        ctx.ptr = null;
        log.err("wgpu request adapter error: [{any}] {s}", .{ status, message });
    }
}

fn handleRequestDevice(
    status: c.WGPURequestDeviceStatus,
    device: c.WGPUDevice,
    message: [*c]const u8,
    userdata: ?*anyopaque,
) callconv(.C) void {
    var ctx: *Device = @ptrCast(@alignCast(userdata));
    if (status == c.WGPURequestDeviceStatus_Success) {
        ctx.ptr = device;
    } else {
        ctx.ptr = null;
        log.err("wgpu request device error: [{any}] {s}", .{ status, message });
    }
}

const LogLevel = enum(u32) {
    off = 0,
    err,
    warn,
    info,
    debug,
    trace,

    fn fromInt(i: u32) LogLevel {
        return @enumFromInt(i);
    }
};

fn logCallback(level: c.WGPULogLevel, message: [*c]const u8, userdata: ?*anyopaque) callconv(.C) void {
    _ = userdata;
    log.info("[wgpu {s}]: {s}", .{ @tagName(LogLevel.fromInt(level)), message });
}
