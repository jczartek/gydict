/* gy-text-view.h
 *
 * Copyright (C) 2015 Jakub Czartek <kuba@linux.pl>
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

#ifndef __GY_TEXT_VIEW_H__
#define __GY_TEXT_VIEW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_TEXT_VIEW  (gy_text_view_get_type ())

G_DECLARE_FINAL_TYPE (GyTextView, gy_text_view, GY, TEXT_VIEW, GtkTextView);

void                        gy_text_view_set_font_desc (GyTextView                 *self,
                                                        const PangoFontDescription *font_desc);
void                        gy_text_view_set_font_name (GyTextView   *self,
                                                        const gchar  *font_name);
const PangoFontDescription* gy_text_view_get_font_desc (GyTextView *self);
void                        gy_text_view_set_background_pattern (GyTextView *self,
                                                                 gboolean    background_pattern);
gboolean                    gy_text_view_get_background_pattern (GyTextView *self);
void                        gy_text_view_clear_buffer (GyTextView *self);
G_END_DECLS

#endif /* __GY_TEXT_VIEW_H__ */
