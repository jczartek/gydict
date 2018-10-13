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

#define G_LOG_DOMAIN "GyPwnDict"

#include <string.h>
#include <stdlib.h>
#include <zlib.h>

#include "gy-pwn-dict.h"
#include "gy-dict.h"
#include "gy-german-pwn.h"
#include "gy-english-pwn.h"
#include "gy-pwntabs.h"
#include "gy-parsable.h"
#include "gy-parser-pwn.h"
#include "entryview/gy-text-buffer.h"

#define SIZE_BUFFER  128
#define SIZE_ENTRY   64

#define DEFAULT_ENCODING "ISO8859-2"

#define GY_PWN_DICT_ERROR gy_pwn_dict_error_quark ()

static gboolean gy_pwn_dict_check_checksum (GyPwnDict  *self,
                                            GFile      *file,
                                            GError    **err);
static void gy_pwn_dict_query (GyPwnDict      *self,
                               GyDictPwnQuery *query);
static void gy_pwn_dict_parsable_iface_init (GyParsableInterface *iface);

typedef struct _ParserData
{
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  GHashTable *table_tags;
  GtkTextTagTable *table_buffer_tags;
} ParserData;

typedef struct
{
  GFile      *file;
  guint      *offsets;
  GHashTable *entities;

  GyMarkupParserPwn *parser;
  ParserData        *pdata;
} GyPwnDictPrivate;

G_DEFINE_TYPE_WITH_CODE (GyPwnDict, gy_pwn_dict, GY_TYPE_DICT,
                         G_ADD_PRIVATE (GyPwnDict)
                         G_IMPLEMENT_INTERFACE (GY_TYPE_PARSABLE,
                                                gy_pwn_dict_parsable_iface_init))

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
gy_pwn_dict_map (GyDict  *dict,
                 GError **err)
{
  guint32 word_count = 0, index_base = 0, word_base = 0;
  g_autofree guint32 *offsets = NULL;
  g_autoptr(GFileInputStream) in = NULL;
  g_autoptr(GSettings)  settings = NULL;
  g_autofree gchar     *path = NULL;
  const gchar *id = NULL;
  gchar buf[SIZE_BUFFER];
  gchar entry[SIZE_ENTRY];
  guint16 magic;
  g_autoptr(GtkListStore) model = NULL;
  GtkTreeIter iter;
  GyDictPwnQuery query = GY_PWN_DICT_QUERY_INIT;
  GyPwnDict *self = GY_PWN_DICT (dict);
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);

  g_return_if_fail (GY_IS_PWN_DICT (self));

  g_object_get (dict, "identifier", &id, NULL);

  settings = g_settings_new ("org.gtk.gydict.paths");
  path = g_settings_get_string (settings, id);

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
  g_object_set (dict, "model", model, NULL);
  g_object_set (dict, "is-mapped", TRUE, NULL);
  return;
out:
  g_object_set (dict, "is-mapped", FALSE, NULL);
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

  g_return_val_if_fail (gy_dict_is_mapped (GY_DICT (self)), NULL);

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
            default:
              ;
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

  if (priv->pdata)
    {
      g_object_unref (priv->pdata->buffer);
      g_hash_table_unref (priv->pdata->table_tags);
      g_clear_pointer (&priv->pdata, g_free);
    }

  if (priv->parser) g_clear_pointer (&priv->parser, gy_markup_parser_pwn_free);

  if (priv->entities) g_clear_pointer (&priv->entities, g_hash_table_unref);

  if (priv->file) g_clear_object (&priv->file);

  if (priv->offsets) g_clear_pointer (&priv->offsets, g_free);

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
      g_value_set_static_boxed (value, priv->entities);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_pwn_dict_class_init (GyPwnDictClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GyDictClass *dict_class = GY_DICT_CLASS (klass);

  object_class->finalize = gy_pwn_dict_finalize;
  object_class->get_property = gy_pwn_dict_get_property;

  dict_class->map = gy_pwn_dict_map;

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
    g_param_spec_boxed ("entities",
                        "Entities",
                        "The table of PWN entities",
                        G_TYPE_HASH_TABLE,
                        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class,
                                     LAST_PROP,
                                     gParamSpecs);

}

static void
gy_pwn_dict_init (GyPwnDict *self)
{
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);

  priv->entities = gy_tabs_get_entity_table ();
  priv->parser = NULL;
  priv->pdata = NULL;
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

/* Interface */
static gchar *format_tags[] = {"A","B", "BIG", "PH", "SMALL", "I", "SUB", "SUP"};
static gchar *roman_numbers[] = {"", "I", "II", "III", "IV", "V", "VI", "VII",
                                 "VIII", "IX", "X", "XI", "XII", "XIII", "XIV",
                                 "XV", "XVI", "XVII", "XVIII", "XIX", "XX"};

static inline gboolean
is_tag_format (const gchar *tag)
{
  for (gint i = 0; i < G_N_ELEMENTS (format_tags); i++)
    if (strcmp (tag, format_tags[i]) == 0)
      return TRUE;
  return FALSE;
}


