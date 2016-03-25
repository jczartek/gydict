/* gy-pwntabs.h
 *
 * Copyright (C) 2014 Jakub Czartaek <kuba@linux.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GY_PWNTABS_H__
#define __GY_PWNTABS_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum
{
  GY_ENCODING_NONE = -1,
  GY_ENCODING_CP1250,
  GY_ENCODING_ISO88592,
  GY_ENCODING_UTF8
} GyDictEncoding;

void gy_tabs_convert_character (gchar **buffer,
                                const gchar *c,
                                GyDictEncoding encoding);
GHashTable * gy_tabs_get_entity_table (void);
const gchar * gy_tabs_get_superscript (guint n);

G_END_DECLS

#endif /* end of include guard: __GY_PWNTABS_H__ */
