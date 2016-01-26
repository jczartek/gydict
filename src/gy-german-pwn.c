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

#include <string.h>
#include <stdlib.h>
#include <zlib.h>

#include "gy-german-pwn.h"
#include "gy-utility-func.h"
#include "gy-pwntabs.h"
#include "gy-parser-pwn.h"
#include "gy-parsable.h"
#include "gy-window.h"

#define MD5_NIEMPOL   "c0f2280d5bedfc5c88620dcef512b897"
#define MD5_POLNIEM   "2b551364dd36ef263381276ee352c59f"

#define SIZE_BUFFER  128
#define SIZE_ENTRY   64

#define GY_GERMAN_PWN_ERROR gy_german_pwn_error_quark ()

static void gy_german_pwn_start_tag (const gchar     *tag_name,
                                     const GPtrArray *attribute_name,
                                     const GPtrArray *attribute_value,
                                     gpointer         data);

static void gy_german_pwn_end_tag (const gchar *tag_name,
                                   gpointer     data);
static void gy_german_pwn_insert_text (const gchar *text,
                                       gsize        len,
                                       gpointer     data);
static void gy_german_pwn_parseable_iface_init (GyParsableInterface *iface);
static void gy_german_pwn_parse_lexical_unit (GyParsable    *parser,
                                              GtkTextBuffer *buffer,
                                              gint           index);

typedef struct _ParserData ParserData;

struct _GyGermanPwn
{
  GyDict parent;

  GFile      *file;
  guint32    *offsets;
  GHashTable *entities;

  GyMarkupParserPwn *parser;
  ParserData        *pdata;
};

struct _ParserData
{
  GtkTextBuffer      *buffer;
  GtkTextIter         iter;
  GHashTable         *table_tags;
  GtkTextTagTable    *table_buffer_tags;
};


G_DEFINE_TYPE_WITH_CODE (GyGermanPwn, gy_german_pwn, GY_TYPE_DICT,
                         G_IMPLEMENT_INTERFACE (GY_TYPE_PARSABLE,
                                                gy_german_pwn_parseable_iface_init));