static void
gy_pwn_dict_start_tag (const gchar     *tag_name,
                       const GPtrArray *attribute_name,
                       const GPtrArray *attribute_value,
                       gpointer         data)
{
  ParserData *pdata = (ParserData *) data;

  if (is_tag_format (tag_name))
  {
    GtkTextTag *tag = NULL;
    gchar *name = g_utf8_strdown (tag_name, -1);

    tag = gtk_text_tag_table_lookup (pdata->table_buffer_tags,
                                     (const gchar *) name);
    g_hash_table_insert (pdata->table_tags,
                         (gpointer) name,
                         (gpointer) tag);
    g_assert (GTK_IS_TEXT_TAG (tag));
    return;
  }

  if (strcmp (tag_name, "P") == 0)
  {
    gtk_text_buffer_insert (pdata->buffer,
                            &pdata->iter, "\n", -1);
    return;
  }

  if (strcmp (tag_name, "IMG") == 0)
  {
    g_return_if_fail (attribute_name->len == attribute_value->len);
    gchar *str = *attribute_value->pdata;
    if (g_str_has_prefix (str, "rzym") && g_str_has_suffix (str, ".jpg"))
    {
#define LENGTH_PREFIX	4
#define LENGTH_SUFFIX	4
      gulong end_pos = LENGTH_PREFIX + (strlen (str) - (LENGTH_PREFIX + LENGTH_SUFFIX));
      gchar *number = g_utf8_substring (str, LENGTH_SUFFIX, end_pos);
      gint index = atoi ((const gchar *) number);
      g_free (number);
      gy_text_buffer_insert_text_with_tags (GY_TEXT_BUFFER (pdata->buffer),
                                            &pdata->iter,
                                            roman_numbers[index], -1,
                                            pdata->table_tags);
#undef LENGTH_PREFIX
#undef LENGTH_SUFFIX
    }
    else if (g_str_has_prefix (str, "idioms"))
    {
      gy_text_buffer_insert_text_with_tags (GY_TEXT_BUFFER (pdata->buffer),
                                            &pdata->iter,
                                            "IDIOM", -1,
                                            pdata->table_tags);
    }
    return;
  }
}

static void
gy_pwn_dict_end_tag (const gchar *tag_name,
                     gpointer     data)
{
  ParserData *pdata = (ParserData *) data;

  if (is_tag_format (tag_name))
    {
      gchar *name = g_utf8_strdown (tag_name, -1);
      g_hash_table_remove (pdata->table_tags, name);
      g_free (name);
    }
}

static void
gy_pwn_dict_insert_text (const gchar *text,
                         gsize        len,
                         gpointer     data)
{
  ParserData *pdata = (ParserData *) data;

  gy_text_buffer_insert_text_with_tags (GY_TEXT_BUFFER (pdata->buffer),
                                        &pdata->iter,
                                        text, len,
                                        pdata->table_tags);
}

static void
gy_pwn_dict_parse_lexical_unit (GyParsable    *p,
                                GtkTextBuffer *buffer,
                                gint           index)
{
  GyPwnDict *self = GY_PWN_DICT (p);
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);
  GError *err = NULL;
  g_autofree gchar *lexical_unit = NULL;
  gboolean is_mapped = FALSE;

  g_return_if_fail (GY_IS_PWN_DICT (self));
  g_return_if_fail (GTK_IS_TEXT_BUFFER (buffer));

  g_object_get (GY_DICT (self), "is-mapped", &is_mapped, NULL);
  g_return_if_fail (is_mapped);

  if (priv->parser == NULL && priv->pdata == NULL)
    {
      priv->pdata = g_malloc0 (sizeof (ParserData));
      priv->pdata->buffer = g_object_ref (buffer);
      priv->pdata->table_tags = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                       g_free, NULL);
      priv->pdata->table_buffer_tags = gtk_text_buffer_get_tag_table (buffer);

      priv->parser = gy_markup_parser_pwn_new (gy_pwn_dict_start_tag,
                                               gy_pwn_dict_end_tag,
                                               gy_pwn_dict_insert_text,
                                               priv->entities,
                                               priv->pdata, NULL);
      g_assert (GTK_IS_TEXT_TAG_TABLE (priv->pdata->table_buffer_tags) &&
                priv->parser != NULL);
    }

  lexical_unit = gy_pwn_dict_get_lexical_unit (self, index, &err);
  if (err)
    {
      g_critical ("%s", err->message);
      g_clear_error (&err);
      return;
    }

  gtk_text_buffer_get_iter_at_offset (priv->pdata->buffer,
                                      &priv->pdata->iter, 0);
  gy_markup_parser_pwn_parse (priv->parser, (const gchar *) lexical_unit,
                              -1, GY_ENCODING_ISO88592);
}

static void
gy_pwn_dict_parsable_iface_init (GyParsableInterface *iface)
{
  iface->parse = gy_pwn_dict_parse_lexical_unit;
}
