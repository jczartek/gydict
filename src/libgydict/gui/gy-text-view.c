/* gy-text-view.c
 *
 * Copyright (C) 2015 Jakub Czartek <kuba@linux.pl>
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

#include <dazzle.h>
#include "gy-text-view.h"
#include "gy-text-buffer.h"
#include "gy-def-list.h"
#include "helpers/gy-utility-func.h"

struct _GyTextView
{
  GtkTextView           parent;

  GtkCssProvider       *css_provider;
  PangoFontDescription *font_desc;

  GdkRGBA background_pattern_color;
  guint background_pattern_grid_set:  1;
};

G_DEFINE_TYPE (GyTextView, gy_text_view, GTK_TYPE_TEXT_VIEW)

enum {
  PROP_0,
  PROP_FONT_NAME,
  PROP_FONT_DESC,
  PROP_BACKGROUND_PATTERN,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static void
gy_text_view_rebuild_css (GyTextView *self)
{
  g_return_if_fail (GY_IS_TEXT_VIEW (self));

  if (!self->css_provider)
    {
      GtkStyleContext *style_context;
      self->css_provider = gtk_css_provider_new ();
      style_context = gtk_widget_get_style_context (GTK_WIDGET (self));
      gtk_style_context_add_provider (style_context,
                                      GTK_STYLE_PROVIDER (self->css_provider),
                                      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

  if (self->font_desc)
    {
      g_autofree gchar *str = NULL;
      g_autofree gchar *css = NULL;

      str = dzl_pango_font_description_to_css (self->font_desc);

#if (GTK_MAJOR_VERSION >= 3) && (GTK_MINOR_VERSION >= 20)
      css = g_strdup_printf ("textview { %s }", str ?: "");
#else
      css = g_strdup_printf ("GyTextView { %s }", str ?: "");
#endif
      gtk_css_provider_load_from_data (self->css_provider, css, -1, NULL);
    }
}

static void
gy_text_view_paint_background_pattern_grid (GyTextView *self,
                                            cairo_t    *cr)
{
  /*
   * NOTE: This code were taken from gtksourcseview.c
   * Plese see: https://github.com/GNOME/gtksourceview/blob/master/gtksourceview/gtksourceview.c
   */

  GdkRectangle clip, vis;
  gdouble x, y;
  PangoContext *context;
  PangoLayout  *layout;
  gint grid_width = 16;
  gint grid_height = 16;

  context = gtk_widget_get_pango_context (GTK_WIDGET (self));
  layout = pango_layout_new (context);
  pango_layout_set_text (layout, "X", 1);
  pango_layout_get_pixel_size (layout, &grid_width, &grid_height);
  g_object_unref (layout);

  /* each character becomes 2 stacked boxes */
  grid_height = MAX (1, grid_height /2);
  grid_width = MAX (1, grid_width);

  cairo_save (cr);
  cairo_set_line_width (cr, 1.0);
  gdk_cairo_get_clip_rectangle (cr, &clip);
  gtk_text_view_get_visible_rect (GTK_TEXT_VIEW (self), &vis);

  gdk_cairo_set_source_rgba (cr, &self->background_pattern_color);

  /*
   * The following constants come from gtktextview.c pixel cache
	 * settings. Sadly, they are not exposed in the public API,
	 * just keep them in sync here. 64 for X, height/2 for Y.
	 */
  x = (grid_width - (vis.x % grid_width)) - (64 / grid_width * grid_width) - grid_width + 2;
  y = (grid_height - (vis.y % grid_height)) - (vis.height / 2 / grid_height * grid_height) - grid_height;

  for (; x <= clip.x + clip.width; x += grid_width)
    {
      cairo_move_to (cr, x + .5, clip.y - .5);
      cairo_line_to (cr, x + .5, clip.y + clip.height - .5);
    }

	for (; y <= clip.y + clip.height; y += grid_height)
    {
      cairo_move_to (cr, clip.x + .5, y - .5);
      cairo_line_to (cr, clip.x + clip.width + .5, y - .5);
    }


	cairo_stroke (cr);
	cairo_restore (cr);
}

