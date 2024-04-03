#include <stdio.h>
#include <stdlib.h>
// https://freetype.org/freetype2/docs/documentation.html
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
// https://harfbuzz.github.io
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

// TODO:
// * Hook up to graphics rendering
// * Text layout
//   * First pass determines positioning/sizing
//   * Second renders
// * Caching renders

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
  if (FT_New_Face(
        library,
        filename,
        /* face_index */ 0,
        &face)) {
      fprintf(stderr, "Could not open font file.\n");
      FT_Done_FreeType(library);
      return 1;
  }

  // Set the font size
  // widths and heights are specified in 1/64 of points
  // 1 pt == 1/72 of an inch
  // xy res are expressed in dots-per-inch, or dpi
  // Value of 0 for the {width, x res} means same as other dim
  if (FT_Set_Char_Size(
        face,
        /* char_width */ 0,
        /* char_height */ 16 * 64,
        /* x res */ 0,
        /* y res */ 300)) {
      fprintf(stderr, "Could not set font size.\n");
      FT_Done_Face(face);
      FT_Done_FreeType(library);
      return 1;
  }

  int num_glyphs = face->num_glyphs;
  printf("num_glyphs=%d\n", num_glyphs);


  // Affine transform to be applied on glyph load
  // Coefficients of the matrix are otherwise in 16.16 fixed-point units
  // The vector coordinates are expressed in 1/64 of a pixel (also known as
  // 26.6 fixed-point numbers)
  FT_Set_Transform( face , /* 2x2 matrix */ NULL , /* 2d vector delta */ NULL );

  for (int glyph_ind = 0 ; glyph_ind < num_glyphs; glyph_ind++ ) {
    if ( FT_Load_Glyph( face, glyph_ind, FT_LOAD_DEFAULT ) )
      exit( 1 );
    FT_Glyph glyph;
    if ( FT_Get_Glyph( face->glyph, &glyph ) )
      exit( 1 );

    // If it is not already a bitmap, render it to one
    FT_Vector  pen;
    pen.x = 0;
    pen.y = 0;
    if ( glyph->format != FT_GLYPH_FORMAT_BITMAP )
      if ( FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, &pen, 0 ) )
        exit( 1 );

    char char_name[256];
    if ( FT_Get_Glyph_Name( face, glyph_ind, char_name, 16 ) )
      exit( 1 );

    FT_BitmapGlyph  bit = (FT_BitmapGlyph)glyph;
    FT_Bitmap* bitmap = &bit->bitmap;
    FT_Glyph_Metrics* glyph_metrics = &face->glyph->metrics;

    // This function should perform linear blending with gamma correction,
    // using the bitmap as an alpha channel
    // my_draw_bitmap(
    //     bitmap,
    //     pen.x + bit->bitmap_left,
    //     pen.y - bit->bitmap_top);
    // The advance vector is expressed in 1/64 of pixels, and is truncated to
    // integer pixels on each iteration
    // pen.x += bit->advance.x >> 6;
    // pen.y += bit->advance.y >> 6;

    printf( "Glyph %d  name %s %ld %ld %ld %d %d\n",
            glyph_ind,
            char_name,
            glyph_metrics->horiBearingX / 64,
            glyph_metrics->horiBearingY / 64,
            glyph_metrics->horiAdvance / 64,
            bitmap->width , bitmap->rows );

    FT_Done_Glyph( glyph );
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
