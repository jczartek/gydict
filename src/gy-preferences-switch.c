/* gy-preferences-switch.c
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
#define G_LOG_DOMAIN "GyPreferencesSwitch"

#include "gy-preferences-switch.h"

struct _GyPreferencesSwitch
{
  GtkEventBox  parent;

  GtkBox      *controls_box;
  GtkLabel    *description_label;
  GtkLabel    *title_label;
  GtkSwitch   *settings_switch;

  GSettings   *settings;
  gchar       *settings_scheme_key;
  GVariant    *settings_scheme_value;

  guint        in_widget : 1;
};

G_DEFINE_TYPE (GyPreferencesSwitch, gy_preferences_switch, GTK_TYPE_EVENT_BOX)

enum {
  PROP_0,
  PROP_TITLE,
  PROP_DESCRIPTION,
  PROP_SETTINGS,
  PROP_SETTINGS_SCHEMA_KEY,
  PROP_SETTINGS_SCHEME_VALUE,
  PROP_SIZE_GROUP,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

GyPreferencesSwitch *
gy_preferences_switch_new (void)
{
  return g_object_new (GY_TYPE_PREFERENCES_SWITCH, NULL);
}

static void
gy_preferences_switch_update_settings (GyPreferencesSwitch *self)
{
  g_return_if_fail (GY_IS_PREFERENCES_SWITCH (self));

  if ((self->settings != NULL) && (self->settings_scheme_key != NULL))
    {
      GSimpleActionGroup *group;
      GAction            *action;
      gchar              *name = NULL;

      action = g_settings_create_action (self->settings, self->settings_scheme_key);
      group = g_simple_action_group_new ();
      g_action_map_add_action( G_ACTION_MAP (group), G_ACTION (action));
      gtk_widget_insert_action_group (GTK_WIDGET (self),
                                      "settings",
                                      G_ACTION_GROUP (group));
      g_object_unref (action);
      name = g_strdup_printf ("settings.%s", self->settings_scheme_key);

      gtk_actionable_set_action_name (GTK_ACTIONABLE (self->settings_switch), name);
      g_free (name);
    }
}

static void
gy_preferences_switch_set_settings (GyPreferencesSwitch *self,
                                    GSettings           *settings)
{
  g_return_if_fail (GY_IS_PREFERENCES_SWITCH (self));
  g_return_if_fail (G_IS_SETTINGS (settings));

  if (g_set_object (&self->settings, settings))
    gy_preferences_switch_update_settings (self);
}

static void
gy_preferences_switch_set_settings_schema_value (GyPreferencesSwitch *self,
                                                 GVariant             *variant)
{
  g_return_if_fail (GY_IS_PREFERENCES_SWITCH (self));

  gtk_actionable_set_action_target_value (GTK_ACTIONABLE(self->settings_switch),
                                          variant);
}

static void
gy_preferences_switch_set_settings_schema_key (GyPreferencesSwitch *self,
                                               const gchar         *settings_scheme_key)
{
  g_return_if_fail (GY_IS_PREFERENCES_SWITCH (self));

  if (self->settings_scheme_key != settings_scheme_key)
    {
      if (self->settings_scheme_key)
        g_free (self->settings_scheme_key);
      self->settings_scheme_key = g_strdup (settings_scheme_key);
      gy_preferences_switch_update_settings (self);
    }
}

static void
gy_preferences_switch_set_size_group (GyPreferencesSwitch *self,
                                      GtkSizeGroup        *group)
{
  g_return_if_fail (GY_IS_PREFERENCES_SWITCH (self));
  g_return_if_fail (!group || GTK_IS_SIZE_GROUP (group));

  if (group != NULL)
    gtk_size_group_add_widget (group, GTK_WIDGET (self->controls_box));
}

static void
gy_preferences_switch_finalize (GObject *object)
{
  GyPreferencesSwitch *self = (GyPreferencesSwitch *)object;

  G_OBJECT_CLASS (gy_preferences_switch_parent_class)->finalize (object);
}

static void
gy_preferences_switch_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  GyPreferencesSwitch *self = GY_PREFERENCES_SWITCH (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_preferences_switch_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GyPreferencesSwitch *self = GY_PREFERENCES_SWITCH (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      gtk_label_set_label (self->title_label, g_value_get_string (value));
      break;
    case PROP_DESCRIPTION:
      gtk_label_set_label (self->description_label, g_value_get_string (value));
      break;
    case PROP_SETTINGS:
      gy_preferences_switch_set_settings (self, g_value_get_object (value));
      break;
    case PROP_SETTINGS_SCHEMA_KEY:
      gy_preferences_switch_set_settings_schema_key (self, g_value_get_string (value));
      break;
    case PROP_SETTINGS_SCHEME_VALUE:
      gy_preferences_switch_set_settings_schema_value (self, g_value_get_variant(value));
      break;
    case PROP_SIZE_GROUP:
      gy_preferences_switch_set_size_group (self, g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static gboolean
gy_preferences_switch_enter_notify (GtkWidget        *widget,
                                    GdkEventCrossing *crossing)
{
  GyPreferencesSwitch *self = (GyPreferencesSwitch *)widget;
  gboolean             ret  = FALSE;

  g_assert (GY_IS_PREFERENCES_SWITCH (self));

  self->in_widget = TRUE;
  gtk_widget_queue_draw (widget);

  return ret;
}

static gboolean
gy_preferences_switch_leave_notify (GtkWidget        *widget,
                                    GdkEventCrossing *crossing)
{
  GyPreferencesSwitch *self = (GyPreferencesSwitch *)widget;
  gboolean             ret  = FALSE;

  g_assert (GY_IS_PREFERENCES_SWITCH (self));

  self->in_widget = FALSE;
  gtk_widget_queue_draw (widget);

  return ret;
}

static gboolean
gy_preferences_switch_button_release_event (GtkWidget      *widget,
                                            GdkEventButton *button)
{
  GyPreferencesSwitch *self = (GyPreferencesSwitch *)widget;
  gboolean             ret;

  g_assert (GY_IS_PREFERENCES_SWITCH (self));

  ret = GTK_WIDGET_CLASS (gy_preferences_switch_parent_class)->button_release_event (widget, button);

  if ((ret == FALSE) && (self->in_widget == TRUE))
    {
      if (button->button == GDK_BUTTON_PRIMARY)
        {
          g_signal_emit_by_name (self->settings_switch, "activate");
          ret = TRUE;
        }
    }
  return ret;
}

static gboolean
gy_preferences_switch_draw (GtkWidget *widget,
                            cairo_t   *cr)
{
  GyPreferencesSwitch *self = (GyPreferencesSwitch *)widget;
  GtkStyleContext     *style_context;
  GtkStateFlags        flags;
  gboolean             ret = FALSE;

  g_assert (GY_IS_PREFERENCES_SWITCH (self));

  style_context = gtk_widget_get_style_context (widget);

  gtk_style_context_save (style_context);

  if (self->in_widget)
    {
      flags = gtk_style_context_get_state (style_context);
      gtk_style_context_set_state (style_context, flags | GTK_STATE_FLAG_PRELIGHT);
    }
  ret = GTK_WIDGET_CLASS (gy_preferences_switch_parent_class)-> draw (widget, cr);

  gtk_style_context_restore (style_context);

  return ret;
}

static void
gy_preferences_switch_class_init (GyPreferencesSwitchClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gy_preferences_switch_finalize;
  object_class->get_property = gy_preferences_switch_get_property;
  object_class->set_property = gy_preferences_switch_set_property;

  widget_class->draw = gy_preferences_switch_draw;
  widget_class->enter_notify_event = gy_preferences_switch_enter_notify;
  widget_class->leave_notify_event = gy_preferences_switch_leave_notify;
  widget_class->button_release_event = gy_preferences_switch_button_release_event;

  gParamSpecs[PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "The title of the switch.",
                         NULL,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_DESCRIPTION] =
    g_param_spec_string ("description",
                         "Description",
                         "The description for the switch.",
                         NULL,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_SETTINGS] =
    g_param_spec_object ("settings",
                         "Settings",
                         "The GSettings for the setting.",
                         G_TYPE_SETTINGS,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_SETTINGS_SCHEMA_KEY] =
    g_param_spec_string ("settings-schema-key",
                         "Settings Schema Key",
                         "The settings schema key.",
                         NULL,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_SETTINGS_SCHEME_VALUE] =
  g_param_spec_variant ("settings-schema-value",
                        "Settings Schema Value",
                        "An action-target for the settings action.",
                        G_VARIANT_TYPE_ANY,
                        NULL,
                        (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  gParamSpecs[PROP_SIZE_GROUP] =
    g_param_spec_object ("size-group",
                         "Size Group",
                         "The sizing group for the control.",
                         GTK_TYPE_SIZE_GROUP,
                         (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-preferences-switch.ui");
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesSwitch, description_label);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesSwitch, title_label);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesSwitch, settings_switch);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesSwitch, controls_box);

}

static void
gy_preferences_switch_init (GyPreferencesSwitch *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
  gtk_widget_add_events (GTK_WIDGET (self),
                         (GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON_RELEASE_MASK |
                          GDK_ENTER_NOTIFY_MASK |
                          GDK_LEAVE_NOTIFY_MASK));
}
