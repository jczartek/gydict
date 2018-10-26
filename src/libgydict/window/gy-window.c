/* gy-window.c
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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

#include "config.h"
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include "gy-window.h"
#include "gy-window-settings.h"
#include "gy-header-bar.h"
#include "gy-workspace.h"
#include "helpers/gy-utility-func.h"
#include "printing/gy-print.h"
#include "entryview/gy-text-view.h"
#include "deflist/gy-def-list.h"

#define MOUSE_UP_BUTTON   8
#define MOUSE_DOWN_BUTTON 9

static void gear_menu_cb (GSimpleAction *action,
                          GVariant      *parametr,
                          gpointer       data);
static void dict_menu_cb (GSimpleAction *action,
                          GVariant      *parametr,
                          gpointer       data);
static void quit_win_cb (GSimpleAction *action,
                         GVariant      *parameter,
                         gpointer       data);
static void respond_clipboard_cb (GSimpleAction *action,
                                  GVariant      *parameter,
                                  gpointer       data);

struct _GyWindow
{
  DzlApplicationWindow  __parent__;
  GyWorkspace          *workspace;
  GyHeaderBar          *header_bar;
  GtkWidget            *findbar;
  GtkClipboard         *clipboard; /* Non free! */
};

G_DEFINE_TYPE (GyWindow, gy_window, DZL_TYPE_APPLICATION_WINDOW);

static GActionEntry win_entries[] =
{
  { "print", gy_print_do_printing, NULL, NULL, NULL },
  { "clip", respond_clipboard_cb, NULL, "false", NULL },
  { "close", quit_win_cb, NULL, NULL, NULL },
  { "dict-menu", dict_menu_cb, NULL, "false", NULL },
  { "gear-menu", gear_menu_cb, NULL, "false", NULL },
};

/**STATIC FUNCTION**/
static void
owner_change_cb (GtkClipboard        *clipboard,
                 GdkEventOwnerChange *event G_GNUC_UNUSED,
                 gpointer             data)
{
  gchar *text = NULL, *word = NULL;
  GyWindow *self = GY_WINDOW (data);

  if ((text = gtk_clipboard_wait_for_text (clipboard)))
  {
    GRegex *regex;
    GMatchInfo *match_info;

    regex = g_regex_new ("[[:alpha:]|]+([[:space:]]|[-])?[[:alpha:]|]+",
                         0, 0, NULL);
    g_regex_match (regex, text, 0, &match_info);
    word = g_match_info_fetch (match_info, 0);

    if (word)
      gy_header_bar_set_text_in_entry (self->header_bar, (const gchar *) word);

    g_match_info_free (match_info);
    g_regex_unref (regex);
    g_free (text);
    g_free (word);
  }

}

static void
respond_clipboard_cb (GSimpleAction *action,
                      GVariant      *parameter G_GNUC_UNUSED,
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

static void
dict_menu_cb(GSimpleAction *action,
             GVariant      *parametr G_GNUC_UNUSED,
             gpointer       data G_GNUC_UNUSED)
{
  GVariant *state;

  state = g_action_get_state (G_ACTION (action));
  g_action_change_state (G_ACTION (action),
                         g_variant_new_boolean (!g_variant_get_boolean (state)));
  g_variant_unref (state);
}

static void
gear_menu_cb(GSimpleAction *action,
             GVariant      *parametr G_GNUC_UNUSED,
             gpointer       data G_GNUC_UNUSED)
{
  GVariant *state;

  state = g_action_get_state (G_ACTION (action));
  g_action_change_state (G_ACTION (action),
                         g_variant_new_boolean (!g_variant_get_boolean (state)));
  g_variant_unref (state);
}

static void
quit_win_cb (GSimpleAction *action G_GNUC_UNUSED,
             GVariant      *parameter G_GNUC_UNUSED,
             gpointer       data)
{
  GyWindow *self = GY_WINDOW (data);
  gtk_widget_destroy (GTK_WIDGET (self));
}

static gboolean
gy_window_button_press_event (GtkWidget      *w,
                              GdkEventButton *e,
                              gpointer        d)
{
  GyDefList *dl = GY_DEF_LIST (d);

  if (e->type == GDK_DOUBLE_BUTTON_PRESS && e->button == MOUSE_UP_BUTTON)
    {
      gy_def_list_select_previous_item (dl);
    }
  else if (e->type == GDK_DOUBLE_BUTTON_PRESS && e->button == MOUSE_DOWN_BUTTON)
    {
      gy_def_list_select_next_item (dl);
    }

  return FALSE;
}

static void
gy_window_init (GyWindow *self)
{
  GtkTreeView *treeview;
  GtkEntry    *entry;

  gtk_widget_init_template (GTK_WIDGET (self));

  g_action_map_add_action_entries (G_ACTION_MAP (self), win_entries,
                                   G_N_ELEMENTS (win_entries), self);
  gy_workspace_attach_action (self->workspace, self);

  gy_window_settings_register (GTK_WINDOW (self));

  g_object_get (self->workspace, "left-widget", &treeview, NULL);
  entry = gy_header_bar_get_entry (self->header_bar);
  gtk_tree_view_set_search_entry (treeview, entry);

  self->clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

  gy_header_bar_grab_focus_for_entry (self->header_bar);

  g_signal_connect (self, "button-press-event",
                    G_CALLBACK (gy_window_button_press_event), treeview);

}

static void
gy_window_class_init (GyWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gtk/gydict/gy-window.ui");
  gtk_widget_class_bind_template_child (widget_class, GyWindow, header_bar);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, workspace);
}

/**PUBLIC METHOD**/
GtkWidget *
gy_window_new (GyApp *application)
{
  GyWindow *window;

  window = g_object_new (GY_TYPE_WINDOW,
                         "application", application, NULL);

  return GTK_WIDGET (window);
}

GtkWidget *
gy_window_get_text_view (GyWindow *self)
{
  GtkWidget *tv = NULL;

  g_object_get (self->workspace, "right-widget", &tv, NULL);
  g_return_val_if_fail (GY_IS_TEXT_VIEW (tv), NULL);

  return tv;
}

void
gy_window_grab_focus (GyWindow *self)
{
  g_return_if_fail (GY_IS_WINDOW (self));

  gy_header_bar_grab_focus_for_entry (self->header_bar);
}

void
gy_window_clear_search_entry (GyWindow *self)
{
  g_return_if_fail (GY_IS_WINDOW (self));

  gy_header_bar_set_text_in_entry (self->header_bar, "");
}
