/* gy-shortcuts-window.c
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
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

#include <glib/gi18n.h>

#include "gy-shortcuts-window.h"

struct _GyShortcutsWindow
{
  GtkShortcutsWindow __parent__;
};

G_DEFINE_TYPE (GyShortcutsWindow, gy_shortcuts_window, GTK_TYPE_SHORTCUTS_WINDOW)

static void
gy_shortcuts_window_class_init (GyShortcutsWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-shortcuts-window.ui");
}

static void
gy_shortcuts_window_init (GyShortcutsWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
