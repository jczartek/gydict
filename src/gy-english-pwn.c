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
#include "gy-text-buffer.h"
#include "gy-utility-func.h"
#include "gy-pwntabs.h"
#include "gy-parser-pwn.h"
#include "gy-parsable.h"

#define MD5_ANGPOL_06_07  "91f9344b0b9d40dcbb0a4c6027de98c1"
#define MD5_POLANG_06_07  "a1603dd25a7911d40edbb2b8fa89945d"

#define GY_ENGLISH_PWN_ERROR gy_english_pwn_error_quark ()


static void gy_english_pwn_start_tag (const gchar     *tag_name,
                                      const GPtrArray *attribute_name,
                                      const GPtrArray *attribute_value,
                                      gpointer         data);

static void gy_english_pwn_end_tag (const gchar *tag_name,
                                    gpointer     data);
static void gy_english_pwn_insert_text (const gchar *text,
                                        gsize        len,
                                        gpointer     data);
static void gy_english_pwn_parseable_iface_init (GyParsableInterface *iface);
static void gy_english_pwn_parse_lexical_unit (GyParsable    *parser,
                                               GtkTextBuffer *buffer,
                                               gint           index);

typedef struct _ParserData
{
  GtkTextBuffer   *buffer;
  GtkTextIter	     iter;
  GHashTable      *table_tags;
  GtkTextTagTable *table_buffer_tags;
} ParserData;

struct _GyEnglishPwn
{
  GyPwnDict  __parent__;
  GHashTable *entities;

  GyMarkupParserPwn *parser;
  ParserData        *pdata;
};

G_DEFINE_TYPE_WITH_CODE (GyEnglishPwn, gy_english_pwn, GY_TYPE_PWN_DICT,
                         G_IMPLEMENT_INTERFACE (GY_TYPE_PARSABLE,
                                                gy_english_pwn_parseable_iface_init))

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
gy_english_pwn_finalize (GObject *object)
{
  G_OBJECT_CLASS (gy_english_pwn_parent_class)->finalize (object);
}

static void
gy_english_pwn_constructed (GObject *object)
{
  GyEnglishPwn *self = GY_ENGLISH_PWN (object);

  G_OBJECT_CLASS (gy_english_pwn_parent_class)->constructed (object);

  g_object_get (GY_PWN_DICT (object),
                "entities", &self->entities, NULL);
  g_assert (self->entities);

  self->pdata = g_malloc0 (sizeof (ParserData));

  g_object_get (GY_DICT (object),
                "buffer", &self->pdata->buffer, NULL);
  g_assert (GTK_IS_TEXT_BUFFER (self->pdata->buffer));

  self->pdata->table_tags = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   g_free, NULL);
  self->pdata->table_buffer_tags = gtk_text_buffer_get_tag_table (self->pdata->buffer);

  g_assert (GTK_IS_TEXT_TAG_TABLE (self->pdata->table_buffer_tags) && self->pdata->table_tags);

  self->parser = gy_markup_parser_pwn_new (gy_english_pwn_start_tag,
                                           gy_english_pwn_end_tag,
                                           gy_english_pwn_insert_text,
                                           self->entities,
                                           self->pdata, NULL);
}

static void
gy_english_pwn_class_init (GyEnglishPwnClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GyPwnDictClass  *pwn_dict_class = GY_PWN_DICT_CLASS (klass);

  object_class->constructed = gy_english_pwn_constructed;
  object_class->finalize = gy_english_pwn_finalize;

  pwn_dict_class->check_checksum = gy_english_pwn_checksum;
  pwn_dict_class->query = gy_english_pwn_query;
}

static void
gy_english_pwn_init (GyEnglishPwn *self)
{
}

/* IFace */

static void
gy_english_pwn_parseable_iface_init (GyParsableInterface *iface)
{
  iface->parse = gy_english_pwn_parse_lexical_unit;
}

static void
gy_english_pwn_parse_lexical_unit (GyParsable    *parser,
                                   GtkTextBuffer *buffer,
                                   gint           index)
{
  GyDict *dict = GY_DICT (parser);
  GyEnglishPwn *self = GY_ENGLISH_PWN (parser);
  GError *err = NULL;
  g_autofree gchar *lexical_unit = NULL;
  gboolean is_mapped = FALSE;

  g_return_if_fail (GY_IS_DICT (dict));

  g_object_get (GY_DICT (parser),
                "is-mapped", &is_mapped, NULL);

  g_return_if_fail (is_mapped);

  lexical_unit = gy_pwn_dict_get_lexical_unit (GY_PWN_DICT (parser),
                                               index, &err);

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
gy_english_pwn_start_tag (const gchar     *tag_name,
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
gy_english_pwn_end_tag (const gchar *tag_name,
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
gy_english_pwn_insert_text (const gchar *text,
                           gsize        len,
                           gpointer     data)
{
  ParserData *pdata = (ParserData *) data;

  gy_text_buffer_insert_text_with_tags (GY_TEXT_BUFFER (pdata->buffer),
                                        &pdata->iter,
                                        text, len,
                                        pdata->table_tags);
}
