/*
 * gy-search-bar.c
 * Copyright (C) 2014 Jakub Czartek  <kuba@linux.pl>
 *
 * gy-search-bar.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gy-search-bar.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gy-search-bar.h"
#include "./entryview/gy-text-buffer.h"

struct _GySearchBar
{
	GtkBin          __parent__;
  GtkSearchEntry *entry;
  GtkTextBuffer  *buffer;
  GtkButton      *close_button;
  gboolean        search_mode_enabled;
};

G_DEFINE_TYPE (GySearchBar, gy_search_bar, GTK_TYPE_BIN)

enum {
	PROP_0,
  PROP_BUFFER,
  PROP_SEARCH_MODE_ENABLED,
	N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gboolean
gy_search_bar_is_keynav_event (GdkEvent *event,
                               guint     keyval)
{
  GdkModifierType state = 0;

  gdk_event_get_state (event, &state);

  if (keyval == GDK_KEY_Up ||
      keyval == GDK_KEY_KP_Up ||
      keyval == GDK_KEY_Down ||
      keyval == GDK_KEY_KP_Down ||
      keyval == GDK_KEY_Home ||
      keyval == GDK_KEY_KP_Home ||
      keyval == GDK_KEY_End ||
      keyval == GDK_KEY_KP_End ||
      keyval == GDK_KEY_Page_Up ||
      keyval == GDK_KEY_KP_Page_Up ||
      keyval == GDK_KEY_Page_Down ||
      keyval == GDK_KEY_KP_Page_Down ||
      keyval == GDK_KEY_Tab ||
      keyval == GDK_KEY_KP_Tab ||
      ((state & (GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 0))
      return GDK_EVENT_STOP;

  return GDK_EVENT_PROPAGATE;
}

static gboolean
gy_search_bar__search_entry_key_press_event (GtkWidget G_GNUC_UNUSED *widget,
                                             GdkEvent                *event,
                                             gpointer                 data)
{
  GySearchBar *self = GY_SEARCH_BAR (data);
  guint keyval;

  if (!gdk_event_get_keyval (event, &keyval) ||
      keyval == GDK_KEY_Escape)
  {
    GtkWidget *toplevel;

    toplevel = gtk_widget_get_toplevel (GTK_WIDGET (self));

    if (gtk_widget_is_toplevel (toplevel))
      {
        GActionGroup *dockbin_actions;

        dockbin_actions = gtk_widget_get_action_group (toplevel, "dockbin");
        g_action_group_activate_action (dockbin_actions, "top-visible", NULL);
      }

    return GDK_EVENT_STOP;
  }

  if (gy_search_bar_is_keynav_event (event, keyval))
    return GDK_EVENT_STOP;

  return GDK_EVENT_PROPAGATE;
}

static void
gy_search_bar__search_entry_search_changed (GtkSearchEntry *entry,
                                            gpointer        data)
{
  const gchar     *searched_string = NULL;
  GySearchBar     *self = (GySearchBar *) data;
  GtkStyleContext *context;

  gy_text_buffer_remove_tags_by_name (GY_TEXT_BUFFER (self->buffer),
                                      "search", "search_next", NULL);
  context = gtk_widget_get_style_context (GTK_WIDGET (self->entry));
  gtk_style_context_remove_class (context, "search-missing");

  searched_string = gtk_entry_get_text (GTK_ENTRY (entry));

  if (searched_string[0] != '\0')
    {
      gboolean    found = FALSE;
      GtkTextIter start, end;

      gtk_text_buffer_get_iter_at_offset (self->buffer, &start, 0);
      gtk_text_buffer_get_iter_at_offset (self->buffer, &end, 0);

      found = gtk_text_iter_forward_search (&start, searched_string,
                                            (GTK_TEXT_SEARCH_VISIBLE_ONLY | GTK_TEXT_SEARCH_TEXT_ONLY),
                                            &start, &end, NULL);

      if (found)
        {
          GtkTextMark *mark = gtk_text_buffer_get_mark (self->buffer, "searched");
          GtkTextTag *tag = gy_text_buffer_get_tag_by_name (GY_TEXT_BUFFER (self->buffer), "search");
          GtkTextView *tv = g_object_get_data (G_OBJECT (self->buffer), "textview");

          gtk_text_buffer_move_mark (self->buffer, mark, &end);
          while (found)
            {
              gtk_text_buffer_apply_tag (self->buffer, tag, &start, &end);
              found = gtk_text_iter_forward_search (&end, searched_string,
                                                    (GTK_TEXT_SEARCH_VISIBLE_ONLY | GTK_TEXT_SEARCH_TEXT_ONLY),
                                                    &start, &end, NULL);
            }
          gtk_text_buffer_get_iter_at_mark (self->buffer, &end, mark);

          start = end;
          while (!gtk_text_iter_starts_tag (&start, tag) && gtk_text_iter_backward_char (&start))
            ;

          gtk_text_buffer_apply_tag_by_name (self->buffer, "search_next", &start, &end);

          if (GTK_IS_TEXT_VIEW (tv))
            {
              gtk_text_view_scroll_mark_onscreen (tv, mark);
            }
        }
      else
        {
          gtk_style_context_add_class (context, "search-missing");
        }
    }
}

static void
gy_search_bar__clicked_close_button (GySearchBar *self,
                                     GtkButton   *bt)
{
  GtkWidget *toplevel;

  toplevel = gtk_widget_get_toplevel (GTK_WIDGET (self));

  if (gtk_widget_is_toplevel (toplevel))
    {
      GActionGroup *dockbin_actions;

      dockbin_actions = gtk_widget_get_action_group (toplevel, "dockbin");
      g_action_group_activate_action (dockbin_actions, "top-visible", NULL);
    }
}

static void
gy_search_bar_finalize (GObject *object)
{
	GySearchBar *self = (GySearchBar *)object;

  g_clear_object (&self->buffer);

	G_OBJECT_CLASS (gy_search_bar_parent_class)->finalize (object);
}

static void
gy_search_bar_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
	GySearchBar *self = GY_SEARCH_BAR (object);

	switch (prop_id)
	  {
    case PROP_BUFFER:
      self->buffer = g_value_dup_object (value);
      break;
    case PROP_SEARCH_MODE_ENABLED:
      gy_search_bar_set_search_mode_enabled (self, g_value_get_boolean (value));
      break;
	  default:
	    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	  }
}

static void
gy_search_bar_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GySearchBar *self = GY_SEARCH_BAR (object);

  switch (prop_id)
    {
    case PROP_SEARCH_MODE_ENABLED:
      g_value_set_boolean (value, self->search_mode_enabled);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_search_bar_move_search (GySearchBar      *self,
                           GtkDirectionType  where)
{
  GtkTextMark *mark;
  GtkTextTag  *tag;
  GtkTextIter  iter,
               start,
               end;
  gboolean     found = FALSE;

  g_return_if_fail (GY_IS_SEARCH_BAR (self));

  gy_text_buffer_remove_tags_by_name (GY_TEXT_BUFFER (self->buffer),
                                      "search_next", NULL);
  mark = gtk_text_buffer_get_mark (self->buffer, "searched");
  tag = gy_text_buffer_get_tag_by_name (GY_TEXT_BUFFER (self->buffer), "search");
  gtk_text_buffer_get_iter_at_mark (self->buffer, &iter, mark);

  if (!gtk_text_iter_toggles_tag (&iter, tag)) return;

  if (where == GTK_DIR_DOWN)
    {
      if ((found = gtk_text_iter_forward_to_tag_toggle (&iter, tag)))
        {
          start = iter;
          while (!gtk_text_iter_ends_tag (&iter, tag) && gtk_text_iter_forward_char (&iter))
            ;
          end = iter;
        }
    }
  else /* where == GTK_DIR_UP */
    {
      if (gtk_text_iter_ends_tag (&iter, tag))
        {
          while (!gtk_text_iter_starts_tag (&iter, tag) && gtk_text_iter_backward_char (&iter))
            ;

          if ((found = gtk_text_iter_backward_to_tag_toggle (&iter, tag)))
            {
              end = iter;

              while (!gtk_text_iter_starts_tag (&iter, tag) && gtk_text_iter_backward_char (&iter))
                ;
              start = iter;
            }
        }
    }

  if (found)
    {
      GtkTextView *tv = g_object_get_data (G_OBJECT (self->buffer), "textview");

      gtk_text_buffer_move_mark (self->buffer, mark, &end);
      gtk_text_buffer_apply_tag_by_name (self->buffer, "search_next", &start, &end);
      gtk_text_view_scroll_mark_onscreen (tv, mark);
    }
}

