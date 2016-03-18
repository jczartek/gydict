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

struct _GyWindowPrivate
{
  GtkWidget *main_box;
  GtkWidget *child_box;
  GtkWidget *tree_view;
  GyTextView *text_view;
  GtkWidget *findbar;
  GtkWidget *infobar;
  GtkWidget *header_bar;
  GtkWidget *entry;
  GtkWidget *back;
  GtkWidget *forward;
  GtkWidget *text_box;
  GtkWidget *revealer_buttons;

  GtkTextBuffer *buffer;
  GtkTreeSelection *selection;
  GData *datalist;
  GQuark qvalue;
  guint timeout_history;
  gchar *string_history;

  GyHistory *history;
  GHashTable *histories_dictionaries;
  GAction  *next;
  GAction  *prev;
  GBinding *bind[GY_N_BINDINGS];

  GtkClipboard *clipboard; /* Non free! */

  /* Window State */
  gint current_width;
  gint current_height;
  gboolean is_maximized;
};

G_DEFINE_TYPE_WITH_PRIVATE (GyWindow, gy_window, GTK_TYPE_APPLICATION_WINDOW);

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
  GyWindow *window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  if ((text = gtk_clipboard_wait_for_text (clipboard)))
  {
    GRegex *regex;
    GMatchInfo *match_info;

    regex = g_regex_new ("[[:alpha:]|]+([[:space:]]|[-])?[[:alpha:]|]+",
                         0, 0, NULL);
    g_regex_match (regex, text, 0, &match_info);
    word = g_match_info_fetch (match_info, 0);

    if (word)
      gtk_entry_set_text (GTK_ENTRY (priv->entry), (const gchar *) word);

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
  GyWindow *window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  if (!gy_utility_handlers_is_blocked_by_func (priv->clipboard,
                                               owner_change_cb, window))
  {
    g_signal_handlers_block_by_func (priv->clipboard,
                                     owner_change_cb, window);
  }

  return FALSE;
}

