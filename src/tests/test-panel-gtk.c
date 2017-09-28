/* test-gtk-panel.c
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
#include <pnl.h>

#define UI_FILE  "test-panel-gtk.ui"

#define TEST_TYPE_APP (test_app_get_type())
G_DECLARE_FINAL_TYPE (TestApp, test_app, TEST, APP, GtkApplication)

struct _TestApp {
  GtkApplication __parent__;
};

G_DEFINE_TYPE (TestApp, test_app, GTK_TYPE_APPLICATION)

#define TEST_TYPE_APP_WINDOW (test_app_window_get_type())
G_DECLARE_FINAL_TYPE (TestAppWindow, test_app_window, TEST, APP_WINDOW, GtkApplicationWindow)

struct _TestAppWindow {
  GtkApplicationWindow __parent__;
  GtkWidget*           dockbin;
};

G_DEFINE_TYPE (TestAppWindow, test_app_window, GTK_TYPE_APPLICATION_WINDOW)

static GBytes *
test_read_all_bytes (void)
{
  g_autoptr(GFile) file = NULL;
  g_autoptr(GFileInputStream) in = NULL;
  g_autoptr(GFileInfo) finfo = NULL;
  gsize size = 0;

  g_message ("%s", TEST_DATA_DIR UI_FILE);
  g_assert (g_file_test (TEST_DATA_DIR UI_FILE, G_FILE_TEST_EXISTS));
  file = g_file_new_for_path (TEST_DATA_DIR UI_FILE);

  in = g_file_read (file, NULL, NULL);
  finfo = g_file_input_stream_query_info (in, "standard::*", NULL, NULL);

  size = g_file_info_get_size (finfo);
  return g_input_stream_read_bytes (G_INPUT_STREAM (in),
                                    size, NULL, NULL);
}

static void
test_app_window_class_init (TestAppWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template (GTK_WIDGET_CLASS (klass),
                                 test_read_all_bytes ());

  gtk_widget_class_bind_template_child (widget_class, TestAppWindow, dockbin);

}

static void
test_app_window_init (TestAppWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

static TestAppWindow *
test_app_window_new (TestApp *app)
{
  return g_object_new (TEST_TYPE_APP_WINDOW,
                       "application", app, NULL);
}

/* TestApp */

static void
test_app_activate (GApplication *app)
{
  TestAppWindow *win = NULL;
  PnlDockBin *dock;
  PnlDockRevealer *edge;
  GActionGroup *group;

  win = test_app_window_new (TEST_APP(app));
  gtk_window_present (GTK_WINDOW (win));

  dock = PNL_DOCK_BIN (win->dockbin);
  edge = PNL_DOCK_REVEALER (pnl_dock_bin_get_left_edge (dock));
  pnl_dock_revealer_set_position (PNL_DOCK_REVEALER (edge), 200);
  pnl_dock_revealer_set_reveal_child (edge, TRUE);
  group = gtk_widget_get_action_group (GTK_WIDGET (dock), "dockbin");
  gtk_widget_insert_action_group (GTK_WIDGET (win), "dockbin", group);
}

static void
test_app_class_init (TestAppClass *klass)
{
  G_APPLICATION_CLASS (klass)->activate = test_app_activate;
}

static void
test_app_init (TestApp *self)
{
}

static TestApp *
test_app_new (void)
{
  return g_object_new (TEST_TYPE_APP,
                       "application-id", "org.gtk.testapp", NULL);
}

gint
main (gint   argc,
      gchar *argv[])
{
  return g_application_run (G_APPLICATION (test_app_new ()), argc, argv);
}
