const app = @import("app");
const gpu = @import("gpu");

extern fn initGlfwWgpuSurface(gpu.c.WGPUInstance, *app.glfw.c.GLFWwindow, *gpu.c.WGPUSurface) c_int;

pub fn getSurface(instance: gpu.Instance, ctx: *app.Ctx) !gpu.Surface {
    switch (app.platform) {
        .mac, .linux, .windows => {
            var surface: gpu.Surface = undefined;
            if (initGlfwWgpuSurface(instance.ptr, ctx.glfwWindow(), &surface.ptr) != 0) return error.Glue;
            return surface;
        },
        .ios => {
            var desc = gpu.c.WGPUSurfaceDescriptorFromMetalLayer{
                .chain = .{
                    .sType = gpu.c.WGPUSType_SurfaceDescriptorFromMetalLayer,
                },
                .layer = ctx.getMetalLayer(),
            };
            return .{ .ptr = gpu.c.wgpuInstanceCreateSurface(instance.ptr, &.{
                .nextInChain = @ptrCast(&desc),
            }) orelse return error.SurfaceFail };
        },
        .android => {
            @compileError("unimpl");
        },
    }
}
