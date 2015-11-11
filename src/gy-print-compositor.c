/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * gy-print-compositor.c
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
 *
 * gy-print-compositor.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-print-compositor.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * Wiekszość tego kodu pochodzi z biblioteki GtkSourceView z pliku gtksoureprintcompositor.c */
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include "gy-print-compositor.h"

#undef ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG(x) (x)
#else
#define DEBUG(x)
#endif

#define DEFAULT_NAME_FONT		"Normal 10"
#define SEPARATOR_LINE_WIDTH 		0.7
#define HEADER_FOOTER_SIZE_FACTOR	2.2
#define PAGINATION_CHUNK_SIZE		3

#define RGB16(x) ((guint16) (0.5 + CLAMP ((x), 0., 1.) * 65535))

typedef struct _GyPrintCompositorPrivate GyPrintCompositorPrivate;

enum
{
  PROP_0,
  PROP_BUFFER,
  PROP_WRAP_MODE,
  PROP_BODY_FONT_NAME,
  PROP_HEADER_FONT_NAME,
  PROP_FOOTER_FONT_NAME,
  PROP_N_PAGES
};

typedef enum
{
  INIT,
  PAGINATING,
  DONE
} PaginatorState;

struct _GyPrintCompositorPrivate
{
  GtkTextBuffer *buffer;
  GtkWrapMode wrap_mode;

  PangoFontDescription *body_font;
  PangoFontDescription *header_font;
  PangoFontDescription *footer_font;

  /* Paper size, stored in pointes */
  gdouble paper_width;
  gdouble paper_height;

  /* Margins are stored in mm */
  gdouble margin_top;
  gdouble margin_bottom;
  gdouble margin_left;
  gdouble margin_right;

  /*State*/
  PaginatorState state;

  GArray *pages;

  guint paginated_lines;
  gint n_pages;
  gint current_page;

  /* Stored in pointes */
  gdouble header_height;
  gdouble footer_height;
  gdouble footer_font_descent;

  /* layoout objects */
  PangoLayout *layout;
  PangoLayout *header_layout;
  PangoLayout *footer_layout;

  gdouble real_margin_top;
  gdouble real_margin_bottom;
  gdouble real_margin_left;
  gdouble real_margin_right;

  gdouble page_margin_top;
  gdouble page_margin_left;

  PangoLanguage *language;
  GtkTextMark *pagination_mark;
};

G_DEFINE_TYPE_WITH_PRIVATE (GyPrintCompositor, gy_print_compositor, G_TYPE_OBJECT);

#define MM_PER_INCH	25.4
#define POINTS_PER_INCH	72

static gdouble G_GNUC_UNUSED
convert_to_mm (gdouble len,
	       GtkUnit unit) 
{
  switch (unit)
  {
    case GTK_UNIT_MM:
      return len;
    case GTK_UNIT_INCH:
      return len * MM_PER_INCH;
    case GTK_UNIT_PIXEL:
      g_warning ("Unsupported unit");
    case GTK_UNIT_POINTS:
      return len * (MM_PER_INCH / POINTS_PER_INCH);
  }
}

static gdouble
convert_from_mm (gdouble len,
		 GtkUnit unit)
{
  switch (unit)
  {
    case GTK_UNIT_MM:
      return len;
    case GTK_UNIT_INCH:
      return len / MM_PER_INCH;
    case GTK_UNIT_PIXEL:
      g_warning ("Unsupported unit");
    case GTK_UNIT_POINTS:
      return len / (MM_PER_INCH / POINTS_PER_INCH);
  }
}

static gboolean
set_font_description_from_name (GyPrintCompositor      *compositor,
                                PangoFontDescription  **font,
                                const gchar            *font_name)
{
  PangoFontDescription *new = NULL;
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  if (font_name != NULL)
    {
      new = pango_font_description_from_string (font_name);
    }
  else
    {
      g_return_val_if_fail (priv->body_font != NULL, FALSE);
      new = pango_font_description_copy (priv->body_font);
    }

  if (*font == NULL || !pango_font_description_equal (*font, new))
    {
      if (*font != NULL)
        pango_font_description_free (*font);

      *font = new;
      return TRUE;
    }
  else
    {
      pango_font_description_free (new);
      return FALSE;
    }
}

