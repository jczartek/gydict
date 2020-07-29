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
 * gy_dict_service_get_model:
 * @self: a dictionary service
 * @err: addres of return location for errors, or %NULL
 *
 * Returns: (transfer none): Returns #GtkTreeModel
 */
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

/**
 * gy_dict_service_get_formatter:
 * @self: a service
 *
 * Returns: (transfer full): a formatter
 */
GyDictFormatter *
gy_dict_service_get_formatter (GyDictService *self)
{
  GyDictServiceInterface *iface;

  g_return_val_if_fail (GY_IS_DICT_SERVICE (self), NULL);

  iface = GY_DICT_SERVICE_GET_IFACE (self);

  g_assert (iface->get_formatter != NULL);

  return iface->get_formatter (self);

}