static void
gy_text_view_update_background_pattern (GyTextView *self)
{
  GtkSettings *gtk_settings = NULL;
  gboolean prefer_dark_theme = FALSE;
  g_return_if_fail (GY_IS_TEXT_VIEW (self));

  gtk_settings = g_object_ref (gtk_settings_get_default ());

  g_assert (GTK_IS_SETTINGS (gtk_settings));

  g_object_get (gtk_settings,
                "gtk-application-prefer-dark-theme", &prefer_dark_theme,
                NULL);

  if (prefer_dark_theme)
    {
      gdk_rgba_parse (&self->background_pattern_color,
                      "#e7e7e7");
      self->background_pattern_color.alpha = 0.027;
    }
  else
    {
      self->background_pattern_color.red = .125;
      self->background_pattern_color.green = .125;
      self->background_pattern_color.blue = .125;
      self->background_pattern_color.alpha = .025;
    }


  gtk_widget_queue_draw (GTK_WIDGET (self));
  g_object_unref (gtk_settings);
}

static void
gy_text_view__settings_notify_gtk_application_prefer_dark_theme (GyTextView  *self,
                                                                 GParamSpec  *pspec,
                                                                 GtkSettings *settings)
{
  g_return_if_fail (GY_IS_TEXT_VIEW (self));

  gy_text_view_update_background_pattern (self);

}

static void
gy_text_view_draw_layer (GtkTextView      *view,
                         GtkTextViewLayer  layer,
                         cairo_t          *cr)
{
  GyTextView *self = GY_TEXT_VIEW (view);

  cairo_save (cr);

  if (layer == GTK_TEXT_VIEW_LAYER_BELOW)
    {
      if (self->background_pattern_grid_set)
        gy_text_view_paint_background_pattern_grid (self, cr);
    }

  cairo_restore (cr);
}

static void
gy_text_view_realize (GtkWidget *widget)
{
  GtkTextBuffer *buffer = NULL;

  GTK_WIDGET_CLASS (gy_text_view_parent_class)->realize (widget);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
  if (buffer)
    {
      GtkClipboard *cb = NULL;
      cb = gtk_widget_get_clipboard (widget, GDK_SELECTION_PRIMARY);
      gtk_text_buffer_remove_selection_clipboard (buffer, cb);
    }
}

static void
gy_text_view_unrealize (GtkWidget *widget)
{
  GtkTextBuffer *buffer = NULL;
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));

  /*
   * The primary clipboard is being added to the buffer in order to
   * prevent printing a warning through the unrealize function of the parent.
   */
  if (buffer)
    {
      GtkClipboard *cb = NULL;
      cb = gtk_widget_get_clipboard (widget, GDK_SELECTION_PRIMARY);
      gtk_text_buffer_add_selection_clipboard (buffer, cb);
    }

  GTK_WIDGET_CLASS (gy_text_view_parent_class)->unrealize (widget);
}

static void
gy_text_view_event_after_signal (GtkWidget *widget,
                                 GdkEvent  *event,
                                 gpointer   data)
{
  if (event->type == GDK_DOUBLE_BUTTON_PRESS)
    {
      GtkTextBuffer *buffer = NULL;
      GtkTextIter start, end;

      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));

      g_return_if_fail (GTK_IS_TEXT_BUFFER (buffer));

      if (gtk_text_buffer_get_selection_bounds (buffer, &start, &end))
        {
          g_autofree gchar *selected_text = NULL;

          selected_text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

          if ((selected_text != NULL) && (g_utf8_strlen (selected_text, -1)))
            {
              GtkClipboard *cb = NULL;

              cb = gtk_widget_get_clipboard (widget, GDK_SELECTION_PRIMARY);
              gtk_clipboard_set_text (cb, selected_text, -1);
            }
        }
    }
}

