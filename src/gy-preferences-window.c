/* gy-preferences-window.c
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
#include "gy-preferences-window.h"
#include "gy-preferences-page.h"
#include "gy-preferences-page-general.h"

struct _GyPreferencesWindow
{
  GtkWindow  parent;

  GtkWidget *right_header_bar;
};

G_DEFINE_TYPE (GyPreferencesWindow, gy_preferences_window, GTK_TYPE_WINDOW)

enum {
  PROP_0,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

GyPreferencesWindow *
gy_preferences_window_new (void)
{
  return g_object_new (GY_TYPE_PREFERENCES_WINDOW, NULL);
}

static void
gy_preferences_window_finalize (GObject *object)
{
  GyPreferencesWindow *self = (GyPreferencesWindow *)object;

  G_OBJECT_CLASS (gy_preferences_window_parent_class)->finalize (object);
}

static void
gy_preferences_window_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  GyPreferencesWindow *self = GY_PREFERENCES_WINDOW (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_preferences_window_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GyPreferencesWindow *self = GY_PREFERENCES_WINDOW (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_preferences_window_class_init (GyPreferencesWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gy_preferences_window_finalize;
  object_class->get_property = gy_preferences_window_get_property;
  object_class->set_property = gy_preferences_window_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-preferences-window.ui");
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesWindow, right_header_bar);
}

static void
gy_preferences_window_init (GyPreferencesWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
