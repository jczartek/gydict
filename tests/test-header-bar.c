/* test-header-bar.c
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

#include <gtk/gtk.h>
#include <gydict.h>

gint
main (gint   argc,
      gchar *argv[])
{
  GtkWidget *window;
  GyHeaderBar *header_bar;

  gtk_init (&argc, &argv);
  g_type_ensure (GY_TYPE_LEX_SEARCH_BOX);
  g_resources_register (gy_get_resource ());
  header_bar = gy_header_bar_new ();
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 500, 300);
  gtk_window_set_titlebar (GTK_WINDOW (window), GTK_WIDGET (header_bar));
  gtk_widget_show_all (window);

  g_signal_connect (window, "delete-event", G_CALLBACK (gtk_main_quit), NULL);

  gtk_main ();
  return 0;
}
