/* gy-text-attribute.c
 *
 * Copyright 2019 Jakub Czartek <kuba@linux.pl>
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
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "gy-text-attribute.h"

struct _GyTextAttribute
{
  guint ref_count;
  guint start_index;
  guint end_index;
  GyTextAttrType type;
  union
    {
      gboolean              attr_bool;
      gint                  attr_int;
      gdouble               attr_float;
      gchar                *attr_string;
      PangoLanguage        *attr_language;
      PangoFontDescription *attr_desc;
      PangoColor            attr_color;
    };
};


G_DEFINE_BOXED_TYPE (GyTextAttribute, gy_text_attribute,
                     gy_text_attribute_copy,
                     gy_text_attribute_unref)

/**
 * gy_text_attribute_new:
 *
 * Creates a new text attribute. It will initialize
 * initially index to %PANGO_ATTR_INDEX_FROM_TEXT_BEGINNING
 * and final index to %PANGO_ATTR_INDEX_TO_TEXT_END
 * such that the text attribute applies to the entire text by default.
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_new ()
{
  GyTextAttribute *attr = g_slice_new0 (GyTextAttribute);

  attr->ref_count = 1;
  attr->type = GY_TEXT_ATTR_INVALID;
  attr->start_index = PANGO_ATTR_INDEX_FROM_TEXT_BEGINNING;
  attr->end_index = PANGO_ATTR_INDEX_TO_TEXT_END;

  return attr;
}

/**
 * gy_text_attribute_ref:
 * @attr: (nullable): a #GyTextAttribute, may be %NULL
 *
 * Increase the reference count of the given text attribute by one.
 *
 * Return value: The text attribute passed in
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_ref (GyTextAttribute *attr)
{

  if (attr == NULL) return NULL;

  g_atomic_int_inc ((int *) &attr->ref_count);

  return attr;
}

/**
 * gy_text_attribute_unref:
 * @attr: (nullable): a #GyTextAttribute, may be %NULL
 *
 * Decrease the reference count of the given text attribute by one.
 * If the result is zero, free the text attribute.
 *
 * Since: 0.6
 **/
void
gy_text_attribute_unref (GyTextAttribute *attr)
{

  if (attr == NULL) return;

  g_return_if_fail (attr->ref_count > 0);

  if (g_atomic_int_dec_and_test ((int *) &attr->ref_count))
    {
      if (attr->type == GY_TEXT_ATTR_FAMILY)
        g_free (attr->attr_string);

      if (attr->type == GY_TEXT_ATTR_FONT_DESC)
        pango_font_description_free (attr->attr_desc);

      if (attr->type == GY_TEXT_ATTR_FONT_FEATURES)
        g_free (attr->attr_string);

      g_slice_free (GyTextAttribute, attr);
    }
}

/**
 * gy_text_attribute_copy:
 * @attr: a #GyTextAttribute
 *
 * Make a copy of an text attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_copy (GyTextAttribute *attr)
{
  GyTextAttribute *new;

  g_return_val_if_fail (attr != NULL, NULL);

  new = gy_text_attribute_new ();

  new->start_index = attr->start_index;
  new->end_index = attr->end_index;
  new->type = attr->type;

  if (attr->type == GY_TEXT_ATTR_FAMILY ||
      attr->type == GY_TEXT_ATTR_FONT_FEATURES)
    new->attr_string = g_strdup (attr->attr_string);

  if (attr->type == GY_TEXT_ATTR_FONT_DESC)
    new->attr_desc = pango_font_description_copy (attr->attr_desc);

  if (attr->type == GY_TEXT_ATTR_LANGUAGE)
    new->attr_language = attr->attr_language;

  if (attr->type == GY_TEXT_ATTR_STYLE ||
      attr->type == GY_TEXT_ATTR_WEIGHT ||
      attr->type == GY_TEXT_ATTR_VARIANT ||
      attr->type == GY_TEXT_ATTR_STRETCH ||
      attr->type == GY_TEXT_ATTR_SIZE ||
      attr->type == GY_TEXT_ATTR_UNDERLINE ||
      attr->type == GY_TEXT_ATTR_RISE ||
      attr->type == GY_TEXT_ATTR_LETTER_SPACING ||
      attr->type == GY_TEXT_ATTR_FOREGROUND_ALPHA ||
      attr->type == GY_TEXT_ATTR_BACKGROUND_ALPHA)
    new->attr_int = attr->attr_int;

  if (attr->type == GY_TEXT_ATTR_STRIKETHROUGH ||
      attr->type == GY_TEXT_ATTR_FALLBACK)
    new->attr_bool = attr->attr_bool;

  if (attr->type == GY_TEXT_ATTR_SCALE)
    new->attr_float = attr->attr_float;

  if (attr->type == GY_TEXT_ATTR_FOREGROUND ||
      attr->type == GY_TEXT_ATTR_BACKGROUND ||
      attr->type == GY_TEXT_ATTR_UNDERLINE_COLOR ||
      attr->type == GY_TEXT_ATTR_STRIKETHROUGH_COLOR)
    {
      new->attr_color.red = attr->attr_color.red;
      new->attr_color.blue = attr->attr_color.blue;
      new->attr_color.green = attr->attr_color.green;
    }

  return new;
}

/**
 * gy_text_attribute_set_start_index:
 * @attr: a text attribute
 * @start_index: initially index of the range in bytes
 *
 * Sets initially index.
 *
 * Since: 0.6
 **/
