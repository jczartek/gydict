/* gy-text-attribute.h
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

#pragma once

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include <pango/pango.h>

G_BEGIN_DECLS

typedef struct _GyTextAttribute GyTextAttribute;
typedef struct _GyTextAttrList GyTextAttrList;
typedef struct _GyTextAttrIterator GyTextAttrIterator;

/**
 * GyTextAttrType:
 * @GY_TEXT_ATTR_INVALID: does not happen
 * @GY_TEXT_ATTR_LANGUAGE: language ""
 * @GY_TEXT_ATTR_FAMILY: font family name list
 * @GY_TEXT_ATTR_STYLE: font slant style
 * @GY_TEXT_ATTR_WEIGHT: font weight
 * @GY_TEXT_ATTR_VARIANT: font variant (normal or small caps)
 * @GY_TEXT_ATTR_STRETCH: font stretch
 * @GY_TEXT_ATTR_SIZE: font size in points scaled by %PANGO_SCALE
 * @GY_TEXT_ATTR_FONT_DESC: font description
 * @GY_TEXT_ATTR_FOREGROUND: foreground color
 * @GY_TEXT_ATTR_BACKGROUND: background color
 * @GY_TEXT_ATTR_UNDERLINE: whether the text has an underline
 * @GY_TEXT_ATTR_STRIKETHROUGH: whether the text is struck-through
 * @GY_TEXT_ATTR_RISE: baseline displacement
 * @GY_TEXT_ATTR_SHAPE: shape
 * @GY_TEXT_ATTR_SCALE: font size scale factor
 * @GY_TEXT_ATTR_FALLBACK: whether fallback is enabled
 * @GY_TEXT_ATTR_LETTER_SPACING: letter spacing
 * @GY_TEXT_ATTR_UNDERLINE_COLOR: underline color
 * @GY_TEXT_ATTR_STRIKETHROUGH_COLOR: strikethrough color
 * @GY_TEXT_ATTR_ABSOLUTE_SIZE: font size in pixels scaled by %PANGO_SCALE
 * @GY_TEXT_ATTR_GRAVITY: base text gravity
 * @GY_TEXT_ATTR_GRAVITY_HINT: gravity hint
 * @GY_TEXT_ATTR_FONT_FEATURES: OpenType font features
 * @GY_TEXT_ATTR_FOREGROUND_ALPHA: foreground alpha
 * @GY_TEXT_ATTR_BACKGROUND_ALPHA: background alpha
 *
 * The #GyTextAttrType distinguishes between different types of attributes.
 */
typedef enum
{
  GY_TEXT_ATTR_INVALID             = PANGO_ATTR_INVALID,
  GY_TEXT_ATTR_LANGUAGE            = PANGO_ATTR_LANGUAGE,
  GY_TEXT_ATTR_FAMILY              = PANGO_ATTR_FAMILY,
  GY_TEXT_ATTR_STYLE               = PANGO_ATTR_STYLE,
  GY_TEXT_ATTR_WEIGHT              = PANGO_ATTR_WEIGHT,
  GY_TEXT_ATTR_VARIANT             = PANGO_ATTR_VARIANT,
  GY_TEXT_ATTR_STRETCH             = PANGO_ATTR_STRETCH,
  GY_TEXT_ATTR_SIZE                = PANGO_ATTR_SIZE,
  GY_TEXT_ATTR_FONT_DESC           = PANGO_ATTR_FONT_DESC,
  GY_TEXT_ATTR_FOREGROUND          = PANGO_ATTR_FOREGROUND,
  GY_TEXT_ATTR_BACKGROUND          = PANGO_ATTR_BACKGROUND,
  GY_TEXT_ATTR_UNDERLINE           = PANGO_ATTR_UNDERLINE,
  GY_TEXT_ATTR_STRIKETHROUGH       = PANGO_ATTR_STRIKETHROUGH,
  GY_TEXT_ATTR_RISE                = PANGO_ATTR_RISE,
  GY_TEXT_ATTR_SHAPE               = PANGO_ATTR_SHAPE,
  GY_TEXT_ATTR_SCALE               = PANGO_ATTR_SCALE,
  GY_TEXT_ATTR_FALLBACK            = PANGO_ATTR_FALLBACK,
  GY_TEXT_ATTR_LETTER_SPACING      = PANGO_ATTR_LETTER_SPACING,
  GY_TEXT_ATTR_UNERLINE_COLOR      = PANGO_ATTR_UNDERLINE_COLOR,
  GY_TEXT_ATTR_STRIKETHROUGH_COLOR = PANGO_ATTR_STRIKETHROUGH_COLOR,
  GY_TEXT_ATTR_ABSOLUTE_SIZE       = PANGO_ATTR_ABSOLUTE_SIZE,
  GY_TEXT_ATTR_GRAVITY             = PANGO_ATTR_GRAVITY,
  GY_TEXT_ATTR_GRAVITY_HINT        = PANGO_ATTR_GRAVITY_HINT,
  GY_TEXT_ATTR_FONT_FEATURES       = PANGO_ATTR_FONT_FEATURES,
  GY_TEXT_ATTR_FOREGROUND_ALPHA    = PANGO_ATTR_FOREGROUND_ALPHA,
  GY_TEXT_ATTR_BACKGROUND_ALPHA    = PANGO_ATTR_BACKGROUND_ALPHA,
} GyTextAttrType;

