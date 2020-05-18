/* gy-window-settings.c
 *
 * Copyright (C) 2017 Jakub Czartek <kuba@linux.pl>
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

/*
 * NOTE: This code were taken from ide-window-settings.c
 * Please see: https://github.com/chergert/gnome-builder/blob/master/libide/util/ide-window-settings.c
 */

#include <dazzle.h>
#include "gy-window.h"
#include "gy-window-settings.h"

#define SAVE_TIMEOUT_SECS    1

static GSettings *settings;

static gboolean
gy_window_settings__window_save_settings_cb (gpointer data)
{
  GtkWindow    *window    = (GtkWindow *) data;
  GdkRectangle  geom      = { 0 };
  gboolean      maximized = FALSE;

  g_assert (GTK_IS_WINDOW (window));
  g_assert (G_IS_SETTINGS (settings));

  g_object_set_data (G_OBJECT (window), "SETTINGS_HANDLER_ID", NULL);

  gtk_window_get_size (window, &geom.width, &geom.height);
  gtk_window_get_position (window, &geom.x, &geom.y);
  maximized = gtk_window_is_maximized (window);

  g_settings_set (settings, "window-size", "(ii)", geom.width, geom.height);
  g_settings_set (settings, "window-position", "(ii)", geom.x, geom.y);
  g_settings_set_boolean (settings, "window-maximized", maximized);

  return G_SOURCE_REMOVE;
}

static gboolean
gy_window_settings__window_configure_event (GtkWindow         *window,
                                            GdkEventConfigure *event)
{
  guint handler;

  g_assert (GTK_IS_WINDOW (window));
  g_assert (event != NULL);
  g_assert (G_IS_SETTINGS (settings));

  handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (window), "SETTINGS_HANDLER_ID"));

  if (handler == 0)
    {
      handler = g_timeout_add_seconds (SAVE_TIMEOUT_SECS,
                                       gy_window_settings__window_save_settings_cb,
                                       window);
      g_object_set_data (G_OBJECT (window), "SETTINGS_HANDLER_ID", GINT_TO_POINTER (handler));
    }

  return GDK_EVENT_PROPAGATE;
}

static void
gy_window_settings__window_realize (GtkWindow *window)
{
  GdkRectangle geom      = { 0 };
  gboolean     maximized = FALSE;

  g_assert (GTK_IS_WINDOW (window));
  g_assert (G_IS_SETTINGS (settings));

  g_settings_get (settings, "window-position", "(ii)", &geom.x, &geom.y);
  g_settings_get (settings, "window-size", "(ii)", &geom.width, &geom.height);
  g_settings_get (settings, "window-maximized", "b", &maximized);

  gtk_window_set_default_size (window, geom.width, geom.height);

  gtk_window_move (window, geom.x, geom.y);

  if (maximized)
    gtk_window_maximize (window);

  DzlDockBin* dockbin = gy_window_get_dockbin (GY_WINDOW (window));
  GtkWidget * edge = dzl_dock_bin_get_left_edge (dockbin);

  g_settings_bind (settings, "left-panel-visible", edge, "reveal-child", G_SETTINGS_BIND_DEFAULT);
  g_settings_bind (settings, "left-panel-position", edge, "position", G_SETTINGS_BIND_DEFAULT);

  edge = dzl_dock_bin_get_right_edge (dockbin);

  g_settings_bind (settings, "right-panel-visible", edge, "reveal-child", G_SETTINGS_BIND_DEFAULT);
  g_settings_bind (settings, "right-panel-position", edge, "position", G_SETTINGS_BIND_DEFAULT);


}

static void
gy_window_settings__window_destroy (GtkWindow *window)
{
    guint handler;

  g_assert (GTK_IS_WINDOW (window));
  g_assert (G_IS_SETTINGS (settings));

  handler = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (window), "SETTINGS_HANDLER_ID"));

  if (handler != 0)
    {
      g_source_remove (handler);
      g_object_set_data (G_OBJECT (window), "SETTINGS_HANDLER_ID", NULL);
    }

  g_signal_handlers_disconnect_by_func (window,
                                        G_CALLBACK (gy_window_settings__window_configure_event),
                                        NULL);

  g_signal_handlers_disconnect_by_func (window,
                                        G_CALLBACK (gy_window_settings__window_destroy),
                                        NULL);

  g_signal_handlers_disconnect_by_func (window,
                                        G_CALLBACK (gy_window_settings__window_realize),
                                        NULL);

  g_object_unref (settings);
}

void
_gy_window_settings_register (GtkWindow *window)
{
  if (settings == NULL)
    {
      settings = g_settings_new ("org.gtk.gydict");
      g_object_add_weak_pointer (G_OBJECT (settings), (gpointer *) &settings);
    }
  else
    {
      g_object_ref (settings);
    }

  g_signal_connect (window, "configure-event",
                    G_CALLBACK (gy_window_settings__window_configure_event), NULL);

  g_signal_connect (window, "destroy",
                    G_CALLBACK (gy_window_settings__window_destroy), NULL);

  g_signal_connect (window, "realize",
                    G_CALLBACK (gy_window_settings__window_realize), NULL);
}