void
gy_text_attribute_set_start_index (GyTextAttribute *attr,
                                   guint            start_index)
{
  g_return_if_fail (attr != NULL);

  attr->start_index = start_index;
}

/**
 * gy_text_attribute_get_start_index:
 * @attr: a text attribute
 *
 * Gets the initially index of the range.
 *
 * Returns: the initialy index
 *
 * Since: 0.6
 **/
guint
gy_text_attribute_get_start_index (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, 0);

  return attr->start_index;
}

/**
 * gy_text_attribute_set_end_index:
 * @attr: a #GyTextAttribute
 * @end_index: final index of the range in bytes
 *
 * Sets final index. The character at this index
 * is not included in the range.
 *
 * Since: 0.6
 */
void
gy_text_attribute_set_end_index (GyTextAttribute *attr,
                                 guint            end_index)
{
  g_return_if_fail (attr != NULL);

  attr->end_index = end_index;
}

/**
 * gy_text_attribute_get_end_index:
 * @attr: a text attribute
 *
 * Gets the final index of the range.
 *
 * Returns: the final index
 *
 * Since: 0.6
 **/
guint
gy_text_attribute_get_end_index (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, 0);

  return attr->end_index;
}

/**
 * gy_text_attribute_get_boolean:
 * @attr: a text attribute
 *
 * Returns the boolean value of @attr
 *
 * Returns: %TRUE or %FALSE
 *
 * Since: 0.6
 **/
gboolean
gy_text_attribute_get_boolean (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, FALSE);

  return attr->attr_bool;
}

/**
 * gy_text_attribute_get_int:
 * @attr: a text attribute
 *
 * Returns the integer value of @attr
 *
 * Returns: an integer
 *
 * Since: 0.6
 **/
gint
gy_text_attribute_get_int (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, 0);

  return attr->attr_int;
}

/**
 * gy_text_attribute_get_float:
 * @attr: a text attribute
 *
 * Returns the double precision floating point value of @attr
 *
 * Returns:a %gdouble
 *
 * Since: 0.6
 **/
gdouble
gy_text_attribute_get_float (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, 0.0);

  return attr->attr_float;
}

/**
 * gy_text_attribute_get_string:
 * @attr: a text attribute
 *
 * Returns the string value of @attr. The return value remains valid
 * as long as @attr exists.
 *
 * Returns: (transfer none): the constant string
 *
 * Since: 0.6
 **/
const gchar*
gy_text_attribute_get_string (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, NULL);

  return attr->attr_string;
}

/**
 * gy_text_attribute_get_language:
 * @attr: a text attribute
 *
 * Returns the pango language of @attr. The return value remains valid
 * as long as @attr exists.
 *
 * Returns: (transfer none): the pango language
 *
 * Since: 0.6
 */
const PangoLanguage *
gy_text_attribute_get_language (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, NULL);

  return attr->attr_language;
}

/**
 * gy_text_attribute_get_font_desc:
 * @attr: a text attribute
 *
 * Returns the font description of @attr. The return value remains valid
 * as long as @attr exists.
 *
 * Returns: (transfer none): the font description
 *
 * Since: 0.6
 */
