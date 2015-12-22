/*
 * gy-pwn.c
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
 *
 * gy-pwn.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-pwn.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <glib/gi18n-lib.h>
#include <zlib.h>
#include <string.h>
#include <stdlib.h>

#include "gy-dict.h"
#include "gy-parsable.h"
#include "gy-pwn.h"
#include "gy-pwntabs.h"
#include "gy-utility-func.h"
#include "gy-parser-pwn.h"
#include "gy-window.h"

#define PWNDICT_MAGIC_11 0x81115747 /* Dictionary 2003 	*/
#define PWNDICT_MAGIC_12 0x81125747 /* Dictionary 2004  */
#define PWNDICT_MAGIC_13 0x81135747 /* Dictionary 2005  */
#define PWNDICT_MAGIC_14 0x81145747 /* Dictionary 06/07 */

typedef struct _ParserData ParserData;
typedef struct _GyPwnPrivate GyPwnPrivate;

struct _GyPwnPrivate
{
  guint32           *offset;
  FILE              *file_dict;
  GyMarkupParserPwn *parser;
  ParserData	      *pdata;
};

struct _ParserData
{
  GtkTextBuffer   *buffer;
  GtkTextIter	     iter;
  GHashTable      *table_tags;
  GtkTextTagTable *table_buffor_tags;
};

static void start_tag_cb (const gchar *tag_name,
                          const GPtrArray *attribute_name,
                          const GPtrArray *attribute_value,
                          gpointer data);
static void end_tag_cb (const gchar *tag_name,
                        gpointer     data);
static void text_cb (const gchar *text,
                     gsize        text_len,
                     gpointer     data);
static void gy_parser_dict_interface_init (GyParsableInterface *iface);
static void gy_pwn_parser_dict_parse (GyParsable    *parser,
                                      GtkTextBuffer *buffer,
                                      gint           row);

G_DEFINE_TYPE_WITH_CODE (GyPwn, gy_pwn, GY_TYPE_DICT,
                         G_ADD_PRIVATE (GyPwn)
                         G_IMPLEMENT_INTERFACE (GY_TYPE_PARSABLE,
                                                gy_parser_dict_interface_init));
static guint
gy_pwn_set_dictionary (GyDict *dict)
{
  g_autofree gchar * path_file = NULL;
  g_autoptr(GSettings) settings = NULL;
  GyPwnPrivate *priv = gy_pwn_get_instance_private (GY_PWN (dict));

  settings = g_settings_new ("org.gtk.gydict");
  path_file = g_settings_get_string (settings,
                                     gy_dict_get_id_string (dict));

  if (!g_file_test (path_file, G_FILE_TEST_EXISTS))
    {
      g_message ("File: %s does not exist!", path_file);
      return GY_EXISTS_FILE_ERROR;
    }

  if ( !(priv->file_dict = fopen (path_file, "rb")))
    {
      g_message ("Cannot open the %s!", path_file);
      return GY_OPEN_FILE_ERROR;
    }

  return GY_OK;
}

