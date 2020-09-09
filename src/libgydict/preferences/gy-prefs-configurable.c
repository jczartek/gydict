/* gy-prefs-configurable.c
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

#include "gy-prefs-configurable.h"

/**
 * SECTION:gy-prefs-configurable
 * @short_description: Interface for providing a plugin configuration UI.
 *
 * The #GyPrefsConfigurable interface will allow a plugin to provide a
 * graphical interface for the user to configure the plugin through the
 * #GyPrefsView.
 *
 * To allow plugin configuration from the #GyPrefView, the
 * plugin writer will just need to implement the
 * GyPeasConfigurableInterface.create_configure_widget() method.
 **/

G_DEFINE_INTERFACE (GyPrefsConfigurable, gy_prefs_configurable, G_TYPE_OBJECT)

static void
gy_prefs_configurable_default_init (GyPrefsConfigurableInterface *iface)
{
}

/**
 * gy_prefs_configurable_create_configure_widget:
 * @configurable: A #GyPrefsConfigurable
 *
 * Creates the configure widget for the plugin. The returned widget
 * should allow configuring all the relevant aspects of the plugin.
 *
 * #GyPrefsView will embed the returned widget into a list box.
 *
 * This method should always return a valid #GtkWidget instance, never %NULL.
 *
 * Returns: (transfer full): A #GtkWidget used for configuration or empty widget
 * if the plugin doesn't provide the GyPrefsConfigurable interface.
 */
GtkWidget *
gy_prefs_configurable_create_configure_widget (GyPrefsConfigurable *configurable)
{
  GyPrefsConfigurableInterface *iface;

  g_return_val_if_fail (GY_IS_PREFS_CONFIGURABLE (configurable), NULL);

  iface = GY_PREFS_CONFIGURABLE_GET_IFACE (configurable);

  if (G_LIKELY (iface->create_configure_widget != NULL))
    return iface->create_configure_widget(configurable);

  return (GtkWidget *) g_object_new (GTK_TYPE_BIN, "visible", FALSE, NULL);
}