const PangoFontDescription *
gy_text_attribute_get_font_desc (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, NULL);

  return attr->attr_desc;
}


/**
 * gy_text_attribute_get_color:
 * @attr: a text attribute
 *
 * Returns the pango color of @attr. The return value remains valid
 * as long as @attr exists.
 *
 * Returns: (transfer none): the pango color
 *
 * Since: 0.6
 */
const PangoColor *
gy_text_attribute_get_color (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, NULL);

  return &attr->attr_color;
}
/**
 * gy_text_attribute_set_attry_type:
 * @attr: a text attribute
 * @type: the type of attribute
 *
 * Sets the type of the given attribute.
 *
 * Since: 0.6
 **/
void
gy_text_attribute_set_attr_type (GyTextAttribute *attr,
                                 GyTextAttrType  type)
{
  g_return_if_fail (attr != NULL);

  attr->type = type;
}

/**
 * gy_text_attribut_get_attr_type:
 * @attr: a text attribute
 *
 * Get the type of the given attribute.
 *
 * Returns: type of the given attribute
 *
 * Since: 0.6
 **/
GyTextAttrType
gy_text_attribute_get_attr_type (GyTextAttribute *attr)
{
  g_return_val_if_fail (attr != NULL, GY_TEXT_ATTR_INVALID);

  return attr->type;
}

