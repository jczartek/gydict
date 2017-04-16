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
        }
      else
        {
          gtk_style_context_add_class (context, "search-missing");
        }

      while (found)
        {
          gtk_text_buffer_apply_tag_by_name (self->buffer, "search", &start, &end);
          found = gtk_text_iter_forward_search (&end, searched_string,
                                                (GTK_TEXT_SEARCH_VISIBLE_ONLY | GTK_TEXT_SEARCH_TEXT_ONLY),
                                                &start, &end, NULL);
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












/*#include <string.h>
#include "gy-search-bar.h"
#include "window/gy-window.h"

typedef struct _GySearchBarPrivate GySearchBarPrivate;

struct _GySearchBarPrivate
{
  GtkWidget *toolbar;
  GtkWidget *search_entry;
  GtkWidget *revealer;
  GtkWidget *go_up_button;
  GtkWidget *go_down_button;
  GtkWidget *close_button;

  gboolean search_mode;
  gchar *search_string;
  GtkTextBuffer *buffer;
};


G_DEFINE_TYPE_WITH_PRIVATE (GySearchBar, gy_search_bar, GTK_TYPE_BIN);

enum
{
  PROP_0,
  PROP_SEARCH_STRING
};

static void gy_search_bar_get_property (GObject     *object,
                                        guint        prop_id,
                                        GValue      *value,
                                        GParamSpec  *pspec);
static void gy_search_bar_set_property (GObject       *object,
                                        guint          prop_id,
                                        const GValue  *value,
                                        GParamSpec    *pspec);



static gboolean
is_keynav_event (GdkEvent *event,
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
      ((state & (GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 0))
      return TRUE;

  return FALSE;
}

static void
set_search_string (GySearchBar *bar,
                   const gchar *search_string)
{
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);

  g_return_if_fail (GY_IS_SEARCH_BAR (bar));

  g_object_freeze_notify (G_OBJECT (bar));

  if (priv->search_string != search_string)
    {
      gboolean non_empty = TRUE;

      if ((search_string && *search_string == '\0'))
        non_empty = FALSE;

      if ((priv->search_string && search_string &&
           strcmp ((const gchar *) priv->search_string, search_string) != 0) ||
          (priv->search_string == NULL && search_string))
        {
          if (priv->search_string)
            g_free (priv->search_string);

          priv->search_string = g_strdup (search_string);
          gtk_widget_set_sensitive (GTK_WIDGET (priv->go_up_button), non_empty);
          gtk_widget_set_sensitive (GTK_WIDGET (priv->go_down_button), non_empty);

          g_object_notify (G_OBJECT (bar), "search-string");
        }
    }

  g_object_thaw_notify (G_OBJECT (bar));
}

inline static void
clear_tag_buffer(GtkTextBuffer *buffer,
                 const gchar   *name_tag)
{
  GtkTextIter i_start, i_end;

  if (!name_tag)
    return;

  gtk_text_buffer_get_start_iter (buffer, &i_start);
  gtk_text_buffer_get_end_iter (buffer, &i_end);
  gtk_text_buffer_remove_tag_by_name (buffer, name_tag,
                                      &i_start, &i_end);
}

static void
button_down_cb (GtkButton *button G_GNUC_UNUSED,
                gpointer   data)
{
  GySearchBar *bar = GY_SEARCH_BAR (data);
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);
  GtkTextMark *m_start, *m_end;
  GtkTextIter i_start, i_end;
  gboolean found = FALSE;

  g_return_if_fail (GTK_IS_TEXT_BUFFER (priv->buffer));

  m_start = gtk_text_buffer_get_mark (priv->buffer, "last_pos_start");
  m_end = gtk_text_buffer_get_mark (priv->buffer, "last_pos_end");

  if (!m_start && !m_end)
    return;

  gtk_text_buffer_get_iter_at_mark (priv->buffer,&i_start,m_end);
  found = gtk_text_iter_forward_search (&i_start, priv->search_string,
                                        (GTK_TEXT_SEARCH_VISIBLE_ONLY|GTK_TEXT_SEARCH_TEXT_ONLY),
                                        &i_start, &i_end, NULL);

  if (found)
  {
    GtkWidget *toplevel;
    toplevel = gtk_widget_get_toplevel (GTK_WIDGET (bar));

    clear_tag_buffer (priv->buffer, "search_next");
    gtk_text_buffer_apply_tag_by_name (priv->buffer, "search_next",
                                       &i_start, &i_end);
    gtk_text_buffer_move_mark(priv->buffer, m_start, &i_start);
    gtk_text_buffer_move_mark(priv->buffer, m_end, &i_end);

    if (gtk_widget_is_toplevel (toplevel))
      gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (gy_window_get_text_view (GY_WINDOW (toplevel))), m_end);
  }
}

static void
button_up_cb (GtkButton *button G_GNUC_UNUSED,
              gpointer   data)
{
  GySearchBar *bar = GY_SEARCH_BAR (data);
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);
  GtkTextMark *m_start, *m_end;
  GtkTextIter i_start, i_end;
  gboolean found = FALSE;

  g_return_if_fail (GTK_IS_TEXT_BUFFER (priv->buffer));

  m_start = gtk_text_buffer_get_mark (priv->buffer, "last_pos_start");
  m_end = gtk_text_buffer_get_mark (priv->buffer, "last_pos_end");

  if (!m_start && !m_end)
    return;

  gtk_text_buffer_get_iter_at_mark (priv->buffer, &i_start, m_start);
  found = gtk_text_iter_backward_search (&i_start, priv->search_string,
                                         (GTK_TEXT_SEARCH_VISIBLE_ONLY|GTK_TEXT_SEARCH_TEXT_ONLY),
                                         &i_start ,&i_end, NULL);

  if (found)
  {
    GtkWidget *toplevel;
    gint iter_pos = 0;

    toplevel = gtk_widget_get_toplevel (GTK_WIDGET (bar));
    iter_pos = gtk_text_iter_get_offset (&i_start) + strlen (priv->search_string);
    gtk_text_buffer_get_iter_at_offset (priv->buffer, &i_end, iter_pos);
    clear_tag_buffer (priv->buffer, "search_next");
    gtk_text_buffer_apply_tag_by_name (priv->buffer, "search_next",
                                       &i_start, &i_end);
    gtk_text_buffer_move_mark(priv->buffer, m_start, &i_start);
    gtk_text_buffer_move_mark(priv->buffer, m_end, &i_end);

    if (gtk_widget_is_toplevel (toplevel))
      gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (gy_window_get_text_view (GY_WINDOW (toplevel))), m_end);
  }
}

static void
button_close_cb (GtkButton *button G_GNUC_UNUSED,
                 gpointer   data)
{
  GySearchBar *bar = GY_SEARCH_BAR (data);
  GtkWidget *toplevel;

  toplevel = gtk_widget_get_toplevel (GTK_WIDGET (bar));

  if (gtk_widget_is_toplevel (toplevel))
      g_action_group_activate_action (G_ACTION_GROUP (toplevel),
                                      "find-menu", NULL);
}

static void
reveal_child_cb (GObject    *object G_GNUC_UNUSED,
                 GParamSpec *spec G_GNUC_UNUSED,
                 gpointer    data)
{
  GySearchBar *bar = GY_SEARCH_BAR (data);
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);

  if (priv->revealer && priv->search_mode)
  {
    gtk_entry_set_text (GTK_ENTRY (priv->search_entry), "");

    if (!gtk_widget_is_focus (priv->search_entry))
      gtk_widget_grab_focus (priv->search_entry);
  }

  if (!priv->search_mode)
  {
    //GtkWidget *toplevel;
    //toplevel = gtk_widget_get_toplevel (GTK_WIDGET (bar));
    clear_tag_buffer (priv->buffer, "search_next");
    clear_tag_buffer (priv->buffer, "search");

    //if (gtk_widget_is_toplevel (toplevel))
    //{
     // GtkWidget *main_entry;
     // main_entry = gy_window_get_entry (GY_WINDOW (toplevel));

    //  if (!gtk_widget_is_focus (main_entry))
     //   gtk_widget_grab_focus (main_entry);
   // }
  }
}

static void
search_string_changed_cb (GObject     *object,
                          GParamSpec  *spec G_GNUC_UNUSED,
                          gpointer     data G_GNUC_UNUSED)
{
  GySearchBar *bar = GY_SEARCH_BAR (object);
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);
  GtkTextIter i_start, i_end;
  gboolean found = FALSE;

  g_return_if_fail (priv->buffer == NULL || GTK_IS_TEXT_BUFFER (priv->buffer));

  gtk_text_buffer_get_start_iter (priv->buffer, &i_start);
  gtk_text_buffer_get_end_iter (priv->buffer, &i_end);
  gtk_text_buffer_remove_tag_by_name (priv->buffer, "search",
                                      &i_start, &i_end);
  gtk_text_buffer_remove_tag_by_name (priv->buffer, "search_next",
                                      &i_start, &i_end);

  if (priv->search_string != '\0')
  {
    found = gtk_text_iter_forward_search (&i_start, priv->search_string,
                                          (GTK_TEXT_SEARCH_VISIBLE_ONLY|GTK_TEXT_SEARCH_TEXT_ONLY),
                                          &i_start, &i_end, NULL);

    if (found)
    {
      GtkWidget *toplevel;
      toplevel = gtk_widget_get_toplevel (GTK_WIDGET (bar));

      gtk_text_buffer_move_mark_by_name (priv->buffer, "last_pos_start", &i_start);
      gtk_text_buffer_move_mark_by_name (priv->buffer, "last_pos_end", &i_end);
      gtk_text_buffer_apply_tag_by_name (priv->buffer, "search_next", &i_start, &i_end);

      if (gtk_widget_is_toplevel (toplevel))
        gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (gy_window_get_text_view (GY_WINDOW (toplevel))),
                                            gtk_text_buffer_get_mark (priv->buffer, "last_pos_end"));
    }
  }

  while (found)
  {
    gtk_text_buffer_apply_tag_by_name (priv->buffer, "search", &i_start, &i_end);
    found = gtk_text_iter_forward_search (&i_end, priv->search_string,
                                          (GTK_TEXT_SEARCH_VISIBLE_ONLY|GTK_TEXT_SEARCH_TEXT_ONLY),
                                          &i_start, &i_end, NULL);
  }
}

static gboolean
entry_key_press_event_cb (GtkWidget  *widget G_GNUC_UNUSED,
                          GdkEvent   *event,
                          gpointer    data)
{
  GySearchBar *bar = GY_SEARCH_BAR (data);
  guint keyval;

  if (!gdk_event_get_keyval (event, &keyval) ||
      keyval == GDK_KEY_Escape)
  {
    GtkWidget *toplevel;
    toplevel = gtk_widget_get_toplevel (GTK_WIDGET (bar));

    if (gtk_widget_is_toplevel (toplevel))
      g_action_group_activate_action (G_ACTION_GROUP (toplevel), "find-menu", NULL);
    return GDK_EVENT_STOP;
  }

  if (is_keynav_event (event, keyval))
    return GDK_EVENT_STOP;

  return GDK_EVENT_PROPAGATE;
}

static void
entry_changed_cb (GtkEditable *editable,
                  gpointer     data)
{
  GySearchBar *bar = GY_SEARCH_BAR (data);

  set_search_string (bar, gtk_entry_get_text (GTK_ENTRY (editable)));
}

static void
gy_search_bar_init (GySearchBar *bar)
{
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);

  gtk_widget_init_template (GTK_WIDGET (bar));

  priv->buffer = NULL;
  priv->search_string = NULL;
  priv->search_mode = FALSE;

  gtk_widget_show_all (priv->toolbar);
  gtk_widget_set_sensitive (GTK_WIDGET (priv->go_up_button), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (priv->go_down_button), FALSE);

  g_signal_connect (GTK_BUTTON (priv->go_up_button), "clicked",
                    G_CALLBACK (button_up_cb), (gpointer) bar);
  g_signal_connect (GTK_BUTTON (priv->go_down_button), "clicked",
                    G_CALLBACK (button_down_cb), (gpointer) bar);
  g_signal_connect (GTK_BUTTON (priv->close_button), "clicked",
                    G_CALLBACK (button_close_cb), (gpointer) bar);
  g_signal_connect (G_OBJECT (priv->revealer), "notify::reveal-child",
                    G_CALLBACK (reveal_child_cb), (gpointer) bar);
  g_signal_connect (priv->search_entry, "key-press-event",
                    G_CALLBACK (entry_key_press_event_cb), (gpointer) bar);
  g_signal_connect (GTK_EDITABLE (priv->search_entry), "changed",
                    G_CALLBACK (entry_changed_cb), (gpointer) bar);
  g_signal_connect (G_OBJECT (bar), "notify::search-string",
                    G_CALLBACK (search_string_changed_cb), NULL);
}

static void
gy_search_bar_finalize (GObject *object)
{
  G_OBJECT_CLASS (gy_search_bar_parent_class)->finalize (object);
}

static void
gy_search_bar_class_init (GySearchBarClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = gy_search_bar_get_property;
  object_class->set_property = gy_search_bar_set_property;
  object_class->finalize = gy_search_bar_finalize;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gysearchbar.ui");
  gtk_widget_class_bind_template_child_internal_private (widget_class, GySearchBar, toolbar);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GySearchBar, search_entry);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GySearchBar, revealer);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GySearchBar, go_up_button);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GySearchBar, go_down_button);
  gtk_widget_class_bind_template_child_internal_private (widget_class, GySearchBar, close_button);

  g_object_class_install_property (object_class, PROP_SEARCH_STRING,
                                   g_param_spec_string ("search-string", "Search string",
                                                        "The name of the string to be found", NULL,
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gy_search_bar_get_property (GObject     *object,
                            guint        prop_id,
                            GValue      *value,
                            GParamSpec  *pspec)
{
  GySearchBar *bar = GY_SEARCH_BAR (object);
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);

  switch (prop_id)
  {
    case PROP_SEARCH_STRING:
      g_value_set_string(value, priv->search_string);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gy_search_bar_set_property (GObject       *object,
                            guint          prop_id,
                            const GValue  *value,
                            GParamSpec    *pspec)
{
  GySearchBar *bar = GY_SEARCH_BAR (object);

  switch (prop_id)
  {
    case PROP_SEARCH_STRING:
      set_search_string (bar, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

GtkWidget *
gy_search_bar_new (void)
{
  return g_object_new (GY_TYPE_SEARCH_BAR, NULL);
}

void
gy_search_bar_set_search_mode (GySearchBar *bar,
                               gboolean     search_mode)
{
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);

  g_return_if_fail (GY_IS_SEARCH_BAR (bar));

  priv->search_mode = search_mode;
  gtk_revealer_set_reveal_child (GTK_REVEALER (priv->revealer), search_mode);
}

void
gy_search_bar_connect_text_buffer (GySearchBar   *bar,
                                   GtkTextBuffer *buffer)
{
  GySearchBarPrivate *priv = gy_search_bar_get_instance_private (bar);

  g_return_if_fail (GY_IS_SEARCH_BAR (bar));
  g_return_if_fail (buffer == NULL || GTK_IS_TEXT_BUFFER (buffer));

  if (!priv->buffer)
    {
      priv->buffer = buffer;
      g_object_add_weak_pointer (G_OBJECT (priv->buffer), (gpointer *) &priv->buffer);
    }
}
 */
