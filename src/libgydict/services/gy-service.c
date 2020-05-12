/* gy-service.c
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

#include "gy-service.h"

G_DEFINE_INTERFACE (GyService, gy_service, G_TYPE_OBJECT)

static void
gy_service_default_init (GyServiceInterface *iface)
{
      g_object_interface_install_property (iface,
                                       g_param_spec_string ("service-id",
                                                            "Service identifier",
                                                            "",
                                                            NULL,
                                                            G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY));

}

const gchar *
gy_service_get_service_id (GyService *self)
{
  GyServiceInterface *iface;

  g_return_val_if_fail (GY_IS_SERVICE (self), NULL);

  iface = GY_SERVICE_GET_IFACE (self);

  g_assert (iface->get_service_id != NULL);

  return iface->get_service_id (self);
}
