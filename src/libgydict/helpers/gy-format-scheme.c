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
  guint ref_count;

  gchar          *parsed_text;
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

  return scheme;
}

GyFormatScheme *
gy_format_scheme_copy (GyFormatScheme *scheme)
{
  if (scheme == NULL) return NULL;

  GyFormatScheme *new = gy_format_scheme_new ();

  new->parsed_text = g_strdup(scheme->parsed_text);
  new->attrs = gy_text_attr_list_copy (scheme->attrs);

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
      if (scheme->parsed_text != NULL)
        g_clear_pointer (&scheme->parsed_text, g_free);

      if (scheme->attrs != NULL)
        {
          gy_text_attr_list_unref (scheme->attrs);
          scheme->attrs = NULL;
        }
    }
}


const gchar *
gy_format_scheme_get_parsed_text (GyFormatScheme *scheme)
{
  g_return_val_if_fail (scheme != NULL, NULL);

  return scheme->parsed_text;
}

void
gy_format_scheme_set_parsed_text (GyFormatScheme *scheme,
                                     gchar            *parsed_text)
{
  g_return_if_fail (scheme != NULL);

  if (scheme->parsed_text != NULL)
    g_free (scheme->parsed_text);

  scheme->parsed_text = parsed_text;
}

void
gy_format_scheme_dup_parsed_text (GyFormatScheme *scheme,
                                     const gchar      *parsed_text)
{
  g_return_if_fail (scheme != NULL);

  if (scheme->parsed_text != NULL)
    g_free (scheme->parsed_text);

  scheme->parsed_text = g_strdup(parsed_text);
}

const GyTextAttrList *
gy_format_scheme_get_attrs (GyFormatScheme *scheme)
{
  g_return_val_if_fail (scheme != NULL, NULL);

  return scheme->attrs;
}

void
gy_format_scheme_set_attrs (GyFormatScheme *scheme,
                               GyTextAttrList   *attrs)
{
  g_return_if_fail (scheme != NULL);

  if (scheme->attrs != NULL)
    gy_text_attr_list_unref (scheme->attrs);

  scheme->attrs = attrs;
}
