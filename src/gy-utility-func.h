/* gy-utility-func.h
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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

#ifndef __GY_UTILITY_H__
#define __GY_UTILITY_H__

#include <gtk/gtk.h>
#include <pango/pango.h>

G_BEGIN_DECLS

extern void gy_utility_delete_text_in_buffer(GtkTextBuffer *buffer);
extern gboolean gy_utility_handlers_is_blocked_by_func(gpointer instance,
                                                       gpointer func,
                                                       gpointer data);
extern gint gy_utility_strcmp (const gchar *p1,
                               const gchar *p2,
                               size_t n);
extern void gy_utility_text_buffer_insert_text_with_tags (GtkTextBuffer *buffer,
                                                          GtkTextIter   *iter,
                                                          const gchar   *text,
                                                          gint           len,
                                                          GHashTable    *table_tags);

extern gchar *gy_utility_pango_font_description_to_css (const PangoFontDescription *font_desc);

extern gchar *gy_utility_compute_md5_for_file (GFile  *file,
                                               GError **err);

G_END_DECLS

#endif
