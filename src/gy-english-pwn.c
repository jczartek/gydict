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

#include <string.h>
#include <stdlib.h>
#include <zlib.h>

#include "gy-english-pwn.h"
#include "gy-utility-func.h"
#include "gy-pwntabs.h"

#define MD5_ANGPOL_06_07  "91f9344b0b9d40dcbb0a4c6027de98c1"
#define MD5_POLANG_06_07  "a1603dd25a7911d40edbb2b8fa89945d"

#define SIZE_BUFFER  128
#define SIZE_ENTRY   64

struct _GyEnglishPwn
{
  GyDict parent;

  GFile      *file;
  guint32    *offsets;
  GHashTable *entities;
};

G_DEFINE_TYPE (GyEnglishPwn, gy_english_pwn, GY_TYPE_DICT)

enum {
  PROP_0,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static void
gy_english_pwn_map (GyDict  *dict,
                   GError **err)
{
  guint32 word_count = 0, index_base = 0, word_base = 0;
  g_autofree gchar *md5 = NULL;
  g_autofree guint32 *offsets = NULL;
  g_autoptr(GFileInputStream) in = NULL;
  g_autoptr(GSettings)  settings = NULL;
  g_autofree gchar     *path = NULL;
  gchar buf[SIZE_BUFFER];
  gchar entry[SIZE_ENTRY];
  guint16 magic;
  GtkListStore *model = NULL;
  GtkTreeIter iter;
  GyEnglishPwn *self = GY_ENGLISH_PWN (dict);

  g_return_if_fail (GY_IS_ENGLISH_PWN (self));

  settings = g_settings_new ("org.gtk.gydict");
  path = g_settings_get_string (settings,
                                gy_dict_get_id_string (GY_DICT(self)));
  if (self->file)
    g_object_unref (self->file);

  self->file = g_file_new_for_path (path);

  model = gtk_list_store_new (1, G_TYPE_STRING);

  if (!(md5 = gy_utility_compute_md5_for_file (self->file, err)))
    goto out;

  if ((g_strcmp0 (md5, MD5_ANGPOL_06_07) != 0) && (g_strcmp0 (md5, MD5_POLANG_06_07) != 0))
    g_warning ("");

  if (!(in = g_file_read (self->file, NULL, err)))
    goto out;

  if (!g_seekable_seek (G_SEEKABLE (in), 0x68, G_SEEK_SET, NULL, err))
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), &word_count, sizeof(word_count), NULL, err)) <= 0)
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), &index_base, sizeof(index_base), NULL, err)) <= 0)
    goto out;

  if (!g_seekable_seek (G_SEEKABLE (in), 0x04, G_SEEK_CUR, NULL, err))
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), &word_base, sizeof(word_base), NULL, err)) <= 0)
    goto out;

  offsets = (guint32 *) g_malloc0 ((word_count + 1) * sizeof (guint32));
  self->offsets = (guint32 *) g_malloc0 ((word_count + 1) * sizeof (guint32));

  if (!g_seekable_seek (G_SEEKABLE (in), index_base, G_SEEK_SET, NULL, err))
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), offsets, (word_count * sizeof (guint32)), NULL, err)) <= 0)
    goto out;

	for (guint i = 0, j = 0; i < word_count; i++)
    {
#define MAGIC_OFFSET 0x03
#define OFFSET (12 - (MAGIC_OFFSET + sizeof (guint16)))

      magic = 0;
      offsets[i] &= 0x07ffffff;

      if (!g_seekable_seek (G_SEEKABLE (in), word_base+offsets[i]+MAGIC_OFFSET, G_SEEK_SET, NULL, err))
        goto out;

      if ((g_input_stream_read (G_INPUT_STREAM (in), &magic, sizeof (guint16), NULL, err)) <= 0)
        goto out;

      if (magic == 0x11dd || magic == 0x11d7)
        {
          g_autofree gchar *buf_conv = NULL;
          gchar *str = NULL;
          gsize len = 0;

          memset (entry, 0, SIZE_ENTRY);

          if ((g_input_stream_read (G_INPUT_STREAM (in), buf, SIZE_BUFFER, NULL, err)) <= 0)
            goto out;

          if (!(buf_conv = g_convert_with_fallback (buf+OFFSET, -1, "UTF-8", "ISO8859-2", NULL, NULL, NULL, err)))
            goto out;
          str = buf_conv;

          len = strcspn (str, "<&");
          strncat (entry, str,len);
          str = str + len;

          while (*str)
            {
              if (*str == '<')
                {
                  str = str + strcspn (str, ">") + 1;

                  if (g_ascii_isdigit (*str))
                    {
                      const gchar *sscript = gy_tabs_get_superscript ((*str) - 48);
                      strcat (entry, sscript);
                    }

                  str = str + strcspn (str, ">") + 1;
                }
              else if (*str == '&')
                {
                  g_autofree gchar *entity = NULL;

                  len = strcspn (str, ";");
                  entity = g_strndup (str, len);

                  strcat (entry,
                          (const gchar *) g_hash_table_lookup (self->entities, entity));
                  str += len + 1;
                }
              else
                {
                  len = strcspn (str, "<&");
                  strncat (entry, str, len);
                  str = str + len;
                }
            }
          gtk_list_store_append (model, &iter);
          gtk_list_store_set (model, &iter, 0, entry, -1);
          self->offsets[j++] = offsets[i] + word_base;
        }
#undef MAGIC
#undef MAGIC_OFFSET
#undef OFFSET
    }
  gy_dict_set_tree_model (dict, GTK_TREE_MODEL (model));
  g_object_set (dict, "is-map", TRUE, NULL);
  return;
out:
  g_debug ("");
  g_object_set (dict, "is-map", FALSE, NULL);
  return;
}


static void
gy_english_pwn_finalize (GObject *object)
{
  GyEnglishPwn *self = (GyEnglishPwn *)object;

  G_OBJECT_CLASS (gy_english_pwn_parent_class)->finalize (object);
}

static void
gy_english_pwn_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GyEnglishPwn *self = GY_ENGLISH_PWN (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_english_pwn_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GyEnglishPwn *self = GY_ENGLISH_PWN (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_english_pwn_class_init (GyEnglishPwnClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GyDictClass  *dict_class = GY_DICT_CLASS (klass);

  object_class->finalize = gy_english_pwn_finalize;
  object_class->get_property = gy_english_pwn_get_property;
  object_class->set_property = gy_english_pwn_set_property;

  dict_class->map = gy_english_pwn_map;
}

static void
gy_english_pwn_init (GyEnglishPwn *self)
{
}
