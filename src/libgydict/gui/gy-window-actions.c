/* gy-window-actions.c
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

#include "gy-window-private.h"
#include "helpers/gy-print-compositor.h"


static void
gy_window_actions_set_dict_service (GSimpleAction *action,
                                    GVariant      *parameter,
                                    gpointer       data)
{
  GyWindow *self = (GyWindow *) data;

  g_autoptr(GVariant) state = NULL;
  state = g_action_get_state (G_ACTION (action));

  if (g_variant_compare (parameter, state) == 0) return;

  self->service_id = g_variant_get_string (parameter, NULL);
  GyService *service = gy_service_provider_get_service_by_id (self->service_provider,
                                                              self->service_id);
  if (GY_IS_DICT_SERVICE (service))
    {
      GError *error = NULL;
      GtkTreeModel *model = gy_dict_service_get_model((GyDictService *)service, &error);
      if (error != NULL)
        {
          g_critical ("Error: %s", error->message);
          g_error_free(error);
          return;
        }
      gy_def_list_set_model (self->deflist, model);
    }
  else
    g_critical ("The dictionary services: %s is not available.", self->service_id );


  g_action_change_state (G_ACTION (action), parameter);
}

static void
gy_window_actions_quit_win (GSimpleAction *action    G_GNUC_UNUSED,
                              GVariant    *parameter G_GNUC_UNUSED,
                              gpointer     data)
{
  GyWindow *self = GY_WINDOW (data);
  gtk_widget_destroy (GTK_WIDGET (self));
}

static void
owner_change_cb (GtkClipboard        *clipboard,
                 GdkEventOwnerChange *event G_GNUC_UNUSED,
                 gpointer             data)
{
  gchar *text = NULL, *word = NULL;
  GyWindow *self = GY_WINDOW (data);

  g_message("%s", "Jestem w "__FILE__);

  if ((text = gtk_clipboard_wait_for_text (clipboard)))
  {
    GRegex *regex;
    GMatchInfo *match_info;

    regex = g_regex_new ("[[:alpha:]|]+([[:space:]]|[-])?[[:alpha:]|]+",
                         0, 0, NULL);
    g_regex_match (regex, text, 0, &match_info);
    word = g_match_info_fetch (match_info, 0);

    if (word) gtk_entry_set_text (GTK_ENTRY (self->search_entry), (const gchar *) word);

    g_match_info_free (match_info);
    g_regex_unref (regex);
    g_free (text);
    g_free (word);
  }

}

static void
gy_window_actions_respond_clipboard (GSimpleAction *action,
                                     GVariant      *parameter  G_GNUC_UNUSED,
                                     gpointer       data)
{
  gboolean respond;
  GVariant *state;
  GyWindow *self = GY_WINDOW (data);

  state = g_action_get_state (G_ACTION (action));
  respond = g_variant_get_boolean (state);
  g_action_change_state (G_ACTION (action),
                         g_variant_new_boolean (!respond));
  g_variant_unref (state);

  if (!respond)
  {
    g_signal_connect (self->clipboard, "owner-change",
                      G_CALLBACK (owner_change_cb), self);
  }
  else
  {
    g_signal_handlers_disconnect_by_func (self->clipboard,
                                          owner_change_cb, self);
  }
}

/* Print Actions */
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

static void
gy_window_actions_print (GSimpleAction *action    G_GNUC_UNUSED,
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

  uri = g_strconcat ("file://", dir, "/", "print-word", ext, NULL);
  gtk_print_settings_set (settings,  GTK_PRINT_SETTINGS_OUTPUT_URI, uri);
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


static GActionEntry entries[] =
{
  { "print", gy_window_actions_print, NULL, NULL, NULL },
  { "clip", gy_window_actions_respond_clipboard, NULL, "false", NULL },
  { "close", gy_window_actions_quit_win, NULL, NULL, NULL },
  { "set-dict-service", gy_window_actions_set_dict_service, "s", "''", NULL},
};

void
_gy_window_actions_init (GyWindow *self)
{
  g_action_map_add_action_entries (G_ACTION_MAP (self), entries,
                                   G_N_ELEMENTS (entries), self);

  GActionGroup *dockbin_actions  = gtk_widget_get_action_group (GTK_WIDGET(self->dockbin), "dockbin");
  gtk_widget_insert_action_group (GTK_WIDGET (self), "dockbin", dockbin_actions);
}
