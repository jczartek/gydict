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

#include "gy-app.h"
#include "gy-window.h"
#include "gy-utility-func.h"
#include "gy-dict.h"
#include "gy-print.h"
#include "gy-history.h"
#include "gy-history-iterable.h"
#include "gy-parsable.h"
#include "gy-search-bar.h"
#include "gy-text-view.h"
#include "gy-text-buffer.h"
#include "gy-tree-view.h"

typedef struct _GyWindowPrivate GyWindowPrivate;

static void gear_menu_cb (GSimpleAction *action,
                          GVariant      *parametr,
                          gpointer       data);
static void find_menu_cb (GSimpleAction *action,
                          GVariant      *parametr,
                          gpointer       data);
static void dict_menu_cb (GSimpleAction *action,
                          GVariant      *parametr,
                          gpointer       data);
static void dict_radio_cb (GSimpleAction *action,
                           GVariant      *parameter,
                           gpointer       data);
static void quit_win_cb (GSimpleAction *action,
                         GVariant      *parameter,
                         gpointer       data);
static void paste_cb (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       data);
static void copy_cb (GSimpleAction *action,
                     GVariant      *parameter,
                     gpointer       data);
static void go_back_cb (GSimpleAction *action,
                        GVariant      *parameter,
                        gpointer       data);
static void go_forward_cb (GSimpleAction *action,
                           GVariant      *parameter,
                           gpointer       data);
static void paste_sign_cb (GSimpleAction *action,
                           GVariant      *parameter,
                           gpointer       data);
static void  buttons_signs_cb (GSimpleAction *action,
                               GVariant      *parametr,
                               gpointer       data);
static void respond_clipboard_cb (GSimpleAction *action,
                                  GVariant      *parameter,
                                  gpointer       data);
static void on_window_size_allocate (GtkWidget     *widget,
                                     GtkAllocation *allocation);
static gboolean on_window_state_event (GtkWidget           *widget,
                                       GdkEventWindowState *event);
static void on_window_destroy (GtkWidget *widget);
static void on_window_constructed (GObject *object);
static void dispose (GObject *object);

enum
{
  GY_BINDING_ACTION_PREV,
  GY_BINDING_ACTION_NEXT,
  GY_N_BINDINGS
};

struct _GyWindow
{
  GtkApplicationWindow  __parent__;
  GtkWidget            *main_box;
  GtkWidget            *child_box;
  GtkWidget            *tree_view;
  GyTextView           *text_view;
  GtkWidget            *findbar;
  GtkWidget            *header_bar;
  GtkWidget            *entry;
  GtkWidget            *back;
  GtkWidget            *forward;
  GtkWidget            *text_box;
  GtkWidget            *revealer_buttons;

  GtkTextBuffer        *buffer;
  GtkTreeSelection     *selection;
  GData                *datalist;
  GQuark                qvalue;
  guint                 timeout_history;
  gchar                *string_history;

  GyHistory            *history;
  GHashTable           *histories_dictionaries;
  GAction              *next;
  GAction              *prev;
  GBinding             *bind[GY_N_BINDINGS];

  GtkClipboard         *clipboard; /* Non free! */

  /* Window State */
  gint                  current_width;
  gint                  current_height;
  gboolean              is_maximized;
};

G_DEFINE_TYPE (GyWindow, gy_window, GTK_TYPE_APPLICATION_WINDOW);

/**STATIC DATA**/

static PangoFontDescription *font_desc = NULL;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

static GActionEntry win_entries[] =
{
  { "print", gy_print_do_printing, NULL, NULL, NULL },
  { "copy", copy_cb, NULL, NULL, NULL },
  { "paste", paste_cb, NULL, NULL, NULL },
  { "clip", respond_clipboard_cb, NULL, "false", NULL },
  { "signs", buttons_signs_cb, NULL, "false", NULL },
  { "close", quit_win_cb, NULL, NULL, NULL },
  { "go-back", go_back_cb, NULL, NULL, NULL },
  { "go-forward", go_forward_cb, NULL, NULL, NULL },
  { "dict", dict_radio_cb, "s", "''", NULL },
  { "find-menu", find_menu_cb, NULL, "false", NULL },
  { "dict-menu", dict_menu_cb, NULL, "false", NULL },
  { "gear-menu", gear_menu_cb, NULL, "false", NULL },
  { "paste-sign", paste_sign_cb, "s", "''", NULL },
};