static void
gy_print_compositor_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GyPrintCompositor *compositor = GY_PRINT_COMPOSITOR (object);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  g_return_if_fail (GY_IS_PRINT_COMPOSITOR (compositor));

  switch (prop_id)
  {
    case PROP_BUFFER:
      g_value_set_object (value, priv->buffer);
      break;
    case PROP_WRAP_MODE:
      g_value_set_enum (value, gy_print_compositor_get_wrap_mode (compositor));
      break;
    case PROP_BODY_FONT_NAME:
      g_value_set_string (value, gy_print_compositor_get_body_font_name (compositor));
      break;
    case PROP_HEADER_FONT_NAME:
      g_value_set_string (value, gy_print_compositor_get_header_font_name (compositor));
      break;
    case PROP_FOOTER_FONT_NAME:
      g_value_set_string (value, gy_print_compositor_get_footer_font_name (compositor));
      break;
    case PROP_N_PAGES:
      g_value_set_int (value, gy_print_compositor_get_n_pages (compositor));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gy_print_compositor_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GyPrintCompositor *compositor = GY_PRINT_COMPOSITOR (object);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  g_return_if_fail (GY_IS_PRINT_COMPOSITOR (compositor));

  switch (prop_id)
  {
    case PROP_BUFFER:
      priv->buffer = GTK_TEXT_BUFFER (g_value_dup_object (value));
      break;
    case PROP_WRAP_MODE:
      gy_print_compositor_set_wrap_mode (compositor, 
                                         g_value_get_enum (value));
      break;
    case PROP_BODY_FONT_NAME:
      gy_print_compositor_set_body_font_name (compositor, 
                                              g_value_get_string (value));
      break;
    case PROP_HEADER_FONT_NAME:
      gy_print_compositor_set_header_font_name (compositor, 
                                                g_value_get_string (value));
      break;
    case PROP_FOOTER_FONT_NAME:
      gy_print_compositor_set_footer_font_name (compositor, 
                                                g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gy_print_compositor_init (GyPrintCompositor *compositor)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  priv->buffer = NULL;
  priv->wrap_mode = GTK_WRAP_NONE;

  priv->body_font = pango_font_description_from_string (DEFAULT_NAME_FONT);
  priv->header_font = NULL;
  priv->footer_font = NULL;
  
  priv->paper_width = 0.0;
  priv->paper_height = 0.0;

  priv->margin_top = 0.0;
  priv->margin_bottom = 0.0;
  priv->margin_left = 0.0;
  priv->margin_right = 0.0;

  priv->state = INIT;
  priv->pages = NULL;

  priv->paginated_lines = 0;
  priv->n_pages = -1;
  priv->current_page = -1;

  priv->layout = NULL;
  priv->header_layout = NULL;
  priv->footer_layout = NULL;

  priv->language = gtk_get_default_language ();

  priv->header_height = -1.0;
  priv->footer_height = -1.0;

  priv->pagination_mark = NULL;
}

static void
gy_print_compositor_dispose (GObject *object)
{
  GyPrintCompositor *compositor = GY_PRINT_COMPOSITOR (object);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  if (priv->buffer)
    g_clear_object (&priv->buffer);
  G_OBJECT_CLASS (gy_print_compositor_parent_class)->dispose (object);
}

static void
gy_print_compositor_finalize (GObject *object)
{
  GyPrintCompositor *compositor = GY_PRINT_COMPOSITOR (object);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  if (priv->pages)
    g_array_free (priv->pages, TRUE);
  
  if (priv->layout)
    g_object_unref (priv->layout);
  
  if (priv->header_layout)
    g_object_unref (priv->header_layout);
  
  if (priv->footer_layout)
    g_object_unref (priv->footer_layout);
  
  if (priv->body_font)
    pango_font_description_free (priv->body_font);

  if (priv->header_font)
    pango_font_description_free (priv->header_font);

  if (priv->footer_font)
    pango_font_description_free (priv->footer_font);
  
  G_OBJECT_CLASS (gy_print_compositor_parent_class)->finalize (object);
}

static void
gy_print_compositor_class_init (GyPrintCompositorClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = gy_print_compositor_get_property;
  object_class->set_property = gy_print_compositor_set_property;
  object_class->dispose = gy_print_compositor_dispose;
  object_class->finalize = gy_print_compositor_finalize;

  g_object_class_install_property (object_class,
                                   PROP_BUFFER,
                                   g_param_spec_object ("buffer",
                                                        "Program Buffer",
                                                        "The GtkTextBuffer object to print",
                                                        GTK_TYPE_TEXT_BUFFER,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class,
                                   PROP_WRAP_MODE,
                                   g_param_spec_enum ("wrap-mode",
                                                      "Wrap Mode",
                                                      "Whether to wrap lines never, at word boundaries, or at character boundaries.",
                                                      GTK_TYPE_WRAP_MODE,
                                                      GTK_WRAP_NONE,
                                                      G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_HEADER_FONT_NAME,
                                   g_param_spec_string ("header-font-name",
                                                        "Header Font Name",
                                                        "Name of the font to use for the page header (e.g. \"Normal 10\")",
                                                        NULL, G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_FOOTER_FONT_NAME,
                                   g_param_spec_string ("foother-font-name",
                                                        "Foother Font Name",
                                                        "Name of the font to use for the page footer (e.g. \"Normal 10\")",
                                                        NULL, G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_BODY_FONT_NAME,
                                   g_param_spec_string ("body-font-name",
                                                        "Body Font Name",
                                                        "Name of the font to use for the text body (e.g. \"Normal 10\")",
                                                        NULL, G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_N_PAGES,
                                   g_param_spec_int ("n-pages",
                                                     "Number of pages",
                                                     "The number of pages in the document (-1 means the document has not been completely paginated).",
                                                     -1, G_MAXINT, -1,
                                                     G_PARAM_READABLE));
}

static inline gdouble
get_text_width (GyPrintCompositor *compositor)
{
  gdouble w;
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  w = priv->paper_width - priv->real_margin_left - priv->real_margin_right;

  if (w < convert_from_mm ( 50, GTK_UNIT_POINTS))
  {
    g_warning ("Printable page width too little.");
    return convert_from_mm (50, GTK_UNIT_POINTS);
  }

  return w;
}

static inline gdouble
get_text_height (GyPrintCompositor *compositor)
{
  gdouble h;
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  h = priv->paper_height - priv->real_margin_top - priv->real_margin_bottom -
      priv->header_height - priv->footer_height;

  if (h < convert_from_mm ( 50, GTK_UNIT_POINTS))
  {
    g_warning ("Printable page width too little.");
    return convert_from_mm (50, GTK_UNIT_POINTS);
  }

  return h;
}

static inline void
get_layout_size (PangoLayout *layout,
                 gdouble     *width,
                 gdouble     *height)
{
  PangoRectangle rect;

  pango_layout_get_extents (layout, NULL, &rect);

  if (width)
    *width = (gdouble) rect.width / (double) PANGO_SCALE;

  if (height)
    *height = (gdouble) rect.height / (double) PANGO_SCALE;
}

static inline gdouble 
get_text_y (GyPrintCompositor *compositor)
{
  gdouble y;
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  y = priv->real_margin_top + priv->header_height;

  return y;
}

static GSList *
get_iters_attrs (GyPrintCompositor *compositor G_GNUC_UNUSED,
                 GtkTextIter       *iter,
                 GtkTextIter       *limit)
{
  GSList *attrs = NULL, *tags = NULL;
  PangoAttribute *bg = NULL, *fg = NULL, *style = NULL, *ul = NULL;
  PangoAttribute *weight = NULL, *size = NULL, *rise = NULL, *family = NULL;
  PangoAttribute *scale = NULL;

  tags = gtk_text_iter_get_tags (iter);
  gtk_text_iter_forward_to_tag_toggle (iter, NULL);

  if (gtk_text_iter_compare (iter, limit) > 0)
    *iter = *limit;

  while (tags)
  {
    GtkTextTag *tag;
    gboolean bg_set, fg_set, style_set, ul_set;
    gboolean weight_set, size_set, rise_set, family_set;
    gboolean scale_set;

    tag = tags->data;
    tags = g_slist_delete_link (tags, tags);

    g_object_get (tag,
                  "background-set", &bg_set,
                  "foreground-set", &fg_set,
                  "style-set", &style_set,
                  "underline-set", &ul_set,
                  "weight-set", &weight_set,
                  "size-set", &size_set,
                  "rise-set", &rise_set,
                  "family-set", &family_set,
                  "scale-set", &scale_set,
                  NULL);
    if (bg_set)
    {
      GdkRGBA *color = NULL;
      if (bg) pango_attribute_destroy (bg);
      g_object_get (tag, "background-rgba", &color, NULL);
      bg = pango_attr_background_new (RGB16(color->red), 
                                      RGB16(color->green),
                                      RGB16(color->blue));
      gdk_rgba_free (color);
    }

    if (fg_set)
    {
      GdkRGBA *color = NULL;
      if (fg) pango_attribute_destroy (fg);
      g_object_get (tag, "foreground-rgba", &color, NULL);
      fg = pango_attr_foreground_new (RGB16(color->red), 
                                      RGB16(color->green),
                                      RGB16(color->blue));
      gdk_rgba_free (color);
    }

    if (style_set)
    {
      PangoStyle style_value;
      if (style) pango_attribute_destroy (style);
      g_object_get (tag, "style", &style_value, NULL);
      style = pango_attr_style_new (style_value);
    }

    if (ul_set)
    {
      PangoUnderline underline;
      if (ul) pango_attribute_destroy (ul);
      g_object_get (tag, "underline", &underline, NULL);
      ul = pango_attr_underline_new (underline);
    }

    if (weight_set)
    {
      PangoWeight weight_value;
      if (weight) pango_attribute_destroy (weight);
      g_object_get (tag, "weight", &weight_value, NULL);
      weight = pango_attr_weight_new (weight_value);
    }

    if (size_set)
    {
      gint size_value;
      if (size) pango_attribute_destroy (size);
      g_object_get (tag, "size", &size_value, NULL);
      size = pango_attr_size_new (size_value);
    }

    if (rise_set)
    {
      gint rise_value;
      if (rise) pango_attribute_destroy (rise);
      g_object_get (tag, "rise", &rise_value, NULL);
      rise = pango_attr_rise_new (rise_value);
    }

    if (family_set)
    {
      gchar *family_value = NULL;
      if (family) pango_attribute_destroy (family);
      g_object_get (tag, "family", &family_value, NULL);
      family = pango_attr_family_new (family_value);
      g_free (family_value);
    }

    if (scale_set)
    {
      gdouble scale_value;
      if (scale) pango_attribute_destroy (scale);
      g_object_get (tag, "scale", &scale_value, NULL);
      scale = pango_attr_scale_new (scale_value);
    }
  }

  if (bg)
    attrs = g_slist_prepend (attrs, bg);
  if (fg)
    attrs = g_slist_prepend (attrs, fg);
  if (style)
    attrs = g_slist_prepend (attrs, style);
  if (ul)
    attrs = g_slist_prepend (attrs, ul);
  if (weight)
    attrs = g_slist_prepend (attrs, weight);
  if (size)
    attrs = g_slist_prepend (attrs, size);
  if (rise)
    attrs = g_slist_prepend (attrs, rise);
  if (family)
    attrs = g_slist_prepend (attrs, family);
  if (scale)
    attrs = g_slist_prepend (attrs, scale);
  
  return attrs;
}

static void
setup_pango_layouts (GyPrintCompositor *compositor,
                     GtkPrintContext   *context)
{
  PangoLayout *layout = NULL;
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  /* Layout for the text */
  layout = gtk_print_context_create_pango_layout (context);
  pango_layout_set_font_description (layout, priv->body_font);

  switch (priv->wrap_mode)
  {
    case GTK_WRAP_CHAR:
      pango_layout_set_wrap (layout, PANGO_WRAP_CHAR);
      break;
    case GTK_WRAP_WORD:
      pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
      break;
    case GTK_WRAP_WORD_CHAR:
      pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
      break;
    case GTK_WRAP_NONE:
      pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
      break;
  }

  g_return_if_fail (priv->layout == NULL);
  priv->layout = layout;

  /* Layout for the header */
  layout = gtk_print_context_create_pango_layout (context);

  if (priv->header_font == NULL)
    priv->header_font = pango_font_description_copy_static (priv->body_font);

  pango_layout_set_font_description (layout, priv->header_font);
  g_return_if_fail (priv->header_layout == NULL);
  priv->header_layout = layout;

  /* Layout for the footer */
  layout = gtk_print_context_create_pango_layout (context);

  if (priv->footer_font == NULL)
    priv->footer_font = pango_font_description_copy_static (priv->body_font);

  pango_layout_set_font_description (layout, priv->footer_font);
  g_return_if_fail (priv->footer_layout == NULL);
  priv->footer_layout = layout;
}

static gdouble
calculate_header_footer_height (GyPrintCompositor    *compositor,
                                GtkPrintContext      *context,
                                PangoFontDescription *font,
                                gdouble              *d)
{
  PangoContext *pango_context;
  PangoFontMetrics *font_metrics;
  gdouble ascent, descent;
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  pango_context = gtk_print_context_create_pango_context (context);
  pango_context_set_font_description (pango_context, font);

  font_metrics = pango_context_get_metrics (pango_context,
                                            font,
                                            priv->language);
  ascent = (gdouble) pango_font_metrics_get_ascent (font_metrics) / PANGO_SCALE;
  descent = (gdouble) pango_font_metrics_get_descent (font_metrics) / PANGO_SCALE;
  pango_font_metrics_unref (font_metrics);
  g_object_unref (pango_context);

  if (d != NULL)
    *d = descent;

  return HEADER_FOOTER_SIZE_FACTOR * (ascent + descent);
}

static void
calculate_header_height (GyPrintCompositor *compositor,
                         GtkPrintContext   *context)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  g_return_if_fail (priv->header_font != NULL);

  priv->header_height = calculate_header_footer_height (compositor,
                                                        context,
                                                        priv->header_font, NULL);
  DEBUG ({
      g_debug ("header_height: %f pointes :: %f mm",
               priv->header_height,
               convert_to_mm (priv->header_height, GTK_UNIT_POINTS));
      });
}

static void
calculate_footer_height (GyPrintCompositor  *compositor,
                         GtkPrintContext    *context)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  g_return_if_fail (priv->footer_font != NULL);

  priv->footer_height = calculate_header_footer_height (compositor,
                                                        context,
                                                        priv->footer_font, NULL);
  DEBUG ({
      g_debug ("footer_height: %f pointes :: %f mm",
               priv->footer_height,
               convert_to_mm (priv->footer_height, GTK_UNIT_POINTS));

  });
}

static void
calculate_page_size_and_margins (GyPrintCompositor *compositor,
                                 GtkPrintContext   *context)
{
  GtkPageSetup *page_setup;
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  g_return_if_fail (priv->header_height >= 0.0);
  g_return_if_fail (priv->footer_height >= 0.0);

  page_setup = gtk_print_context_get_page_setup (context);

  priv->page_margin_top = gtk_page_setup_get_top_margin (page_setup,
                                                         GTK_UNIT_POINTS);
  priv->page_margin_left = gtk_page_setup_get_left_margin (page_setup,
                                                           GTK_UNIT_POINTS);

  priv->real_margin_top = MAX (priv->page_margin_top,
                               convert_from_mm (priv->margin_top, GTK_UNIT_POINTS));
  priv->real_margin_bottom = MAX (gtk_page_setup_get_bottom_margin (page_setup, GTK_UNIT_POINTS),
                                  convert_from_mm (priv->margin_bottom, GTK_UNIT_POINTS));
  priv->real_margin_left = MAX (priv->page_margin_left,
                                convert_from_mm (priv->margin_left, GTK_UNIT_POINTS));
  priv->real_margin_right = MAX (gtk_page_setup_get_right_margin (page_setup, GTK_UNIT_POINTS),
                                 convert_from_mm (priv->margin_right, GTK_UNIT_POINTS));

  DEBUG ({
      g_debug ("real_margin_top: %f points :: %f mm",
               priv->real_margin_top,
               convert_to_mm (priv->real_margin_top, GTK_UNIT_POINTS));
      g_debug ("real_margin_botton: %f points :: %f mm",
               priv->real_margin_bottom,
               convert_to_mm (priv->real_margin_bottom, GTK_UNIT_POINTS));
      g_debug ("real_margin_left: %f points :: %f mm",
               priv->real_margin_left,
               convert_to_mm (priv->real_margin_left, GTK_UNIT_POINTS));
      g_debug ("real_margin_right: %f points :: %f mm",
               priv->real_margin_right,
               convert_to_mm (priv->real_margin_right, GTK_UNIT_POINTS));
      });

  priv->paper_width = gtk_page_setup_get_paper_width (page_setup, GTK_UNIT_POINTS);
  priv->paper_height = gtk_page_setup_get_paper_height (page_setup, GTK_UNIT_POINTS);

  DEBUG ({
      gdouble text_width;
      gdouble text_height;
      g_debug ("paper_width: %f points (%f mm)",
               priv->paper_width,
               convert_to_mm (priv->paper_width, GTK_UNIT_POINTS));
      g_debug ("paper_heigth: %f points (%f mm)",
               priv->paper_height,
               convert_to_mm (priv->paper_height, GTK_UNIT_POINTS));
      text_width = get_text_width (compositor);
      text_height = get_text_height (compositor);
      g_debug ("text_width: %f points (%f mm)", 
               text_width, convert_to_mm (text_width, GTK_UNIT_POINTS));
      g_debug ("text_height: %f points (%f mm)", 
               text_height, convert_to_mm (text_height, GTK_UNIT_POINTS));

  });
}

static inline void
set_pango_layouts_width (GyPrintCompositor *compositor)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  g_return_if_fail (priv->layout != NULL);

  pango_layout_set_width (priv->layout,
                          get_text_width (compositor) * PANGO_SCALE);
}

static gboolean
is_empty_line (const gchar *text)
{
  if (*text != '\0')
  {
    const gchar *p;
    for (p = text; p!= NULL; p = g_utf8_next_char (p))
    {
      if (!g_unichar_isspace (*p))
      {
        return FALSE;
      }
    }
  }
  return FALSE;
}

static void
layout_paragraph (GyPrintCompositor *compositor,
                  GtkTextIter       *start,
                  GtkTextIter       *end)
{
  gchar *text;

  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  text = gtk_text_iter_get_slice (start, end);

  if (gtk_text_iter_ends_line (start) ||
      is_empty_line (text))
  {
    pango_layout_set_text (priv->layout, " ", 1);
    g_free (text);
    return;
  }

  pango_layout_set_text (priv->layout, text, -1);
  g_free (text);

  if (1) /* Na przyszłość zrobić możliwość włączania i wyłaczania kolorowania. */
  {
    PangoAttrList *attr_list = NULL;
    GtkTextIter segm_start, segm_end;
    gint start_index;

    segm_start = *start;
    start_index = gtk_text_iter_get_line_index (start);

    while (gtk_text_iter_compare (&segm_start, end) < 0)
    {
      GSList *attrs = NULL;
      gint si, ei;
      segm_end = segm_start;
      attrs = get_iters_attrs (compositor, &segm_end, end);

      if (attrs)
      {
        si = gtk_text_iter_get_line_index (&segm_start) - start_index;
        ei = gtk_text_iter_get_line_index (&segm_end) - start_index;
      }

      while (attrs)
      {
        PangoAttribute *a = attrs->data;
        a->start_index = si;
        a->end_index = ei;

        if (!attr_list)
          attr_list = pango_attr_list_new ();

        pango_attr_list_insert (attr_list, a);

        attrs = g_slist_delete_link (attrs, attrs);
      }
      segm_start = segm_end;

    }
    pango_layout_set_attributes (priv->layout, attr_list);

    if (attr_list)
      pango_attr_list_unref (attr_list);
  }
}

static void
print_header_string (GyPrintCompositor *compositor,
                     cairo_t           *cr,
                     PangoAlignment     alignment,
                     const gchar       *text)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);
  if (text != NULL)
  {
    PangoLayoutLine *line;
    gdouble baseline;
    PangoLayoutIter *iter;

    gdouble layout_width;
    gdouble layout_height;
    gdouble header_width;
    gdouble x;

    header_width = priv->paper_width - priv->real_margin_left - priv->real_margin_right;
    pango_layout_set_text (priv->header_layout, text, -1);

    iter = pango_layout_get_iter (priv->header_layout);
    baseline = (gdouble) pango_layout_iter_get_baseline (iter) / (gdouble) PANGO_SCALE;

    get_layout_size (priv->header_layout, &layout_width, &layout_height);

    switch (alignment)
    {
      case PANGO_ALIGN_RIGHT:
      x = priv->real_margin_left + header_width - layout_width;
      break;

      case PANGO_ALIGN_CENTER:
      x = priv->real_margin_left + header_width / 2 - layout_width / 2;
      break;

      case PANGO_ALIGN_LEFT:
      default:
      x = priv->real_margin_left;
      break;
    }

    DEBUG ({
           cairo_save (cr);
           cairo_set_line_width (cr, 1.);
           cairo_set_source_rgb (cr, 0., 0., 1.);
           cairo_rectangle (cr, x,
                            priv->real_margin_top,
                            layout_width,
                            layout_height);
           cairo_stroke (cr);
           cairo_restore (cr);

    });
    line = pango_layout_iter_get_line_readonly (iter);

    cairo_move_to (cr, x, priv->real_margin_top + baseline);
    pango_cairo_show_layout_line (cr, line);
    pango_layout_iter_free (iter);
  }
}

static void
print_header (GyPrintCompositor *compositor,
              cairo_t           *cr)
{
  gchar *time_string = NULL;
  gchar *page_string = NULL;
  time_t time_value;
  struct tm *local_time;
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  time (&time_value);
  local_time = localtime (&time_value);
  time_string = asctime (local_time);

  pango_cairo_update_layout (cr, priv->header_layout);

  print_header_string (compositor, cr, PANGO_ALIGN_LEFT, time_string);

  page_string = g_strdup_printf ("Page %d to %d", priv->current_page + 1, priv->n_pages);

  print_header_string (compositor, cr, PANGO_ALIGN_RIGHT, page_string);

  g_free (page_string);
}

/********* PUBLIC METHOD ********/
/**::GETTERS::**/
gchar *
gy_print_compositor_get_body_font_name (GyPrintCompositor *compositor)
{
  g_return_val_if_fail (GY_IS_PRINT_COMPOSITOR (compositor), NULL);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  return pango_font_description_to_string (priv->body_font);
}

gchar *
gy_print_compositor_get_header_font_name (GyPrintCompositor *compositor)
{
  g_return_val_if_fail (GY_IS_PRINT_COMPOSITOR (compositor), NULL);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  return pango_font_description_to_string (priv->header_font);
}

gchar *
gy_print_compositor_get_footer_font_name (GyPrintCompositor *compositor)
{
  g_return_val_if_fail (GY_IS_PRINT_COMPOSITOR (compositor), NULL);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  return pango_font_description_to_string (priv->footer_font);
}

gint
gy_print_compositor_get_n_pages (GyPrintCompositor *compositor)
{
  g_return_val_if_fail (GY_IS_PRINT_COMPOSITOR (compositor), -1);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  if (priv->state != DONE)
    return -1;

  return priv->n_pages;
}

GtkWrapMode
gy_print_compositor_get_wrap_mode (GyPrintCompositor *compositor)
{
  g_return_val_if_fail (GY_IS_PRINT_COMPOSITOR (compositor), GTK_WRAP_NONE);
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  return priv->wrap_mode;
}

/**::SETTERS::**/
void
gy_print_compositor_set_wrap_mode (GyPrintCompositor *compositor,
                                   GtkWrapMode        wrap_mode)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  g_return_if_fail (GY_IS_PRINT_COMPOSITOR (compositor));
  g_return_if_fail (priv->state == INIT);

  if (wrap_mode == priv->wrap_mode)
    return;

  priv->wrap_mode = wrap_mode;

  g_object_notify (G_OBJECT (compositor), "wrap-mode");
}

void
gy_print_compositor_set_body_font_name (GyPrintCompositor *compositor,
                                        const gchar       *font_name)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);
  g_return_if_fail (GY_IS_PRINT_COMPOSITOR (compositor));
  g_return_if_fail (priv->state == INIT);

  if (set_font_description_from_name (compositor,
                                      &priv->body_font,
                                      font_name))
  {
    g_object_notify (G_OBJECT (compositor), "body-font-name");
  }
}