static gboolean
release_button_text_view_cb (GtkWidget      *widget G_GNUC_UNUSED,
                             GdkEventButton *event G_GNUC_UNUSED,
                             gpointer        data)
{
  GyWindow *window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  if (gy_utility_handlers_is_blocked_by_func (priv->clipboard,
                                              owner_change_cb, window))
  {
    g_signal_handlers_unblock_by_func (priv->clipboard,
                                       owner_change_cb, window);
    gtk_text_buffer_copy_clipboard (priv->buffer, priv->clipboard);
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
  GyWindow *window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  state = g_action_get_state (G_ACTION (action));
  respond = g_variant_get_boolean (state);
  g_action_change_state (G_ACTION (action),
                         g_variant_new_boolean (!respond));
  g_variant_unref (state);

  if (!respond)
  {
    g_signal_connect (priv->clipboard, "owner-change",
                      G_CALLBACK (owner_change_cb), window);
    g_signal_connect (priv->text_view, "button-press-event",
                      G_CALLBACK (press_button_text_view_cb), window);
    g_signal_connect (priv->text_view, "button-release-event",
                      G_CALLBACK (release_button_text_view_cb), window);
  }
  else
  {
    g_signal_handlers_disconnect_by_func (priv->clipboard,
                                          owner_change_cb, window);
    g_signal_handlers_disconnect_by_func (priv->text_view,
                                          press_button_text_view_cb, window);
    g_signal_handlers_disconnect_by_func (priv->text_view,
                                          release_button_text_view_cb, window);
  }
}

static void 
find_menu_cb (GSimpleAction *action,
              GVariant      *parametr G_GNUC_UNUSED,
              gpointer       data)
{
  GyWindow *window = GY_WINDOW(data);
  GyWindowPrivate *priv = gy_window_get_instance_private(window);

  GVariant *state;

  state = g_action_get_state (G_ACTION (action));
  g_action_change_state (G_ACTION (action),
                         g_variant_new_boolean (!g_variant_get_boolean (state)));
  gy_search_bar_set_search_mode (GY_SEARCH_BAR (priv->findbar),
                                 !g_variant_get_boolean (state));
  g_variant_unref (state);

}

static void 
buttons_signs_cb(GSimpleAction *action,
                 GVariant      *parametr G_GNUC_UNUSED,
                 gpointer       data)
{
  GyWindow *window = GY_WINDOW(data);
  GyWindowPrivate *priv = gy_window_get_instance_private(window);

  GVariant *state;

  state = g_action_get_state (G_ACTION (action));
  g_action_change_state (G_ACTION (action),
                         g_variant_new_boolean (!g_variant_get_boolean (state)));
  gtk_revealer_set_reveal_child (GTK_REVEALER (priv->revealer_buttons), 
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
  GyWindow *window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);
  GyDict *dict;

  value = g_variant_get_string (parameter, NULL);
  priv->qvalue = g_quark_from_string (value);

  if (!(dict = g_datalist_id_get_data (&priv->datalist, priv->qvalue)))
    {
      GyHistory *history = NULL;

      dict = GY_DICT(gy_dict_new (value, priv->buffer));

      if (!dict)
        return;

      g_datalist_id_set_data_full (&priv->datalist,
                                   priv->qvalue,dict,
                                   g_object_unref);

      if (priv->histories_dictionaries != NULL)
        {
          history = gy_history_new ();
          g_hash_table_insert (priv->histories_dictionaries,
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

  gtk_tree_view_set_model (GTK_TREE_VIEW (priv->tree_view),
                           gy_dict_get_tree_model (dict));
  gy_utility_delete_text_in_buffer (priv->buffer);
  gtk_entry_set_text (GTK_ENTRY (priv->entry), "");

  priv->history = GY_HISTORY (g_hash_table_lookup (priv->histories_dictionaries,
                                                   value));

  if (priv->bind[GY_BINDING_ACTION_PREV] != NULL &&
      priv->bind[GY_BINDING_ACTION_NEXT] != NULL)
    {
      g_binding_unbind (priv->bind[GY_BINDING_ACTION_PREV]);
      g_binding_unbind (priv->bind[GY_BINDING_ACTION_NEXT]);
    }

  priv->bind[GY_BINDING_ACTION_PREV] = g_object_bind_property (G_OBJECT (priv->history), "is-enabled-action-prev",
                                                               G_OBJECT (priv->prev),    "enabled",
                                                               G_BINDING_DEFAULT);

  priv->bind[GY_BINDING_ACTION_NEXT] = g_object_bind_property (G_OBJECT (priv->history),  "is-enabled-action-next",
                                                               G_OBJECT (priv->next),     "enabled",
                                                               G_BINDING_DEFAULT);
  gy_history_update (priv->history);

  if (gy_history_length (priv->history) != 0)
    {
      /* Gets the end item in the history. */
      str = gy_history_iterable_get_item (GY_HISTORY_ITERABLE (priv->history));
      gtk_entry_set_text (GTK_ENTRY (priv->entry), str);
    }

  gtk_header_bar_set_title (GTK_HEADER_BAR (priv->header_bar), str == NULL ? "" : str);

  g_action_change_state (G_ACTION (action), parameter);
}

static void 
quit_win_cb (GSimpleAction *action G_GNUC_UNUSED,
             GVariant      *parameter G_GNUC_UNUSED,
             gpointer       data)
{
  GyWindow *window = GY_WINDOW (data);
  gtk_widget_destroy (GTK_WIDGET (window));
}

static void 
copy_cb (GSimpleAction *action G_GNUC_UNUSED,
         GVariant      *parameter G_GNUC_UNUSED,
         gpointer       data)
{
  GtkClipboard * clip;
  GyWindow * window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  clip = gtk_clipboard_get (GDK_NONE);
  gtk_text_buffer_copy_clipboard (priv->buffer,clip);
}

static void 
paste_cb (GSimpleAction *action G_GNUC_UNUSED,
          GVariant      *parameter G_GNUC_UNUSED,
          gpointer       data)
{
  GtkWidget *focus;
  GyWindow *window = GY_WINDOW (data);

  focus = gtk_window_get_focus (GTK_WINDOW (window));

  if (GTK_IS_EDITABLE (focus))
    gtk_editable_paste_clipboard (GTK_EDITABLE (focus));
}

static void 
go_back_cb (GSimpleAction *action G_GNUC_UNUSED,
            GVariant      *parameter G_GNUC_UNUSED,
            gpointer       data)
{
  GyWindow * window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  gy_history_iterable_previous_item (GY_HISTORY_ITERABLE (priv->history));
  const gchar *text = gy_history_iterable_get_item (GY_HISTORY_ITERABLE (priv->history));

  if (text)
    gtk_entry_set_text (GTK_ENTRY (priv->entry), text);

}

static void 
go_forward_cb (GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *parameter G_GNUC_UNUSED,
               gpointer       data)
{
  GyWindow * window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  gy_history_iterable_next_item (GY_HISTORY_ITERABLE (priv->history));
  const gchar *text = gy_history_iterable_get_item (GY_HISTORY_ITERABLE (priv->history));

  if (text)
    gtk_entry_set_text (GTK_ENTRY (priv->entry), text);
}

static void 
paste_sign_cb (GSimpleAction *action G_GNUC_UNUSED,
               GVariant      *parameter,
               gpointer       data)
{
  gchar *concat = NULL;
  GyWindow *window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  concat = g_strconcat (gtk_entry_get_text (GTK_ENTRY (priv->entry)),
                        g_variant_get_string (parameter, NULL), NULL);
  gtk_entry_set_text (GTK_ENTRY (priv->entry), concat);
  gtk_editable_set_position (GTK_EDITABLE (priv->entry), -1);
  g_free (concat);
}

static gboolean
source_func (gpointer data)
{
  GyWindow *window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  gy_history_append (priv->history, priv->string_history );

  g_free (priv->string_history);
  priv->string_history = NULL;
  priv->timeout_history = 0;
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
  GyWindow *window = GY_WINDOW (data);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  buffer = gy_window_get_text_buffer (window);

  if (priv->timeout_history)
    {
      g_source_remove (priv->timeout_history);
      priv->timeout_history = 0;
    }

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      path = gtk_tree_model_get_path (model, &iter);
      row = gtk_tree_path_get_indices (path);

      gy_utility_delete_text_in_buffer (buffer);
      gy_parsable_parse (GY_PARSABLE (gy_window_get_dictionary (window)),
                         buffer, *row);

      gtk_tree_model_get (model, &iter, 0, &value, -1);
      gtk_header_bar_set_title (GTK_HEADER_BAR (gy_window_get_header_bar (window)),
                                (const gchar *) value);

      /* the variable @value is freed in the function source_func */
      priv->string_history = value;
      priv->timeout_history = g_timeout_add (2000, (GSourceFunc) source_func, data);
      gtk_tree_path_free (path);
    }
}

static gboolean
tree_view_search_equal_func (GtkTreeModel *model,
                             gint          column,
                             const gchar  *key,
                             GtkTreeIter  *iter,
                             gpointer      search_data G_GNUC_UNUSED)
{
  gboolean retval = TRUE;
  const gchar *str;
  gchar *normalized_string;
  gchar *normalized_key;
  gchar *case_normalized_string = NULL;
  gchar *case_normalized_key = NULL;
  GValue value = G_VALUE_INIT;
  GValue transformed = G_VALUE_INIT;

  gtk_tree_model_get_value (model, iter, column, &value);

  g_value_init (&transformed, G_TYPE_STRING);

  if (!g_value_transform (&value, &transformed))
    {
      g_value_unset (&value);
      return TRUE;
    }

  g_value_unset (&value);

  str = g_value_get_string (&transformed);
  if (!str)
    {
      g_value_unset (&transformed);
      return TRUE;
    }

  normalized_string = g_utf8_normalize (str, -1, G_NORMALIZE_ALL);
  normalized_key = g_utf8_normalize (key, -1, G_NORMALIZE_ALL);

  if (normalized_string && normalized_key)
    {
      case_normalized_string = g_utf8_casefold (normalized_string, -1);
      case_normalized_key = g_utf8_casefold (normalized_key, -1);

      if (gy_utility_strcmp (case_normalized_key, case_normalized_string, strlen (case_normalized_key)) == 0)
        retval = FALSE;
    }

  g_value_unset (&transformed);
  g_free (normalized_key);
  g_free (normalized_string);
  g_free (case_normalized_key);
  g_free (case_normalized_string);

  return retval;
}

static GtkTextBuffer*
set_text_buffer_on_text_view (GyTextView* text_view)
{
  GtkTextIter iter;
  GtkTextBuffer *buffer;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
  gtk_text_buffer_get_start_iter (buffer,&iter);
  gtk_text_buffer_create_mark (buffer,"last_pos_start",&iter,FALSE);
  gtk_text_buffer_create_mark (buffer,"last_pos_end",&iter,FALSE);

  return buffer;
}

static void
create_info_bar (GyWindow *window)
{
  GtkWidget *label = NULL, *area = NULL;
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  label = gtk_label_new (_("Error!!!. Can't open the dictionary.\n See description error on terminal."));
  gtk_widget_show (label);
  area = gtk_info_bar_get_content_area (GTK_INFO_BAR (priv->infobar));
  gtk_container_add (GTK_CONTAINER (area), label);
  gtk_info_bar_set_message_type (GTK_INFO_BAR (priv->infobar),
                                 GTK_MESSAGE_ERROR);
  gtk_info_bar_add_button (GTK_INFO_BAR (priv->infobar), "OK",
                           GTK_RESPONSE_OK);
  g_signal_connect (priv->infobar, "response",
                    G_CALLBACK (gtk_widget_hide), NULL);
  gtk_widget_set_no_show_all (priv->infobar, TRUE);
}

static void
gy_pwn_finalize (GObject *object)
{
  GyWindow *window = GY_WINDOW (object);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  g_datalist_clear (&priv->datalist);
  priv->qvalue = 0;

  if (priv->histories_dictionaries != NULL)
  {
    g_hash_table_destroy (priv->histories_dictionaries);
    priv->histories_dictionaries = NULL;
    priv->history = NULL;
  }
  G_OBJECT_CLASS (gy_window_parent_class)->finalize (object);
}

static void
gy_window_init (GyWindow *window)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  priv->timeout_history = 0;
  priv->string_history = NULL;
  g_datalist_init (&priv->datalist);

  gtk_widget_init_template (GTK_WIDGET (window));

  /* Add actions */
  g_action_map_add_action_entries (G_ACTION_MAP (window),
                                   win_entries, G_N_ELEMENTS (win_entries),
                                   window);

  create_info_bar (window);

  priv->buffer = set_text_buffer_on_text_view (priv->text_view);

  priv->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tree_view));
  gtk_tree_selection_set_mode (priv->selection, GTK_SELECTION_BROWSE);
  gtk_tree_view_set_search_entry (GTK_TREE_VIEW (priv->tree_view),
                                  GTK_ENTRY (priv->entry));
  gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW (priv->tree_view),
                                       tree_view_search_equal_func,
                                       NULL, NULL);

  /* Create findbar */
  priv->findbar = gy_search_bar_new ();
  gtk_box_pack_end (GTK_BOX (priv->text_box), priv->findbar, FALSE, FALSE, 0);
  gy_search_bar_connect_text_buffer (GY_SEARCH_BAR (priv->findbar), priv->buffer);

  /* Create history */
  priv->histories_dictionaries = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                        g_free, g_object_unref);
  priv->next = g_action_map_lookup_action (G_ACTION_MAP (window), "go-forward");
  priv->prev = g_action_map_lookup_action (G_ACTION_MAP (window), "go-back");
  g_simple_action_set_enabled (G_SIMPLE_ACTION (priv->next), FALSE);
  g_simple_action_set_enabled (G_SIMPLE_ACTION (priv->prev), FALSE);
  priv->bind[GY_BINDING_ACTION_PREV] = priv->bind[GY_BINDING_ACTION_NEXT] = NULL;

  /* Set clipboard */
  priv->clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

  /* Connect signals */
  g_signal_connect (G_OBJECT (priv->selection), "changed",
                    G_CALLBACK(tree_selection_cb), window);
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
  gtk_widget_class_bind_template_child_private (widget_class, GyWindow, child_box);
  gtk_widget_class_bind_template_child_private (widget_class, GyWindow, infobar);
  gtk_widget_class_bind_template_child_private (widget_class, GyWindow, header_bar);
  gtk_widget_class_bind_template_child_private (widget_class, GyWindow, entry);
  gtk_widget_class_bind_template_child_private (widget_class, GyWindow, tree_view);
  gtk_widget_class_bind_template_child_private (widget_class, GyWindow, text_view);
  gtk_widget_class_bind_template_child_private (widget_class, GyWindow, text_box);
  gtk_widget_class_bind_template_child_private (widget_class, GyWindow, revealer_buttons);
}

