#include <stdio.h>
#include <stdlib.h>
// https://freetype.org/freetype2/docs/documentation.html
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

int main(int argc, char** argv) {
  char* filename = argv[1];

  // Initialize the FreeType library
  FT_Library library;
  if (FT_Init_FreeType(&library)) {
      fprintf(stderr, "Could not initialize the FreeType library.\n");
      return 1;
  }

  // Load the font face from the font file
  FT_Face face;
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
  if (FT_HAS_FIXED_SIZES(face)){
      FT_Select_Size(face, 0);  // use first fixed size
  } else {
    if (FT_Set_Char_Size(
          face,
          /* char_width */ 0,
          /* char_height */ 16 * 64,
          /* x res */ 0,
          /* y res */ 300)) {
        fprintf(stderr, "warn: Could not set font size.\n");
    }
  }

  int load_flags = FT_LOAD_DEFAULT;
  if(FT_HAS_COLOR(face)) load_flags |= FT_LOAD_COLOR;

  int num_glyphs = face->num_glyphs;
  printf("num_glyphs=%d\n", num_glyphs);

  // Affine transform to be applied on glyph load
  // Coefficients of the matrix are otherwise in 16.16 fixed-point units
  // The vector coordinates are expressed in 1/64 of a pixel (also known as
  // 26.6 fixed-point numbers)
  FT_Set_Transform( face , /* 2x2 matrix */ NULL , /* 2d vector delta */ NULL );

  // Print glpyh info and ascii render
  for (int glyph_ind = 0 ; glyph_ind < num_glyphs; glyph_ind++ ) {
    if ( FT_Load_Glyph( face, glyph_ind, load_flags ) ) {
      fprintf(stderr, "Could not load glyph %d\n", glyph_ind);
      continue;
    }
    FT_Glyph glyph;
    if ( FT_Get_Glyph( face->glyph, &glyph ) ) {
      fprintf(stderr, "Could not get glyph %d\n", glyph_ind);
      exit( 1 );
    }

    // If it is not already a bitmap, render it to one
    FT_Vector  pen;
    pen.x = 0;
    pen.y = 0;
    if ( glyph->format != FT_GLYPH_FORMAT_BITMAP )
      if ( FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, &pen, 0 ) ) {
        fprintf(stderr, "Could not get bitmap");
        exit( 1 );
      }

    char char_name[256];
    if ( FT_Get_Glyph_Name( face, glyph_ind, char_name, 16 ) )
      sprintf(char_name, "noname");

    FT_BitmapGlyph  bit = (FT_BitmapGlyph)glyph;
    FT_Bitmap* bitmap = &bit->bitmap;
    FT_Glyph_Metrics* glyph_metrics = &face->glyph->metrics;

    // ASCII render
    for (int i = 0; i < bitmap->rows; ++i) {
      for (int j = 0; j < bitmap->width; ++j) {
        char* s = bitmap->buffer[i * bitmap->width + j] == 0 ? "_" : "X";
        printf("%s", s);
      }
      printf("\n");
    }

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

  printf("ok");
}

// TODO:
// * Glyph lifetimes (glyph vs bitmap, destroy arg, FT_Done_Glyph)
// * Emoji (bw + color)
