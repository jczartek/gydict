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
#include "gy-parser.h"
#include "gy-search-bar.h"
#include "gy-settings.h"

typedef struct _GyWindowPrivate GyWindowPrivate;

static void gear_menu_cb (GSimpleAction *action,
			  GVariant 	*parametr,
			  gpointer 	 data);
static void find_menu_cb (GSimpleAction *action,
			  GVariant 	*parametr,
			  gpointer 	 data);
static void dict_menu_cb (GSimpleAction *action,
			  GVariant 	*parametr,
			  gpointer 	 data);
static void dict_radio_cb (GSimpleAction *action,
			   GVariant 	 *parameter,
			   gpointer 	  data);
static void quit_win_cb (GSimpleAction *action,
			GVariant       *parameter,
			gpointer        data);
static void paste_cb (GSimpleAction *action,
		      GVariant 	    *parameter,
	     	      gpointer 	     data);
static void copy_cb (GSimpleAction *action,
	    	     GVariant 	   *parameter,
    		     gpointer       data);
static void go_back_cb (GSimpleAction *action,
	    	    	GVariant      *parameter,
    		    	gpointer       data);
static void go_forward_cb (GSimpleAction *action,
			   GVariant 	 *parameter,
			   gpointer 	  data);
static void paste_sign_cb (GSimpleAction *action,
			   GVariant 	 *parameter,
			   gpointer 	  data);
static void  buttons_signs_cb (GSimpleAction *action,
			       GVariant      *parametr,
			       gpointer       data);
static void respond_clipboard_cb (GSimpleAction *action,
				  GVariant	*parameter,
				  gpointer 	 data);
static void changed_history_cb (GObject    *object,
			        GParamSpec *spec,
			        gpointer    data);
static void window_check_history (GyWindow *window);
static void dispose (GObject *object);

struct _GyWindowPrivate
{
  GtkWidget *main_box;
  GtkWidget *child_box;
  GtkWidget *tree_view;
  GtkWidget *text_view;
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
  GySettings *settings;

  GtkClipboard *clipboard; /* Non free! */
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
owner_change_cb (GtkClipboard 	     *clipboard,
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
		      GVariant	    *parameter G_GNUC_UNUSED,
		      gpointer	     data)
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
  }
  else
  {
    g_signal_handlers_disconnect_by_func (priv->clipboard,
					  owner_change_cb, window);
  }
}

static void 
find_menu_cb (GSimpleAction *action,
	      GVariant 	    *parametr G_GNUC_UNUSED,
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
	     GVariant  	   *parametr G_GNUC_UNUSED,
	     gpointer 	    data G_GNUC_UNUSED)
{
    GVariant *state;

    state = g_action_get_state (G_ACTION (action));
    g_action_change_state (G_ACTION (action),
			   g_variant_new_boolean (!g_variant_get_boolean (state)));
    g_variant_unref (state);
}


static void 
gear_menu_cb(GSimpleAction *action,
	     GVariant 	   *parametr G_GNUC_UNUSED,
	     gpointer 	    data G_GNUC_UNUSED)
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
    const gchar *value;
    GyWindow *window = GY_WINDOW (data);
    GyWindowPrivate *priv = gy_window_get_instance_private (window);
    GyDict *dict;

    value = g_variant_get_string (parameter, NULL);
    priv->qvalue = g_quark_from_string (value);

    if (!(dict = g_datalist_id_get_data (&priv->datalist, priv->qvalue)))
    {
      GyHistory *history = NULL;
      dict = gy_dict_new_object (value);

      if(gy_dict_set_dictionary (dict) ||
	 gy_dict_init_list (dict))
	{
	  gtk_widget_show (priv->infobar);
	  return;
	}

      g_datalist_id_set_data_full (&priv->datalist,
				   priv->qvalue,dict,
				   g_object_unref);

      if (priv->histories_dictionaries != NULL)
      {
       	history = gy_history_new ();
	g_signal_connect (G_OBJECT (history), "notify::start-list",
	  		  G_CALLBACK (changed_history_cb), window);
	g_signal_connect (G_OBJECT (history), "notify::end-list",
	  		  G_CALLBACK (changed_history_cb), window);
	g_hash_table_insert (priv->histories_dictionaries,
	 		    (gpointer) g_strdup (value),
	 		    history);
      }
    }

    gy_utility_delete_text_in_buffer (priv->buffer);
    gtk_header_bar_set_title (GTK_HEADER_BAR (priv->header_bar), "");
    priv->history = GY_HISTORY (g_hash_table_lookup (priv->histories_dictionaries,
				value));
    window_check_history (window);

    gtk_tree_view_set_model (GTK_TREE_VIEW (priv->tree_view),
		             gy_dict_get_tree_model (dict));

    g_action_change_state (G_ACTION (action), parameter);
}