static void
gy_text_view_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GyTextView *self = GY_TEXT_VIEW (object);

  switch (prop_id)
    {
    case PROP_FONT_DESC:
      g_value_set_boxed (value, gy_text_view_get_font_desc (self));
      break;
    case PROP_BACKGROUND_PATTERN:
      g_value_set_boolean (value, gy_text_view_get_background_pattern (self));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_text_view_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GyTextView *self = GY_TEXT_VIEW (object);

  switch (prop_id)
    {
    case PROP_FONT_NAME:
      gy_text_view_set_font_name (self, g_value_get_string (value));
      break;
    case PROP_FONT_DESC:
      gy_text_view_set_font_desc (self, g_value_get_boxed (value));
      break;
    case PROP_BACKGROUND_PATTERN:
      gy_text_view_set_background_pattern (self, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_text_view_constructed (GObject *object)
{
  GyTextView *self = GY_TEXT_VIEW (object);
  g_autoptr(GSettings) settings = NULL;
  g_autoptr(GtkSettings) gtk_settings = NULL;

  G_OBJECT_CLASS (gy_text_view_parent_class)->constructed (object);

  settings = g_settings_new ("org.gtk.gydict");
  g_settings_bind (settings, "font-name",
                   self,     "font-name",
                   G_SETTINGS_BIND_GET);
  g_settings_bind (settings, "show-grid-lines",
                   self,     "background-pattern",
                   G_SETTINGS_BIND_DEFAULT);

  gtk_settings = g_object_ref (gtk_settings_get_default ());

  g_signal_connect_swapped (gtk_settings, "notify::gtk-application-prefer-dark-theme",
                            G_CALLBACK (gy_text_view__settings_notify_gtk_application_prefer_dark_theme), self);
  gy_text_view_update_background_pattern (self);
}

static void
gy_text_view_class_init (GyTextViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkTextViewClass *textview_class = GTK_TEXT_VIEW_CLASS (klass);

  object_class->constructed = gy_text_view_constructed;
  object_class->get_property = gy_text_view_get_property;
  object_class->set_property = gy_text_view_set_property;

  widget_class->realize = gy_text_view_realize;
  widget_class->unrealize = gy_text_view_unrealize;

  textview_class->draw_layer = gy_text_view_draw_layer;

  gParamSpecs [PROP_FONT_DESC] =
    g_param_spec_boxed ("font-desc",
                        "Font Description",
                        "The Pango font descritpion to use for rendering text.",
                        PANGO_TYPE_FONT_DESCRIPTION,
                        (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gParamSpecs [PROP_FONT_NAME] =
    g_param_spec_string ("font-name",
                         "Font Name",
                         "The Pango font name to use for rendering source",
                         "Cantarell",
                         (G_PARAM_WRITABLE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS));

  gParamSpecs [PROP_BACKGROUND_PATTERN] =
    g_param_spec_boolean ("background-pattern",
                          "Background Pattern",
                          "Whether to draw the background pattern",
                          FALSE,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);
}

static void
gy_text_view_init (GyTextView *self)
{
  self->background_pattern_grid_set = FALSE;

  g_signal_connect_after (self, "event-after",
                          G_CALLBACK (gy_text_view_event_after_signal), NULL);
}

void
gy_text_view_set_font_desc (GyTextView                 *self,
                            const PangoFontDescription *font_desc)
{
  g_return_if_fail (GY_IS_TEXT_VIEW (self));

  if (font_desc != self->font_desc)
    {
      g_clear_pointer (&self->font_desc, pango_font_description_free);

      if (font_desc)
        self->font_desc = pango_font_description_copy (font_desc);
      else
        self->font_desc = pango_font_description_from_string ("Sans");

      gy_text_view_rebuild_css (self);
    }
}

void
gy_text_view_set_font_name (GyTextView  *self,
                            const gchar *font_name)
{
  PangoFontDescription *font_desc = NULL;

  g_return_if_fail (GY_IS_TEXT_VIEW (self));

  if (font_name)
    font_desc = pango_font_description_from_string (font_name);
  gy_text_view_set_font_desc (self, font_desc);

  if (font_name)
    pango_font_description_free (font_desc);
}

const PangoFontDescription *
gy_text_view_get_font_desc (GyTextView *self)
{
  g_return_val_if_fail (GY_IS_TEXT_VIEW (self), NULL);

  return self->font_desc;
}

void
gy_text_view_set_background_pattern (GyTextView *self,
                                     gboolean    background_pattern)
{
  g_return_if_fail (GY_IS_TEXT_VIEW (self));

  if (self->background_pattern_grid_set != background_pattern)
    {
      self->background_pattern_grid_set = background_pattern;

      gtk_widget_queue_draw (GTK_WIDGET (self));

      g_object_notify_by_pspec (G_OBJECT (self), gParamSpecs[PROP_BACKGROUND_PATTERN]);
    }
}

gboolean
gy_text_view_get_background_pattern (GyTextView *self)
{
  g_return_val_if_fail (GY_IS_TEXT_VIEW (self), FALSE);

  return self->background_pattern_grid_set;
}

void
gy_text_view_clear_buffer (GyTextView *self)
{
  GtkTextBuffer *tb;

  g_return_if_fail (GY_IS_TEXT_VIEW(self));

  tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self));
  gy_text_buffer_clean_buffer (GY_TEXT_BUFFER (tb));
}