/*
 * The text attribute
 */
GType gy_text_attribute_get_type (void) G_GNUC_CONST;
GyTextAttribute *gy_text_attribute_new ();
GyTextAttribute *gy_text_attribute_ref (GyTextAttribute *attr);
void gy_text_attribute_unref (GyTextAttribute *attr);
GyTextAttribute * gy_text_attribute_copy (GyTextAttribute *attr);
void gy_text_attribute_set_start_index (GyTextAttribute *attr,
                                        guint            start_index);
guint gy_text_attribute_get_start_index (GyTextAttribute *attr);
void gy_text_attribute_set_end_index (GyTextAttribute *attr,
                                      guint            end_index);
guint gy_text_attribute_get_end_index (GyTextAttribute *attr);

void gy_text_attribute_set_attr_type (GyTextAttribute *attr,
                                      GyTextAttrType   type);
GyTextAttrType gy_text_attribute_get_attr_type (GyTextAttribute *attr);
gboolean gy_text_attribute_get_boolean (GyTextAttribute *attr);
gint gy_text_attribute_get_int (GyTextAttribute *attr);
gdouble gy_text_attribute_get_float (GyTextAttribute *attr);
const gchar* gy_text_attribute_get_string (GyTextAttribute *attr);
const PangoLanguage* gy_text_attribute_get_language (GyTextAttribute *attr);
const PangoFontDescription *gy_text_attribute_get_font_desc (GyTextAttribute *attr);
const PangoColor* gy_text_attribute_get_color (GyTextAttribute *attr);
GyTextAttribute * gy_text_attribute_language_new (PangoLanguage *language);
GyTextAttribute * gy_text_attribute_family_new (const gchar *family);
GyTextAttribute * gy_text_attribute_style_new (PangoStyle style);
GyTextAttribute * gy_text_attribute_weight_new (PangoWeight weight);
GyTextAttribute * gy_text_attribute_variant_new (PangoVariant variant);
GyTextAttribute * gy_text_attribute_stretch_new (PangoStretch stretch);
GyTextAttribute * gy_text_attribute_size_new (gint size);
GyTextAttribute * gy_text_attribute_font_desc_new (const PangoFontDescription *desc);
GyTextAttribute * gy_text_attribute_foreground_new (guint16 red,
                                                    guint16 green,
                                                    guint16 blue);
GyTextAttribute * gy_text_attribute_foreground_new_from_hex (const gchar *spec);
GyTextAttribute * gy_text_attribute_background_new (guint16 red,
                                                    guint16 green,
                                                    guint16 blue);
GyTextAttribute * gy_text_attribute_background_new_from_hex (const gchar *spec);
GyTextAttribute * gy_text_attribute_underline_new (PangoUnderline underline);
GyTextAttribute * gy_text_attribute_underline_color_new (guint16 red,
                                                         guint16 green,
                                                         guint16 blue);
GyTextAttribute * gy_text_attribute_strikethrough_new (gboolean strikethrough);
GyTextAttribute * gy_text_attribute_strikethrough_color_new (guint16 red,
                                                             guint16 green,
                                                             guint16 blue);
GyTextAttribute *gy_text_attribute_rise_new (gint rise);
GyTextAttribute *gy_text_attriubte_scale_new (gdouble scale_factor);
GyTextAttribute *gy_text_attribute_fallback_new (gboolean enable_fallback);
GyTextAttribute *gy_text_attribute_letter_spacing_new (gint letter_spacing);
GyTextAttribute *gy_text_attribute_font_features_new (const gchar *features);
GyTextAttribute *gy_text_attribute_foreground_alpha_new (guint16 alpha);
GyTextAttribute *gy_text_attribute_background_alpha_new (guint16 alpha);

/*
 * The text attriubte list
 */
GType            gy_text_attr_list_get_type       (void) G_GNUC_CONST;
GyTextAttrList*  gy_text_attr_list_new            (void);
GyTextAttrList*  gy_text_attr_list_ref            (GyTextAttrList *list);
void             gy_text_attr_list_unref          (GyTextAttrList *list);
GyTextAttrList*  gy_text_attr_list_copy           (GyTextAttrList *list);
void             gy_text_attr_list_insert         (GyTextAttrList  *list,
                                                   GyTextAttribute *attr);
void             gy_text_attr_list_insert_before  (GyTextAttrList  *list,
                                                   GyTextAttribute *attr);
GSList*          gy_text_attr_list_get_attributes (GyTextAttrList *list);
GyTextAttrIterator *gy_text_attr_list_get_iterator (GyTextAttrList *list);

/*
 * The text attribute iterator
 */
GType               gy_text_attr_iterator_get_type (void) G_GNUC_CONST;
GyTextAttrIterator* gy_text_attr_iterator_copy     (GyTextAttrIterator *iterator);
void                gy_text_attr_iterator_destroy  (GyTextAttrIterator *iterator);
gboolean            gy_text_attr_iterator_next     (GyTextAttrIterator *iterator);
void                gy_text_attr_iterator_range    (GyTextAttrIterator *iterator,
                                                    gint               *start,
                                                    gint               *end);
GyTextAttribute*    gy_text_attr_iterator_get      (GyTextAttrIterator *iterator,
                                                    GyTextAttrType      type);

G_END_DECLS
