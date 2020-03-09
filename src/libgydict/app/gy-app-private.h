/* gy-app-private.h
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

#include <dazzle.h>
#include <libpeas/peas.h>
#include "gy-app.h"

G_BEGIN_DECLS

struct _GyApp
{
  DzlApplication        __parent__;

  PeasExtensionSet      *extens;
  GHashTable            *plugin_settings;
  GHashTable            *plugin_gresources;
};

void _gy_app_action_init (GyApp *self);
void _gy_app_init_shortcuts (GyApp *self);
void _gy_app_cmd_line_add_options (GyApp *self);
void _gy_app_plugins_init_plugins (GyApp *self);
G_END_DECLS