void
gy_print_compositor_set_header_font_name (GyPrintCompositor *compositor,
                                          const gchar       *font_name)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);
  g_return_if_fail (GY_IS_PRINT_COMPOSITOR (compositor));
  g_return_if_fail (priv->state == INIT);

  if (set_font_description_from_name (compositor,
                                      &priv->header_font,
                                      font_name))
  {
    g_object_notify (G_OBJECT (compositor), "header-font-name");
  }
}

void
gy_print_compositor_set_footer_font_name (GyPrintCompositor *compositor,
                                          const gchar       *font_name)
{
  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);
  g_return_if_fail (GY_IS_PRINT_COMPOSITOR (compositor));
  g_return_if_fail (priv->state == INIT);

  if (set_font_description_from_name (compositor,
                                      &priv->footer_font,
                                      font_name))
  {
    g_object_notify (G_OBJECT (compositor), "footer-font-name");
  }
}

/*::CREATE::*/
GyPrintCompositor *
gy_print_compositor_new_from_view (GtkTextView *view)
{
  gchar *font_name = NULL;
  GtkTextBuffer *buffer = NULL;
  PangoContext *pango_context = NULL;
  PangoFontDescription *font_desc = NULL;
  GyPrintCompositor *compositor = NULL;

  g_return_val_if_fail (GTK_TEXT_VIEW (view), NULL);

  buffer = gtk_text_view_get_buffer (view);
  pango_context = gtk_widget_get_pango_context (GTK_WIDGET (view));
  font_desc = pango_context_get_font_description (pango_context);
  font_name = pango_font_description_to_string (font_desc);

  compositor = GY_PRINT_COMPOSITOR (
  g_object_new (GY_TYPE_PRINT_COMPOSITOR,
                "buffer", buffer,
                "body-font-name", font_name,
                "wrap-mode", gtk_text_view_get_wrap_mode (view), NULL));
  g_free (font_name);
  return compositor;
}

