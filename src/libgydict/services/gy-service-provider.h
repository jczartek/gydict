/* gy-service-provider.h
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

G_BEGIN_DECLS

#define GY_TYPE_SERVICE_PROVIDER (gy_service_provider_get_type())

G_DECLARE_FINAL_TYPE (GyServiceProvider, gy_service_provider, GY, SERVICE_PROVIDER, GObject)

GyServiceProvider *gy_service_provider_new (void);

void gy_service_provider_register_service (GyServiceProvider *self,
                                           GyService         *service);
void gy_service_provider_unregister_service (GyServiceProvider *self,
                                             GyService         *service);
GyService *gy_service_provider_get_service_by_id (GyServiceProvider *self,
                                                  const gchar       *service_id);

G_END_DECLS