static guint
gy_pwn_init_list (GyDict *dict)
{
  guint32 word_count = 0, index_base = 0, word_base = 0, magic = 0;
  guint32 *offset = NULL;
  guint i = 0, j = 0;
  gchar * wordbuffer = NULL;
  GtkTreeIter iter;
  GtkListStore *model;
  GyPwnPrivate *priv = gy_pwn_get_instance_private (GY_PWN (dict));

  model =  gtk_list_store_new (1, G_TYPE_STRING);

  if (fread (&magic, sizeof (magic), 1, priv->file_dict) != 1)
    {
      g_warning ("Error:: Load data with file failed!!! (fread)");
    }

    switch (magic)
      {
        case PWNDICT_MAGIC_11:
        g_object_set (dict, "encoding-dict", GY_ENCODING_CP1250, NULL);
        break;
        case PWNDICT_MAGIC_12:
        g_object_set (dict, "encoding-dict", GY_ENCODING_CP1250, NULL);
        break;
        case PWNDICT_MAGIC_13:
        g_object_set (dict, "encoding-dict", GY_ENCODING_ISO88592 , NULL);
        break;
        case PWNDICT_MAGIC_14:
        g_object_set (dict, "encoding-dict", GY_ENCODING_ISO88592, NULL);
        break;
        default:
        g_warning ("Error:: The bad dictionary!");
        break;
    }
  if (magic == PWNDICT_MAGIC_14)
    {
      if (fseek (priv->file_dict, 0x68, SEEK_SET) == (off_t) -1)
        g_warning ("Error:: a file pointer is not moved");

      if (fread (&word_count, sizeof (word_count), 1, priv->file_dict) != 1)
        g_warning ("Error:: Load data with file failed!!! (fread)");

      if (fread( &index_base, sizeof (index_base), 1, priv->file_dict) != 1)
        g_warning ("Error:: Load data with file failed!!! (fread)");

      if (fseek (priv->file_dict, 0x04, SEEK_CUR) == (off_t) -1)
        g_warning ("Error:: a file pointer is not moved");

      if (fread (&word_base, sizeof (word_base), 1, priv->file_dict) != 1)
        g_warning ("Error:: Load data with file failed!!! (fread)");
    }
  else
    {
      if (fseek (priv->file_dict, 0x18, SEEK_SET) == (off_t) -1)
        g_warning ("Error:: a file pointer is not moved");

      if ( fread (&word_count, sizeof (word_count), 1, priv->file_dict) != 1)
        g_warning ("Error:: Load data with file failed!!! (fread)");

      if( fread (&index_base, sizeof (index_base), 1, priv->file_dict) != 1)
        g_warning ("Error:: Load data with file failed!!! (fread)");

      if( fread (&word_base, sizeof (word_base), 1, priv->file_dict) != 1)
        g_warning ("Error:: Load data with file failed!!! (fread)");

    }

  if(!(offset = (guint32 *) g_try_malloc0_n (word_count+1, sizeof (guint32))) )
    g_warning ("Error:: memory is not allocated for buffer (offset)");

  if (fseek (priv->file_dict, index_base, SEEK_SET) == (off_t) -1)
    g_warning ("Error:: a file pointer is not moved");

  if (fread (offset, 1, word_count * 4, priv->file_dict) != (word_count * 4))
    g_warning ("Error:: Load data with file failed!!! (fread)");

  for (i = 0; i<word_count; i++)
    offset[i] &= 0x07ffffff;

  if (!(priv->offset = (guint32 *) g_try_malloc0_n (word_count+1, sizeof (guint32))) )
    g_warning ("Error:: memory is not allocated for buffer (priv-offset)");

  if (!(wordbuffer = (gchar *) g_try_malloc0_n (300, sizeof (gchar))) )
    g_warning ("Error:: memory is not allocated for buffer (wordbuffer)");

  gchar buf[256];
  i=0;

  do
    {
      guint k,x;
      k = x = 0;
      memset(buf,0,G_N_ELEMENTS(buf));

      if (fseek(priv->file_dict, (word_base+offset[i]), SEEK_SET) == (off_t) -1)
        g_warning ("Error:: a file pointer is not moved");

      if (fread (wordbuffer, 1, 300, priv->file_dict) != 300)
        g_warning ("Error:: Load data with file failed!!! (fread)");

      if ((wordbuffer[2+1] != 0x49))
        {
          while (wordbuffer[12+k])
            {
              switch (wordbuffer[12+k])
                {
                  case '<':
                  {
                    guint8 len = 1;
                    gchar token[30];
                    const gchar *str;
                    token[0] = wordbuffer[12+k];
                    while( *(wordbuffer+12+k++) != '>')
                      token[len++] = (guchar) wordbuffer[12+k];
                    token[len] = '\0';

                    if( (g_strcmp0(token,"<SUP>")==0) && (g_ascii_isdigit(wordbuffer[12+k])) )
                      {
                        guint8 z;
                        str = pwndict_superscript_to_utf8_table[(guint) wordbuffer[12+k] - 48];

                        for(z=0;str[z] && k < G_N_ELEMENTS(buf);z++)
                          buf[x++] = (guchar) str[z];
                        x--;
                        break;
                      }
                    continue;
                  }
                  case '&':
                  {
                    guint8 len = 0;
                    guint8 z;
                    gchar token[20];
                    const gchar *str;

                    while( wordbuffer[12+k] != ';')
                      token[len++] = wordbuffer[12+k++];

                    token[len] = '\0';
                    str = pwn_encje_zam[gydict_utility_search_entity (pwn_encje,token)];

                    for(z=0;str[z] && k < G_N_ELEMENTS(buf);z++)
                      buf[x++] = (guchar) str[z];

                    x--;
                    break;
                  }
                  default:
                  if( (guchar) wordbuffer[12+k] < 127)
                    buf[x] = (gchar) wordbuffer[12+k];
                  else
                    {
                      const gchar *str;
                      guint8 z;
                      gint encoding = gy_dict_get_encoding (dict);
                      str = *(**(array_of_pointer_to_arrays_of_character_set + (guint) encoding)+((guchar) wordbuffer[k+12]-128));
                      for(z=0;str[z] && k < sizeof(buf);z++)
                        buf[x++] = (guchar) str[z];
                      x--;
                    }
                  break;

                }/* End switch */
              k++;
              x++;

            }/* End while */
          gtk_list_store_append (model, &iter);
          gtk_list_store_set (model, &iter, 0, buf, -1);
          priv->offset[j] = offset[i] + word_base;
          j++;

        }

    } while(++i < word_count);

  gy_dict_set_tree_model (dict, GTK_TREE_MODEL (model));
  g_free (offset);
  g_free (wordbuffer);

  return GY_OK;
}

