/* gy-dict-data-scheme.c
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

#include "gy-dict-data-scheme.h"

struct _GyDictDataScheme
{
  guint ref_count;

  gchar          *parsed_text;
  GyTextAttrList *attrs;
};

G_DEFINE_BOXED_TYPE (GyDictDataScheme, gy_dict_data_scheme,
                     gy_dict_data_scheme_copy,
                     gy_dict_data_scheme_unref)


GyDictDataScheme *
gy_dict_data_scheme_new (void)
{
  GyDictDataScheme *scheme = g_slice_new0(GyDictDataScheme);

  scheme->ref_count = 1;

  return scheme;
}

GyDictDataScheme *
gy_dict_data_scheme_copy (GyDictDataScheme *scheme)
{
  if (scheme == NULL) return NULL;

  GyDictDataScheme *new = gy_dict_data_scheme_new ();

  new->parsed_text = g_strdup(scheme->parsed_text);
  new->attrs = gy_text_attr_list_copy (scheme->attrs);

  return new;
}

GyDictDataScheme *
gy_dict_data_scheme_ref (GyDictDataScheme *scheme)
{
  if (scheme == NULL) return NULL;

  g_atomic_int_inc ((int *) &scheme->ref_count);

  return scheme;
}

void
gy_dict_data_scheme_unref (GyDictDataScheme *scheme)
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
gy_dict_data_scheme_get_parsed_text (GyDictDataScheme *scheme)
{
  g_return_val_if_fail (scheme != NULL, NULL);

  return scheme->parsed_text;
}

void
gy_dict_data_scheme_set_parsed_text (GyDictDataScheme *scheme,
                                     gchar            *parsed_text)
{
  g_return_if_fail (scheme != NULL);

  if (scheme->parsed_text != NULL)
    g_free (scheme->parsed_text);

  scheme->parsed_text = parsed_text;
}

void
gy_dict_data_scheme_dup_parsed_text (GyDictDataScheme *scheme,
                                     const gchar      *parsed_text)
{
  g_return_if_fail (scheme != NULL);

  if (scheme->parsed_text != NULL)
    g_free (scheme->parsed_text);

  scheme->parsed_text = g_strdup(parsed_text);
}

const GyTextAttrList *
gy_dict_data_scheme_get_attrs (GyDictDataScheme *scheme)
{
  g_return_val_if_fail (scheme != NULL, NULL);

  return scheme->attrs;
}

void
gy_dict_data_scheme_set_attrs (GyDictDataScheme *scheme,
                               GyTextAttrList   *attrs)
{
  g_return_if_fail (scheme != NULL);

  if (scheme->attrs != NULL)
    gy_text_attr_list_unref (scheme->attrs);

  scheme->attrs = attrs;
}