static void
window_store_state (GyWindow *window)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (window);
  GKeyFile *keyfile = g_key_file_new ();
  
  g_key_file_set_integer (keyfile, "WindowState", "width", priv->current_width);
  g_key_file_set_integer (keyfile, "WindowState", "height", priv->current_height);
  g_key_file_set_boolean (keyfile, "WindowState", "IsMaximized", priv->is_maximized);

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
window_load_state (GyWindow *window)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (window);
  const gchar *id_app = g_application_get_application_id (g_application_get_default ());
  gchar *file = g_build_filename (g_get_user_cache_dir (), id_app, "state.ini", NULL);
  GKeyFile *keyfile = g_key_file_new ();

  if (!g_key_file_load_from_file (keyfile, file, G_KEY_FILE_NONE, NULL))
  {
    goto out;
  }

  GError *error = NULL;
  priv->current_width = g_key_file_get_integer (keyfile, "WindowState", "width", &error);
  if (error != NULL)
  {
    g_clear_error (&error);
    priv->current_width = -1;
  }

  priv->current_height = g_key_file_get_integer (keyfile, "WindowState", "height", &error);
  if (error != NULL)
  {
    g_clear_error (&error);
    priv->current_height = -1;
  }

  priv->is_maximized = g_key_file_get_boolean (keyfile, "WindowState", "IsMaximized", &error);
  if (error != NULL)
  {
    g_clear_error (&error);
    priv->is_maximized = FALSE;
  }