static gpointer
gy_pwn_read_definition (GyDict *dict,
                        guint   index)
{
  guint32 maxlen = 90000;
  gchar *word_buffer = NULL, *out_buffer = NULL;
  guint i = 0;
  GyPwnPrivate *priv = gy_pwn_get_instance_private (GY_PWN (dict));

  if ((word_buffer = (gchar *) g_try_malloc0_n (maxlen,sizeof (gchar))) == NULL)
    {
      g_warning ("Error:: memory is not allocated for buffer with definition word");
      return NULL;
    }
  if ((out_buffer =  (gchar *) g_try_malloc0_n (maxlen,sizeof (gchar))) == NULL)
    {
      g_warning ("Error:: memory is not allocated for buffer with definition word");
      return NULL;
    }
  if (fseek (priv->file_dict, priv->offset[index], SEEK_SET) == (off_t) -1)
    {
      g_warning ("Error:: a file pointer is not moved");
      return NULL;
    }
  if (fread (word_buffer,1,maxlen,priv->file_dict) != (maxlen))
    {
      g_warning ("Error:: is not read definition word");
      return NULL;
    }

  i = 2+4+6+strlen (&word_buffer[2+4+6])+2;
  if (word_buffer[i] < 20)
    {
      int unzipresult;
      uLongf destlen;
      i += word_buffer[i]+1;
      destlen = maxlen;
      if( (unzipresult = uncompress ((Bytef*) out_buffer, &destlen, (const Bytef*) &word_buffer[i], maxlen)) != Z_OK)
        {
          g_warning("Error:: decompression failed. Error is %d",unzipresult);
          return NULL;
        }
    }
  else
    {
      out_buffer = g_strdup(&word_buffer[i]);
    }
  g_free(word_buffer);
  return (gchar *) out_buffer;
}

static void
gy_pwn_finalize (GObject *object)
{
  GyDict *dict = GY_DICT (object);
  GyPwnPrivate *priv = gy_pwn_get_instance_private (GY_PWN (dict));
  g_clear_pointer (&priv->pdata,
                   g_free);
  g_clear_pointer (&priv->offset,
                   g_free);
  g_clear_pointer (&priv->parser,
                   (GDestroyNotify) gy_markup_parser_pwn_free);
  if (fclose (priv->file_dict) == 0)
    priv->file_dict = NULL;

  G_OBJECT_CLASS (gy_pwn_parent_class)->finalize (object);
}

