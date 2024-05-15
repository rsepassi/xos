// ██████  ██    ██ ███    ███ ███    ███ ██    ██     ██████   █████   ██████ ██   ██ ███████ ███    ██ ██████
// ██   ██ ██    ██ ████  ████ ████  ████  ██  ██      ██   ██ ██   ██ ██      ██  ██  ██      ████   ██ ██   ██
// ██   ██ ██    ██ ██ ████ ██ ██ ████ ██   ████       ██████  ███████ ██      █████   █████   ██ ██  ██ ██   ██
// ██   ██ ██    ██ ██  ██  ██ ██  ██  ██    ██        ██   ██ ██   ██ ██      ██  ██  ██      ██  ██ ██ ██   ██
// ██████   ██████  ██      ██ ██      ██    ██        ██████  ██   ██  ██████ ██   ██ ███████ ██   ████ ██████
//
// >>dummy backend

_SOKOL_PRIVATE void _sg_dummy_setup_backend(const sg_desc* desc) {
    SOKOL_ASSERT(desc);
    _SOKOL_UNUSED(desc);
    _sg.backend = SG_BACKEND_DUMMY;
    for (int i = SG_PIXELFORMAT_R8; i < SG_PIXELFORMAT_BC1_RGBA; i++) {
        _sg.formats[i].sample = true;
        _sg.formats[i].filter = true;
        _sg.formats[i].render = true;
        _sg.formats[i].blend = true;
        _sg.formats[i].msaa = true;
    }
    _sg.formats[SG_PIXELFORMAT_DEPTH].depth = true;
    _sg.formats[SG_PIXELFORMAT_DEPTH_STENCIL].depth = true;
}

_SOKOL_PRIVATE void _sg_dummy_discard_backend(void) {
    // empty
}

_SOKOL_PRIVATE void _sg_dummy_reset_state_cache(void) {
    // empty
}

