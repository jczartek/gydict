/* gy-preferences-file-chooser.c
 *
 * Copyright (C) 2015 Jakub Czartek <kuba@linux.pl>
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
#define G_LOG_DOMAIN "GyPreferencesFileChooser"

#include "gy-preferences-file-chooser.h"

struct _GyPreferencesFileChooser
{
  GtkEventBox  parent;

  GtkLabel             *title_label;
  GtkLabel             *description_label;
  GtkBox               *controls_box;
  GtkFileChooserButton *chooser_button;

  GSettings            *settings;
  gchar                *settings_scheme_key;
  gchar                *path;
};

G_DEFINE_TYPE (GyPreferencesFileChooser, gy_preferences_file_chooser, GTK_TYPE_EVENT_BOX);

enum {
  PROP_0,
  PROP_TITLE,
  PROP_DESCRIPTION,
  PROP_SIZE_GROUP,
  PROP_SETTINGS,
  PROP_SETTINGS_SCHEMA_KEY,
  PROP_PATH,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static void
gy_preferences_file_chooser_file_set (GtkFileChooserButton *chooser_button,
                                      gpointer              user_data)
{
}

static void
gy_preferences_file_chooser_set_size_group (GyPreferencesFileChooser *self,
                                            GtkSizeGroup             *group)
{
  g_return_if_fail (GY_IS_PREFERENCES_FILE_CHOOSER (self));
  g_return_if_fail (!group || GTK_IS_SIZE_GROUP (group));

  if (group != NULL)
    gtk_size_group_add_widget (group, GTK_WIDGET (self->chooser_button));
}

static void
gy_preferences_file_chooser_set_settings_schema_key (GyPreferencesFileChooser *self,
                                                     const gchar              *settings_scheme_key)
{
  g_return_if_fail (GY_IS_PREFERENCES_FILE_CHOOSER (self));

  if (self->settings_scheme_key != settings_scheme_key)
    {
      if (self->settings_scheme_key)
        g_free (self->settings_scheme_key);
      self->settings_scheme_key = g_strdup (settings_scheme_key);
      g_object_notify_by_pspec (G_OBJECT (self), gParamSpecs[PROP_SETTINGS_SCHEMA_KEY]);
    }
}

static void
gy_preferences_file_chooser_set_path (GyPreferencesFileChooser *self,
                                      const gchar              *path)
{
  g_return_if_fail (GY_IS_PREFERENCES_FILE_CHOOSER (self));

  if (self->path != path)
    {
      if (self->path)
        g_free (self->path);
      self->path = g_strdup (path);
      g_object_notify_by_pspec (G_OBJECT (self), gParamSpecs[PROP_PATH]);
    }
}

static void
gy_preferences_file_chooser_set_settings (GyPreferencesFileChooser *self,
                                          GSettings                *settings)
{
  g_return_if_fail (GY_IS_PREFERENCES_FILE_CHOOSER (self));
  g_return_if_fail (G_IS_SETTINGS (settings));

  if (g_set_object (&self->settings, settings))
    g_object_notify_by_pspec (G_OBJECT (self), gParamSpecs[PROP_SETTINGS]);
}

static void
gy_preferences_file_chooser_finalize (GObject *object)
{
  GyPreferencesFileChooser *self = (GyPreferencesFileChooser *)object;

  G_OBJECT_CLASS (gy_preferences_file_chooser_parent_class)->finalize (object);
}

static void
gy_preferences_file_chooser_get_property (GObject    *object,
                                          guint       prop_id,
                                          GValue     *value,
                                          GParamSpec *pspec)
{
  GyPreferencesFileChooser *self = GY_PREFERENCES_FILE_CHOOSER (object);

  switch (prop_id)
    {
    case PROP_PATH:
      g_value_set_string (value, self->path);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_preferences_file_chooser_set_property (GObject      *object,
                                          guint         prop_id,
                                          const GValue *value,
                                          GParamSpec   *pspec)
{
  GyPreferencesFileChooser *self = GY_PREFERENCES_FILE_CHOOSER (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      gtk_label_set_label (self->title_label, g_value_get_string (value));
      break;
    case PROP_DESCRIPTION:
      gtk_label_set_label (self->description_label, g_value_get_string (value));
      break;
    case PROP_SIZE_GROUP:
      gy_preferences_file_chooser_set_size_group (self, g_value_get_object (value));
      break;
    case PROP_SETTINGS:
      gy_preferences_file_chooser_set_settings (self, g_value_get_object (value));
      break;
    case PROP_SETTINGS_SCHEMA_KEY:
      gy_preferences_file_chooser_set_settings_schema_key (self, g_value_get_string (value));
      break;
    case PROP_PATH:
      gy_preferences_file_chooser_set_path (self, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_preferences_constructed (GObject *object)
{
  GyPreferencesFileChooser *self = GY_PREFERENCES_FILE_CHOOSER (object);

  G_OBJECT_CLASS (gy_preferences_file_chooser_parent_class)->constructed (object);

  g_signal_connect (self->chooser_button, "file-set",
                    G_CALLBACK (gy_preferences_file_chooser_file_set), self);
}

static void
gy_preferences_file_chooser_class_init (GyPreferencesFileChooserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = gy_preferences_constructed;
  object_class->finalize = gy_preferences_file_chooser_finalize;
  object_class->get_property = gy_preferences_file_chooser_get_property;
  object_class->set_property = gy_preferences_file_chooser_set_property;

  gParamSpecs[PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "The title of the file chooser.",
                         NULL,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_DESCRIPTION] =
    g_param_spec_string ("description",
                         "Description",
                         "The description for the file chooser.",
                         NULL,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_SIZE_GROUP] =
    g_param_spec_object ("size-group",
                         "Size Group",
                         "The sizing group for the control.",
                         GTK_TYPE_SIZE_GROUP,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_SETTINGS] =
    g_param_spec_object ("settings",
                         "Settings",
                         "The GSettings for the setting.",
                         G_TYPE_SETTINGS,
                         (G_PARAM_WRITABLE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_SETTINGS_SCHEMA_KEY] =
    g_param_spec_string ("settings-schema-key",
                         "Settings Schema Key",
                         "The settings schema key.",
                         NULL,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_PATH] =
    g_param_spec_string ("path",
                         "Path",
                         "The path for a dictionary.",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-preferences-file-chooser.ui");
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesFileChooser, title_label);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesFileChooser, description_label);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesFileChooser, controls_box);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesFileChooser, chooser_button);
}

static void
gy_preferences_file_chooser_init (GyPreferencesFileChooser *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

}
