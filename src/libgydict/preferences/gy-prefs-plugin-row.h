/* gy-pref-plugin-row.h
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
#include <dazzle.h>

G_BEGIN_DECLS

#define GY_TYPE_PREFS_PLUGIN_ROW (gy_prefs_plugin_row_get_type())

G_DECLARE_FINAL_TYPE (GyPrefsPluginRow, gy_prefs_plugin_row, GY, PREFS_PLUGIN_ROW, GtkBin)

GyPrefsPluginRow *gy_prefs_plugin_row_new (const gchar *name,
                                           const gchar *description,
                                           const gchar *schema_id,
                                           const gchar *key,
                                           const gchar *path);

G_END_DECLS