#pragma GCC diagnostic pop

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
      gtk_entry_set_text (GTK_ENTRY (self->entry), (const gchar *) word);

    g_match_info_free (match_info);
    g_regex_unref (regex);
    g_free (text);
    g_free (word);
  }

}

static gboolean
press_button_text_view_cb (GtkWidget      *widget G_GNUC_UNUSED,
                           GdkEventButton *event G_GNUC_UNUSED,
                           gpointer        data)
{
  GyWindow *self = GY_WINDOW (data);

  if (!gy_utility_handlers_is_blocked_by_func (self->clipboard,
                                               owner_change_cb, self))
  {
    g_signal_handlers_block_by_func (self->clipboard,
                                     owner_change_cb, self);
  }

  return FALSE;
}

static gboolean
release_button_text_view_cb (GtkWidget      *widget G_GNUC_UNUSED,
                             GdkEventButton *event G_GNUC_UNUSED,
                             gpointer        data)
{
  GyWindow *self = GY_WINDOW (data);

  if (gy_utility_handlers_is_blocked_by_func (self->clipboard,
                                              owner_change_cb, self))
  {
    g_signal_handlers_unblock_by_func (self->clipboard,
                                       owner_change_cb, self);
    gtk_text_buffer_copy_clipboard (self->buffer, self->clipboard);
  }

  return FALSE;
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
    g_signal_connect (self->text_view, "button-press-event",
                      G_CALLBACK (press_button_text_view_cb), self);
    g_signal_connect (self->text_view, "button-release-event",
                      G_CALLBACK (release_button_text_view_cb), self);
  }
  else
  {
    g_signal_handlers_disconnect_by_func (self->clipboard,
                                          owner_change_cb, self);
    g_signal_handlers_disconnect_by_func (self->text_view,
                                          press_button_text_view_cb, self);
    g_signal_handlers_disconnect_by_func (self->text_view,
                                          release_button_text_view_cb, self);
  }
}

static void 
find_menu_cb (GSimpleAction *action,
              GVariant      *parametr G_GNUC_UNUSED,
              gpointer       data)
{
  GyWindow *self = GY_WINDOW(data);

  GVariant *state;

  state = g_action_get_state (G_ACTION (action));
  g_action_change_state (G_ACTION (action),
                         g_variant_new_boolean (!g_variant_get_boolean (state)));
  gy_search_bar_set_search_mode (GY_SEARCH_BAR (self->findbar),
                                 !g_variant_get_boolean (state));
  g_variant_unref (state);

}

