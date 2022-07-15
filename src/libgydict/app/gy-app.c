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
/* #include "gy-app-addin.h" */
/* #include "resources/gy-resources.h" */

G_DEFINE_TYPE (GyApp, gy_app, ADW_TYPE_APPLICATION);

enum
{
  PROP_0,
  PROP_SERVICE_PROVIDER,
  N_PROPERTIES
};

GParamSpec *properties[N_PROPERTIES];

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

/* static void */
/* gy_app_register_theme_overrides (GyApp *self) */
/* { */
/*   g_autoptr(GSettings)  settings = NULL; */
/*   g_autoptr(GtkCssProvider) provider = NULL; */
/*   GtkSettings          *gtk_settings; */
/*   GdkScreen            *screen; */

/*   provider = dzl_css_provider_new ("/org/gtk/gydict/themes"); */
/*   screen = gdk_screen_get_default (); */

/*   gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), */
/*                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION); */

/*   gtk_settings = gtk_settings_get_for_screen (screen); */
/*   settings = g_settings_new ("org.gtk.gydict"); */
/*   g_settings_bind (settings,     "night-mode", */
/*                    gtk_settings, "gtk-application-prefer-dark-theme", */
/*                    G_SETTINGS_BIND_DEFAULT); */
/* } */

static void
startup (GApplication *application)
{
  GyApp *app = GY_APP (application);

  /* g_resources_register (gy_get_resource ()); */
  /* g_application_set_resource_base_path (application, "/org/gtk/gydict"); */

  /* Chain up parent's class */
  G_APPLICATION_CLASS (gy_app_parent_class)->startup (application);

  /* Setup actions */
  _gy_app_action_init (app);

  /* Setup accelerators */
  setup_accels (app);

  /* Setup theme */
  /* gy_app_register_theme_overrides (app); */

  /* Init shortcuts */
  /* _gy_app_init_shortcuts (app); */
}

static void
gy_app_activate (GApplication *app)
{
  GyApp *self = GY_APP (app);

  /* _gy_app_plugins_init_plugins (self); */

  gy_app_new_window (self);

}

static void
gy_app_shutdown (GApplication *app)
{
  GyApp *self = GY_APP (app);

  /* g_clear_pointer (&self->plugin_settings, g_hash_table_destroy); */
  /* g_clear_pointer (&self->plugin_gresources, g_hash_table_destroy); */
  /* g_clear_object (&self->extens); */
  g_clear_object (&self->service_provider);

  G_APPLICATION_CLASS (gy_app_parent_class)->shutdown (app);
}

static void
gy_app_init (GyApp *self)
{
  g_set_application_name ("Gydict");

  /* self->plugin_settings = g_hash_table_new_full (g_str_hash, g_str_equal, */
  /*                                              g_free, g_object_unref); */
  /* self->plugin_gresources = g_hash_table_new_full (g_str_hash, g_str_equal, */
  /*                                                  g_free, (GDestroyNotify) g_resource_unref); */

  self->service_provider = gy_service_provider_new();
}


static void
gy_app_get_property (GObject    *object,
                     guint       prop_id,
                     GValue     *value,
                     GParamSpec *pspec)
{
  GyApp *self = GY_APP (object);

  switch (prop_id)
    {
    case PROP_SERVICE_PROVIDER:
      g_value_take_object (value, self->service_provider);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static void
gy_app_class_init (GyAppClass *klass)
{
  GObjectClass *obj_class = G_OBJECT_CLASS (klass);
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  obj_class->get_property = gy_app_get_property;

  app_class->startup = startup;
  app_class->activate = gy_app_activate;
  app_class->shutdown = gy_app_shutdown;

  properties[PROP_SERVICE_PROVIDER] =
    g_param_spec_object ("service-provider",
                         "Service provider",
                         "",
                         GY_TYPE_SERVICE_PROVIDER,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties (obj_class, N_PROPERTIES, properties);
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

/**
 * gy_app_get_service_provider:
 * @self: object of the application
 *
 * Returns: (transfer none): the service provider
 */
GyServiceProvider *
gy_app_get_service_provider(GyApp *self)
{
  g_return_val_if_fail(GY_IS_APP (self), NULL);

  return self->service_provider;
};

