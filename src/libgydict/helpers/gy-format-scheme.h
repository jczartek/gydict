/* gy-format-scheme.h
 *
 * Copyright 2020 Jakub Czartek <kuba@linux.pl>
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
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "gy-text-attribute.h"

G_BEGIN_DECLS

typedef struct _GyFormatScheme GyFormatScheme;

GType gy_format_scheme_get_type (void) G_GNUC_CONST;
GyFormatScheme* gy_format_scheme_new (void);
GyFormatScheme* gy_format_scheme_copy (GyFormatScheme *scheme);
GyFormatScheme* gy_format_scheme_ref (GyFormatScheme *scheme);
void gy_format_scheme_unref (GyFormatScheme *scheme);

const GyTextAttrList* gy_format_scheme_get_attrs (GyFormatScheme *scheme);
void gy_format_scheme_add_text_attr (GyFormatScheme  *scheme,
                                     GyTextAttribute *attr);

void gy_format_scheme_append_text (GyFormatScheme *scheme,
                                   const gchar    *text);
void gy_format_scheme_append_text_len (GyFormatScheme *scheme,
                                       const gchar    *text,
                                       gssize          len);
void gy_format_scheme_append_char (GyFormatScheme *scheme,
                                   gchar           ch);
void gy_format_scheme_append_unichar (GyFormatScheme *scheme,
                                      gunichar        uch);

void gy_format_scheme_prepend_text (GyFormatScheme *scheme,
                                    const gchar    *text);
void gy_format_scheme_prepend_text_len (GyFormatScheme *scheme,
                                        const gchar    *text,
                                        gssize          len);
void gy_format_scheme_prepend_char (GyFormatScheme *scheme,
                                    gchar           ch);
void gy_format_scheme_prepend_unichar (GyFormatScheme *scheme,
                                       gunichar        uch);
const gchar* gy_format_scheme_get_lexical_unit (GyFormatScheme *scheme);

gsize gy_format_scheme_length_lexical_unit (GyFormatScheme *scheme);
G_END_DECLS
