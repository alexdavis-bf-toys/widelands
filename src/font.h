/*
 * Copyright (C) 2002 by the Widelands Development Team
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __S__FONT_H
#define __S__FONT_H

#include "graphic.h"
#include "singleton.h"


enum Align {
	Align_Left = 0,
	Align_HCenter = 1,
	Align_Right = 2,
	Align_Horizontal = 3,
	
	Align_Top = 0,
	Align_VCenter = 4,
	Align_Bottom = 8,
	Align_Vertical = 12,

	Align_TopLeft = 0,
	Align_CenterLeft = Align_VCenter,
	Align_BottomLeft = Align_Bottom,
	
	Align_TopCenter = Align_HCenter,
	Align_Center = Align_HCenter|Align_VCenter,
	Align_BottomCenter = Align_HCenter|Align_Bottom,

	Align_TopRight = Align_Right,
	Align_CenterRight = Align_Right|Align_VCenter,
	Align_BottomRight = Align_Right|Align_Bottom,
};


#define MAX_FONTS	5


#define WLFF_VERSION 	0x0001

#define WLFF_SUFFIX		".wff"
#define WLFF_MAGIC      "WLff"
#define WLFF_VERSIONMAJOR(a)  (a >> 8)
#define WLFF_VERSIONMINOR(a)  (a & 0xFF)

#define FERR_INVAL_FILE -127
#define FERR_INVAL_VERSION -128

/** class Font_Handler
 *
 * This class generates font Pictures out of strings and returns them
 *
 * It's a singleton
 *
 * It's a little ugly, since every char is hold in it's own pic which is quite a waste of 
 * good resources
 * 
 * DEPENDS: class	Graph::Pic
 * DEPENDS:	func	Graph::copy_pic
 * DEPENDS: class	myfile
 */
class Font_Handler : public Singleton<Font_Handler> {
		  Font_Handler(const Font_Handler&);
		  Font_Handler& operator=(const Font_Handler&);

		  public:
		  
					 Font_Handler();
					 ~Font_Handler();

					 
					 int load_font(const char*, const ushort);
					 Pic* get_string(const char*, const ushort);
					 
					 int calc_linewidth(const char* string, int wrap, const char** nextline, ushort font = 0);
					 void draw_string(Bitmap* dst, int x, int y, const char* string, Align align = Align_Left,
					                  int wrap = -1, ushort font = 0);
					 void get_size(const char* string, int* pw, int* ph, int wrap = -1, ushort font = 0);
					 
					 /** inline ushort get_fh(ushort f) 
					  *
					  * This function returns the height of the given font
					  * Args: f	which font to check
					  * Returns: height
					  */
					 inline ushort get_fh(ushort f) { assert(f<MAX_FONTS); return fonts[f].h; }
					 
					 /** inline ushort get_fw(ushort f) 
					  *
					  * This function returns the width of the given font
					  * Args: f	which font to check
					  * Returns: width
					  */
					// inline ushort get_fw(ushort f) { assert(f<MAX_FONTS); assert(w[f]); return w[f]; }
					 
		  private:
					 struct __font {
								ushort h;
								Pic p[96];
					 } fonts[MAX_FONTS];

					 struct FHeader {
								char magic[6];
								ushort version;
								char name[20];
								ushort clrkey;
								ushort height;
					 };
					 
};

#define FIXED_FONT1 0

#define g_fh	Font_Handler::get_singleton()

#endif /* __S__FONT_H */
