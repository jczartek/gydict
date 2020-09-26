/* gy-greeting-view.c
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

#include "gy-greeting-view.h"

struct _GyGreetingView
{
  GtkBin parent_instance;
};

G_DEFINE_TYPE (GyGreetingView, gy_greeting_view, GTK_TYPE_BIN)

static void
gy_greeting_view_class_init (GyGreetingViewClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);


  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-greeting-view.ui");
}

static void
gy_greeting_view_init (GyGreetingView *self)
{
  gtk_widget_init_template ((GtkWidget *) self);
}