static void 
buttons_signs_cb(GSimpleAction *action,
                 GVariant      *parametr G_GNUC_UNUSED,
                 gpointer       data)
{
  GyWindow *self = GY_WINDOW(data);

  GVariant *state;

  state = g_action_get_state (G_ACTION (action));
  g_action_change_state (G_ACTION (action),
                         g_variant_new_boolean (!g_variant_get_boolean (state)));
  gtk_revealer_set_reveal_child (GTK_REVEALER (self->revealer_buttons),
                                 !g_variant_get_boolean (state));
  g_variant_unref (state);

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
dict_radio_cb (GSimpleAction *action,
               GVariant *parameter,
               gpointer data)
{
  const gchar *value,
              *str = NULL;
  GyWindow *self = GY_WINDOW (data);
  GyDict *dict;

  value = g_variant_get_string (parameter, NULL);
  self->qvalue = g_quark_from_string (value);

  if (!(dict = g_datalist_id_get_data (&self->datalist, self->qvalue)))
    {
      GyHistory *history = NULL;

      dict = GY_DICT(gy_dict_new (value, self->buffer));

      if (!dict)
        return;

      g_datalist_id_set_data_full (&self->datalist,
                                   self->qvalue,dict,
                                   g_object_unref);

      if (self->histories_dictionaries != NULL)
        {
          history = gy_history_new ();
          g_hash_table_insert (self->histories_dictionaries,
                               (gpointer) g_strdup (value), history);

        }
    }

  if (!gy_dict_is_mapped (dict))
    {
      GError *err = NULL;

      gy_dict_map (dict, &err);

      if (err != NULL)
        {
          g_critical ("%s", err->message);
          return;
        }
    }

  gtk_tree_view_set_model (GTK_TREE_VIEW (self->tree_view),
                           gy_dict_get_tree_model (dict));
  gy_text_buffer_clean_buffer (GY_TEXT_BUFFER (self->buffer));
  gtk_entry_set_text (GTK_ENTRY (self->entry), "");

  self->history = GY_HISTORY (g_hash_table_lookup (self->histories_dictionaries,
                                                   value));

  if (self->bind[GY_BINDING_ACTION_PREV] != NULL &&
      self->bind[GY_BINDING_ACTION_NEXT] != NULL)
    {
      g_binding_unbind (self->bind[GY_BINDING_ACTION_PREV]);
      g_binding_unbind (self->bind[GY_BINDING_ACTION_NEXT]);
    }

  self->bind[GY_BINDING_ACTION_PREV] = g_object_bind_property (G_OBJECT (self->history), "is-enabled-action-prev",
                                                               G_OBJECT (self->prev),    "enabled",
                                                               G_BINDING_DEFAULT);

  self->bind[GY_BINDING_ACTION_NEXT] = g_object_bind_property (G_OBJECT (self->history),  "is-enabled-action-next",
                                                               G_OBJECT (self->next),     "enabled",
                                                               G_BINDING_DEFAULT);
  gy_history_update (self->history);

  if (gy_history_length (self->history) != 0)
    {
      /* Gets the end item in the history. */
      str = gy_history_iterable_get_item (GY_HISTORY_ITERABLE (self->history));
      gtk_entry_set_text (GTK_ENTRY (self->entry), str);
    }

  gtk_header_bar_set_title (GTK_HEADER_BAR (self->header_bar), str == NULL ? "" : str);

  g_action_change_state (G_ACTION (action), parameter);
}

static void 
quit_win_cb (GSimpleAction *action G_GNUC_UNUSED,
             GVariant      *parameter G_GNUC_UNUSED,
             gpointer       data)
{
  GyWindow *self = GY_WINDOW (data);
  gtk_widget_destroy (GTK_WIDGET (self));
}

static void 
copy_cb (GSimpleAction *action G_GNUC_UNUSED,
         GVariant      *parameter G_GNUC_UNUSED,
         gpointer       data)
{
  GtkClipboard * clip;
  GyWindow *self = GY_WINDOW (data);

  clip = gtk_clipboard_get (GDK_NONE);
  gtk_text_buffer_copy_clipboard (self->buffer,clip);
}

static void 
paste_cb (GSimpleAction *action G_GNUC_UNUSED,
          GVariant      *parameter G_GNUC_UNUSED,
          gpointer       data)
{
  GtkWidget *focus;
  GyWindow *self = GY_WINDOW (data);

  focus = gtk_window_get_focus (GTK_WINDOW (self));

  if (GTK_IS_EDITABLE (focus))
    gtk_editable_paste_clipboard (GTK_EDITABLE (focus));
}

static void 
go_back_cb (GSimpleAction *action G_GNUC_UNUSED,
            GVariant      *parameter G_GNUC_UNUSED,
            gpointer       data)
{
  GyWindow *self = GY_WINDOW (data);

  gy_history_iterable_previous_item (GY_HISTORY_ITERABLE (self->history));
  const gchar *text = gy_history_iterable_get_item (GY_HISTORY_ITERABLE (self->history));

  if (text)
    gtk_entry_set_text (GTK_ENTRY (self->entry), text);

}

static void 
go_forward_cb (GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *parameter G_GNUC_UNUSED,
               gpointer       data)
{
  GyWindow * self = GY_WINDOW (data);

  gy_history_iterable_next_item (GY_HISTORY_ITERABLE (self->history));
  const gchar *text = gy_history_iterable_get_item (GY_HISTORY_ITERABLE (self->history));

  if (text)
    gtk_entry_set_text (GTK_ENTRY (self->entry), text);
}

static void 
paste_sign_cb (GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *parameter,
               gpointer       data)
{
  gchar *concat = NULL;
  GyWindow *self = GY_WINDOW (data);

  concat = g_strconcat (gtk_entry_get_text (GTK_ENTRY (self->entry)),
                        g_variant_get_string (parameter, NULL), NULL);
  gtk_entry_set_text (GTK_ENTRY (self->entry), concat);
  gtk_editable_set_position (GTK_EDITABLE (self->entry), -1);
  g_free (concat);
}

static gboolean
source_func (gpointer data)
{
  GyWindow *self = GY_WINDOW (data);

  gy_history_append (self->history, self->string_history );

  g_free (self->string_history);
  self->string_history = NULL;
  self->timeout_history = 0;
  return FALSE;
}

static void
tree_selection_cb (GtkTreeSelection *selection, 
                   gpointer          data)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreePath * path;
  gint * row;
  GtkTextBuffer * buffer;
  gchar * value;
  GyWindow *self = GY_WINDOW (data);

  buffer = gy_window_get_text_buffer (self);

  if (self->timeout_history)
    {
      g_source_remove (self->timeout_history);
      self->timeout_history = 0;
    }

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      path = gtk_tree_model_get_path (model, &iter);
      row = gtk_tree_path_get_indices (path);

      gy_text_buffer_clean_buffer (GY_TEXT_BUFFER (buffer));
      gy_parsable_parse (GY_PARSABLE (gy_window_get_dictionary (self)),
                         buffer, *row);

      gtk_tree_model_get (model, &iter, 0, &value, -1);
      gtk_header_bar_set_title (GTK_HEADER_BAR (gy_window_get_header_bar (self)),
                                (const gchar *) value);

      /* the variable @value is freed in the function source_func */
      self->string_history = value;
      self->timeout_history = g_timeout_add (2000, (GSourceFunc) source_func, data);
      gtk_tree_path_free (path);
    }
}