static void
gy_search_bar_actions_next_search_result (GSimpleAction *action,
                                          GVariant      *state,
                                          gpointer       data)
{
  gy_search_bar_move_search (GY_SEARCH_BAR (data), GTK_DIR_DOWN);
}

static void
gy_search_bar_actions_previous_search_result (GSimpleAction *action,
                                              GVariant      *state,
                                              gpointer       data)
{
  gy_search_bar_move_search (GY_SEARCH_BAR (data), GTK_DIR_UP);
}

static const GActionEntry GySearchBarActions[] = {
    {"next-search-result", gy_search_bar_actions_next_search_result},
    {"previous-search-result", gy_search_bar_actions_previous_search_result}
};

static void
gy_search_bar_actions_init (GySearchBar *self)
{
  GSimpleActionGroup *group;

  g_assert (GY_IS_SEARCH_BAR (self));

  group = g_simple_action_group_new ();
  g_action_map_add_action_entries (G_ACTION_MAP (group), GySearchBarActions,
                                   G_N_ELEMENTS (GySearchBarActions), self);
  gtk_widget_insert_action_group (GTK_WIDGET (self), "search", G_ACTION_GROUP (group));
  g_object_unref (group);
}

static void
gy_search_bar_class_init (GySearchBarClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->finalize = gy_search_bar_finalize;
	object_class->set_property = gy_search_bar_set_property;
  object_class->get_property = gy_search_bar_get_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-search-bar.ui");
  gtk_widget_class_bind_template_child (widget_class, GySearchBar, entry);
  gtk_widget_class_bind_template_child (widget_class, GySearchBar, close_button);

  /**
   *
   * GySearchBar:buffer:
   *
   * The buffer which is searched.
   */
  properties[PROP_BUFFER] =
    g_param_spec_object ("buffer",
                         "buffer",
                         "The buffer which is searched",
                         GTK_TYPE_TEXT_BUFFER,
                         G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS);

  properties [PROP_SEARCH_MODE_ENABLED] =
    g_param_spec_boolean ("search-mode-enabled",
                          "Search Mode Enabled",
                          "Search Mode Enabled",
                          FALSE,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_css_name (widget_class, "gysearchbar");
}

static void
gy_search_bar_init (GySearchBar *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  gy_search_bar_actions_init (self);

  self->search_mode_enabled = FALSE;

  g_signal_connect (self->entry, "key-press-event",
                    G_CALLBACK (gy_search_bar__search_entry_key_press_event), self);
  g_signal_connect (self->entry, "search-changed",
                    G_CALLBACK (gy_search_bar__search_entry_search_changed), self);
  g_signal_connect_swapped (self->close_button, "clicked",
                            G_CALLBACK (gy_search_bar__clicked_close_button), self);
}

GySearchBar *
gy_search_bar_new (void)
{
	return g_object_new (GY_TYPE_SEARCH_BAR, NULL);
}

void
gy_search_bar_set_search_mode_enabled (GySearchBar *self,
                                       gboolean     search_mode_enabled)
{

  g_return_if_fail (GY_IS_SEARCH_BAR (self));

  search_mode_enabled = !!search_mode_enabled;

  if (search_mode_enabled != self->search_mode_enabled)
    {
      self->search_mode_enabled = search_mode_enabled;
      gtk_entry_set_text (GTK_ENTRY (self->entry), "");
      if (search_mode_enabled)
        gtk_widget_grab_focus (GTK_WIDGET (self->entry));

      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SEARCH_MODE_ENABLED]);
    }
}