/**
 * gy_text_attribute_language_new:
 * @language: language tag
 *
 * Create a new language tag attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_language_new (PangoLanguage *language)
{
  GyTextAttribute * attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_LANGUAGE;
  attr->attr_language = language;

  return attr;
}

/**
 * gy_text_attribute_family_new:
 * @family: the family or comma separated list of families
 *
 * Create a new font family attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_family_new (const gchar *family)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_FAMILY;
  attr->attr_string = g_strdup (family);

  return attr;
}

/**
 * gy_text_attribute_style_new:
 * @style: the slant style
 *
 * Create a new font slant style text attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_style_new (PangoStyle style)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_STYLE;
  attr->attr_int = style;

  return attr;
}

/**
 * gy_text_attribute_weight_new:
 * @weight: the weight
 *
 * Create a new font weight attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_weight_new (PangoWeight weight)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_WEIGHT;
  attr->attr_int = weight;

  return attr;
}

/**
 * gy_text_attribute_variant_new:
 * @variant: the variant
 *
 * Create a new font variant attribute (normal or small caps)
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_variant_new (PangoVariant variant)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_VARIANT;
  attr->attr_int = variant;

  return attr;
}

/**
 * gy_text_attribute_stretch_new:
 * @stretch: the stretch
 *
 * Create a new font stretch attribute
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_stretch_new (PangoStretch stretch)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_STRETCH;
  attr->attr_int = stretch;

  return attr;
}

/**
 * gy_text_attribute_size_new:
 * @size: the font size, in %PANGO_SCALE<!-- -->ths of a point.
 *
 * Create a new font-size attribute in fractional points.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_size_new (gint size)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_SIZE;
  attr->attr_int = size;

  return attr;
}

/**
 * gy_text_attribute_font_desc_new:
 * @desc: the font description
 *
 * Create a new font description attribute. This attribute
 * allows setting family, style, weight, variant, stretch,
 * and size simultaneously.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_font_desc_new (const PangoFontDescription *desc)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_FONT_DESC;
  attr->attr_desc = pango_font_description_copy (desc);

  return attr;
}

/**
 * gy_text_foreground_new:
 * @red: the red value (ranging from 0 to 65535)
 * @green: the green value
 * @blue: the blue value
 *
 * Create a new foreground color attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_foreground_new (guint16 red,
                                  guint16 green,
                                  guint16 blue)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_FOREGROUND;
  attr->attr_color.red   = red;
  attr->attr_color.green = green;
  attr->attr_color.blue  = blue;

  return attr;
}

GyTextAttribute *
gy_text_attribute_foreground_new_from_hex (const gchar *spec)
{
  PangoColor color;

  if (!pango_color_parse (&color, spec))
    return NULL;

  return gy_text_attribute_foreground_new (color.red, color.green, color.blue);
}

/**
 * gy_text_attribute_background_new:
 * @red: the red value (ranging from 0 to 65535)
 * @green: the green value
 * @blue: the blue value
 *
 * Create a new background color attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_background_new (guint16 red,
                                  guint16 green,
                                  guint16 blue)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_BACKGROUND;
  attr->attr_color.red   = red;
  attr->attr_color.green = green;
  attr->attr_color.blue  = blue;

  return attr;
}

GyTextAttribute *
gy_text_attribute_background_new_from_hex (const gchar *spec)
{
  PangoColor color;

  if (!pango_color_parse (&color, spec))
    return NULL;

  return gy_text_attribute_background_new (color.red, color.green, color.blue);
}

/**
 * gy_text_attribute_underline_new:
 * @underline: the underline style.
 *
 * Create a new underline-style attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_underline_new (PangoUnderline underline)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_UNDERLINE;
  attr->attr_int = underline;

  return attr;
}

/**
 * gy_text_attribute_underline_color_new:
 * @red: the red value (ranging from 0 to 65535)
 * @green: the green value
 * @blue: the blue value
 *
 * Create a new underline color attribute. This attribute
 * modifies the color of underlines. If not set, underlines
 * will use the foreground color.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_underline_color_new (guint16 red,
                                       guint16 green,
                                       guint16 blue)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_UNDERLINE_COLOR;
  attr->attr_color.red = red;
  attr->attr_color.green = green;
  attr->attr_color.blue = blue;

  return attr;
}

/**
 * gy_text_attribute_strikethrough_new:
 * @strikethrough: %TRUE if the text should be struck-through.
 *
 * Create a new strike-through attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_strikethrough_new (gboolean strikethrough)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_STRIKETHROUGH;
  attr->attr_bool = strikethrough;

  return attr;
}

/**
 * gy_text_attribute_strikethrough_color_new:
 * @red: the red value (ranging from 0 to 65535)
 * @green: the green value
 * @blue: the blue value
 *
 * Create a new strikethrough color attribute. This attribute
 * modifies the color of strikethrough lines. If not set, strikethrough
 * lines will use the foreground color.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_strikethrough_color_new (guint16 red,
                                           guint16 green,
                                           guint16 blue)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_STRIKETHROUGH_COLOR;
  attr->attr_color.red = red;
  attr->attr_color.green = green;
  attr->attr_color.blue = blue;

  return attr;
}

/**
 * gy_text_attribute_rise_new:
 * @rise: the amount that the text should be displaced vertically,
 *        in Pango units. Positive values displace the text upwards.
 *
 * Create a new baseline displacement attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_rise_new (gint rise)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_RISE;
  attr->attr_int = rise;

  return attr;
}

/**
 * gy_text_attribute_scale_new:
 * @scale_factor: factor to scale the font
 *
 * Create a new font size scale attribute. The base font for the
 * affected text will have its size multiplied by @scale_factor.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribure_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_scale_new (gdouble scale_factor)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_SCALE;
  attr->attr_float = scale_factor;

  return attr;
}

/**
 * gy_text_attribute_fallback_new:
 * @enable_fallback: %TRUE if we should fall back on other fonts
 *                   for characters the active font is missing.
 *
 * Create a new font fallback attribute.
 *
 * If fallback is disabled, characters will only be used from the
 * closest matching font on the system. No fallback will be done to
 * other fonts on the system that might contain the characters in the
 * text.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_fallback_new (gboolean enable_fallback)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_FALLBACK;
  attr->attr_bool = enable_fallback;

  return attr;
}

/**
 * gy_text_attribute_letter_spacing_new:
 * @letter_spacing: amount of extra space to add between graphemes
 *   of the text, in Pango units.
 *
 * Create a new letter-spacing attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_letter_spacing_new (gint letter_spacing)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_LETTER_SPACING;
  attr->attr_int = letter_spacing;

  return attr;
}

/**
 * gy_text_attribute_font_features_new:
 * @features: a string with OpenType font features, in CSS syntax
 *
 * Create a new font features tag attribute.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 **/
GyTextAttribute *
gy_text_attribute_font_features_new (const gchar *features)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_FONT_FEATURES;
  attr->attr_string = g_strdup (features);

  return attr;
}

/**
 * gy_text_attribute_foreground_alpha_new:
 * @alpha: the alpha value, between 1 and 65536
 *
 * Create a new foreground alpha attribute.
 *
 * Return value: (transfer full): the new allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 */
