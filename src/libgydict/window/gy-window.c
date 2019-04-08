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
#include <libpeas/peas.h>

#include "gy-window.h"
#include "gy-window-addin.h"
#include "gy-window-settings.h"
#include "gy-header-bar.h"
#include "gy-history-box.h"
#include "dictionaries/gy-dict.h"
#include "dictionaries/gy-dict-manager.h"
#include "deflist/gy-def-list.h"
#include "entryview/gy-text-view.h"
#include "entryview/gy-text-buffer.h"
#include "helpers/gy-utility-func.h"
#include "printing/gy-print.h"
#include "search/gy-search-bar.h"

#define MOUSE_UP_BUTTON   8
#define MOUSE_DOWN_BUTTON 9

struct _GyWindow
{
  DzlApplicationWindow  __parent__;
  DzlDockBin           *dockbin;
  GyDefList            *deflist;
  GyTextView           *textview;
  GyTextBuffer         *buffer;
  GyDictManager        *manager_dicts;
  GyHistoryBox         *history_box;
  GyHeaderBar          *header_bar;
  GySearchBar          *search_bar;
  PeasExtensionSet     *extens;
  GtkClipboard         *clipboard; /* Non free! */
};

G_DEFINE_TYPE (GyWindow, gy_window, DZL_TYPE_APPLICATION_WINDOW);

enum
{
  PROP_0,
  PROP_MANAGER_DICTS,
  N_PROPS
};

static GParamSpec *properties[N_PROPS];

