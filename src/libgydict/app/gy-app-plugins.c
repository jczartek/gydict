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

static void
gy_app_load_plugin_resources (GyApp          *self,
                              PeasEngine     *engine,
                              PeasPluginInfo *plugin_info)
{
  g_autofree gchar *gresources_path = NULL;
  g_autofree gchar *gresources_basename = NULL;

  const gchar *module_dir;
  const gchar *module_name;

  g_assert (GY_IS_APP (self));
  g_assert (plugin_info != NULL);
  g_assert (PEAS_IS_ENGINE (engine));

  module_dir = peas_plugin_info_get_module_dir (plugin_info);
  module_name = peas_plugin_info_get_module_name (plugin_info);
  gresources_basename = g_strdup_printf ("%s.gresource", module_name);
  gresources_path = g_build_filename (module_dir, gresources_basename, NULL);

  if (g_file_test (gresources_path, G_FILE_TEST_IS_REGULAR))
    {
      g_autoptr (GError) error = NULL;
      GResource *resource;

      resource = g_resource_load (gresources_path, &error);

      if (resource == NULL)
        {
          g_warning ("Failed to load gresources: %s", error->message);
          return;
        }

      g_hash_table_insert (self->plugin_gresources, g_strdup (module_name), resource);
      g_resources_register (resource);
    }
}

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

void
_gy_app_plugins_init_plugins (GyApp *self)
{
  PeasEngine *engine = peas_engine_get_default ();
  g_autofree gchar *local_plugins_dir = NULL;
  GError *error = NULL;

  peas_engine_add_search_path (engine, PACKAGE_LIBDIR"/plugins", PACKAGE_DATADIR"/plugins");

  local_plugins_dir = g_build_filename (g_get_home_dir (), ".local", "share", "gydict", "plugins", NULL);

  peas_engine_add_search_path (engine, local_plugins_dir, local_plugins_dir);

  g_irepository_prepend_search_path (PACKAGE_LIBDIR"/girepository-1.0");

   if (!g_irepository_require (NULL, "Gtk", "3.0", 0, &error) ||
      !g_irepository_require (NULL, "Gio", "2.0", 0, &error) ||
      !g_irepository_require (NULL, "GLib", "2.0", 0, &error) ||
      !g_irepository_require (NULL, "Dazzle", "1.0", 0, &error) ||
      !g_irepository_require (NULL, "Gydict", "1.0", 0, &error))
    g_critical ("Cannot enable Python 3 plugins: %s", error->message);
  else
    peas_engine_enable_loader (engine, "python3");

  peas_engine_rescan_plugins (engine);

  const GList *plugs = peas_engine_get_plugin_list (engine);

  for (const GList *iter = plugs; iter; iter = iter->next)
    {
      const gchar *name;
      g_autofree gchar *path = NULL;

      name = peas_plugin_info_get_module_name (iter->data);
      path = g_strdup_printf ("/org/gtk/gydict/plugins/%s/", name);

      GSettings *settings = g_settings_new_with_path ("org.gtk.gydict.plugin", path);
      g_hash_table_insert (self->plugin_settings, g_strdup (name), settings);


      if (peas_plugin_info_get_external_data (iter->data, "Has-Resources"))
        gy_app_load_plugin_resources (self, engine, iter->data);

      g_signal_connect (settings, "changed",
                        G_CALLBACK (gy_app_settings_changed), iter->data);

      gboolean enabled = g_settings_get_boolean (settings, "enabled");
      if (!peas_plugin_info_is_loaded (iter->data) && enabled)
        peas_engine_load_plugin (engine, iter->data);
    }
}

