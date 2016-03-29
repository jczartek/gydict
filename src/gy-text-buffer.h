/* gy-text-buffer.h
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
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

#ifndef __GY_TEXT_BUFFER_H__
#define __GY_TEXT_BUFFER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_TEXT_BUFFER (gy_text_buffer_get_type())

G_DECLARE_FINAL_TYPE (GyTextBuffer, gy_text_buffer, GY, TEXT_BUFFER, GtkTextBuffer)

GyTextBuffer *gy_text_buffer_new          (void);
void          gy_text_buffer_clean_buffer (GyTextBuffer *self);

G_END_DECLS

#endif /* __GY_TEXT_BUFFER_H__ */
