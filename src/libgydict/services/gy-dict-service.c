/* gy-dict-service.c
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

#include "gy-dict-service.h"

G_DEFINE_INTERFACE (GyDictService, gy_dict_service, GY_TYPE_SERVICE)


static void
gy_dict_service_default_init (GyDictServiceInterface *iface)
{
}

/**
 * gy_dict_service_parse:
 * @self: a dictionary service
 * @text_to_parse: text to parse
 * @err: addres of return location for errors, or %NULL
 *
 * Return value: (transfer full): #GyDictDataScheme
 */
GyDictDataScheme *
gy_dict_service_parse (GyDictService   *self,
                       const gchar     *text_to_parse,
                       GError         **err)
{
  GyDictServiceInterface *iface;

  g_return_val_if_fail (GY_IS_DICT_SERVICE (self), NULL);
  g_return_val_if_fail (g_utf8_validate (text_to_parse, -1, NULL), NULL);

  iface = GY_DICT_SERVICE_GET_IFACE (self);

  g_assert (iface->parse != NULL);

  return iface->parse (self, text_to_parse, err);
}


GtkTreeModel *
gy_dict_service_get_model (GyDictService  *self,
                           GError        **err)
{
  GyDictServiceInterface *iface;

  g_return_val_if_fail (GY_IS_DICT_SERVICE (self), NULL);

  iface = GY_DICT_SERVICE_GET_IFACE (self);

  g_assert (iface->get_model != NULL);

  return iface->get_model (self, err);
}

gchar *
gy_dict_service_get_lexical_unit (GyDictService  *self,
                                  guint           idx,
                                  GError        **err)
{
  GyDictServiceInterface *iface;

  g_return_val_if_fail (GY_IS_DICT_SERVICE (self), NULL);

  iface = GY_DICT_SERVICE_GET_IFACE (self);

  g_assert (iface->get_lexical_unit != NULL);

  return iface->get_lexical_unit (self, idx, err);
}

