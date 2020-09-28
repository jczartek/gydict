/* gy-plugins.c
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

#include "gy-plugins.h"

guint
gy_plugins_get_number_loaded_plugins (void)
{
  PeasEngine *engine = peas_engine_get_default ();
  const GList *plugins = peas_engine_get_plugin_list (engine);

  guint number_plugins = 0;
  for(GList *iter = (GList *) plugins; iter != NULL; iter = g_list_next(iter))
    if (peas_plugin_info_is_loaded (iter->data))
      number_plugins++;

  return number_plugins;
}
