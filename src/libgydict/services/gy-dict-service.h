/* gy-dict-service.h
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

#include <gtk/gtk.h>
#include "gy-service.h"
#include "gy-dict-formatter.h"

G_BEGIN_DECLS

#define GY_TYPE_DICT_SERVICE (gy_dict_service_get_type ())

G_DECLARE_INTERFACE (GyDictService, gy_dict_service, GY, DICT_SERVICE, GyService)

struct _GyDictServiceInterface
{
  GTypeInterface parent;

  GtkTreeModel *(*get_model) (GyDictService  *self,
                              GError        **err);

  gchar* (*get_lexical_unit) (GyDictService  *self,
                              guint           idx,
                              GError        **err);

  GyDictFormatter* (*get_formatter) (GyDictService *self);

};

GtkTreeModel* gy_dict_service_get_model (GyDictService  *self,
                                         GError        **err);

gchar* gy_dict_service_get_lexical_unit (GyDictService  *self,
                                         guint           idx,
                                         GError        **err);

GyDictFormatter *gy_dict_service_get_formatter (GyDictService *self);


G_END_DECLS