static void
gy_pwn_init (GyPwn *self)
{
  GApplication *app = NULL;
  GtkWindow *win = NULL;
  GyPwnPrivate *priv = gy_pwn_get_instance_private (self);

  priv->offset = NULL;
  priv->file_dict = NULL;
  priv->parser = NULL;
  priv->pdata = g_malloc0 (sizeof (ParserData));

  app = g_application_get_default ();
  win = gtk_application_get_active_window (GTK_APPLICATION (app));

  priv->pdata->buffer = gy_window_get_text_buffer (GY_WINDOW (win));
  priv->pdata->table_tags = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   g_free, NULL);
  priv->pdata->table_buffor_tags = gtk_text_buffer_get_tag_table (priv->pdata->buffer);

  g_return_if_fail (GTK_IS_TEXT_BUFFER (priv->pdata->buffer) &&
                    GTK_IS_TEXT_TAG_TABLE (priv->pdata->table_buffor_tags) &&
                    priv->pdata->table_tags);

  priv->parser = gy_markup_parser_pwn_new (start_tag_cb,
                                           end_tag_cb,
                                           text_cb,
                                           gy_tabs_get_entity_table (),
                                           priv->pdata, NULL);
}

static void
gy_pwn_class_init (GyPwnClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  GyDictClass *dict_class = GY_DICT_CLASS (klass);

  object_class->finalize = gy_pwn_finalize;

  dict_class->set_dictionary = gy_pwn_set_dictionary;
  dict_class->init_list = gy_pwn_init_list;
  dict_class->read_definition = gy_pwn_read_definition;
}


/************************IMPLEMENTED INTERFACE********************************/

static void
gy_parser_dict_interface_init (GyParsableInterface *iface)
{
  iface->parse = gy_pwn_parser_dict_parse;
}

static void
delete_double_new_lines (GtkTextBuffer *buffer)
{
  GtkTextIter match_start, match_end;

  gtk_text_buffer_get_start_iter (buffer, &match_start);
  gtk_text_buffer_get_start_iter (buffer, &match_end);

  while (gtk_text_iter_forward_search (&match_end, "\n\n",
                                       GTK_TEXT_SEARCH_VISIBLE_ONLY,
                                       &match_start, &match_end, NULL))
  {
    gtk_text_iter_forward_char (&match_start);
    gtk_text_buffer_delete (buffer,
                            &match_start,
                            &match_end);
  }
}
static void
gy_pwn_parser_dict_parse (GyParsable    *parser,
                          GtkTextBuffer *buffer,
                          gint 	         row)
{
  GyDict *dict = GY_DICT (parser);
  GyPwnPrivate *priv = gy_pwn_get_instance_private (GY_PWN (dict));
  gchar *buf = NULL;
  const gchar *id_dict = gy_dict_get_id_string (GY_DICT (parser));
  gint encoding = gy_dict_get_encoding (dict);

  g_return_if_fail (GY_IS_DICT (dict));
  g_return_if_fail (GTK_IS_TEXT_BUFFER (buffer));
  g_return_if_fail (priv->parser != NULL);
  g_return_if_fail (priv->pdata != NULL);

  buf = gy_dict_read_definition (dict, (guint) row);
  gtk_text_buffer_get_iter_at_offset (buffer, &priv->pdata->iter, 0);
  gy_markup_parser_pwn_parse (priv->parser, (const gchar *) buf, -1, encoding);

  if (strcmp (id_dict, "dict-pwn-polang") == 0)
    {
      /*
       * W słowniku polsko-angielskim, występuje podwójny znak nowej linii.
       * Nie jest to wina parsera, tylko słownika. Funkcja delete_double_new_lines
       * wyszukuje podwójny znak nowej linii w GtkTextBuffer i usuwa jeden ze znaków.
       */
      delete_double_new_lines (buffer);
    }

  g_free (buf);
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
start_tag_cb (const gchar     *tag_name,
              const GPtrArray *attribute_name,
              const GPtrArray *attribute_value,
              gpointer         data)
{
  ParserData *pdata = (ParserData *) data;
  if (is_tag_format (tag_name))
  {
    GtkTextTag *tag = NULL;
    gchar *name = g_utf8_strdown (tag_name, -1);

    tag = gtk_text_tag_table_lookup (pdata->table_buffor_tags,
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
end_tag_cb (const gchar *tag_name,
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
text_cb (const gchar *text,
         gsize        text_len,
         gpointer     data)
{
  ParserData *pdata = (ParserData *) data;

  gy_utility_text_buffer_insert_text_with_tags (pdata->buffer,
                                                &pdata->iter,
                                                text, text_len,
                                                pdata->table_tags);
}