static void 
quit_win_cb (GSimpleAction *action G_GNUC_UNUSED,
	     GVariant 	   *parameter G_GNUC_UNUSED,
	     gpointer 	    data)
{
    GyWindow *window = GY_WINDOW (data);
    gtk_widget_destroy (GTK_WIDGET (window));
}

static void 
copy_cb (GSimpleAction *action G_GNUC_UNUSED,
	 GVariant      *parameter G_GNUC_UNUSED,
	 gpointer 	data)
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
     	  gpointer 	 data)
{
    GtkWidget *focus;
    GyWindow *window = GY_WINDOW (data);

    focus = gtk_window_get_focus (GTK_WINDOW (window));

    if (GTK_IS_EDITABLE (focus))
	gtk_editable_paste_clipboard (GTK_EDITABLE (focus));
}

static void 
go_back_cb (GSimpleAction *action G_GNUC_UNUSED,
	    GVariant 	  *parameter G_GNUC_UNUSED,
	    gpointer  	   data)
{
    GyWindow * window = GY_WINDOW (data);
    GyWindowPrivate *priv = gy_window_get_instance_private (window);

    gy_history_go_back (priv->history);
    const gchar *text = gy_history_get_string_from_quark (priv->history);
    gy_history_update_current_history (priv->history);

    gtk_entry_set_text (GTK_ENTRY (priv->entry), text);
}

static void 
go_forward_cb (GSimpleAction *action G_GNUC_UNUSED,
	       GVariant      *parameter G_GNUC_UNUSED,
	       gpointer       data)
{
    GyWindow * window = GY_WINDOW (data);
    GyWindowPrivate *priv = gy_window_get_instance_private (window);
    gy_history_go_forward (priv->history);
    const gchar *text = gy_history_get_string_from_quark (priv->history);
    gy_history_update_current_history (priv->history);

    gtk_entry_set_text (GTK_ENTRY (priv->entry), text);
}

static void 
changed_history_cb (GObject    *object G_GNUC_UNUSED,
		    GParamSpec *spec G_GNUC_UNUSED,
		    gpointer    data)
{
    GyWindow *window = GY_WINDOW (data);
    
    window_check_history (window);
}

static void 
paste_sign_cb (GSimpleAction *action G_GNUC_UNUSED,
	       GVariant *parameter,
	       gpointer data)
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

    gy_history_add_list (priv->history, priv->string_history);

    g_free (priv->string_history);
    priv->string_history = NULL;
    priv->timeout_history = 0;
    return FALSE;
}

static void
tree_selection_cb (GtkTreeSelection *selection, 
		   gpointer 	     data)
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
	gy_parser_lexer_buffer (GY_PARSER (gy_window_get_dictionary (window)), 
					   buffer, *row);

	gtk_tree_model_get (model, &iter, 0, &value, -1);
	gtk_header_bar_set_title (GTK_HEADER_BAR (gy_window_get_header_bar (window)),
				 (const gchar *) value);

	/* the variable-value frees in function source_func */
	priv->string_history = value;
	priv->timeout_history = g_timeout_add (2000,
					(GSourceFunc) source_func,
					data);
	gtk_tree_path_free (path);
    }
}

static gboolean
tree_view_search_equal_func (GtkTreeModel *model,
			     gint 	   column,
			     const gchar  *key,
			     GtkTreeIter  *iter,
			     gpointer 	   search_data G_GNUC_UNUSED)
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

static void
settings_fonts_changed_cb (GySettings  *settings G_GNUC_UNUSED,
			   const gchar *text_font,
			   const gchar *tree_font,
			   gpointer     data)
{
    GyWindow *window = GY_WINDOW (data);
    GyWindowPrivate *priv = gy_window_get_instance_private (window);

    font_desc = pango_font_description_from_string (text_font);
    gtk_widget_override_font (priv->text_view, font_desc);
    pango_font_description_free (font_desc);

    font_desc = pango_font_description_from_string (tree_font);
    gtk_widget_override_font (priv->tree_view, font_desc);
    pango_font_description_free (font_desc);

}