enum {
  PROP_0,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static GQuark
gy_german_pwn_error_quark (void)
{
  return g_quark_from_static_string ("gy-german-pwn-error-quark");
}

static void
gy_german_pwn_map (GyDict  *dict,
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
  GyGermanPwn *self = GY_GERMAN_PWN (dict);

  g_return_if_fail (GY_IS_GERMAN_PWN (self));

  settings = g_settings_new ("org.gtk.gydict");
  path = g_settings_get_string (settings,
                                gy_dict_get_id_string (GY_DICT(self)));
  if (self->file)
    g_object_unref (self->file);

  self->file = g_file_new_for_path (path);

  model = gtk_list_store_new (1, G_TYPE_STRING);

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

static gchar *
gy_german_pwn_get_lexical_unit (GyDict  *dict,
                                guint    index,
                                GError **err)
{
  g_autoptr(GFileInputStream) in = NULL;
  GyGermanPwn *self = GY_GERMAN_PWN (dict);
  gchar *in_buffer = NULL;
  gchar *out_buffer = NULL;
  guint i = 0;

  g_return_val_if_fail (GY_IS_DICT (dict), NULL);
  g_return_val_if_fail (gy_dict_is_map (dict), NULL);

#define MAXLEN 1024 * 90
#define OFFSET 12

  in_buffer = (gchar *) g_alloca (MAXLEN);
  out_buffer = (gchar *) g_malloc0 (MAXLEN);

  if (!(in = g_file_read (self->file, NULL, err)))
    goto out;

  if (!g_seekable_seek (G_SEEKABLE (in), self->offsets[index], G_SEEK_SET, NULL, err))
    goto out;

  if ((g_input_stream_read (G_INPUT_STREAM (in), in_buffer, MAXLEN, NULL, err)) <= 0)
    goto out;

  i = 12 + strlen (in_buffer+12) + 2;

  if (in_buffer[i] < 20)
    {
      gint zerr;
      uLongf destlen;
      i += in_buffer[i]+1;
      destlen = MAXLEN;

      if ((zerr = uncompress ((Bytef *)out_buffer, &destlen, (const Bytef *)in_buffer+i, MAXLEN)) != Z_OK)
        {
          switch (zerr)
            {
            case Z_BUF_ERROR:
              g_set_error (err, GY_GERMAN_PWN_ERROR, zerr,
                           "The buffer out_buffer was not large enough to hold the uncompressed data!");
              break;
            case Z_MEM_ERROR:
              g_set_error (err, GY_GERMAN_PWN_ERROR, zerr,
                           "Insufficient memory!");
              break;
            case Z_DATA_ERROR:
              g_set_error (err, GY_GERMAN_PWN_ERROR, zerr,
                           "The compressed data (referenced by in_buffer) was corrupted!");
              break;
            };
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

static guint
gy_german_pwn_set_dictionary (GyDict *self)
{
  return GY_OK;
}

static guint
gy_german_pwn_init_list (GyDict *self)
{
  GError *err = NULL;

  gy_german_pwn_map (self, &err);

  if (err != NULL)
    {
      g_critical ("%s", err->message);
      return GY_FAILED_OBJECT;
    }

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

  g_clear_object (&self->file);
  g_clear_pointer (&self->offsets, g_free);
  g_clear_pointer (&self->entities, g_hash_table_unref);
  g_clear_pointer (&self->parser, gy_markup_parser_pwn_free);
  g_clear_pointer (&self->pdata->table_tags, g_hash_table_unref);
  g_clear_pointer (&self->pdata, g_free);

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
gy_german_pwn_constructed (GObject *object)
{
  G_OBJECT_CLASS (gy_german_pwn_parent_class)->constructed (object);
}

static void
gy_german_pwn_class_init (GyGermanPwnClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GyDictClass  *dict_class = GY_DICT_CLASS (klass);

  object_class->finalize = gy_german_pwn_finalize;
  object_class->constructed = gy_german_pwn_constructed;
  object_class->get_property = gy_german_pwn_get_property;
  object_class->set_property = gy_german_pwn_set_property;

  dict_class->set_dictionary = gy_german_pwn_set_dictionary;
  dict_class->init_list = gy_german_pwn_init_list;
  dict_class->map = gy_german_pwn_map;
}

static void
gy_german_pwn_init (GyGermanPwn *self)
{
  GApplication *app = NULL;
  GtkWindow    *win = NULL;

  self->file = NULL;
  self->offsets = NULL;
  self->parser = NULL;
  self->pdata = g_malloc0 (sizeof (ParserData));
  self->entities = gy_tabs_get_entity_table ();

  app = g_application_get_default ();
  win = gtk_application_get_active_window (GTK_APPLICATION (app));

  self->pdata->buffer = gy_window_get_text_buffer (GY_WINDOW (win));
  self->pdata->table_tags = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   g_free, NULL);
  self->pdata->table_buffer_tags = gtk_text_buffer_get_tag_table (self->pdata->buffer);

  g_assert (GTK_IS_TEXT_BUFFER (self->pdata->buffer) &&
            GTK_IS_TEXT_TAG_TABLE (self->pdata->table_buffer_tags) &&
            self->pdata->table_tags);

  self->parser = gy_markup_parser_pwn_new (gy_german_pwn_start_tag,
                                           gy_german_pwn_end_tag,
                                           gy_german_pwn_insert_text,
                                           self->entities,
                                           self->pdata, NULL);


}

/* IFace */

static void
gy_german_pwn_parseable_iface_init (GyParsableInterface *iface)
{
  iface->parse = gy_german_pwn_parse_lexical_unit;
}

static void
gy_german_pwn_parse_lexical_unit (GyParsable    *parser,
                                  GtkTextBuffer *buffer,
                                  gint           index)
{
  GyDict *dict = GY_DICT (parser);
  GyGermanPwn *self = GY_GERMAN_PWN (parser);
  GError *err = NULL;
  g_autofree gchar *lexical_unit = NULL;
  gboolean is_map = FALSE;

  g_return_if_fail (GY_IS_DICT (dict));

  g_object_get (GY_DICT (parser),
                "is-map", &is_map, NULL);

  g_return_if_fail (is_map);

  lexical_unit = gy_german_pwn_get_lexical_unit (dict, index, &err);

  if (err != NULL)
    {
      g_critical ("%s", err->message);
      g_clear_error (&err);
      goto out;
    }
  gtk_text_buffer_get_iter_at_offset (self->pdata->buffer,
                                      &self->pdata->iter, 0);
  gy_markup_parser_pwn_parse (self->parser, (const gchar *) lexical_unit,
                              -1, GY_ENCODING_ISO88592);

out:
  return;
}

static gchar *format_tags[] = {"B", "BIG", "PH", "SMALL", "I", "SUB", "SUP"};
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
gy_german_pwn_start_tag (const gchar     *tag_name,
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
      gy_utility_text_buffer_insert_text_with_tags (pdata->buffer,
                                                    &pdata->iter,
                                                    roman_numbers[index], -1,
                                                    pdata->table_tags);
#undef LENGTH_PREFIX
#undef LENGTH_SUFFIX
    }
    else if (g_str_has_prefix (str, "idioms"))
    {
      gy_utility_text_buffer_insert_text_with_tags (pdata->buffer,
                                                    &pdata->iter,
                                                    "IDIOM", -1,
                                                    pdata->table_tags);
    }
    return;
  }
}

static void
gy_german_pwn_end_tag (const gchar *tag_name,
                       gpointer     data)
{
  ParserData *pdata = (ParserData *) data;

  if (is_tag_format (tag_name))
    {
      gchar *name = g_utf8_strdown (tag_name, -1);
      g_hash_table_remove (pdata->table_tags, name);
      g_free (name);
      return;

    }
}

static void
gy_german_pwn_insert_text (const gchar *text,
                           gsize        len,
                           gpointer     data)
{
  ParserData *pdata = (ParserData *) data;

  gy_utility_text_buffer_insert_text_with_tags (pdata->buffer,
                                                &pdata->iter,
                                                text, len,
                                                pdata->table_tags);
}
