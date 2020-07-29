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

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "../helpers/gy-text-attribute.h"
#include "../services/gy-dict-formatter.h"

G_BEGIN_DECLS

#define GY_TYPE_TEXT_BUFFER (gy_text_buffer_get_type())

G_DECLARE_FINAL_TYPE (GyTextBuffer, gy_text_buffer, GY, TEXT_BUFFER, GtkTextBuffer)

GyTextBuffer *gy_text_buffer_new                   (void);
void          gy_text_buffer_clean_buffer          (GyTextBuffer *self);
void          gy_text_buffer_insert_text_with_tags (GyTextBuffer   *self,
                                                    GtkTextIter    *iter,
                                                    const gchar    *text,
                                                    gint            len,
                                                    GHashTable     *table_tags);
void          gy_text_buffer_remove_tags_by_name    (GyTextBuffer *self,
                                                     const gchar  *first_tag_name,
                                                     ...);
GtkTextTag *gy_text_buffer_get_tag_by_name          (GyTextBuffer *self,
                                                     const gchar  *name_tag);

void gy_text_buffer_insert_with_attributes (GyTextBuffer  *self,
                                            GtkTextIter   *iter,
                                            const gchar   *text,
                                            GyTextAttrList *attributes);
void gy_text_buffer_insert_and_format (GyTextBuffer    *self,
                                       const gchar     *text,
                                       GyDictFormatter *formatter);

G_END_DECLS

#endif /* __GY_TEXT_BUFFER_H__ */
