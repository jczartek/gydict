/* gy-prefs-view-addin.c
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

#include "gy-prefs-view-addin.h"

G_DEFINE_INTERFACE (GyPrefsViewAddin, gy_prefs_view_addin, G_TYPE_OBJECT)

static void
gy_prefs_view_addin_real_load (GyPrefsViewAddin *self,
                               GyPrefsView      *prefs)
{
}

static void
gy_prefs_view_addin_real_unload (GyPrefsViewAddin *self,
                                 GyPrefsView      *prefs)
{
}

static void
gy_prefs_view_addin_default_init (GyPrefsViewAddinInterface *iface)
{
  iface->load = gy_prefs_view_addin_real_load;
  iface->unload = gy_prefs_view_addin_real_unload;
}

void
gy_prefs_view_addin_load (GyPrefsViewAddin *self,
                          GyPrefsView      *prefs)
{
  g_return_if_fail (GY_IS_PREFS_VIEW_ADDIN (self));
  g_return_if_fail (GY_IS_PREFS_VIEW (prefs));

  GY_PREFS_VIEW_ADDIN_GET_IFACE (self)->load (self, prefs);
}

void
gy_prefs_view_addin_unload (GyPrefsViewAddin *self,
                            GyPrefsView      *prefs)
{
  g_return_if_fail (GY_IS_PREFS_VIEW_ADDIN (self));
  g_return_if_fail (GY_IS_PREFS_VIEW (prefs));

  GY_PREFS_VIEW_ADDIN_GET_IFACE (self)->load (self, prefs);
}
