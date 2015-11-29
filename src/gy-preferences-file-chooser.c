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

  GtkLabel    *title_label;
  GtkLabel    *description_label;
};

G_DEFINE_TYPE (GyPreferencesFileChooser, gy_preferences_file_chooser, GTK_TYPE_EVENT_BOX);

enum {
  PROP_0,
  PROP_TITLE,
  PROP_DESCRIPTION,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

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
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_preferences_file_chooser_class_init (GyPreferencesFileChooserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

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

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-preferences-file-chooser.ui");
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesFileChooser, title_label);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesFileChooser, description_label);
}

static void
gy_preferences_file_chooser_init (GyPreferencesFileChooser *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
