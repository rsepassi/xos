#include <stdio.h>
#include <stdlib.h>
#include <freetype/freetype.h>
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

int main(int argc, char** argv) {
	// Initialize the FreeType library
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
			fprintf(stderr, "Could not initialize the FreeType library.\n");
			return 1;
	}

	// Load the font face from the font file
	FT_Face face;
	char* filename = "/tmp/CourierPrime-Regular.ttf";
	if (FT_New_Face(library, filename, 0, &face)) {
			fprintf(stderr, "Could not open font file.\n");
			FT_Done_FreeType(library);
			return 1;
	}

	// Set the font size
	if (FT_Set_Pixel_Sizes(face, 0, 16)) {
			fprintf(stderr, "Could not set font size.\n");
			FT_Done_Face(face);
			FT_Done_FreeType(library);
			return 1;
	}

	int num_glyphs = face->num_glyphs;
  printf("num_glyphs=%d\n", num_glyphs);
  FT_Set_Transform( face , NULL , NULL );

	for (int glyph_ind = 0 ; glyph_ind < num_glyphs; glyph_ind++ ) {
		if ( FT_Load_Glyph( face, glyph_ind, FT_LOAD_DEFAULT ) )
			exit( 1 );
		FT_GlyphSlot cur_glyph = face->glyph;
		if ( cur_glyph->format != FT_GLYPH_FORMAT_BITMAP )
			if ( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO ) )
				exit( 1 );
    char char_name[256];
		if ( FT_Get_Glyph_Name( face, glyph_ind, char_name, 16 ) )
			exit( 1 );

		FT_Bitmap* bitmap = &cur_glyph->bitmap;
		FT_Glyph_Metrics* glyph_metrics = &cur_glyph->metrics;

		printf( "Glyph %d  name %s %ld %ld %ld %d %d\n",
						glyph_ind,
						char_name,
						glyph_metrics->horiBearingX / 64,
						glyph_metrics->horiBearingY / 64,
						glyph_metrics->horiAdvance / 64,
						bitmap->width , bitmap->rows );
	}

	char* text = "hello world";
	hb_buffer_t *buf = hb_buffer_create();
  if (!buf) exit(1);
	hb_buffer_add_utf8(buf, text, -1, 0, -1);
	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));

	hb_face_t* hb_face = hb_ft_face_create_referenced(face);
	hb_font_t *font = hb_font_create(hb_face);
  if (!font) exit(1);

	hb_shape(font, buf, NULL, 0);

	unsigned int glyph_count;
	hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

	hb_position_t cursor_x = 0;
	hb_position_t cursor_y = 0;
	for (unsigned int i = 0; i < glyph_count; i++) {
		hb_codepoint_t glyphid  = glyph_info[i].codepoint;
		hb_position_t x_offset  = glyph_pos[i].x_offset;
		hb_position_t y_offset  = glyph_pos[i].y_offset;
		hb_position_t x_advance = glyph_pos[i].x_advance;
		hb_position_t y_advance = glyph_pos[i].y_advance;
    printf("%d %d %d %d %d\n", glyphid, x_offset, y_offset, x_advance, y_advance);
		/* draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset); */
		cursor_x += x_advance;
		cursor_y += y_advance;
	}

  printf("ok\n");

	hb_buffer_destroy(buf);
	hb_font_destroy(font);
	hb_face_destroy(hb_face);

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return 0;
}
