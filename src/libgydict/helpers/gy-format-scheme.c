/* gy-format-scheme.c
 *
 * Copyright 2020 Jakub Czartek <kuba@linux.pl>
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

#include "gy-format-scheme.h"

struct _GyFormatScheme
{
  guint           ref_count;
  GString        *lexical_unit;
  GyTextAttrList *attrs;
};

G_DEFINE_BOXED_TYPE (GyFormatScheme, gy_format_scheme,
                     gy_format_scheme_copy,
                     gy_format_scheme_unref)

GyFormatScheme *
gy_format_scheme_new (void)
{
  GyFormatScheme *scheme = g_slice_new0(GyFormatScheme);

  scheme->ref_count = 1;
  scheme->lexical_unit = g_string_new (NULL);
  scheme->attrs = gy_text_attr_list_new ();

  return scheme;
}

GyFormatScheme *
gy_format_scheme_copy (GyFormatScheme *scheme)
{
  if (scheme == NULL) return NULL;

  GyFormatScheme *new = gy_format_scheme_new ();

  new->attrs = gy_text_attr_list_copy (scheme->attrs);
  new->lexical_unit = g_string_new (scheme->lexical_unit->str);

  return new;
}

GyFormatScheme *
gy_format_scheme_ref (GyFormatScheme *scheme)
{
  if (scheme == NULL) return NULL;

  g_atomic_int_inc ((int *) &scheme->ref_count);

  return scheme;
}

void
gy_format_scheme_unref (GyFormatScheme *scheme)
{
  if (scheme == NULL) return;

  if (g_atomic_int_dec_and_test ((int *) &scheme->ref_count))
    {
      if (scheme->attrs != NULL)
        {
          gy_text_attr_list_unref (scheme->attrs);
          scheme->attrs = NULL;
        }

      if (scheme->lexical_unit != NULL)
        {
          g_string_free (scheme->lexical_unit, TRUE);
          scheme->lexical_unit = NULL;
        }
    }
}

const GyTextAttrList *
gy_format_scheme_get_attrs (GyFormatScheme *scheme)
{
  g_return_val_if_fail (scheme != NULL, NULL);

  return scheme->attrs;
}

void
gy_format_scheme_add_text_attr (GyFormatScheme  *scheme,
                                GyTextAttribute *attr)
{
  g_return_if_fail (scheme != NULL);

  gy_text_attr_list_insert (scheme->attrs, attr);
}

void
gy_format_scheme_append_text (GyFormatScheme *scheme,
                              const gchar    *text)
{
  g_return_if_fail (scheme != NULL);

  scheme->lexical_unit = g_string_append (scheme->lexical_unit, text);
}

void
gy_format_scheme_append_text_len (GyFormatScheme *scheme,
                                   const gchar    *text,
                                   gssize          len)
{
  g_return_if_fail (scheme != NULL);
  scheme->lexical_unit = g_string_append_len (scheme->lexical_unit, text, len);
}

void
gy_format_scheme_append_char (GyFormatScheme *scheme,
                              gchar           ch)
{
  g_return_if_fail (scheme != NULL);

  scheme->lexical_unit = g_string_append_c (scheme->lexical_unit, ch);
}

void
gy_format_scheme_append_unichar (GyFormatScheme *scheme,
                                 gunichar        uch)
{
  g_return_if_fail (scheme != NULL);

  scheme->lexical_unit = g_string_append_unichar (scheme->lexical_unit, uch);
}

void
gy_format_scheme_prepend_text (GyFormatScheme *scheme,
                               const gchar    *text)
{
  g_return_if_fail (scheme != NULL);

  scheme->lexical_unit = g_string_prepend (scheme->lexical_unit, text);
}

void
gy_format_scheme_prepend_text_len (GyFormatScheme *scheme,
                                   const gchar    *text,
                                   gssize          len)
{
  g_return_if_fail (scheme != NULL);
  scheme->lexical_unit = g_string_prepend_len (scheme->lexical_unit, text, len);
}

void
gy_format_scheme_prepend_char (GyFormatScheme *scheme,
                               gchar           ch)
{
  g_return_if_fail (scheme != NULL);

  scheme->lexical_unit = g_string_prepend_c (scheme->lexical_unit, ch);
}

void
gy_format_scheme_prepend_unichar (GyFormatScheme *scheme,
                                  gunichar        uch)
{
  g_return_if_fail (scheme != NULL);

  scheme->lexical_unit = g_string_prepend_unichar (scheme->lexical_unit, uch);
}

const gchar*
gy_format_scheme_get_lexical_unit (GyFormatScheme *scheme)
{
  g_return_val_if_fail (scheme != NULL, NULL);

  return scheme->lexical_unit->str;
}