static void
gy_window_action_add_to_history (GSimpleAction *action,
                                 GVariant      *parameter,
                                 gpointer      data)
{
  gint n_row = -1;
  g_autofree gchar *s = NULL;
  GyWindow *self = GY_WINDOW (data);

  n_row = gy_def_list_get_selected_n_row (self->deflist);
  s = gy_def_list_get_value_for_selected_row (self->deflist);

  if (n_row != -1 && s != NULL)
    gy_history_box_add (self->history_box, s, (guint) n_row);
}

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
gy_window_action_respond_clipboard (GSimpleAction *action,
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

static void
gy_window_action_quit_win_cb (GSimpleAction *action    G_GNUC_UNUSED,
                              GVariant      *parameter G_GNUC_UNUSED,
                              gpointer       data)
{
  GyWindow *self = GY_WINDOW (data);
  gtk_widget_destroy (GTK_WIDGET (self));
}

static void
gy_window_action_switch_dict (GSimpleAction *action,
                              GVariant      *parameter,
                              gpointer       data)
{
  GyWindow    *self   = (GyWindow *) data;
  GyDict      *dict   = NULL;
  g_autoptr(GVariant) state = NULL;
  const gchar *str;

  state = g_action_get_state (G_ACTION (action));

  if (g_variant_compare (parameter, state) == 0) return;

  str = g_variant_get_string (parameter, NULL);

  dict = gy_dict_manager_set_dict (self->manager_dicts, str);
  if (!dict) return;

  gy_text_buffer_clean_buffer (self->buffer);

  gtk_tree_view_set_model (GTK_TREE_VIEW (self->deflist),
                           gy_dict_get_tree_model (dict));

  gy_window_clear_search_entry (self);
  gy_window_grab_focus (GY_WINDOW (self));

  g_object_set (self->history_box, "filter-key", str, NULL);

  g_action_change_state (G_ACTION (action), parameter);
}

static GActionEntry win_entries[] =
{
  { "print", gy_print_do_printing, NULL, NULL, NULL },
  { "clip", gy_window_action_respond_clipboard, NULL, "false", NULL },
  { "close", gy_window_action_quit_win_cb, NULL, NULL, NULL },
  { "switch-dict", gy_window_action_switch_dict, "s", "''", NULL},
  { "add-to-history", gy_window_action_add_to_history, NULL, NULL, NULL}
};


static void
gy_window_set_property (GObject      *object,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
  GyWindow *self = GY_WINDOW (object);

  switch (prop_id)
    {
    case PROP_MANAGER_DICTS:
      self->manager_dicts = g_value_dup_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_window_get_property (GObject    *object,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
  GyWindow *self = GY_WINDOW (object);

  switch (prop_id)
    {
    case PROP_MANAGER_DICTS:
      g_value_set_object (value, self->manager_dicts);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
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
gy_window_notify_top_visible (GObject    *obj,
                              GParamSpec *pspec,
                              gpointer    data)
{
  GyWindow *self = GY_WINDOW (data);
  gboolean  is_visible;

  g_object_get (obj, "top-visible", &is_visible, NULL);

  if (is_visible)
    gy_search_bar_set_search_mode_enabled (self->search_bar, is_visible);
  else
    {
      gy_search_bar_set_search_mode_enabled (self->search_bar, is_visible);
      gy_window_grab_focus (self);
    }
}

static void
gy_window_row_activated (GtkListBox    *box,
                         GtkListBoxRow *row,
                         gpointer       data)
{
  gint      n_row = -1;
  GyWindow *self  = GY_WINDOW (data);

  n_row =  GPOINTER_TO_INT (g_object_get_data (G_OBJECT (row), "idx"));

  gy_def_list_select_row (self->deflist, n_row);

}

static void
gy_window_addin_added (PeasExtensionSet *set,
                       PeasPluginInfo   *plugin_info,
                       PeasExtension    *exten,
                       gpointer          user_data)
{
  GyWindow *self = GY_WINDOW (user_data);
  GyWindowAddin *addin = GY_WINDOW_ADDIN (exten);

  gy_window_addin_load (addin, self);
}


static void
gy_window_addin_removed (PeasExtensionSet *set,
                         PeasPluginInfo   *plugin_info,
                         PeasExtension    *exten,
                         gpointer          user_data)
{
  GyWindow *self = GY_WINDOW (user_data);
  GyWindowAddin *addin = GY_WINDOW_ADDIN (exten);

  gy_window_addin_unload (addin, self);
}

static void
gy_window_dispose (GObject *obj)
{
  GyWindow *self = GY_WINDOW (obj);

  if (self->extens != NULL)
    g_clear_object (&self->extens);

  if (self->manager_dicts != NULL)
    g_clear_object (&self->manager_dicts);

  G_OBJECT_CLASS (gy_window_parent_class)->dispose (obj);
}

static void
gy_window_init (GyWindow *self)
{
  GtkEntry    *entry;
  GActionGroup *dockbin_actions;

  gtk_widget_init_template (GTK_WIDGET (self));

  g_action_map_add_action_entries (G_ACTION_MAP (self), win_entries,
                                   G_N_ELEMENTS (win_entries), self);
  dockbin_actions = gtk_widget_get_action_group (GTK_WIDGET(self->dockbin), "dockbin");
  gtk_widget_insert_action_group (GTK_WIDGET (self), "dockbin", dockbin_actions);

  gy_window_settings_register (GTK_WINDOW (self));

  self->clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

  gy_header_bar_grab_focus_for_entry (self->header_bar);
  entry = gy_header_bar_get_entry (self->header_bar);
  gtk_tree_view_set_search_entry (GTK_TREE_VIEW(self->deflist), entry);

  g_object_set_data (G_OBJECT (self->textview), "manager", self->manager_dicts);
  g_object_set_data (G_OBJECT (self->buffer), "textview", self->textview);
  gy_def_list_register_observer (self->deflist, G_OBSERVER (self->textview));

  g_signal_connect (self, "button-press-event",
                    G_CALLBACK (gy_window_button_press_event), self->deflist);
  g_signal_connect (self->dockbin, "notify::top-visible",
                    G_CALLBACK (gy_window_notify_top_visible), self);
  g_signal_connect (self->history_box, "row-activated",
                    G_CALLBACK (gy_window_row_activated), self);

  PeasEngine *engine = peas_engine_get_default ();
  self->extens = peas_extension_set_new (engine, GY_TYPE_WINDOW_ADDIN, NULL);

  g_signal_connect (self->extens, "extension-added",
                    G_CALLBACK (gy_window_addin_added), self);

  g_signal_connect (self->extens, "extension-removed",
                    G_CALLBACK (gy_window_addin_removed), self);

  peas_extension_set_foreach (self->extens, gy_window_addin_added, self);

}

static void
gy_window_class_init (GyWindowClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->set_property = gy_window_set_property;
  object_class->get_property = gy_window_get_property;
  object_class->dispose = gy_window_dispose;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-window.ui");
  gtk_widget_class_bind_template_child (widget_class, GyWindow, dockbin);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, deflist);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, textview);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, buffer);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, header_bar);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, search_bar);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, history_box);

  properties[PROP_MANAGER_DICTS] =
    g_param_spec_object ("manager-dicts",
                         "manager-dicts",
                         "The manager of dictionaries.",
                         GY_TYPE_DICT_MANAGER,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties (object_class, N_PROPS, properties);
}

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
  g_return_val_if_fail (GY_IS_WINDOW (self), NULL);


  return GTK_WIDGET (self->textview);
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

DzlDockBin *
gy_window_get_dockbin (GyWindow *self)
{
  g_return_val_if_fail (GY_IS_WINDOW (self), NULL);

  return self->dockbin;
}
