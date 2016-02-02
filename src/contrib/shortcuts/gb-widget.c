/* gb-widget.c
 *
 * Copyright (C) 2014 Christian Hergert <christian.hergert@mongodb.com>
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

#include "gb-widget.h"

void
gb_widget_add_style_class (gpointer     widget,
                           const gchar *class_name)
{
  GtkStyleContext *context;

  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (class_name);

  context = gtk_widget_get_style_context (widget);
  gtk_style_context_add_class (context, class_name);
}

