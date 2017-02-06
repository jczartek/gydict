/* gy-german-pwn.c
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

#define G_LOG_DOMAIN "GyGermanPwn"

#include "gy-german-pwn.h"
#include "helpers/gy-utility-func.h"

#define MD5_NIEMPOL   "c0f2280d5bedfc5c88620dcef512b897"
#define MD5_POLNIEM   "2b551364dd36ef263381276ee352c59f"

#define GY_GERMAN_PWN_ERROR gy_german_pwn_error_quark ()


struct _GyGermanPwn
{
  GyPwnDict  __parent__;
};

G_DEFINE_TYPE (GyGermanPwn, gy_german_pwn, GY_TYPE_PWN_DICT)

static GQuark
gy_german_pwn_error_quark (void)
{
  return g_quark_from_static_string ("gy-german-pwn-error-quark");
}

static void
gy_german_pwn_query (GyPwnDict      *self,
                     GyDictPwnQuery *query)
{
  query->offset1 = 0x68;
  query->offset2 = 0x04;
  query->magic1 = 0x11dd;
  query->magic2 = 0x11d7;
}

static gboolean
gy_german_pwn_check_checksum (GyPwnDict  *self G_GNUC_UNUSED,
                              GFile      *file,
                              GError    **err)
{
  g_autofree gchar *md5 = NULL;

  md5 = gy_utility_compute_md5_for_file (file, err);

  if (!md5)
    return FALSE;

  if ((g_strcmp0 (md5, MD5_NIEMPOL) != 0) && (g_strcmp0 (md5, MD5_POLNIEM) != 0))
    {
      g_autofree gchar *path = NULL;

      path = g_file_get_path (file);

      g_set_error (err, GY_GERMAN_PWN_ERROR, G_FILE_ERROR_FAILED,
                   "The %s checksum is not in accordance with the checksum of the file dictionary.", path);
      return FALSE;
    }

  return TRUE;
}

static void
gy_german_pwn_class_init (GyGermanPwnClass *klass)
{
  GyPwnDictClass *pwn_dict_class = GY_PWN_DICT_CLASS (klass);

  pwn_dict_class->check_checksum = gy_german_pwn_check_checksum;
  pwn_dict_class->query = gy_german_pwn_query;
}

static void
gy_german_pwn_init (GyGermanPwn *self)
{
}

