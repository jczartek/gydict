/* gy-window-plugins.c
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

#include "gy-window-private.h"
#include "gy-window-addin.h"

static void
gy_window_plugins_addin_added (PeasExtensionSet *set,
                               PeasPluginInfo   *plugin_info,
                               PeasExtension    *exten,
                               gpointer          user_data)
{
  GyWindow *self = GY_WINDOW (user_data);
  GyWindowAddin *addin = GY_WINDOW_ADDIN (exten);

  gy_window_addin_load (addin, self);
}


static void
gy_window_plugins_addin_removed (PeasExtensionSet *set,
                                 PeasPluginInfo   *plugin_info,
                                 PeasExtension    *exten,
                                 gpointer          user_data)
{
  GyWindow *self = GY_WINDOW (user_data);
  GyWindowAddin *addin = GY_WINDOW_ADDIN (exten);

  gy_window_addin_unload (addin, self);
}


void _gy_window_plugins_init_extens(GyWindow *self)
{
  PeasEngine *engine = peas_engine_get_default ();
  self->extens = peas_extension_set_new (engine, GY_TYPE_WINDOW_ADDIN, NULL);

  g_signal_connect (self->extens, "extension-added",
                    G_CALLBACK (gy_window_plugins_addin_added), self);

  g_signal_connect (self->extens, "extension-removed",
                    G_CALLBACK (gy_window_plugins_addin_removed), self);

  peas_extension_set_foreach (self->extens, gy_window_plugins_addin_added, self );
}
