/* gy-window-addin.c
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

#include "gy-window-addin.h"

G_DEFINE_INTERFACE (GyWindowAddin, gy_window_addin, G_TYPE_OBJECT)

static void
gy_window_addin_real_load (GyWindowAddin *self,
                           GyWindow      *win)
{
}

static void
gy_window_addin_real_unload (GyWindowAddin *self,
                             GyWindow      *win)
{
}

static void
gy_window_addin_default_init (GyWindowAddinInterface *iface)
{
  iface->load = gy_window_addin_real_load;
  iface->unload = gy_window_addin_real_unload;
}

void
gy_window_addin_load (GyWindowAddin *self,
                      GyWindow      *win)
{
  g_return_if_fail (GY_IS_WINDOW_ADDIN (self));
  g_return_if_fail (GY_IS_WINDOW (win));

  GY_WINDOW_ADDIN_GET_IFACE (self)->load (self, win);
}

void
gy_window_addin_unload (GyWindowAddin *self,
                        GyWindow      *win)
{
  g_return_if_fail (GY_IS_WINDOW_ADDIN (self));
  g_return_if_fail (GY_IS_WINDOW (win));

  GY_WINDOW_ADDIN_GET_IFACE (self)->unload (self, win);
}

