/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#ifndef __Gy_PWNTABS_H__
#define __Gy_PWNTABS_H__

#include <gtk/gtk.h>

enum
{
  GY_TABS_CHARACTER_ENCODING_CP1250,
  GY_TABS_CHARACTER_ENCODING_ISO88592
};

extern const gchar *(*array_of_pointer_to_arrays_of_character_set[2])[];
extern const gchar *pwn_encje[];
extern const gchar *pwn_encje_zam[];
extern const gchar *pwndict_superscript_to_utf8_table[];

void gy_tabs_convert_character (gchar **buffer,
				const gchar *c,
				guint encoding);
GHashTable *
gy_tabs_get_entity_table (void);
    									
#endif /* end of include guard: __Gy_PWNTABS_H__ */
