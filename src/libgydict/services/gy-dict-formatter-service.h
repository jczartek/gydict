/* gy-dict-formatter-service.h
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

#include "gy-service.h"
#include "../helpers/gy-format-scheme.h"

G_BEGIN_DECLS

#define GY_TYPE_DICT_FORMATTER_SERVICE (gy_dict_formatter_service_get_type ())

G_DECLARE_INTERFACE (GyDictFormatterService, gy_dict_formatter_service, GY, DICT_FORMATTER_SERVICE, GyService)

struct _GyDictFormatterServiceInterface
{
  GTypeInterface parent;

  GyFormatScheme* (*format) (GyDictFormatterService  *self,
                             const gchar             *text_to_format,
                             GError                 **err);
};

GyFormatScheme* gy_dict_formatter_service_format (GyDictFormatterService  *self,
                                                  const gchar             *text_to_format,
                                                  GError                 **err);

G_END_DECLS