/*::PUBLIC METHOD::*/
gboolean
gy_print_compositor_paginate (GyPrintCompositor *compositor,
                              GtkPrintContext   *context)
{
  GtkTextIter start, end;
  gint page_start_offset;
  gdouble text_height;
  gdouble cur_height;

  gboolean done = FALSE;
  gint pages_count;

  g_return_val_if_fail (GY_IS_PRINT_COMPOSITOR (compositor), TRUE);
  g_return_val_if_fail (GTK_IS_PRINT_CONTEXT (context), TRUE);

  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  if (priv->state == DONE)
    return TRUE;

  if (priv->state == INIT) /**1**/
  {
    g_return_val_if_fail (priv->pages == NULL, TRUE);

    priv->pages = g_array_new (FALSE, FALSE, sizeof (gint));

    setup_pango_layouts (compositor, context);

    calculate_header_height (compositor, context);
    calculate_footer_height (compositor, context);
    calculate_page_size_and_margins (compositor, context);

    set_pango_layouts_width (compositor);

    priv->state = PAGINATING;
  }

  g_return_val_if_fail (priv->state == PAGINATING, FALSE);
  g_return_val_if_fail (priv->layout != NULL, FALSE);

  if (priv->pagination_mark == NULL) /**2**/
  {
    gtk_text_buffer_get_start_iter (priv->buffer, &start);
    priv->pagination_mark = gtk_text_buffer_create_mark (priv->buffer, NULL,
                                                         &start, TRUE);
    page_start_offset = gtk_text_iter_get_offset (&start);
    g_array_append_val (priv->pages, page_start_offset);
  }
  else
  {
    gtk_text_buffer_get_iter_at_mark (priv->buffer, &start,
                                      priv->pagination_mark);
  }

  DEBUG ({
      g_debug ("Start paginating at %d", gtk_text_iter_get_offset (&start));
      });

  gtk_text_buffer_get_end_iter (priv->buffer, &end);
  cur_height = 0;
  text_height = get_text_height (compositor);
  done = gtk_text_iter_compare (&start, &end) >= 0;
  pages_count = 0;

  while (!done && (pages_count < PAGINATION_CHUNK_SIZE))
  {
    gint line_number;
    GtkTextIter line_end;
    gdouble line_height;

    line_number = gtk_text_iter_get_line (&start);
    line_end = start;

    if (!gtk_text_iter_ends_line (&line_end))
      gtk_text_iter_forward_to_line_end (&line_end);

    layout_paragraph (compositor, &start, &line_end);

    get_layout_size (priv->layout, NULL, &line_height);

#define EPS (.1)
    if (cur_height + line_height > text_height + EPS)
    {
      if (priv->wrap_mode != GTK_WRAP_NONE &&
          pango_layout_get_line_count (priv->layout) > 1)
      {
        PangoLayoutIter *layout_iter;
        PangoRectangle logical_rect;
        gboolean is_first_line = TRUE;
        gdouble part_height = 0.0;
        gint idx;

        layout_iter = pango_layout_get_iter (priv->layout);
        do
          {
            double layout_line_height;
            pango_layout_iter_get_line_extents (layout_iter, NULL, &logical_rect);
            layout_line_height = logical_rect.height / PANGO_SCALE;

            if (cur_height + part_height + layout_line_height > text_height + EPS)
              break;

            part_height += layout_line_height;
            is_first_line = FALSE;

          }
        while (pango_layout_iter_next_line (layout_iter));

        idx = gtk_text_iter_get_line_index (&start);
        idx += pango_layout_iter_get_index (layout_iter);
        gtk_text_iter_set_line_index (&start, idx);

        pango_layout_iter_free (layout_iter);
        page_start_offset = gtk_text_iter_get_offset (&start);

        gtk_text_buffer_move_mark (priv->buffer,
                                   priv->pagination_mark, &start);
        if (line_height - part_height > text_height + EPS)
          {
            cur_height = 0;

          }
        else
          {
            cur_height = line_height - part_height;
            gtk_text_iter_forward_line (&start);

          }

      }
      else
        {
          page_start_offset = gtk_text_iter_get_offset (&start);
          gtk_text_buffer_move_mark (priv->buffer,
                                     priv->pagination_mark,
                                     &start);
          cur_height = line_height;
          gtk_text_iter_forward_line (&start);
          cur_height = line_height;
          gtk_text_iter_forward_line (&start);

        }
      g_array_append_val (priv->pages, page_start_offset);
      ++pages_count;

    }
    else
      {
        cur_height += line_height;
        gtk_text_iter_forward_line (&start);

      }
    done = gtk_text_iter_compare (&start, &end) >= 0;

  }
#undef EPS

  if (done)
    {
      priv->state = DONE;
      priv->n_pages = priv->pages->len;
      gtk_text_buffer_delete_mark (priv->buffer, priv->pagination_mark);
      priv->pagination_mark = NULL;

    }

  return (done != FALSE);
}