static void
gy_pwn_finalize (GObject *object)
{
  GyWindow *self = GY_WINDOW (object);

  g_datalist_clear (&self->datalist);
  self->qvalue = 0;

  if (self->histories_dictionaries != NULL)
  {
    g_hash_table_destroy (self->histories_dictionaries);
    self->histories_dictionaries = NULL;
    self->history = NULL;
  }
  G_OBJECT_CLASS (gy_window_parent_class)->finalize (object);
}

static void
gy_window_init (GyWindow *self)
{

  self->timeout_history = 0;
  self->string_history = NULL;
  g_datalist_init (&self->datalist);

  gtk_widget_init_template (GTK_WIDGET (self));

  /* Add actions */
  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   win_entries, G_N_ELEMENTS (win_entries),
                                   self);

  self->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->text_view));

  self->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self->tree_view));
  gtk_tree_selection_set_mode (self->selection, GTK_SELECTION_BROWSE);
  gtk_tree_view_set_search_entry (GTK_TREE_VIEW (self->tree_view),
                                  GTK_ENTRY (self->entry));

  /* Create findbar */
  self->findbar = gy_search_bar_new ();
  gtk_box_pack_end (GTK_BOX (self->text_box), self->findbar, FALSE, FALSE, 0);
  gy_search_bar_connect_text_buffer (GY_SEARCH_BAR (self->findbar), self->buffer);

  /* Create history */
  self->histories_dictionaries = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                        g_free, g_object_unref);
  self->next = g_action_map_lookup_action (G_ACTION_MAP (self), "go-forward");
  self->prev = g_action_map_lookup_action (G_ACTION_MAP (self), "go-back");
  g_simple_action_set_enabled (G_SIMPLE_ACTION (self->next), FALSE);
  g_simple_action_set_enabled (G_SIMPLE_ACTION (self->prev), FALSE);
  self->bind[GY_BINDING_ACTION_PREV] = self->bind[GY_BINDING_ACTION_NEXT] = NULL;

  /* Set clipboard */
  self->clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

  /* Connect signals */
  g_signal_connect (G_OBJECT (self->selection), "changed",
                    G_CALLBACK(tree_selection_cb), self);
}

static void
gy_window_class_init (GyWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = on_window_constructed;
  object_class->finalize = gy_pwn_finalize;

  widget_class->size_allocate = on_window_size_allocate;
  widget_class->window_state_event = on_window_state_event;
  widget_class->destroy = on_window_destroy;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gtk/gydict/gy-window.ui");
  gtk_widget_class_bind_template_child (widget_class, GyWindow, child_box);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, header_bar);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, entry);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, tree_view);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, text_view);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, text_box);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, revealer_buttons);
}

