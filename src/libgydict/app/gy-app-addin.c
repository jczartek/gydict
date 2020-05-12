/* gy-app-addin.c
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

#include "gy-app-addin.h"

G_DEFINE_INTERFACE (GyAppAddin, gy_app_addin, G_TYPE_OBJECT)


static void
gy_app_addin_real_load (GyAppAddin *self,
                        GyApp      *app)
{
}

static void
gy_app_addin_real_unload (GyAppAddin *self,
                          GyApp      *app)
{
}

static void
gy_app_addin_default_init (GyAppAddinInterface *iface)
{
  iface->load = gy_app_addin_real_load;
  iface->unload = gy_app_addin_real_unload;
}

/**
 * gy_app_addin_load:
 * @self: An #GyAppAddin.
 * @app: An #GyApp.
 *
 * This interface method is called when the application is started or the
 * plugin has just been activated.
 *
 * Use this to setup code in your plugin that needs to be loaded once per
 * application process.
 *
 * Since: 0.6
 */
void
gy_app_addin_load (GyAppAddin *self,
                   GyApp      *app)
{
  g_return_if_fail (GY_IS_APP_ADDIN (self));
  g_return_if_fail (GY_IS_APP (app));

  GY_APP_ADDIN_GET_IFACE (self)->load (self, app);
}

/**
 * gy_app_addin_unload:
 * @self: An #GyAppAddin.
 * @app: An #GyApp.
 *
 * This inteface method is called when the application is shutting down or the
 * plugin has been unloaded.
 *
 * Use this function to cleanup after anything setup in
 * gy_app_addin_load().
 *
 * Since: 0.6
 */
void
gy_app_addin_unload (GyAppAddin *self,
                     GyApp      *app)
{
  g_return_if_fail (GY_IS_APP_ADDIN (self));
  g_return_if_fail (GY_IS_APP (app));

  GY_APP_ADDIN_GET_IFACE (self)->unload (self, app);
}