inline static void
set_font (GyWindow *window)
{
    gchar *text_font = NULL, *tree_font = NULL;
    GyWindowPrivate *priv = gy_window_get_instance_private (window);

    if (!gy_settings_get_use_fonts_system (priv->settings))
    {
	text_font = gy_settings_get_font_text (priv->settings);
	tree_font = gy_settings_get_font_tree (priv->settings);

	font_desc = pango_font_description_from_string (text_font);
	gtk_widget_override_font(priv->text_view, font_desc);
	pango_font_description_free (font_desc);

	font_desc = pango_font_description_from_string (tree_font);
	gtk_widget_override_font(priv->tree_view, font_desc);
	pango_font_description_free (font_desc);

	g_free (text_font);
	g_free (tree_font);
    }

}

static GtkTextBuffer*
set_text_buffer_on_text_view (GtkWidget* text_view)
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

  label = gtk_label_new (_("Error!!!. Can not open the dictionary.\n" 
			 "See description error on terminal."));
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

  //setup_new_menu (window);
  create_info_bar (window);

  priv->buffer = set_text_buffer_on_text_view (priv->text_view);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (priv->text_view), GTK_WRAP_WORD);

  //set_attribute_tree_view (priv->tree_view);
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
  priv->histories_dictionaries = g_hash_table_new_full (g_str_hash,
						        g_str_equal,
							g_free,
							g_object_unref);
  g_simple_action_set_enabled (G_SIMPLE_ACTION (g_action_map_lookup_action (G_ACTION_MAP (window), "go-back")), FALSE);
  g_simple_action_set_enabled (G_SIMPLE_ACTION (g_action_map_lookup_action (G_ACTION_MAP (window), "go-forward")), FALSE);
  //priv->history = gy_history_new ();
  //window_check_history (window);

  /* Create settings */
  priv->settings = gy_settings_get ();
  set_font (window);

  /* Set clipboard */
  priv->clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

  /* Connect signals */
  g_signal_connect (G_OBJECT (priv->selection), "changed",
		    G_CALLBACK (tree_selection_cb), window);
  g_signal_connect (priv->text_view, "button-press-event",
		    G_CALLBACK (press_button_text_view_cb), window);
  g_signal_connect (priv->text_view, "button-release-event",
		    G_CALLBACK (release_button_text_view_cb), window);
  /*g_signal_connect (G_OBJECT (priv->history), "notify::start-list",
		    G_CALLBACK (changed_history_cb), window);
  g_signal_connect (G_OBJECT (priv->history), "notify::end-list",
		    G_CALLBACK (changed_history_cb), window);*/
  g_signal_connect (priv->settings, "fonts-changed",
		    G_CALLBACK (settings_fonts_changed_cb), window);
}

static void
window_check_history (GyWindow *window)
{
  GAction *action;
  gboolean disable;
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  g_return_if_fail (priv->history != NULL);

  disable = gy_history_get_start_list (priv->history);
  action = g_action_map_lookup_action (G_ACTION_MAP (window), "go-back");
  g_simple_action_set_enabled (G_SIMPLE_ACTION (action), disable);

  disable = gy_history_get_end_list (priv->history);
  action = g_action_map_lookup_action (G_ACTION_MAP (window), "go-forward");
  g_simple_action_set_enabled (G_SIMPLE_ACTION (action), disable);
}

static void
gy_window_class_init (GyWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = dispose;

  gtk_widget_class_set_template_from_resource (widget_class,
					       "/org/gtk/gydict/gydict.ui");
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
dispose (GObject *object)
{
  GyWindow *window = GY_WINDOW (object);
  GyWindowPrivate *priv = gy_window_get_instance_private (window);

  g_datalist_clear (&priv->datalist);
  priv->qvalue = 0;

  g_clear_object (&priv->settings);
  //g_clear_object (&priv->history);
  if (priv->histories_dictionaries != NULL)
  {
    g_hash_table_destroy (priv->histories_dictionaries);
    priv->histories_dictionaries = NULL;
    priv->history = NULL;
  }

  G_OBJECT_CLASS (gy_window_parent_class)->dispose (object);
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
  return priv->text_view;
}

