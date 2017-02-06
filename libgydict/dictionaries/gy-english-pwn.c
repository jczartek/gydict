/* gy-english-pwn.c
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
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

#include "gy-english-pwn.h"
#include "helpers/gy-utility-func.h"

#define MD5_ANGPOL_06_07  "91f9344b0b9d40dcbb0a4c6027de98c1"
#define MD5_POLANG_06_07  "a1603dd25a7911d40edbb2b8fa89945d"

#define GY_ENGLISH_PWN_ERROR gy_english_pwn_error_quark ()

struct _GyEnglishPwn
{
  GyPwnDict  __parent__;
};

G_DEFINE_TYPE (GyEnglishPwn, gy_english_pwn, GY_TYPE_PWN_DICT);

static GQuark
gy_english_pwn_error_quark (void)
{
  return g_quark_from_static_string ("gy-english-pwn-error-quark");
}

static void
gy_english_pwn_query (GyPwnDict      *self,
                      GyDictPwnQuery *query)
{
  g_return_if_fail (GY_IS_ENGLISH_PWN (self));

  query->offset1 = 0x68, query->offset2 = 0x04;
  query->magic1 = 0x1147, query->magic2 = 0x1148;
}

static gboolean
gy_english_pwn_checksum (GyPwnDict  *self,
                         GFile      *file,
                         GError    **err)
{
  g_autofree gchar *md5 = NULL;

  md5 = gy_utility_compute_md5_for_file (file, err);

  if (!md5)
    return FALSE;

  if ((g_strcmp0 (md5, MD5_ANGPOL_06_07) != 0) && (g_strcmp0 (md5, MD5_POLANG_06_07) != 0))
    {
      g_autofree gchar *path = NULL;

      path = g_file_get_path (file);

      g_set_error (err, GY_ENGLISH_PWN_ERROR, G_FILE_ERROR_FAILED,
                   "The %s checksum is not in accordance with the checksum of the file dictionary.", path);
      return FALSE;
    }
  return TRUE;
}

static void
gy_english_pwn_class_init (GyEnglishPwnClass *klass)
{
  GyPwnDictClass  *pwn_dict_class = GY_PWN_DICT_CLASS (klass);

  pwn_dict_class->check_checksum = gy_english_pwn_checksum;
  pwn_dict_class->query = gy_english_pwn_query;
}

static void
gy_english_pwn_init (GyEnglishPwn *self)
{
}

