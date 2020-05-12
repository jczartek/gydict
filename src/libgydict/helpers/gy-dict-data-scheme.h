/* gy-dict-data-scheme.h
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

typedef struct _GyDictDataScheme GyDictDataScheme;

GType gy_dict_data_scheme_get_type (void) G_GNUC_CONST;

GyDictDataScheme *gy_dict_data_scheme_new (void);

GyDictDataScheme * gy_dict_data_scheme_copy (GyDictDataScheme *scheme);

void gy_dict_data_scheme_unref (GyDictDataScheme *scheme);

const gchar *gy_dict_data_scheme_get_parsed_text (GyDictDataScheme *scheme);

void gy_dict_data_scheme_set_parsed_text (GyDictDataScheme *scheme,
                                          gchar            *parsed_text);

void gy_dict_data_scheme_dup_parsed_text (GyDictDataScheme *scheme,
                                          const gchar      *parsed_text);

const GyTextAttrList * gy_dict_data_scheme_get_attrs (GyDictDataScheme *scheme);

void gy_dict_data_scheme_set_attrs (GyDictDataScheme *scheme,
                                    GyTextAttrList   *attrs);


G_END_DECLS
