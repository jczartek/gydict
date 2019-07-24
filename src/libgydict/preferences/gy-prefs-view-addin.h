/* gy-prefs-view-addin.h
 *
 * Copyright 2019 Jakub Czartek <kuba@linux.pl>
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

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include <gydict.h>

G_BEGIN_DECLS

#define GY_TYPE_PREFS_VIEW_ADDIN (gy_prefs_view_addin_get_type ())

G_DECLARE_INTERFACE (GyPrefsViewAddin, gy_prefs_view_addin, GY, PREFS_VIEW_ADDIN, GObject)

struct _GyPrefsViewAddinInterface
{
  GTypeInterface parent;

  void (*load) (GyPrefsViewAddin *self,
                GyPrefsView      *prefs);
  void (*unload) (GyPrefsViewAddin *self,
                  GyPrefsView      *prefs);
};

void gy_prefs_view_addin_load (GyPrefsViewAddin *self,
                               GyPrefsView      *prefs);

void gy_prefs_view_addin_unload (GyPrefsViewAddin *self,
                                 GyPrefsView      *prefs);

G_END_DECLS