out:
  g_key_file_unref (keyfile);
  g_free (file);
}

static void
on_window_constructed (GObject *object)
{
  GyWindow *window = GY_WINDOW (object);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  priv->current_width = -1;
  priv->current_height = -1;
  priv->is_maximized = FALSE;

  window_load_state (window);

  gtk_window_set_default_size (GTK_WINDOW (window),
                               priv->current_width,
                               priv->current_height);

  if (priv->is_maximized)
    gtk_window_maximize (GTK_WINDOW (window));

  G_OBJECT_CLASS (gy_window_parent_class)->constructed (object);
}


static void
on_window_size_allocate (GtkWidget *widget,
                         GtkAllocation *allocation)
{
  GyWindow *win = GY_WINDOW (widget);
  GyWindowPrivate *priv = gy_window_get_instance_private (win);

  GTK_WIDGET_CLASS (gy_window_parent_class)->size_allocate (widget,
                                                            allocation);
  if (!(priv->is_maximized))
  {
    priv->current_width = allocation->width;
    priv->current_height = allocation->height;
  }
}

static gboolean
on_window_state_event (GtkWidget *widget,
                       GdkEventWindowState *event)
{
  GyWindow *win = GY_WINDOW (widget);
  GyWindowPrivate *priv = gy_window_get_instance_private (win);
  gboolean res = GDK_EVENT_PROPAGATE;

  if (GTK_WIDGET_CLASS (gy_window_parent_class)->window_state_event != NULL)
  {
    res = GTK_WIDGET_CLASS (gy_window_parent_class)->window_state_event (widget, event);
  }

  priv->is_maximized = (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED) != 0;

  return res;
}

static void 
on_window_destroy (GtkWidget *widget)
{
  GyWindow *win = GY_WINDOW (widget);
  window_store_state (win);
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
gy_window_get_text_buffer (GyWindow *window)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (window);
  return priv->buffer;
}

GyDict *
gy_window_get_dictionary (GyWindow *window)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (window);
  return g_datalist_id_get_data (&priv->datalist,priv->qvalue);
} 

GtkWidget *
gy_window_get_header_bar (GyWindow *window)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (window);
  return priv->header_bar;
}

GtkWidget *
gy_window_get_entry (GyWindow *window)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (window);
  return priv->entry;
}

GtkWidget *
gy_window_get_text_view (GyWindow *window)
{
  GyWindowPrivate *priv = gy_window_get_instance_private (window);
  return GTK_WIDGET (priv->text_view);
}

