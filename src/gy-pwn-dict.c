/* gy-pwn-dict.c
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
#include <zlib.h>

#include "gy-pwn-dict.h"
#include "gy-dict.h"
#include "gy-german-pwn.h"
#include "gy-english-pwn.h"

#define GY_PWN_DICT_ERROR gy_pwn_dict_error_quark ()

typedef struct
{
  GFile *file;
  guint *offsets;
} GyPwnDictPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GyPwnDict, gy_pwn_dict, GY_TYPE_DICT)

enum {
  PROP_0,
  PROP_ENCODING,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static GQuark
gy_pwn_dict_error_quark (void)
{
  return g_quark_from_static_string ("gy-pwn-dict-error-quark");
}

static gchar *
gy_pwn_dict____get_lexical_unit (GyPwnDict  *self,
                                 guint       index,
                                 GError    **err)
{
  g_autoptr(GFileInputStream) in = NULL;
  gchar *in_buffer = NULL;
  gchar *out_buffer = NULL;
  guint i = 0;
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);

  g_return_val_if_fail (gy_dict_is_map (GY_DICT (self)), NULL);

#define MAXLEN 1024 * 90
#define OFFSET 12

  in_buffer = (gchar *) g_alloca (MAXLEN);

  if (!(in = g_file_read (priv->file, NULL, err)))
    goto out;

  if (!g_seekable_seek (G_SEEKABLE (in), priv->offsets[index], G_SEEK_SET, NULL, err))
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), in_buffer, MAXLEN, NULL, err)) <= 0)
    goto out;

  i = OFFSET + strlen (in_buffer + OFFSET) + 2;

  if (in_buffer[i] < 20)
    {
      gint zerr;
      uLongf destlen;
      i += in_buffer[i]+1;
      destlen = MAXLEN;

      out_buffer = (gchar *) g_malloc0 (MAXLEN);

      if ((zerr = uncompress ((Bytef *)out_buffer, &destlen, (const Bytef *)in_buffer+i, MAXLEN)) != Z_OK)
        {
          switch (zerr)
            {
            case Z_BUF_ERROR:
              g_set_error (err, GY_PWN_DICT_ERROR, zerr,
                           "The buffer out_buffer was not large enough to hold the uncompressed data!");
              break;
            case Z_MEM_ERROR:
              g_set_error (err, GY_PWN_DICT_ERROR, zerr,
                           "Insufficient memory!");
              break;
            case Z_DATA_ERROR:
              g_set_error (err, GY_PWN_DICT_ERROR, zerr,
                           "The compressed data (referenced by in_buffer) was corrupted!");
              break;
            };
          g_free (out_buffer);
          goto out;
        }
    }
  else
    {
      out_buffer = g_strdup (in_buffer + i);
    }

  return out_buffer;
out:
  return NULL;
#undef MAXLEN
#undef OFFSET
}

static void
gy_pwn_dict_query (GyPwnDict      *self,
                   GyDictPwnQuery *query)
{
  GyPwnDictClass *klass;

  g_return_if_fail (GY_IS_PWN_DICT (self));

  klass = GY_PWN_DICT_GET_CLASS (self);

  g_return_if_fail (klass->query != NULL);

  klass->query (self, query);
}

static gboolean
gy_pwn_dict_check_checksum (GyPwnDict  *self,
                            GFile      *file,
                            GError    **err)
{
  GyPwnDictClass *klass;

  g_return_val_if_fail (GY_IS_PWN_DICT (self), FALSE);

  klass = GY_PWN_DICT_GET_CLASS (self);

  g_return_val_if_fail (klass->check_checksum != NULL, FALSE);

  return klass->check_checksum (self, file, err);
}

static void
gy_pwn_dict_finalize (GObject *object)
{
  GyPwnDict *self = (GyPwnDict *)object;
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);

  G_OBJECT_CLASS (gy_pwn_dict_parent_class)->finalize (object);
}

static void
gy_pwn_dict_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GyPwnDict *self = GY_PWN_DICT (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_pwn_dict_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GyPwnDict *self = GY_PWN_DICT (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_pwn_dict_class_init (GyPwnDictClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_pwn_dict_finalize;
  object_class->get_property = gy_pwn_dict_get_property;
  object_class->set_property = gy_pwn_dict_set_property;

  klass->query = NULL;
  klass->check_checksum = NULL;
  klass->get_lexical_unit = gy_pwn_dict____get_lexical_unit;

  /**
   * GyPwnDict:encoding
   * This property represents an authentic dictionary encoding. It should be
   * overridden in a derived class.
   *
   */
  gParamSpecs [PROP_ENCODING] =
    g_param_spec_string ("encoding",
                         "Encoding",
                         "The authentic encoding of a dictionary.",
                         NULL,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class,
                                     LAST_PROP,
                                     gParamSpecs);

  g_type_ensure (GY_TYPE_GERMAN_PWN);
  g_type_ensure (GY_TYPE_ENGLISH_PWN);
}

static void
gy_pwn_dict_init (GyPwnDict *self)
{
}

/* PUBLIC */

gchar *
gy_pwn_dict_get_lexical_unit (GyPwnDict  *self,
                              guint       index,
                              GError    **err)
{
  GyPwnDictClass *klass;

  g_return_val_if_fail (GY_IS_PWN_DICT (self), NULL);

  klass = GY_PWN_DICT_GET_CLASS (self);

  g_return_val_if_fail (klass->get_lexical_unit != NULL, NULL);

  return klass->get_lexical_unit (self, index, err);
}
