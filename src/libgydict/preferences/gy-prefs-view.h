/* gy-prefs-view.h
 *
 * Copyright 2018 Jakub Czartek <kuba@linux.pl>
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

G_BEGIN_DECLS

#define GY_TYPE_PREFS_VIEW (gy_prefs_view_get_type())

G_DECLARE_FINAL_TYPE (GyPrefsView, gy_prefs_view, GY, PREFS_VIEW, DzlPreferencesView)

GtkSizeGroup * gy_prefs_view_get_size_group (GyPrefsView *self,
                                             const gchar *name_group);
G_END_DECLS
