/* gy-utility-func.c
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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

#include <math.h>
#include "config.h"
#include "gy-utility-func.h"

gboolean
gy_utility_handlers_is_blocked_by_func (gpointer instance,
                                        gpointer func,
                                        gpointer data)
{
  return g_signal_handler_find (instance,
                                G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA | G_SIGNAL_MATCH_UNBLOCKED,
                                0, 0, NULL, func, data) == 0;
}

gboolean
gy_utility_is_handler_connected (gpointer instance,
                                 gpointer handler)
{
  return g_signal_handler_find (instance, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, handler, NULL) != 0;
}

gint
gy_utility_strcmp (const gchar *p1,
                   const gchar *p2,
                   size_t       n)
{
  const guchar *s1 = (const guchar *) p1;
  const guchar *s2 = (const guchar *) p2;
  guchar c1 = '\0', c2 = '\0';

  g_return_val_if_fail (s1 != NULL, -1);
  g_return_val_if_fail (s2 != NULL, -1);

  while (n > 0)
  {
    c1 = (guchar) *s1++;
    c2 = (guchar) *s2++;

    if (*s2 == '|')
    {
      s2++;
      if (*s2 == '\0')
        return c1 - c2;
    }
    if (c1 == '\0' || c1 != c2)
      return c1 - c2;
    n--;
  }

  return c1 - c2;
}

#define FONT_FAMILY  "font-family"
#define FONT_VARIANT "font-variant"
#define FONT_STRETCH "font-stretch"
#define FONT_WEIGHT  "font-weight"
#define FONT_SIZE    "font-size"

gchar *
gy_utility_pango_font_description_to_css (const PangoFontDescription *font_desc)
{
  PangoFontMask  mask;
  GString       *str;

#define ADD_KEYVAL(key,fmt) \
  g_string_append (str, key":"fmt";")
#define ADD_KEYVAL_PRINTF(key,fmt,...) \
  g_string_append_printf (str, key":"fmt";", __VA_ARGS__)

  g_return_val_if_fail (font_desc, NULL);

  str = g_string_new (NULL);

  mask = pango_font_description_get_set_fields (font_desc);

  if ((mask & PANGO_FONT_MASK_FAMILY) != 0)
    {
      const gchar *family;
      family = pango_font_description_get_family (font_desc);
      ADD_KEYVAL_PRINTF (FONT_FAMILY, "\"%s\"", family);
    }

  if ((mask & PANGO_FONT_MASK_STYLE) != 0)
    {
      PangoVariant variant;
      variant = pango_font_description_get_variant (font_desc);

      switch (variant)
        {
        case PANGO_VARIANT_NORMAL:
          ADD_KEYVAL (FONT_VARIANT, "normal");
          break;

        case PANGO_VARIANT_SMALL_CAPS:
          ADD_KEYVAL (FONT_VARIANT, "small-caps");
          break;

          default:
          break;
        }
    }

  if ((mask & PANGO_FONT_MASK_WEIGHT))
    {
      gint weight;
      weight = pango_font_description_get_weight (font_desc);

      /*
       * WORKAROUND:
       *
       * font-weight with numbers does not appear to be working as expected
       * right now. So for the common (bold/normal), let's just use the string
       * and let gtk warn for the other values, which shouldn't really be
       * used for this.
       */

      switch (weight)
        {
        case PANGO_WEIGHT_SEMILIGHT:
          /*
           * 350 is not actually a valid css font-weight, so we will just round
           * up to 400.
           */
        case PANGO_WEIGHT_NORMAL:
          ADD_KEYVAL (FONT_WEIGHT, "normal");
          break;

        case PANGO_WEIGHT_BOLD:
          ADD_KEYVAL (FONT_WEIGHT, "bold");
          break;

        case PANGO_WEIGHT_THIN:
        case PANGO_WEIGHT_ULTRALIGHT:
        case PANGO_WEIGHT_LIGHT:
        case PANGO_WEIGHT_BOOK:
        case PANGO_WEIGHT_MEDIUM:
        case PANGO_WEIGHT_SEMIBOLD:
        case PANGO_WEIGHT_ULTRABOLD:
        case PANGO_WEIGHT_HEAVY:
        case PANGO_WEIGHT_ULTRAHEAVY:
        default:
          /* round to nearest hundred */
          weight = round (weight / 100.0) * 100;
          ADD_KEYVAL_PRINTF ("font-weight", "%d", weight);
          break;
        }
    }
  if ((mask & PANGO_FONT_MASK_SIZE))
    {
      gint font_size;

      font_size = pango_font_description_get_size (font_desc) / PANGO_SCALE;
      ADD_KEYVAL_PRINTF ("font-size", "%dpx", font_size);
    }

  return g_string_free (str, FALSE);

#undef ADD_KEYVAL
#undef ADD_KEYVAL_PRINTF
}

/**
 * gy_utility_compute_md5_for_file:
 * @file: a GFile to compute the checksum of
 * @err: a GError
 *
 * Computes the checksum of a file. The hexadecimal string returned will be in lower case.
 * If the file does not exist, the G_IO_ERROR_NOT_FOUND error will be returned.
 *
 * Returns
 * the checksum as a hexadecimal string or NULL on error. The returned string should be freed with g_free() when done using it.
 */
gchar *
gy_utility_compute_md5_for_file (GFile   *file,
                                 GError **err)
{
  g_autofree gchar *contents = NULL;
  gchar *md5 = NULL;
  gsize length;

  g_return_val_if_fail (file != NULL, NULL);

  if (!g_file_load_contents (file, NULL, &contents, &length, NULL, err))
    return NULL;

  md5 = g_compute_checksum_for_string (G_CHECKSUM_MD5,
                                       (const gchar *) contents,
                                       (gssize) length);

  return md5;
}