GyTextAttribute *
gy_text_attribute_foreground_alpha_new (guint16 alpha)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_FOREGROUND_ALPHA;
  attr->attr_int = (gint) alpha;

  return attr;
}

/**
 * gy_text_attribute_background_alpha_new:
 * @alpha: the alpha value, between 1 and 65536
 *
 * Create a new background alpha attribute.
 *
 * Return value: (transfer full): the new allocated #GyTextAttribute,
 *               which should be freed with gy_text_attribute_unref().
 *
 * Since: 0.6
 */
GyTextAttribute *
gy_text_attribute_background_alpha_new (guint16 alpha)
{
  GyTextAttribute *attr = gy_text_attribute_new ();

  attr->type = GY_TEXT_ATTR_BACKGROUND_ALPHA;
  attr->attr_int = (gint) alpha;

  return attr;
}

/*
 * Text Attribute List
 */

struct _GyTextAttrList
{
  guint ref_count;
  GSList *attrs;
  GSList *tail;
};


struct _GyTextAttrIterator
{
  GSList *next;
  GList *stack;
  guint start_index;
  guint end_index;
};

G_DEFINE_BOXED_TYPE (GyTextAttrList, gy_text_attr_list,
                     gy_text_attr_list_copy,
                     gy_text_attr_list_unref);

/**
 * gy_text_attr_list_new:
 *
 * Create a new empty text attribute list with a reference count of one.
 *
 * Return value: (transfer full): the newly allocated #GyTextAttrList,
 *               which should be freed with gy_text_attr_list_unref().
 * Since: 0.6
 **/
GyTextAttrList *
gy_text_attr_list_new (void)
{
  GyTextAttrList *list = g_slice_new (GyTextAttrList);

  list->ref_count = 1;
  list->attrs = NULL;
  list->tail = NULL;

  return list;
}

/**
 * gy_text_attr_list_ref:
 * @list: (nullable): a #GyTextAttrList, may be %NULL
 *
 * Increase the reference count of the given text attribute list by one.
 *
 * Return value: The text attribute list passed in
 *
 * Since: 0.6
 **/
GyTextAttrList *
gy_text_attr_list_ref (GyTextAttrList *list)
{

  if (list == NULL) return NULL;

  g_atomic_int_inc ((gint *) &list->ref_count);

  return list;
}

/**
 * gy_text_attr_list_unref:
 * @list: (nullable): a #GyTextAttrList, may be %NULL
 *
 * Decrease the reference count of the given text attribute list by one.
 * If the result is zero, free the text attribute list and
 * the text attributes it contains.
 *
 * Since: 0.6
 **/
void
gy_text_attr_list_unref (GyTextAttrList *list)
{
  if (list == NULL) return;

  g_return_if_fail (list->ref_count > 0);

  if (g_atomic_int_dec_and_test ((gint *) &list->ref_count))
    {
      for (GSList *iter = list->attrs; iter != NULL; iter = iter->next)
        gy_text_attribute_unref (iter->data);

      g_slist_free (list->attrs);
      g_slice_free (GyTextAttrList, list);
    }
}

/**
 * gy_text_attr_list_copy:
 * @list: (nullable): a #GyTextAttrList, may be %NULL
 *
 * Copy @list and return an identical new list.
 *
 * Return value: (nullable): the newly allocated #GyTextAttrList, with a
 *               reference count of one, which should
 *               be freed with gy_attr_list_unref().
 *               Returns %NULL if @list was %NULL.
 *
 * Since: 0.6
 **/
GyTextAttrList *
gy_text_attr_list_copy (GyTextAttrList *list)
{
  GyTextAttrList *new;

  if (list == NULL) return NULL;

  new = gy_text_attr_list_new ();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
  new->attrs = g_slist_copy_deep (list->attrs, (GCopyFunc) gy_text_attribute_copy, NULL);
#pragma GCC diagnostic pop
  new->tail = g_slist_last (new->attrs);

  return new;
}