_SOKOL_PRIVATE sg_resource_state _sg_dummy_create_buffer(_sg_buffer_t* buf, const sg_buffer_desc* desc) {
    SOKOL_ASSERT(buf && desc);
    _SOKOL_UNUSED(buf);
    _SOKOL_UNUSED(desc);
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_dummy_discard_buffer(_sg_buffer_t* buf) {
    SOKOL_ASSERT(buf);
    _SOKOL_UNUSED(buf);
}

_SOKOL_PRIVATE sg_resource_state _sg_dummy_create_image(_sg_image_t* img, const sg_image_desc* desc) {
    SOKOL_ASSERT(img && desc);
    _SOKOL_UNUSED(img);
    _SOKOL_UNUSED(desc);
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_dummy_discard_image(_sg_image_t* img) {
    SOKOL_ASSERT(img);
    _SOKOL_UNUSED(img);
}

_SOKOL_PRIVATE sg_resource_state _sg_dummy_create_sampler(_sg_sampler_t* smp, const sg_sampler_desc* desc) {
    SOKOL_ASSERT(smp && desc);
    _SOKOL_UNUSED(smp);
    _SOKOL_UNUSED(desc);
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_dummy_discard_sampler(_sg_sampler_t* smp) {
    SOKOL_ASSERT(smp);
    _SOKOL_UNUSED(smp);
}

_SOKOL_PRIVATE sg_resource_state _sg_dummy_create_shader(_sg_shader_t* shd, const sg_shader_desc* desc) {
    SOKOL_ASSERT(shd && desc);
    _SOKOL_UNUSED(shd);
    _SOKOL_UNUSED(desc);
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_dummy_discard_shader(_sg_shader_t* shd) {
    SOKOL_ASSERT(shd);
    _SOKOL_UNUSED(shd);
}

_SOKOL_PRIVATE sg_resource_state _sg_dummy_create_pipeline(_sg_pipeline_t* pip, _sg_shader_t* shd, const sg_pipeline_desc* desc) {
    SOKOL_ASSERT(pip && desc);
    pip->shader = shd;
    for (int attr_index = 0; attr_index < SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        const sg_vertex_attr_state* a_state = &desc->layout.attrs[attr_index];
        if (a_state->format == SG_VERTEXFORMAT_INVALID) {
            break;
        }
        SOKOL_ASSERT(a_state->buffer_index < SG_MAX_VERTEX_BUFFERS);
        pip->cmn.vertex_buffer_layout_active[a_state->buffer_index] = true;
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_dummy_discard_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    _SOKOL_UNUSED(pip);
}

_SOKOL_PRIVATE sg_resource_state _sg_dummy_create_attachments(_sg_attachments_t* atts, _sg_image_t** color_images, _sg_image_t** resolve_images, _sg_image_t* ds_img, const sg_attachments_desc* desc) {
    SOKOL_ASSERT(atts && desc);
    SOKOL_ASSERT(color_images && resolve_images);

    for (int i = 0; i < atts->cmn.num_colors; i++) {
        const sg_attachment_desc* color_desc = &desc->colors[i];
        _SOKOL_UNUSED(color_desc);
        SOKOL_ASSERT(color_desc->image.id != SG_INVALID_ID);
        SOKOL_ASSERT(0 == atts->dmy.colors[i].image);
        SOKOL_ASSERT(color_images[i] && (color_images[i]->slot.id == color_desc->image.id));
        SOKOL_ASSERT(_sg_is_valid_rendertarget_color_format(color_images[i]->cmn.pixel_format));
        atts->dmy.colors[i].image = color_images[i];

        const sg_attachment_desc* resolve_desc = &desc->resolves[i];
        if (resolve_desc->image.id != SG_INVALID_ID) {
            SOKOL_ASSERT(0 == atts->dmy.resolves[i].image);
            SOKOL_ASSERT(resolve_images[i] && (resolve_images[i]->slot.id == resolve_desc->image.id));
            SOKOL_ASSERT(color_images[i] && (color_images[i]->cmn.pixel_format == resolve_images[i]->cmn.pixel_format));
            atts->dmy.resolves[i].image = resolve_images[i];
        }
    }

    SOKOL_ASSERT(0 == atts->dmy.depth_stencil.image);
    const sg_attachment_desc* ds_desc = &desc->depth_stencil;
    if (ds_desc->image.id != SG_INVALID_ID) {
        SOKOL_ASSERT(ds_img && (ds_img->slot.id == ds_desc->image.id));
        SOKOL_ASSERT(_sg_is_valid_rendertarget_depth_format(ds_img->cmn.pixel_format));
        atts->dmy.depth_stencil.image = ds_img;
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_dummy_discard_attachments(_sg_attachments_t* atts) {
    SOKOL_ASSERT(atts);
    _SOKOL_UNUSED(atts);
}

_SOKOL_PRIVATE _sg_image_t* _sg_dummy_attachments_color_image(const _sg_attachments_t* atts, int index) {
    SOKOL_ASSERT(atts && (index >= 0) && (index < SG_MAX_COLOR_ATTACHMENTS));
    return atts->dmy.colors[index].image;
}

_SOKOL_PRIVATE _sg_image_t* _sg_dummy_attachments_resolve_image(const _sg_attachments_t* atts, int index) {
    SOKOL_ASSERT(atts && (index >= 0) && (index < SG_MAX_COLOR_ATTACHMENTS));
    return atts->dmy.resolves[index].image;
}

_SOKOL_PRIVATE _sg_image_t* _sg_dummy_attachments_ds_image(const _sg_attachments_t* atts) {
    SOKOL_ASSERT(atts);
    return atts->dmy.depth_stencil.image;
}

_SOKOL_PRIVATE void _sg_dummy_begin_pass(const sg_pass* pass) {
    SOKOL_ASSERT(pass);
    _SOKOL_UNUSED(pass);
}

_SOKOL_PRIVATE void _sg_dummy_end_pass(void) {
    // empty
}

_SOKOL_PRIVATE void _sg_dummy_commit(void) {
    // empty
}

_SOKOL_PRIVATE void _sg_dummy_apply_viewport(int x, int y, int w, int h, bool origin_top_left) {
    _SOKOL_UNUSED(x);
    _SOKOL_UNUSED(y);
    _SOKOL_UNUSED(w);
    _SOKOL_UNUSED(h);
    _SOKOL_UNUSED(origin_top_left);
}

_SOKOL_PRIVATE void _sg_dummy_apply_scissor_rect(int x, int y, int w, int h, bool origin_top_left) {
    _SOKOL_UNUSED(x);
    _SOKOL_UNUSED(y);
    _SOKOL_UNUSED(w);
    _SOKOL_UNUSED(h);
    _SOKOL_UNUSED(origin_top_left);
}

_SOKOL_PRIVATE void _sg_dummy_apply_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    _SOKOL_UNUSED(pip);
}

_SOKOL_PRIVATE bool _sg_dummy_apply_bindings(_sg_bindings_t* bnd) {
    SOKOL_ASSERT(bnd);
    SOKOL_ASSERT(bnd->pip);
    _SOKOL_UNUSED(bnd);
    return true;
}

_SOKOL_PRIVATE void _sg_dummy_apply_uniforms(sg_shader_stage stage_index, int ub_index, const sg_range* data) {
    _SOKOL_UNUSED(stage_index);
    _SOKOL_UNUSED(ub_index);
    _SOKOL_UNUSED(data);
}

_SOKOL_PRIVATE void _sg_dummy_draw(int base_element, int num_elements, int num_instances) {
    _SOKOL_UNUSED(base_element);
    _SOKOL_UNUSED(num_elements);
    _SOKOL_UNUSED(num_instances);
}

_SOKOL_PRIVATE void _sg_dummy_update_buffer(_sg_buffer_t* buf, const sg_range* data) {
    SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
    _SOKOL_UNUSED(data);
    if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
        buf->cmn.active_slot = 0;
    }
}

_SOKOL_PRIVATE bool _sg_dummy_append_buffer(_sg_buffer_t* buf, const sg_range* data, bool new_frame) {
    SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
    _SOKOL_UNUSED(data);
    if (new_frame) {
        if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
            buf->cmn.active_slot = 0;
        }
    }
    return true;
}

_SOKOL_PRIVATE void _sg_dummy_update_image(_sg_image_t* img, const sg_image_data* data) {
    SOKOL_ASSERT(img && data);
    _SOKOL_UNUSED(data);
    if (++img->cmn.active_slot >= img->cmn.num_slots) {
        img->cmn.active_slot = 0;
    }
}