void
gy_print_compositor_draw_page (GyPrintCompositor *compositor,
                               GtkPrintContext   *context,
                               gint               page_nr)
{
  cairo_t *cr;
  GtkTextIter start, end;
  gint offset;
  gdouble y;

  g_return_if_fail (GY_IS_PRINT_COMPOSITOR (compositor));
  g_return_if_fail (GTK_IS_PRINT_CONTEXT (context));
  g_return_if_fail (page_nr >= 0);

  GyPrintCompositorPrivate *priv = gy_print_compositor_get_instance_private (compositor);

  priv->current_page = page_nr;

  cr = gtk_print_context_get_cairo_context (context);
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_translate (cr, -1 * priv->page_margin_left, -1 * priv->page_margin_top);

  if (1)
    print_header (compositor, cr);
  
  y = get_text_y (compositor);

  DEBUG ({
         cairo_save (cr);
         cairo_set_line_width (cr, 1.);
         cairo_set_source_rgb (cr, 0., 0., 1.);
         cairo_rectangle (cr, priv->real_margin_left,
                          priv->real_margin_top,
                          priv->paper_width - priv->real_margin_left - priv->real_margin_right,
                          priv->paper_height - priv->real_margin_top - priv->real_margin_bottom);
         cairo_stroke (cr);
         cairo_set_source_rgb (cr, 0., 1., 0.);
         cairo_rectangle (cr, priv->real_margin_left,
                          y, get_text_width (compositor),
                          get_text_height (compositor));
         cairo_stroke (cr);
         cairo_set_source_rgb (cr, 1., 0., 0.);
         cairo_rectangle (cr, 0, 0,
                          priv->paper_width - 1,
                          priv->paper_height - 1);
         cairo_stroke (cr);
         cairo_restore (cr);

  });

  g_return_if_fail (priv->layout != NULL);
  pango_cairo_update_layout (cr, priv->layout);

  offset = g_array_index (priv->pages, int, page_nr);
  gtk_text_buffer_get_iter_at_offset (priv->buffer, &start, offset);

  if ((guint) page_nr + 1 < priv->pages->len)
  {
    offset = g_array_index (priv->pages, int, page_nr + 1);
    gtk_text_buffer_get_iter_at_offset (priv->buffer, &end, offset);
  }
  else
  {
    gtk_text_buffer_get_end_iter (priv->buffer, &end);
  }

  while (gtk_text_iter_compare (&start, &end) < 0)
  {
    GtkTextIter line_end;
    gint line_number;
    gdouble line_height;
    gdouble baseline_offset;

    line_end = start;

    if (!gtk_text_iter_ends_line (&line_end))
      gtk_text_iter_forward_to_line_end (&line_end);

    if (gtk_text_iter_compare (&line_end, &end) > 0)
      line_end = end;

    if (gtk_text_iter_starts_line (&start))
    {
      line_number = gtk_text_iter_get_line (&start);
    }
    else
    {
      line_number = -1;
    }

    layout_paragraph (compositor, &start, &line_end);

    get_layout_size (priv->layout, NULL, &line_height);

    baseline_offset = 0;

    cairo_move_to (cr, priv->real_margin_left, y + baseline_offset);
    pango_cairo_show_layout (cr, priv->layout);

    y += line_height;
    gtk_text_iter_forward_line (&start);
  }
}