static void
gy_text_attr_list_insert_internal (GyTextAttrList  *list,
                                   GyTextAttribute *attr,
                                   gboolean         before)
{
  guint start_index = attr->start_index;

  if (!list->attrs)
    {
      list->attrs = g_slist_prepend (NULL, attr);
      list->tail = list->attrs;
    }
  else if (((GyTextAttribute *)list->tail->data)->start_index < start_index ||
           (!before && ((GyTextAttribute *)list->tail->data)->start_index == start_index))
    {
      list->tail = g_slist_append (list->tail, attr);
      list->tail = list->tail->next;
      g_assert (list->tail);
    }
  else
    {
      GSList *tmp_list, *prev, *link;
      prev = link = NULL;
      tmp_list = list->attrs;

      while (TRUE)
        {
          GyTextAttribute *tmp_attr = tmp_list->data;

          if (tmp_attr->start_index > start_index ||
              (before && tmp_attr->start_index == start_index))
            {
              link = g_slist_alloc ();
              link->next = tmp_list;
              link->data = attr;

              if (prev)
                prev->next = link;
              else
                list->attrs = link;

              break;
            }

          prev = tmp_list;
          tmp_list = tmp_list->next;
        }
    }
}

/**
 * gy_text_attr_list_insert:
 * @list: a #GyTextAttrList
 * @attr: (transfer full): the text attribute to insert. Ownership of this
 *        value is assumed by the list.
 *
 * Insert the given text attribute into the #GyTextAttrList. It will
 * be inserted after all other text attributes with a matching
 * @start_index.
 *
 * Since: 0.6
 **/
void
gy_text_attr_list_insert (GyTextAttrList  *list,
                          GyTextAttribute *attr)
{
  g_return_if_fail (list != NULL);
  g_return_if_fail (attr != NULL);

  gy_text_attr_list_insert_internal (list, attr, FALSE);
}

/**
 * gy_text_attr_list_insert_before:
 * @list: a #GyTextAttrList
 * @attr: (transfer full): the attribute to insert. Ownership of this
 *        value is assumed by the list.
 *
 * Insert the given text attribute into the #GyTextAttrList. It will
 * be inserted before all other attributes with a matching
 * @start_index.
 *
 * Since: 0.6
 **/
void
gy_text_attr_list_insert_before (GyTextAttrList  *list,
                                 GyTextAttribute *attr)
{
  g_return_if_fail (list != NULL);
  g_return_if_fail (attr != NULL);

  gy_text_attr_list_insert_internal (list, attr, TRUE);
}

/**
 * gy_text_attr_list_get_attributes:
 * @list: a #GyTextAttrList
 *
 * Gets a list of all attributes in @list.
 *
 * Return value: (element-type Gydict.TextAttribute) (transfer full):
 *   a list of all attributes in @list. To free this value, call
 *   gy_text_attribute_unref() on each value and g_slist_free()
 *   on the list.
 *
 * Since: 0.6
 */
GSList *
gy_text_attr_list_get_attributes (GyTextAttrList *list)
{
  g_return_val_if_fail (list != NULL, NULL);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
  return g_slist_copy_deep (list->attrs, (GCopyFunc)gy_text_attribute_copy, NULL);
#pragma GCC diagnostic pop
}

/**
 * gy_text_attr_list_get_iterator:
 * @list: list #GyTextAttrList
 *
 * Create a iterator initialized to the beginning of the list.
 * The list must not be modified until this iterator is freed.
 *
 * Return value: (transfer full): the newly allocated GyTextAttrIterator,
 * which should be freed with gy_text_attr_iterator_destroy().
 *
 * Since: 0.6
 */
GyTextAttrIterator *
gy_text_attr_list_get_iterator (GyTextAttrList *list)
{
  GyTextAttrIterator *iterator;

  g_return_val_if_fail (list != NULL, NULL);

  iterator = g_slice_new (GyTextAttrIterator);
  iterator->next = list->attrs;
  iterator->stack = NULL;

  iterator->start_index = 0;
  iterator->end_index = 0;

  if (!gy_text_attr_iterator_next (iterator))
    iterator->end_index = PANGO_ATTR_INDEX_TO_TEXT_END;

  return iterator;
}

/*
 * GyTextAttrIterator
 */

G_DEFINE_BOXED_TYPE (GyTextAttrIterator, gy_text_attr_iterator,
                     gy_text_attr_iterator_copy,
                     gy_text_attr_iterator_destroy);

/**
 * gy_text_attr_iterator_copy:
 * @iterator: an iterator
 *
 * Copy the @iterator
 *
 * Return value: (transfer full): the newly allocated
 *               #GyTextAttrIterator, which should be freed with
 *               gy_text_attr_iterator_destroy().
 *
 * Since: 0.6
 **/
