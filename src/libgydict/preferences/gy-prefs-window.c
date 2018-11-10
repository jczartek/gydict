/* gy-prefs-window.c
 *
 * Copyright 2018 Jakub Czartek <kuba@linux.pl>
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

#include "gy-prefs-window.h"

struct _GyPrefsWindow
{
  GtkWindow parent_instance;
};

G_DEFINE_TYPE (GyPrefsWindow, gy_prefs_window, GTK_TYPE_WINDOW)

GyPrefsWindow *
gy_prefs_window_new (void)
{
  return g_object_new (GY_TYPE_PREFS_WINDOW, NULL);
}

static void
gy_prefs_window_class_init (GyPrefsWindowClass *klass)
{
  GtkWidgetClass *wclass = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (wclass, "/org/gtk/gydict/gy-prefs-window.ui");
}

static void
gy_prefs_window_init (GyPrefsWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
