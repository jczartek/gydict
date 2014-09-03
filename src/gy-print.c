/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <gtk/gtk.h>
#include <math.h>
#include "gy-window.h"
#include "gy-print.h"
#include "gy-print-compositor.h"


static void begin_print (GtkPrintOperation *operation,
			 GtkPrintContext   *context,
			 gpointer 	    data);

static void draw_page (GtkPrintOperation *operation,
		       GtkPrintContext   *context,
		       gint 		  page_nr,
		       gpointer           data);

static void end_print (GtkPrintOperation *operation,
		       GtkPrintContext   *context,
		       gpointer           data);
void
gy_print_do_printing (GSimpleAction *action G_GNUC_UNUSED,
		      GVariant      *parameter G_GNUC_UNUSED,
		      gpointer       data)
{
  GtkPrintOperation *operation;
  GtkPrintSettings *settings;
  gchar *uri, *ext;
  const gchar *dir;
  GError *error = NULL;
  GyWindow *window = GY_WINDOW(data);
  GtkWidget *text_view;

  text_view = gy_window_get_text_view (window);
  GyPrintCompositor *compositor = gy_print_compositor_new_from_view (GTK_TEXT_VIEW (text_view));

  g_return_if_fail (GY_IS_WINDOW (window));
  g_return_if_fail (GY_IS_PRINT_COMPOSITOR (compositor));

  operation = gtk_print_operation_new ();
    
  g_signal_connect (G_OBJECT (operation), "begin-print",
		    G_CALLBACK (begin_print), compositor);
  g_signal_connect (G_OBJECT (operation), "draw-page",
		    G_CALLBACK (draw_page), compositor);
  g_signal_connect (G_OBJECT (operation), "end-print",
		    G_CALLBACK (end_print), compositor);
  
  gtk_print_operation_set_use_full_page (operation, FALSE);
  gtk_print_operation_set_unit (operation, GTK_UNIT_POINTS);
  gtk_print_operation_set_embed_page_setup (operation, TRUE);

  settings = gtk_print_settings_new ();
  dir = g_get_user_special_dir (G_USER_DIRECTORY_DOCUMENTS);

  if (!dir)
    dir = g_get_home_dir ();

  if (g_strcmp0 (gtk_print_settings_get (settings, GTK_PRINT_SETTINGS_OUTPUT_FILE_FORMAT), "ps") == 0)
  {
    ext = ".ps";
  }
  else if (g_strcmp0 (gtk_print_settings_get (settings, GTK_PRINT_SETTINGS_OUTPUT_FILE_FORMAT), "svg") == 0)
  {
    ext = ".svg";
  }
  else
  {
    ext = ".pdf";
  }

  uri = g_strconcat ("file://", dir, "/", 
		     "print-word", ext, NULL);
  gtk_print_settings_set (settings, 
			  GTK_PRINT_SETTINGS_OUTPUT_URI, uri);
  gtk_print_operation_set_print_settings (operation, settings);
  gtk_print_operation_run (operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
			   GTK_WINDOW (window), &error);
  g_object_unref (operation);
  g_object_unref (settings);
  g_free (uri);

  if (error)
  {
    GtkWidget *dialog;
	
    dialog = gtk_message_dialog_new (GTK_WINDOW (window),
				     GTK_DIALOG_DESTROY_WITH_PARENT,
				     GTK_MESSAGE_ERROR,
				     GTK_BUTTONS_CLOSE,
			    	     "%s", error->message);
    g_error_free (error);
    g_signal_connect (dialog, "response",
		      G_CALLBACK (gtk_widget_destroy), NULL);
    gtk_widget_show (dialog);
  }
}

static void 
begin_print (GtkPrintOperation *operation,
	     GtkPrintContext   *context,
	     gpointer           data)
{
  gint n_pages = 0;
  GyPrintCompositor *compositor = GY_PRINT_COMPOSITOR (data);

  while (!gy_print_compositor_paginate (compositor, context))
    ;

  n_pages = gy_print_compositor_get_n_pages (compositor);

  gtk_print_operation_set_n_pages (operation, n_pages);
}

static void 
draw_page (GtkPrintOperation *operation G_GNUC_UNUSED,
	   GtkPrintContext   *context,
	   gint               page_nr,
	   gpointer           data)
{
  GyPrintCompositor *compositor = GY_PRINT_COMPOSITOR (data);

  gy_print_compositor_draw_page (compositor, context, page_nr);
}

static void 
end_print (GtkPrintOperation *operationi G_GNUC_UNUSED,
	   GtkPrintContext   *context G_GNUC_UNUSED,
	   gpointer           data)
{
  GyPrintCompositor *compositor = GY_PRINT_COMPOSITOR (data);
  g_object_unref (compositor);
}