GyTextAttrIterator *
gy_text_attr_iterator_copy (GyTextAttrIterator *iterator)
{
  GyTextAttrIterator *copy;

  g_return_val_if_fail (iterator != NULL, NULL);

  copy = g_slice_new (GyTextAttrIterator);

  *copy = *iterator;

  copy->stack = g_list_copy (iterator->stack);

  return copy;
}

/**
 * gy_text_attr_iterator_destroy:
 * @iterator: an iterator.
 *
 * Destroy the @iterator and free all associated memory.
 *
 * Since: 0.6
 **/
void
gy_text_attr_iterator_destroy (GyTextAttrIterator *iterator)
{
  g_return_if_fail (iterator != NULL);

  g_list_free (iterator->stack);
  g_slice_free (GyTextAttrIterator, iterator);
}

/**
 * gy_text_attr_iterator_next:
 * @iterator: an iterator
 *
 * Advance the iterator until the next change of style.
 *
 * Return value: %FALSE if the iterator is at the end of the list, otherwise %TRUE
 *
 * Since: 0.6
 **/
gboolean
gy_text_attr_iterator_next (GyTextAttrIterator *iterator)
{
  g_return_val_if_fail (iterator != NULL, FALSE);

  if (!iterator->next && !iterator->stack)
    return FALSE;

  iterator->start_index = iterator->end_index;
  iterator->end_index = PANGO_ATTR_INDEX_TO_TEXT_END;

  GyTextAttribute *attr = NULL;
  for (GList *tmp_list = iterator->stack, *next = NULL ; tmp_list != NULL; tmp_list = next)
    {
      next = tmp_list->next;
      attr = tmp_list->data;

      if (attr->end_index == iterator->start_index)
        {
          iterator->stack = g_list_remove_link (iterator->stack, tmp_list);
          g_list_free_1 (tmp_list);
        }
      else
        {
          iterator->end_index = MIN (iterator->end_index, attr->end_index);
        }
    }

  while (iterator->next && ((GyTextAttribute *)iterator->next->data)->start_index == iterator->start_index)
    {
      if (((GyTextAttribute *)iterator->next->data)->end_index > iterator->start_index)
        {
          iterator->stack = g_list_prepend (iterator->stack, iterator->next->data);
          iterator->end_index = MIN (iterator->end_index, ((GyTextAttribute *)iterator->next->data)->end_index);
        }
      iterator->next = iterator->next->next;
    }

  if (iterator->next)
    iterator->end_index = MIN (iterator->end_index, ((GyTextAttribute *)iterator->next->data)->start_index);

  return TRUE;
}

/**
 * gy_text_attr_iterator_range:
 * @iterator: an iterator
 * @start: (out): location to store the start of the range
 * @end: (out): location to store the end of the range
 *
 * Get the range of the current segment. Note that the
 * stored return values are signed, not unsigned like
 * the values in #GyTextAttribute. To deal with this API
 * oversight, stored return values that wouldn't fit into
 * a signed integer are clamped to %G_MAXINT.
 **/
void
gy_text_attr_iterator_range (GyTextAttrIterator *iterator,
                             gint               *start,
                             gint               *end)
{
  g_return_if_fail (iterator != NULL);

  if (start)
    *start = MIN (iterator->start_index, G_MAXINT);
  if (end)
    *end = MIN (iterator->end_index, G_MAXINT);
}

/**
 * gy_text_attr_iterator_get:
 * @iterator: an iterator
 * @type: the type of attribute to find.
 *
 * Find the current attribute of a particular type at the iterator
 * location. When multiple attributes of the same type overlap,
 * the attribute whose range starts closest to the current location
 * is used.
 *
 * Return value: (nullable): the current attribute of the given type,
 *               or %NULL if no attribute of that type applies to the
 *               current location.
 **/
GyTextAttribute *
gy_text_attr_iterator_get (GyTextAttrIterator *iterator,
                           GyTextAttrType      type)
{
  GyTextAttribute *attr = NULL;

  g_return_val_if_fail (iterator != NULL, NULL);

  for (GList *iter = iterator->stack; iter != NULL; iter = iter->next)
    {
      attr = iter->data;

      if (attr->type == type)
        return attr;
    }

  return NULL;
}

