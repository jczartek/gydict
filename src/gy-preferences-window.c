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
#include "gy-preferences-page-dicts.h"

struct _GyPreferencesWindow
{
  GtkWindow     parent;

  GtkHeaderBar *right_header_bar;
  GtkStack     *stack;
  GtkWidget    *visible_child;
  GBinding     *title_binding;

  guint         destroyed : 1;
};

G_DEFINE_TYPE (GyPreferencesWindow, gy_preferences_window, GTK_TYPE_WINDOW)

enum {
  PROP_0,
  LAST_PROP
};

enum {
  CLOSE,
  LAST_SIGNAL
};

static GParamSpec *gParamSpecs [LAST_PROP];
static guint       gSignals    [LAST_SIGNAL];

GyPreferencesWindow *
gy_preferences_window_new (void)
{
  return g_object_new (GY_TYPE_PREFERENCES_WINDOW, NULL);
}

static void
gy_preferences_window_section_changed (GtkStack            *stack,
                                       GParamSpec          *pspec,
                                       GyPreferencesWindow *self)
{
  GtkWidget *visible_child;

  g_return_if_fail (GTK_IS_STACK (stack));
  g_return_if_fail (GY_IS_PREFERENCES_WINDOW (self));

  if (self->destroyed)
    return;

  visible_child = gtk_stack_get_visible_child (stack);

  if (GY_IS_PREFERENCES_PAGE (visible_child))
    {
      const gchar *title = gy_preferences_page_get_title (GY_PREFERENCES_PAGE (visible_child));
      gtk_header_bar_set_title (self->right_header_bar, title);
    }
}

static void
gy_preferences_window_finalize (GObject *object)
{
  GyPreferencesWindow *self = (GyPreferencesWindow *)object;

  G_OBJECT_CLASS (gy_preferences_window_parent_class)->finalize (object);
}

static void
gy_preferences_window_close (GyPreferencesWindow *self)
{
  g_return_if_fail (GY_IS_PREFERENCES_WINDOW (self));

  gtk_window_close (GTK_WINDOW (self));
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
gy_preferences_window_constructed (GObject *object)
{
  GyPreferencesWindow *self = GY_PREFERENCES_WINDOW (object);

  G_OBJECT_CLASS (gy_preferences_window_parent_class)->constructed (object);

  g_signal_connect (self->stack, "notify::visible-child",
                    G_CALLBACK (gy_preferences_window_section_changed), self);
}

static void
gy_preferences_window_destroy (GtkWidget *widget)
{
  GyPreferencesWindow *self = GY_PREFERENCES_WINDOW (widget);

  g_return_if_fail (GY_IS_PREFERENCES_WINDOW (self));

  self->destroyed = TRUE;

  GTK_WIDGET_CLASS (gy_preferences_window_parent_class)->destroy (widget);
}
static void
gy_preferences_window_class_init (GyPreferencesWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkBindingSet *binding_set;

  object_class->constructed = gy_preferences_window_constructed;
  object_class->finalize = gy_preferences_window_finalize;
  object_class->get_property = gy_preferences_window_get_property;
  object_class->set_property = gy_preferences_window_set_property;

  widget_class->destroy = gy_preferences_window_destroy;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-preferences-window.ui");
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesWindow, right_header_bar);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesWindow, stack);

  gSignals [CLOSE] =
    g_signal_new_class_handler ("close",
                                G_TYPE_FROM_CLASS (klass),
                                (G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION),
                                G_CALLBACK (gy_preferences_window_close),
                                NULL, NULL, NULL,
                                G_TYPE_NONE, 0);
  binding_set = gtk_binding_set_by_class (klass);
  gtk_binding_entry_add_signal (binding_set, GDK_KEY_Escape, 0, "close", 0);
}

static void
gy_preferences_window_init (GyPreferencesWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
