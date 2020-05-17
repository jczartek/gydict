/* gy-app-plugins.c
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

#include "config.h"
#include "gy-app.h"
#include "gy-app-private.h"
#include "gy-app-addin.h"

static void
gy_app_settings_changed (GSettings *settings,
                         gchar     *key,
                         gpointer   user_data)
{
  PeasPluginInfo *pinfo = PEAS_PLUGIN_INFO (user_data);
  PeasEngine *engine = peas_engine_get_default ();
  gboolean enabled = g_settings_get_boolean (settings, key);

  if (enabled && !peas_plugin_info_is_loaded (pinfo))
    {
      peas_engine_load_plugin (engine, pinfo);
    }
  else if (peas_plugin_info_is_loaded (pinfo))
    {
      peas_engine_unload_plugin (engine, pinfo);
    }
}

static void
gy_app_addin_added (PeasExtensionSet *set,
                    PeasPluginInfo   *plugin_info,
                    PeasExtension    *exten,
                    gpointer          user_data)
{
  GyApp *self = GY_APP (user_data);
  GyAppAddin *addin = GY_APP_ADDIN (exten);

  gy_app_addin_load (addin, self);
}

static void
gy_app_addin_removed (PeasExtensionSet *set,
                      PeasPluginInfo   *plugin_info,
                      PeasExtension    *exten,
                      gpointer          user_data)
{
  GyApp *self = GY_APP (user_data);
  GyAppAddin *addin = GY_APP_ADDIN (exten);

  gy_app_addin_unload (addin, self);
}

static inline void
gy_app_plugin_configure_search_paths (GyApp      *self,
                                      PeasEngine *engine)
{
  g_autofree gchar *local_plugins_datadir = g_build_filename (g_get_home_dir (), ".local", "share", "gydict", "plugins", NULL);
  g_autofree gchar *local_plugins_moduledir = g_build_filename (g_get_home_dir (),
                                                                ".local", "lib", NULL);

  peas_engine_add_search_path (engine, PACKAGE_LIBDIR"/plugins", PACKAGE_DATADIR"/plugins");
  peas_engine_add_search_path (engine, local_plugins_moduledir, local_plugins_datadir);
  peas_engine_rescan_plugins (engine);
}

static inline void
gy_app_plugin_configure_girepository_and_enable_python_loader (GyApp      *self,
                                                               PeasEngine *engine)
{
  GError *error = NULL;

  g_irepository_prepend_search_path (PACKAGE_LIBDIR"/girepository-1.0");

  if (!g_irepository_require (NULL, "Gtk", "3.0", 0, &error) ||
      !g_irepository_require (NULL, "Gio", "2.0", 0, &error) ||
      !g_irepository_require (NULL, "GLib", "2.0", 0, &error) ||
      !g_irepository_require (NULL, "Dazzle", "1.0", 0, &error) ||
      !g_irepository_require (NULL, "Gydict", "1.0", 0, &error))
    g_critical ("Cannot enable Python 3 plugins: %s", error->message);
  else
    peas_engine_enable_loader (engine, "python3");
}

static inline void
gy_app_plugin_load_all_plugins (GyApp      *self,
                                PeasEngine *engine)
{
  const GList *plugs = peas_engine_get_plugin_list (engine);
  for (const GList *iter = plugs; iter; iter = iter->next)
    {
      const gchar *name;
      g_autofree gchar *path = NULL;

      name = peas_plugin_info_get_module_name (iter->data);
      path = g_strdup_printf ("/org/gtk/gydict/plugins/%s/", name);

      GSettings *settings = g_settings_new_with_path ("org.gtk.gydict.plugin", path);
      g_hash_table_insert (self->plugin_settings, g_strdup (name), settings);

      g_signal_connect (settings, "changed",
                        G_CALLBACK (gy_app_settings_changed), iter->data);

      gboolean enabled = g_settings_get_boolean (settings, "enabled");
      if (!peas_plugin_info_is_loaded (iter->data) && enabled)
        peas_engine_load_plugin (engine, iter->data);
    }
}

void
_gy_app_plugins_init_plugins (GyApp *self)
{
  PeasEngine *engine = peas_engine_get_default ();

  gy_app_plugin_configure_search_paths (self, engine);
  gy_app_plugin_configure_girepository_and_enable_python_loader (self, engine);

  self->extens = peas_extension_set_new (engine, GY_TYPE_APP_ADDIN, NULL);

  g_signal_connect (self->extens, "extension-added",
                    G_CALLBACK (gy_app_addin_added), self);

  g_signal_connect (self->extens, "extension-removed",
                    G_CALLBACK (gy_app_addin_removed), self);


  gy_app_plugin_load_all_plugins (self, engine);


}

