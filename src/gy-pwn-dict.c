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
#include "gy-pwntabs.h"

#define SIZE_BUFFER  128
#define SIZE_ENTRY   64

#define DEFAULT_ENCODING "ISO8859-2"

#define GY_PWN_DICT_ERROR gy_pwn_dict_error_quark ()

static gboolean gy_pwn_dict_check_checksum (GyPwnDict  *self,
                                            GFile      *file,
                                            GError    **err);
static void gy_pwn_dict_query (GyPwnDict      *self,
                               GyDictPwnQuery *query);

typedef struct
{
  GFile      *file;
  guint      *offsets;
  GHashTable *entities;
} GyPwnDictPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GyPwnDict, gy_pwn_dict, GY_TYPE_DICT)

enum {
  PROP_0,
  PROP_ENCODING,
  PROP_ENTITY,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static GQuark
gy_pwn_dict_error_quark (void)
{
  return g_quark_from_static_string ("gy-pwn-dict-error-quark");
}

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
  GyDictPwnQuery query = GY_PWN_DICT_QUERY_INIT;
  GyPwnDict *self = GY_PWN_DICT (dict);
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);

  g_return_if_fail (GY_IS_PWN_DICT (self));

  settings = g_settings_new ("org.gtk.gydict");
  path = g_settings_get_string (settings,
                                gy_dict_get_id_string (GY_DICT(self)));
  if (priv->file)
    g_object_unref (priv->file);

  priv->file = g_file_new_for_path (path);

  model = gtk_list_store_new (1, G_TYPE_STRING);

  gy_pwn_dict_query (self, &query);

  if (!gy_pwn_dict_check_checksum (self, priv->file, err))
    goto out;

  if (!(in = g_file_read (priv->file, NULL, err)))
    goto out;

  if (!g_seekable_seek (G_SEEKABLE (in), query.offset1, G_SEEK_SET, NULL, err))
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), &word_count, sizeof(word_count), NULL, err)) <= 0)
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), &index_base, sizeof(index_base), NULL, err)) <= 0)
    goto out;

  if (!g_seekable_seek (G_SEEKABLE (in), query.offset2, G_SEEK_CUR, NULL, err))
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), &word_base, sizeof(word_base), NULL, err)) <= 0)
    goto out;

  offsets = (guint32 *) g_malloc0 ((word_count + 1) * sizeof (guint32));
  priv->offsets = (guint32 *) g_malloc0 ((word_count + 1) * sizeof (guint32));

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

      if (magic == query.magic1 || magic == query.magic2)
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
                          (const gchar *) g_hash_table_lookup (priv->entities, entity));
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
          priv->offsets[j++] = offsets[i] + word_base;
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

  g_clear_pointer (&priv->entities,
                   g_hash_table_unref);

  G_OBJECT_CLASS (gy_pwn_dict_parent_class)->finalize (object);
}

static void
gy_pwn_dict_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GyPwnDict *self = GY_PWN_DICT (object);
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_ENCODING:
      g_value_set_static_string (value, DEFAULT_ENCODING);
      break;
    case PROP_ENTITY:
      g_value_set_pointer (value, priv->entities);
      break;
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
   * GyPwnDict:encoding:
   * This property represents an default encoding of a PWN dictionary.
   *
   * If a default encoding of a dictionary is not ISO8859-2, this property
   * should be overridden in a derived class.
   *
   */
  gParamSpecs [PROP_ENCODING] =
    g_param_spec_string ("encoding",
                         "Encoding",
                         "The default encoding of a PWN dictionary.",
                         DEFAULT_ENCODING,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * GyPwnDict:entity:
   * This property represents PWN entities.
   *
   */
  gParamSpecs [PROP_ENTITY] =
    g_param_spec_pointer ("entity",
                          "Entity",
                          "The table of PWN entities",
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
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);

  priv->entities = gy_tabs_get_entity_table ();
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
