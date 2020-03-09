/* gy-app.c
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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
 */

#include "config.h"
#include <glib/gi18n-lib.h>
#include "gy-app-private.h"
#include "gy-app.h"
#include "gy-app-addin.h"
#include "window/gy-window.h"
#include "preferences/gy-prefs-window.h"
#include "resources/gy-resources.h"

G_DEFINE_TYPE (GyApp, gy_app, DZL_TYPE_APPLICATION);

static void
setup_accels (GyApp *self)
{
  static const struct {gchar *action; gchar *accel_key;} accels_key[] = {
      {"app.quit", "<ctrl>q"},
      {"app.new-window", "<ctrl>n"},
      {"win.print", "<ctrl>p"},
      {"win.close", "<ctrl>w"},
      {"win.clip", "<ctrl>m"},
      {"win.gear-menu", "F10"},
      {"dockbin.top-visible", "<ctrl>f"},
      {"dockbin.left-visible", "F9"},
      {NULL, NULL}
  };

  for (gint i = 0; accels_key[i].action; i++)
    {
      const gchar *accels[2] = {accels_key[i].accel_key, NULL};
      gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                             accels_key[i].action,
                                             accels);
    }
}

static void
gy_app_register_theme_overrides (GyApp *self)
{
  g_autoptr(GSettings)  settings = NULL;
  g_autoptr(GtkCssProvider) provider = NULL;
  GtkSettings          *gtk_settings;
  GdkScreen            *screen;

  provider = dzl_css_provider_new ("/org/gtk/gydict/themes");
  screen = gdk_screen_get_default ();

  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  gtk_settings = gtk_settings_get_for_screen (screen);
  settings = g_settings_new ("org.gtk.gydict");
  g_settings_bind (settings,     "night-mode",
                   gtk_settings, "gtk-application-prefer-dark-theme",
                   G_SETTINGS_BIND_DEFAULT);
}

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

static void
gy_app_initailize_plugins (GyApp *app)
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
      g_hash_table_insert (app->plugin_settings, g_strdup (name), settings);


      if (peas_plugin_info_get_external_data (iter->data, "Has-Resources"))
        gy_app_load_plugin_resources (app, engine, iter->data);

      g_signal_connect (settings, "changed",
                        G_CALLBACK (gy_app_settings_changed), iter->data);

      gboolean enabled = g_settings_get_boolean (settings, "enabled");
      if (!peas_plugin_info_is_loaded (iter->data) && enabled)
        peas_engine_load_plugin (engine, iter->data);
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

static void
startup (GApplication *application)
{
  GyApp *app = GY_APP (application);
  PeasEngine *engine = peas_engine_get_default ();

  g_resources_register (gy_get_resource ());
  g_application_set_resource_base_path (application, "/org/gtk/gydict");

  /* Chain up parent's class */
  G_APPLICATION_CLASS (gy_app_parent_class)->startup (application);

  /* Setup actions */
  _gy_app_action_init (app);

  /* Setup accelerators */
  setup_accels (app);

  /* Setup theme */
  gy_app_register_theme_overrides (app);

  /* Initialize plugins */
  gy_app_initailize_plugins (app);

  /* Init shortcuts */
  _gy_app_init_shortcuts (app);

  app->extens = peas_extension_set_new (engine, GY_TYPE_APP_ADDIN, NULL);

  g_signal_connect (app->extens, "extension-added",
                    G_CALLBACK (gy_app_addin_added), app);

  g_signal_connect (app->extens, "extension-removed",
                    G_CALLBACK (gy_app_addin_removed), app);
}

static void
gy_app_activate (GApplication *app)
{
  GyApp *self = GY_APP (app);

  gy_app_new_window (self);

  if (self->extens != NULL)
    peas_extension_set_foreach (self->extens, gy_app_addin_added, self);
}

static void
gy_app_shutdown (GApplication *app)
{
  GyApp *self = GY_APP (app);

  g_clear_pointer (&self->plugin_settings, g_hash_table_destroy);
  g_clear_pointer (&self->plugin_gresources, g_hash_table_destroy);
  g_clear_object (&self->extens);

  G_APPLICATION_CLASS (gy_app_parent_class)->shutdown (app);
}

static void
gy_app_init (GyApp *self)
{
  g_set_application_name ("Gydict");

  self->plugin_settings = g_hash_table_new_full (g_str_hash, g_str_equal,
                                               g_free, g_object_unref);
  self->plugin_gresources = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   g_free, (GDestroyNotify) g_resource_unref);
}

static void
gy_app_class_init (GyAppClass *klass)
{
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  app_class->startup = startup;
  app_class->activate = gy_app_activate;
  app_class->shutdown = gy_app_shutdown;
}

GyApp *
gy_app_new(void)
{
  GyApp* application = NULL;

  application = g_object_new (GY_TYPE_APP,
                              "application-id",   "org.gtk.gydict",
                              "flags",            G_APPLICATION_FLAGS_NONE,
                              "register-session", TRUE, NULL);

  g_assert (GY_IS_APP (application));

  _gy_app_cmd_line_add_options (application);

  return application;
}

void
gy_app_new_window (GyApp *self)
{
  g_return_if_fail (GY_IS_APP (self));

  g_action_group_activate_action (G_ACTION_GROUP (self),
                                  "new-window",NULL);
}