static void
window_store_state (GyWindow *self)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (self);
  GKeyFile *keyfile = g_key_file_new ();
  
  g_key_file_set_integer (keyfile, "WindowState", "width", self->current_width);
  g_key_file_set_integer (keyfile, "WindowState", "height", self->current_height);
  g_key_file_set_boolean (keyfile, "WindowState", "IsMaximized", self->is_maximized);

  const gchar *id_app = g_application_get_application_id (g_application_get_default());
  gchar *path = g_build_filename (g_get_user_cache_dir (), id_app, NULL);

  if (g_mkdir_with_parents (path, 0700) < 0)
  {
    goto out;
  }
  gchar *file = g_build_filename (path, "state.ini", NULL);
  g_key_file_save_to_file (keyfile, file, NULL);

  g_free (file);
out:
  g_key_file_unref (keyfile);
  g_free (path);
}

static void
window_load_state (GyWindow *self)
{
  const gchar *id_app = g_application_get_application_id (g_application_get_default ());
  gchar *file = g_build_filename (g_get_user_cache_dir (), id_app, "state.ini", NULL);
  GKeyFile *keyfile = g_key_file_new ();

  if (!g_key_file_load_from_file (keyfile, file, G_KEY_FILE_NONE, NULL))
  {
    goto out;
  }

  GError *error = NULL;
  self->current_width = g_key_file_get_integer (keyfile, "WindowState", "width", &error);
  if (error != NULL)
  {
    g_clear_error (&error);
    self->current_width = -1;
  }

  self->current_height = g_key_file_get_integer (keyfile, "WindowState", "height", &error);
  if (error != NULL)
  {
    g_clear_error (&error);
    self->current_height = -1;
  }

  self->is_maximized = g_key_file_get_boolean (keyfile, "WindowState", "IsMaximized", &error);
  if (error != NULL)
  {
    g_clear_error (&error);
    self->is_maximized = FALSE;
  }
out:
  g_key_file_unref (keyfile);
  g_free (file);
}

static void
on_window_constructed (GObject *object)
{
  GyWindow *self = GY_WINDOW (object);

  self->current_width = -1;
  self->current_height = -1;
  self->is_maximized = FALSE;

  window_load_state (self);

  gtk_window_set_default_size (GTK_WINDOW (self),
                               self->current_width,
                               self->current_height);

  if (self->is_maximized)
    gtk_window_maximize (GTK_WINDOW (self));

  G_OBJECT_CLASS (gy_window_parent_class)->constructed (object);
}


static void
on_window_size_allocate (GtkWidget *widget,
                         GtkAllocation *allocation)
{
  GyWindow *self = GY_WINDOW (widget);

  GTK_WIDGET_CLASS (gy_window_parent_class)->size_allocate (widget,
                                                            allocation);
  if (!(self->is_maximized))
  {
    self->current_width = allocation->width;
    self->current_height = allocation->height;
  }
}

static gboolean
on_window_state_event (GtkWidget *widget,
                       GdkEventWindowState *event)
{
  GyWindow *self = GY_WINDOW (widget);
  gboolean res = GDK_EVENT_PROPAGATE;

  if (GTK_WIDGET_CLASS (gy_window_parent_class)->window_state_event != NULL)
  {
    res = GTK_WIDGET_CLASS (gy_window_parent_class)->window_state_event (widget, event);
  }

  self->is_maximized = (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED) != 0;

  return res;
}

static void 
on_window_destroy (GtkWidget *widget)
{
  GyWindow *self = GY_WINDOW (widget);
  window_store_state (self);
  GTK_WIDGET_CLASS (gy_window_parent_class)->destroy (widget);
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

GtkTextBuffer *
gy_window_get_text_buffer (GyWindow *self)
{
  return self->buffer;
}

GyDict *
gy_window_get_dictionary (GyWindow *self)
{
  return g_datalist_id_get_data (&self->datalist, self->qvalue);
}

GtkWidget *
gy_window_get_header_bar (GyWindow *self)
{
  return self->header_bar;
}

GtkWidget *
gy_window_get_entry (GyWindow *self)
{
  return self->entry;
}

GtkWidget *
gy_window_get_text_view (GyWindow *self)
{
  return GTK_WIDGET (self->text_view);
}

