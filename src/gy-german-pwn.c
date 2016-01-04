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

#include "gy-german-pwn.h"
#include "gy-utility-func.h"

#define MD5_NIEMPOL   "c0f2280d5bedfc5c88620dcef512b897"
#define MD5_POLNIEM   "2b551364dd36ef263381276ee352c59f"

struct _GyGermanPwn
{
  GyDict parent;

  GFile *file;
  guint32 *offsets;
};

G_DEFINE_TYPE (GyGermanPwn, gy_german_pwn, G_TYPE_OBJECT)

enum {
  PROP_0,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static void
gy_german_pwn_initialize (GyDict  *dict,
                          GError **err)
{
  guint32 word_count = 0, index_base = 0, word_base = 0;
	g_autofree gchar *md5 = NULL;
	g_autofree gchar *path = NULL;
	g_autofree guint32 *offsets = NULL;
	g_autoptr(GFileInputStream) in = NULL;
	g_autoptr(GSettings) settings = NULL;
  GyGermanPwn *self = GY_GERMAN_PWN (dict);

  g_return_if_fail (GY_IS_GERMAN_PWN (self));

	settings = g_settings_new ("org.gtk.gydict");
	path = g_settings_get_string (settings, "dict-pwn-niempol");

	self->file = g_file_new_for_path (path);

  if (!(md5 = gy_utility_compute_md5_for_file (self->file, err)))
    goto out;

	if ((g_strcmp0 (md5, MD5_NIEMPOL) != 0) && (g_strcmp0 (md5, MD5_POLNIEM) != 0))
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

	for (guint i = 0; i < word_count; i++)
    {
#define MAGIC_OFFSET 0x03
#define MAGIC 0x11dd
#define OFFSET (12 - (MAGIC_OFFSET + sizeof (guint16)))

      g_autoptr(GString) str = g_string_new_len (NULL, 256);
      g_autofree gchar *buf = (gchar *) g_malloc0 (300 * sizeof (gchar));
      guint16 magic;

      magic = 0;
      offsets[i] &= 0x07ffffff;

		  if (!g_seekable_seek (G_SEEKABLE (in), word_base+offsets[i]+MAGIC_OFFSET, G_SEEK_SET, NULL, err))
        goto out;

      if ((g_input_stream_read (G_INPUT_STREAM (in), &magic, sizeof (guint16), NULL, err)) <= 0)
        goto out;

      if (magic == MAGIC)
        {
          g_autofree gchar *buf_conv = NULL;

          if ((g_input_stream_read (G_INPUT_STREAM (in), buf, 300, NULL, err)) <= 0)
            goto out;

          if (!(buf_conv = g_convert_with_fallback (buf+OFFSET, -1, "UTF-8", "ISO8859-2", NULL, NULL, NULL, err)))
            goto out;

        }
#undef MAGIC
#undef MAGIC_OFFSET
#undef OFFSET
    }

	return;
out:
    g_critical ("");
    return;
}

static guint
gy_german_pwn_set_dictionary (GyDict *self)
{
  return GY_OK;
}

static guint
gy_german_pwn_init_list (GyDict *self)
{
  return GY_OK;
}

static gpointer
gy_german_pwn_read_definition (GyDict *self,
                               guint   index)
{
  return NULL;
}

static void
gy_german_pwn_finalize (GObject *object)
{
  GyGermanPwn *self = (GyGermanPwn *)object;

  G_OBJECT_CLASS (gy_german_pwn_parent_class)->finalize (object);
}

static void
gy_german_pwn_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GyGermanPwn *self = GY_GERMAN_PWN (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_german_pwn_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GyGermanPwn *self = GY_GERMAN_PWN (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_german_pwn_class_init (GyGermanPwnClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GyDictClass  *dict_class = GY_DICT_CLASS (klass);

  object_class->finalize = gy_german_pwn_finalize;
  object_class->get_property = gy_german_pwn_get_property;
  object_class->set_property = gy_german_pwn_set_property;

  dict_class->set_dictionary = gy_german_pwn_set_dictionary;
  dict_class->init_list = gy_german_pwn_init_list;
  dict_class->read_definition = gy_german_pwn_read_definition;
  dict_class->initialize = gy_german_pwn_initialize;
}

static void
gy_german_pwn_init (GyGermanPwn *self)
{
}
