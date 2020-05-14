/* gy-dict-formatter-service.c
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

#include "gy-dict-formatter-service.h"

G_DEFINE_INTERFACE (GyDictFormatterService, gy_dict_formatter_service, GY_TYPE_SERVICE)

static void
gy_dict_formatter_service_default_init (GyDictFormatterServiceInterface *iface)
{
}


/**
 * gy_dict_formatter_service_format:
 * @self: a dictionary service
 * @text_to_parse: text to parse
 * @err: addres of return location for errors, or %NULL
 *
 * Returns: (transfer full) (nullable): #GyDictDataScheme
 */
GyFormatScheme* gy_dict_formatter_service_format (GyDictFormatterService  *self,
                                                  const gchar             *text_to_format,
                                                  GError                 **err)
{
  GyDictFormatterServiceInterface *iface;

  g_return_val_if_fail (GY_IS_DICT_FORMATTER_SERVICE (self), NULL);
  g_return_val_if_fail (g_utf8_validate (text_to_format, -1, NULL), NULL);

  iface = GY_DICT_FORMATTER_SERVICE_GET_IFACE (self);

  g_assert (iface->format != NULL);

  return iface->format (self, text_to_format, err);
}
