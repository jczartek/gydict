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

#include "gy-text-view.h"
#include "gy-utility-func.h"

struct _GyTextView
{
  GtkTextView           parent;

  GtkCssProvider       *css_provider;
  PangoFontDescription *font_desc;
};

G_DEFINE_TYPE (GyTextView, gy_text_view, GTK_TYPE_TEXT_VIEW)

enum {
  PROP_0,
  PROP_FONT_NAME,
  PROP_FONT_DESC,
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

      str = gy_utility_pango_font_description_to_css (self->font_desc);
      css = g_strdup_printf ("GyTextView { %s }", str ?: "");
      gtk_css_provider_load_from_data (self->css_provider, css, -1, NULL);
    }
}

static void
gy_text_view_finalize (GObject *object)
{
  GyTextView *self = (GyTextView *)object;

  G_OBJECT_CLASS (gy_text_view_parent_class)->finalize (object);
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
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_text_view_constructed (GObject *object)
{
  GyTextView *self = GY_TEXT_VIEW (object);
  g_autoptr(GSettings) settings = NULL;

  G_OBJECT_CLASS (gy_text_view_parent_class)->constructed (object);

  settings = g_settings_new ("org.gtk.gydict");
  g_settings_bind (settings, "font-name",
                   self,     "font-name",
                   G_SETTINGS_BIND_GET);
}

static void
gy_text_view_class_init (GyTextViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = gy_text_view_constructed;
  object_class->finalize = gy_text_view_finalize;
  object_class->get_property = gy_text_view_get_property;
  object_class->set_property = gy_text_view_set_property;

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

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);
}

static void
gy_text_view_init (GyTextView *self)
{
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
