/* gy-pref-plugin-row.c
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
#include <libpeas/peas.h>
#include "gy-prefs-plugin-row.h"
#include "gy-prefs-configurable.h"

struct _GyPrefsPluginRow
{
  GtkBin parent_instance;

  /*Props*/
  gchar *name;
  gchar *description;
  gchar *schema_id;
  gchar *path;
  gchar *key;
  gboolean has_configure_widget;

  /*Private Field*/
  GSettings *settings;
  GtkWidget *loaded_switch;
  PeasExtension *exten;
};

G_DEFINE_TYPE (GyPrefsPluginRow, gy_prefs_plugin_row, GTK_TYPE_BIN)

enum {
  PROP_0,
  PROP_NAME,
  PROP_DESCRIPTION,
  PROP_SCHEMA_ID,
  PROP_PATH,
  PROP_KEY,
  PROP_HAS_CONFIGURE_WIDGET,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

GyPrefsPluginRow *
gy_prefs_plugin_row_new (const gchar *name,
                         const gchar *description,
                         const gchar *schema_id,
                         const gchar *key,
                         const gchar *path)
{
  return g_object_new (GY_TYPE_PREFS_PLUGIN_ROW,
                       "name", name,
                       "description", description,
                       "schema_id", schema_id,
                       "key", key,
                       "path", path,
                       NULL);
}

static void
gy_prefs_plugin_row_finalize (GObject *object)
{
  GyPrefsPluginRow *self = (GyPrefsPluginRow *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->description, g_free);
  g_clear_pointer (&self->key, g_free);
  g_clear_pointer (&self->schema_id, g_free);
  g_clear_pointer (&self->path, g_free);
  g_clear_object (&self->settings);
  g_clear_object (&self->exten);

  G_OBJECT_CLASS (gy_prefs_plugin_row_parent_class)->finalize (object);
}

static void
gy_prefs_plugin_row_bind_settings_with_loaded_switch (GyPrefsPluginRow *self)
{
  self->settings = g_settings_new_with_path (self->schema_id, self->path);
  g_settings_bind (self->settings,
                   self->key,
                   self->loaded_switch,
                   "active",
                   G_SETTINGS_BIND_DEFAULT);
}

static void
gy_prefs_plugin_row_create_configurable_extension (GyPrefsPluginRow *self)
{
  PeasEngine *engine = peas_engine_get_default ();
  const GList *plugins = peas_engine_get_plugin_list (engine);

  for (GList *info = (GList *)plugins; info != NULL; info = info->next)
    {
      if (g_strcmp0 (self->name, peas_plugin_info_get_name(info->data)) == 0)
        {
          self->exten = peas_engine_create_extension (engine, info->data, GY_TYPE_PREFS_CONFIGURABLE, NULL);
        }
    }

  g_object_set (self, "has-configure-widget", !!self->exten, NULL);
}

static void
gy_prefs_plugin_row_constructed (GObject *object)
{
  GyPrefsPluginRow *self = (GyPrefsPluginRow *)object;

  G_OBJECT_CLASS (gy_prefs_plugin_row_parent_class)->constructed (object);

  gy_prefs_plugin_row_bind_settings_with_loaded_switch (self);
  gy_prefs_plugin_row_create_configurable_extension (self);
}

static void
gy_prefs_plugin_row_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GyPrefsPluginRow *self = GY_PREFS_PLUGIN_ROW (object);

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_DESCRIPTION:
      g_value_set_string (value, self->description);
      break;

    case PROP_SCHEMA_ID:
      g_value_set_string (value, self->schema_id);
      break;

    case PROP_PATH:
      g_value_set_string (value, self->path);
      break;

    case PROP_KEY:
      if (self->key != NULL)
        g_value_set_string (value, self->key);
      break;

    case PROP_HAS_CONFIGURE_WIDGET:
      g_value_set_boolean (value, self->has_configure_widget);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_prefs_plugin_row_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GyPrefsPluginRow *self = GY_PREFS_PLUGIN_ROW (object);

  switch (prop_id)
    {
    case PROP_NAME:
      self->name = g_value_dup_string (value);
      break;

    case PROP_DESCRIPTION:
      self->description = g_value_dup_string (value);
      break;

    case PROP_SCHEMA_ID:
      self->schema_id = g_value_dup_string (value);
      break;

    case PROP_PATH:
      self->path = g_value_dup_string (value);
      break;

    case PROP_KEY:
      self->key = g_value_dup_string (value);
      break;

    case PROP_HAS_CONFIGURE_WIDGET:
      self->has_configure_widget = g_value_get_boolean (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_prefs_plugin_row_class_init (GyPrefsPluginRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gy_prefs_plugin_row_finalize;
  object_class->constructed = gy_prefs_plugin_row_constructed;
  object_class->get_property = gy_prefs_plugin_row_get_property;
  object_class->set_property = gy_prefs_plugin_row_set_property;

  properties[PROP_NAME] =
    g_param_spec_string ("name",
                         "Name",
                         "Name of plugin", NULL,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_NAME);

  properties[PROP_DESCRIPTION] =
    g_param_spec_string ("description",
                         "Description",
                         "Description of plugin", NULL,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_NAME);

  properties [PROP_SCHEMA_ID] =
    g_param_spec_string ("schema-id",
                         "Schema Id",
                         "Schema Id",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_PATH] =
    g_param_spec_string ("path",
                         "Path",
                         "Path",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties[PROP_KEY] =
    g_param_spec_string ("key",
                         "Key",
                         "Key", NULL,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  properties[PROP_HAS_CONFIGURE_WIDGET] =
    g_param_spec_boolean ("has-configure-widget",
                          "Has configure widget",
                          "The prop determines if the loaded plugin implements the interface GyPrefsConfigurable",
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-prefs-plugin-row.ui");
  gtk_widget_class_bind_template_child (widget_class, GyPrefsPluginRow, loaded_switch);
}

static void
gy_prefs_plugin_row_init (GyPrefsPluginRow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
