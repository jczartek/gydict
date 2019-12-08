/* test-attributes.c
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


#include <mutest.h>
#include <gydict.h>

static gboolean
is_equal_after_copy(GyTextAttribute* attr1,
                    GyTextAttribute* attr2)
{
  if (gy_text_attribute_get_attr_type (attr1) != gy_text_attribute_get_attr_type (attr2))
    return FALSE;

  if (gy_text_attribute_get_start_index (attr1) != gy_text_attribute_get_start_index (attr2))
    return FALSE;

  GyTextAttrType type = gy_text_attribute_get_attr_type (attr1);

   if (type == GY_TEXT_ATTR_LANGUAGE && gy_text_attribute_get_language (attr1) == gy_text_attribute_get_language (attr1))
     return TRUE;

    if ((type == GY_TEXT_ATTR_STYLE ||
         type == GY_TEXT_ATTR_WEIGHT ||
         type == GY_TEXT_ATTR_VARIANT ||
         type == GY_TEXT_ATTR_STRETCH ||
         type == GY_TEXT_ATTR_SIZE ||
         type == GY_TEXT_ATTR_UNDERLINE ||
         type == GY_TEXT_ATTR_RISE ||
         type == GY_TEXT_ATTR_LETTER_SPACING ||
         type == GY_TEXT_ATTR_FOREGROUND_ALPHA ||
         type == GY_TEXT_ATTR_BACKGROUND_ALPHA) &&
        gy_text_attribute_get_int (attr1) == gy_text_attribute_get_int (attr2))
      return TRUE;

  if ((type == GY_TEXT_ATTR_FAMILY ||
       type == GY_TEXT_ATTR_FONT_FEATURES) &&
      g_strcmp0 (gy_text_attribute_get_string(attr1), gy_text_attribute_get_string (attr2)) == 0)
    return TRUE;

  if ((type == GY_TEXT_ATTR_STRIKETHROUGH ||
       type == GY_TEXT_ATTR_FALLBACK) &&
      gy_text_attribute_get_boolean (attr1) == gy_text_attribute_get_boolean (attr2))
    return TRUE;

  if ((type == GY_TEXT_ATTR_FONT_DESC) &&
      pango_font_description_equal (gy_text_attribute_get_font_desc (attr1), gy_text_attribute_get_font_desc (attr2)))
    return TRUE;

  if (type == GY_TEXT_ATTR_BACKGROUND ||
      type == GY_TEXT_ATTR_FOREGROUND ||
      type == GY_TEXT_ATTR_UNDERLINE_COLOR ||
      type == GY_TEXT_ATTR_STRIKETHROUGH_COLOR)
    {
      const PangoColor *color1, *color2;
      color1 = gy_text_attribute_get_color (attr1);
      color2 = gy_text_attribute_get_color (attr2);

      return color1->green == color2->green && color1->red == color2->red && color1->blue == color2->blue;
    }


  return FALSE;
}

static void
attr_equal (void)
{
  GyTextAttribute *attr1, *attr2;
  attr1 = attr2 = NULL;
  gboolean is_equal = FALSE;

    { // Language Attr
      attr1 = gy_text_attribute_language_new (pango_language_from_string ("ja-JP"));
      gy_text_attribute_set_start_index (attr1, 64);
      gy_text_attribute_set_end_index (attr1, 255);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("language attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Style Attr
      attr1 = gy_text_attribute_style_new (PANGO_STYLE_ITALIC);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("style attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Weight Attr
      attr1 = gy_text_attribute_weight_new (PANGO_WEIGHT_HEAVY);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("weight attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Variant Attr
      attr1 = gy_text_attribute_variant_new (PANGO_VARIANT_SMALL_CAPS);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("variant attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Stretch Attr
      attr1 = gy_text_attribute_stretch_new (PANGO_STRETCH_ULTRA_CONDENSED);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("stretch attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Size Attr
      attr1 = gy_text_attribute_size_new (23);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("size attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Underline Attr
      attr1 = gy_text_attribute_underline_new (PANGO_UNDERLINE_DOUBLE);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("underline attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Rise Attr
      attr1 = gy_text_attribute_rise_new(3);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("rise attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Letter Spacing Attr
      attr1 = gy_text_attribute_letter_spacing_new (2);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("letter-spacing attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Foreground Alpha Attr
      attr1 = gy_text_attribute_foreground_alpha_new (64);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("foreground-alpha attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Background Alpha Attr
      attr1 = gy_text_attribute_background_alpha_new (64);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr2);
      mutest_expect ("background-alpha attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Family Attr
      attr1 = gy_text_attribute_family_new ("Times");
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("family attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Font-Features Attr
      attr1 = gy_text_attribute_font_features_new (".hist { font-feature-settings: \"hist\"; }");
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("font-features attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Strikethrough Attr
      attr1 = gy_text_attribute_strikethrough_new (TRUE);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("strikethrough attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Fallback Attr
      attr1 = gy_text_attribute_fallback_new (TRUE);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("fallback attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Font Description Attr
      PangoFontDescription *desc = pango_font_description_from_string ("Computer Modern 12");
      attr1 = gy_text_attribute_font_desc_new (desc);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("font description attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      pango_font_description_free (desc);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Font Background Attr
      attr1 = gy_text_attribute_background_new (255, 255, 255);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("background attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Font Foreground Attr
      attr1 = gy_text_attribute_foreground_new (255, 255, 255);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("foreground attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Font Underline-Color Attr
      attr1 = gy_text_attribute_underline_color_new (255, 255, 255);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("underline-color attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }

    { // Font Strikethrough-Color Attr
      attr1 = gy_text_attribute_strikethrough_color_new (255, 255, 255);
      attr2 = gy_text_attribute_copy (attr1);
      is_equal = is_equal_after_copy (attr1, attr1);
      mutest_expect ("strikethrough-color attrs are equal", mutest_bool_value (is_equal),
                     mutest_to_be, true, NULL);
      gy_text_attribute_unref (attr1);
      gy_text_attribute_unref (attr2);
    }


}

static void
append_to_list(void)
{
  GyTextAttribute *attr;
  GyTextAttrList *attr_list;
  GSList *l = NULL;


  attr_list = gy_text_attr_list_new ();

  attr = gy_text_attribute_size_new (10);
  gy_text_attr_list_insert (attr_list, attr);
  attr = gy_text_attribute_size_new (20);
  gy_text_attr_list_insert (attr_list, attr);
  attr = gy_text_attribute_size_new (30);
  gy_text_attr_list_insert (attr_list, attr);
  attr = gy_text_attribute_size_new (40);
  gy_text_attribute_set_start_index (attr, 10);
  gy_text_attr_list_insert (attr_list, attr);
  attr = gy_text_attribute_size_new (50);
  gy_text_attribute_set_start_index (attr, 10);
  gy_text_attr_list_insert (attr_list, attr);

  l = gy_text_attr_list_get_attributes (attr_list);

  attr = (GyTextAttribute *) g_slist_nth_data(l, 0);
  mutest_expect ("the first text attribute has value 10",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 10, NULL);

  attr = g_slist_nth_data (l, 1);
  mutest_expect ("the second text attribute has value 20",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 20, NULL);


  attr = g_slist_nth_data (l, 2);
  mutest_expect ("the third text attribute has value 30",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 30, NULL);

  attr = g_slist_nth_data (l, 3);
  mutest_expect ("the fourth text attribute has value 40",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 40, NULL);

  attr = g_slist_nth_data (l, 4);
  mutest_expect ("the fifth text attribute has value 50",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 50, NULL);

  gy_text_attr_list_unref (attr_list);
  g_slist_free (l);



}

static void
prepend_to_list (void)
{
  GyTextAttribute *attr;
  GyTextAttrList *attr_list;
  GSList *l = NULL;


  attr_list = gy_text_attr_list_new ();

  attr = gy_text_attribute_size_new (50);
  gy_text_attribute_set_start_index (attr, 23123);
  gy_text_attr_list_insert_before (attr_list, attr);

  attr = gy_text_attribute_size_new (40);
  gy_text_attribute_set_start_index (attr, 123);
  gy_text_attr_list_insert_before (attr_list, attr);

  attr = gy_text_attribute_size_new (30);
  gy_text_attribute_set_start_index (attr, 0);
  gy_text_attr_list_insert_before (attr_list, attr);

  attr = gy_text_attribute_size_new (20);
  gy_text_attribute_set_start_index (attr, 556);
  gy_text_attr_list_insert_before (attr_list, attr);

  attr = gy_text_attribute_size_new (10);
  gy_text_attribute_set_start_index (attr, 0);
  gy_text_attr_list_insert_before (attr_list, attr);

  l = gy_text_attr_list_get_attributes (attr_list);

  attr = (GyTextAttribute *) g_slist_nth_data(l, 0);
  mutest_expect ("the first text attribute has value 10",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 10, NULL);

  attr = g_slist_nth_data (l, 1);
  mutest_expect ("the second text attribute has value 30",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 30, NULL);


  attr = g_slist_nth_data (l, 2);
  mutest_expect ("the third text attribute has value 40",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 40, NULL);

  attr = g_slist_nth_data (l, 3);
  mutest_expect ("the fourth text attribute has value 20",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 20, NULL);

  attr = g_slist_nth_data (l, 4);
  mutest_expect ("the fifth text attribute has value 50",
                 mutest_int_value (gy_text_attribute_get_int (attr)),
                 mutest_to_be, 50, NULL);

  gy_text_attr_list_unref (attr_list);
  g_slist_free (l);
}

static void
attr_iter_order_text_attrs (void)
{
  GyTextAttribute *attr = NULL;
  GyTextAttrList *attr_list = NULL;
  GyTextAttrIterator *iter = NULL;

  struct {
    int start;
    int end;
    gchar *name;
  } params_attrs[10];

  attr_list = gy_text_attr_list_new ();

  attr = gy_text_attribute_size_new (50);
  gy_text_attribute_set_start_index (attr, 0);
  gy_text_attribute_set_end_index (attr, 10);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[0].start = 0;
  params_attrs[0].end = 10;
  params_attrs[0].name = "size attr";

  attr = gy_text_attribute_style_new (PANGO_STYLE_ITALIC);
  gy_text_attribute_set_start_index (attr, 10);
  gy_text_attribute_set_end_index (attr, 20);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[1].start = 10;
  params_attrs[1].end = 20;
  params_attrs[1].name = "style attr";

  attr = gy_text_attribute_weight_new (PANGO_WEIGHT_HEAVY);
  gy_text_attribute_set_start_index (attr, 20);
  gy_text_attribute_set_end_index (attr, 30);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[2].start = 20;
  params_attrs[2].end = 30;
  params_attrs[2].name = "weight attr";

  attr = gy_text_attribute_variant_new (PANGO_VARIANT_SMALL_CAPS);
  gy_text_attribute_set_start_index (attr, 30);
  gy_text_attribute_set_end_index (attr, 40);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[3].start = 30;
  params_attrs[3].end = 40;
  params_attrs[3].name = "variant attr";

  attr = gy_text_attribute_stretch_new (PANGO_STRETCH_ULTRA_CONDENSED);
  gy_text_attribute_set_start_index (attr, 40);
  gy_text_attribute_set_end_index (attr, 50);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[4].start = 40;
  params_attrs[4].end = 50;
  params_attrs[4].name = "stretch attr";

  attr = gy_text_attribute_strikethrough_new (TRUE);
  gy_text_attribute_set_start_index (attr, 50);
  gy_text_attribute_set_end_index (attr, 60);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[5].start = 50;
  params_attrs[5].end = 60;
  params_attrs[5].name = "strikethrough attr";

  attr = gy_text_attribute_strikethrough_color_new (255, 255, 255);
  gy_text_attribute_set_start_index (attr, 60);
  gy_text_attribute_set_end_index (attr, 70);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[6].start = 60;
  params_attrs[6].end = 70;
  params_attrs[6].name = "strikethrough color attr";

  attr = gy_text_attribute_background_alpha_new (64);
  gy_text_attribute_set_start_index (attr, 70);
  gy_text_attribute_set_end_index (attr, 80);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[7].start = 70;
  params_attrs[7].end = 80;
  params_attrs[7].name = "background alpha attr";

  attr = gy_text_attribute_underline_new (PANGO_UNDERLINE_DOUBLE);
  gy_text_attribute_set_start_index (attr, 80);
  gy_text_attribute_set_end_index (attr, 90);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[8].start = 80;
  params_attrs[8].end = 90;
  params_attrs[8].name = "underline attr";

  attr = gy_text_attribute_rise_new(3);
  gy_text_attribute_set_start_index (attr, 90);
  gy_text_attribute_set_end_index (attr, 100);
  gy_text_attr_list_insert (attr_list, attr);
  params_attrs[9].start = 90;
  params_attrs[9].end = 100;
  params_attrs[9].name = "rise attr";

  iter = gy_text_attr_list_get_iterator (attr_list);
  guint i = 0;
  GString *string = g_string_new ("");
  gint start, end;
  do
    {
      gy_text_attr_iterator_range (iter, &start, &end);
      g_string_printf (string, "The %s starts at %d and ends at %d.", params_attrs[i].name, params_attrs[i].start, params_attrs[i].end);
      mutest_expect (string->str,
                     mutest_bool_value (start == params_attrs[i].start && end == params_attrs[i].end),
                     mutest_to_be, TRUE, NULL);
      i++;
    }
  while (gy_text_attr_iterator_next (iter) && i < 10);

  mutest_expect ("The iterator has iterated 10 times.", mutest_int_value (i), mutest_to_be, 10, NULL);

  g_string_free (string, TRUE);
  gy_text_attr_iterator_destroy (iter);
  gy_text_attr_list_unref (attr_list);

}

static void
attributes_suite (void)
{
  mutest_it ("has equality", attr_equal);
}

static void
attributes_list_suite (void)
{
  mutest_it ("appending an text attribute to the attr list", append_to_list);
  mutest_it ("prepending an text attribute to the attr list", prepend_to_list);
}

static void
attr_iter_suite (void)
{
  mutest_it ("returns valid order of text attrs:", attr_iter_order_text_attrs);
}

MUTEST_MAIN (
  mutest_describe ("Text Attributes [GyTextAttributes]", attributes_suite);
  mutest_describe ("Attributes List", attributes_list_suite);
  mutest_describe ("Attrs Iterator", attr_iter_suite);
)
