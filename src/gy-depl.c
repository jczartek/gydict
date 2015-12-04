/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gy-depl.c
 * Copyright (C) 2014 kuba <kuba@linux.pl>
 *
 * gy-depl.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-depl.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <glib/gi18n-lib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "gy-pwntabs.h"
#include "gy-dict.h"
#include "gy-parser.h"
#include "gy-depl.h"

#define SIZE_PARSER_BUFFER 256
#define IS_COMMON_NAME_END_CHAR(c) \
    ((c) == '=' || (c) == '/' || (c) == '>')
#define IS_SPACE(c) \
    ((c) == ' ' || (c) == '\t')
#define IS_ANGLE_OPEN(c) \
    ((c) == '<')
#define IS_ANGLE_CLOSE(c) \
    ((c) == '>')

typedef struct _ParserContext ParserContext;
typedef struct _ParserTagDescription ParserTagDescription;
typedef struct _GyDeplPrivate GyDeplPrivate;

static void gy_parser_interface_init (GyParserDictInterface *iface);
static void parser_dict (GyParserDict  *parser,
                         GtkTextBuffer *buffer,
                         gint           row);
static void start_element_cb (ParserContext *context);
static void end_element_cb (ParserContext *context);
static ParserTagDescription* parse_tag_description_new (void);
static void parse_tag_description_free (ParserTagDescription *tag_description);
ParserContext * parse_context_new (void);
static void parse_context_free_contents_parser_context (ParserContext *context);
static void parse_context_free (ParserContext **context);
static inline void insert_sign_buffer (gchar **dest, 
                                       const gchar *src,
                                       const gchar **tab);
static inline void insert_text_buffor (ParserContext *context);
static inline gboolean advance_char (ParserContext *context);
static inline void skip_spaces (ParserContext *context);
static void advance_to_name_end (ParserContext *context);
static gboolean parse_context_parse (ParserContext *context,
                                     const gchar *text,
                                     gint length_text,
                                     GtkTextBuffer *text_buffer);
typedef enum
{
  STATE_NEUTRAL,
  STATE_AFTER_OPEN_ANGLE,
  STATE_AFTER_CLOSE_ANGLE,
  STATE_OPEN_TAG,
  STATE_CLOSE_TAG,
  STATE_BEFORE_CLOSE_ANGLE,
  STATE_BEFORE_READ_ATTRIBUTE,
  STATE_READ_ATTRIBUTE,
  STATE_BEFORE_READ_VALUE_ATTR,
  STATE_READ_VALUE_ATTR,
  STATE_INSERT_TEXT,
  STATE_ERROR
} ParseState;

struct _ParserContext
{
  void (*start_element) (ParserContext *context);
  void (*end_element) (ParserContext *context);
  void (*buffer_insert) (GtkTextBuffer *buffer,
                         GtkTextIter *iter,
                         const gchar *text,
                         gint len);

  const gchar *current_text;
  const gchar *current_text_end;
  const gchar *iter;
  gsize current_text_size;

  gchar buffer[SIZE_PARSER_BUFFER];
  gchar *current_buffer_pos;
  gchar *buffer_end;

  ParseState state;

  const gchar *start_name;
  GList *tag_stack;
  GString *name_tag;
  GString *attribute_name;
  GString *attribute_value;

  GtkTextBuffer *text_buffer;
  GtkTextIter start_iter_buffer;

  guint is_formating_error: 1;
};

struct _ParserTagDescription
{
  gchar *name_tag;
  gchar *name_buffer_tag;
  gint offset;
};

struct _GyDeplPrivate
{
  gchar **array_words;
  ParserContext *context;
};

G_DEFINE_TYPE_WITH_CODE (GyDepl, gy_depl, GY_TYPE_DICT,
                         G_ADD_PRIVATE (GyDepl)
                         G_IMPLEMENT_INTERFACE (GY_TYPE_PARSER_DICT,
                                                gy_parser_interface_init));

static inline void
insert_sign (gchar        *dest,
             const gchar  *src,
             const gchar **tab,
             gint         *index)
{
  gchar *str = NULL;
  gint i;

  for (i = 0, str = (gchar*) tab[((guchar) *src) - 128]; str[i]; i++)
    dest[(*index)++] = str[i]; /* *index = *index + 1 */
}

static guint
gy_depl_set_dictionary (GyDict *dict)
{
  gchar *buffer = NULL, *buffer_end = NULL, *buffer_tmp = NULL;
  gint fd = 0, i = 0;
  struct stat statbuf;
  g_autoptr(GSettings) settings = NULL;
  g_autofree gchar *path_file = NULL;
  GyDeplPrivate *priv = gy_depl_get_instance_private (GY_DEPL (dict));

  settings = g_settings_new ("org.gtk.gydict");
  path_file = g_settings_get_string (settings,
                                     gy_dict_get_id_string (dict));

  if (!g_file_test (path_file, G_FILE_TEST_EXISTS))
    {
      g_message ("File: %s does not exist!", path_file);
      return GY_EXISTS_FILE_ERROR;
    }

  if ((fd = open (path_file, O_RDONLY)) < 0)
    {
      g_message ("Cannot open the %s!", path_file);
      return GY_OPEN_FILE_ERROR;
    }

  fstat (fd, &statbuf);

  if ((buffer = (gchar *) mmap (0, statbuf.st_size, PROT_READ,
                                MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    return GY_MEMORY_ERROR;

  if (!(priv->array_words = (gchar **) g_try_malloc0_n (55000, sizeof (gchar *))))
    return GY_MEMORY_ERROR;

  buffer_end = buffer + statbuf.st_size;
  buffer_tmp = buffer;
  g_assert (statbuf.st_size == (buffer_end - buffer));

  for (; buffer < buffer_end; buffer++)
    {
      if (*buffer == '\n')
        {
          priv->array_words[i] = (gchar *) g_malloc0 ((buffer-buffer_tmp)+2);
          memcpy (priv->array_words[i], buffer_tmp, (buffer - buffer_tmp)+1);
          i++;
          buffer_tmp = buffer + 1;
          }
    }

  close (fd);
  if (munmap (buffer - statbuf.st_size, statbuf.st_size))
    return GY_MEMORY_ERROR;

  return GY_OK;
}

static guint
gy_depl_init_list (GyDict *dict)
{
  g_autofree gchar *path_file = NULL;
  g_autofree gchar *words = NULL;
  g_autoptr(GSettings) settings = NULL;
  gchar *file_map = NULL, *file_map_end = NULL;
  gint fd, i;
  GtkListStore *model = NULL;
  GtkTreeIter iter;
  struct stat statbuf;

  model = gtk_list_store_new (1, G_TYPE_STRING);
  settings = g_settings_new ("org.gtk.gydict");
  path_file = g_settings_get_string (settings, "dict-depl-b");

  if (!g_file_test (path_file, G_FILE_TEST_EXISTS))
    {
      g_message ("File: %s does not exist!", path_file);
      return GY_EXISTS_FILE_ERROR;
    }

  if ((fd = open (path_file, O_RDONLY)) < 0)
    {
      g_message ("Cannot open the %s!", path_file);
      return GY_OPEN_FILE_ERROR;
    }

  fstat (fd, &statbuf);

  if ((file_map = (gchar *) mmap (0, statbuf.st_size, PROT_READ,
                                  MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    return GY_MEMORY_ERROR;

  if (!(words = (gchar *) g_try_malloc0 (300)))
    return GY_MEMORY_ERROR;

  file_map_end = file_map + statbuf.st_size;
  g_assert ((file_map_end - file_map) == statbuf.st_size);

  for (i = 0; file_map < file_map_end; file_map++)
    {
      if (*(file_map+1) == '\n')
        {
          gtk_list_store_append (model, &iter);
          gtk_list_store_set (model, &iter, 0, words, -1);
          memset (words, 0, i+2);
          i = 0;
          /* Omiń znak nowego wiersza */
          file_map++;
        }
      else if (((guchar) *file_map) < 127)
        {
          words[i++] = *file_map;
        }
      else
        {
          insert_sign (words, file_map,
                       **(array_of_pointer_to_arrays_of_character_set+1), &i);
        }
    }

  gy_dict_set_tree_model (dict, GTK_TREE_MODEL (model));
  close (fd);
  if (munmap (file_map - statbuf.st_size, statbuf.st_size))
    return GY_MEMORY_ERROR;

  return GY_OK;
}

static gpointer
gy_depl_read_definition (GyDict *dict,
                         guint   index)
{
  GyDeplPrivate *priv = gy_depl_get_instance_private (GY_DEPL (dict));
  return priv->array_words[index];
}

static void
dispose (GObject *object)
{
  GyDeplPrivate *priv = gy_depl_get_instance_private (GY_DEPL (object));

  if (priv->array_words)
    {
      g_strfreev (priv->array_words);
      priv->array_words = NULL;
    }

  if (priv->context)
    parse_context_free (&priv->context);

  G_OBJECT_CLASS (gy_depl_parent_class)->dispose (object);
}

static void
finalize (GObject *object)
{
  G_OBJECT_CLASS (gy_depl_parent_class)->finalize (object);
}

static void
gy_depl_init (GyDepl *dict)
{
  GyDeplPrivate *priv = gy_depl_get_instance_private (dict);
  priv->context = parse_context_new ();
}

static void
gy_depl_class_init (GyDeplClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  GyDictClass *dict_class = GY_DICT_CLASS (klass);

  object_class->finalize = finalize;
  object_class->dispose = dispose;
  dict_class->set_dictionary = gy_depl_set_dictionary;
  dict_class->init_list = gy_depl_init_list;
  dict_class->read_definition = gy_depl_read_definition;
}

/************************IMPLEMENTED INTERFACE********************************/
static void
gy_parser_interface_init (GyParserDictInterface *iface)
{
  iface->parser_dict = parser_dict;
}

static void
parser_dict (GyParserDict      *parser,
             GtkTextBuffer     *buffer,
             gint               row)
{
  gchar *buf = NULL;
  GyDict *dict = GY_DICT (parser);
  GyDeplPrivate *priv = gy_depl_get_instance_private (GY_DEPL (dict));

  /* non free! */
  buf = gy_dict_read_definition (dict, (guint) row);
  parse_context_parse (priv->context, buf, -1, buffer);
}

/*******CREATE AND DESTROYED ParserTagDescription*******/
static ParserTagDescription *
parse_tag_description_new (void)
{
  return g_slice_new0 (ParserTagDescription);
}

static void
parse_tag_description_free (ParserTagDescription *tag_description)
{
  if (tag_description)
    {
      if (tag_description->name_tag)
        g_free (tag_description->name_tag);
      if (tag_description->name_buffer_tag)
        g_free (tag_description->name_buffer_tag);
      g_slice_free (ParserTagDescription, tag_description);
    }
}
/*******************************************************/

/*******CREATE AND DESTROYED ParserContext**************/
ParserContext *
parse_context_new (void)
{
  ParserContext *new = NULL;

  if (!(new = g_slice_new0 (ParserContext)))
    return NULL;
    
  new->state = STATE_NEUTRAL;
  new->tag_stack = NULL;
  new->attribute_name = NULL;
  new->attribute_value = NULL;
  new->start_name = NULL;
  new->start_element = start_element_cb;
  new->end_element = end_element_cb;
  new->buffer_insert = gtk_text_buffer_insert;

  return new;
}

static void
parse_context_free_contents_parser_context (ParserContext *context)
{
  if (context->name_tag)
    {
      g_string_free (context->name_tag, TRUE);
      context->name_tag = NULL;
    }
    
  if (context->attribute_name)
    {
      g_string_free (context->attribute_name, TRUE);
      context->attribute_name = NULL;
    }

  if (context->attribute_value)
    {
      g_string_free (context->attribute_value, TRUE);
      context->attribute_value = NULL;
    }

  if (context->tag_stack)
    {
      g_list_free_full (context->tag_stack,
                        (GDestroyNotify) parse_tag_description_free);
      context->tag_stack = NULL;
    }
}

static void
parse_context_free (ParserContext **context)
{
  g_slice_free (ParserContext, *context);
  *context = NULL;
}

/*******************************************************/

static void 
start_element_cb (ParserContext *context)
{
  ParserTagDescription *new_tag_description = NULL;

  new_tag_description = parse_tag_description_new();
  new_tag_description->name_tag = g_strdup (context->name_tag->str);
  new_tag_description->offset = gtk_text_iter_get_offset (&context->start_iter_buffer);

  if ((g_strcmp0 (context->name_tag->str, "i") == 0))
    {
      new_tag_description->name_buffer_tag = g_strdup ("i");
    }
  else if ((g_strcmp0 (context->name_tag->str, "b") == 0))
    {
      new_tag_description->name_buffer_tag = g_strdup ("b");
    }
  else if ((g_strcmp0 (context->name_tag->str, "font") == 0))
    {
      if (context->attribute_value)
        new_tag_description->name_buffer_tag = g_strdup (context->attribute_value->str);
    }
  else if ((g_strcmp0 (context->name_tag->str, "a") == 0))
    {
      new_tag_description->name_buffer_tag = g_strdup ("link");
    }
  else if ((g_strcmp0 (context->name_tag->str, "sup") == 0))
    {
      new_tag_description->name_buffer_tag = g_strdup ("sup");
    }
  else if ((g_strcmp0 (context->name_tag->str, "acronym") == 0))
    {
      new_tag_description->name_buffer_tag = g_strdup ("acronym");
    }

  context->tag_stack = g_list_append (context->tag_stack,
                                      (gpointer) new_tag_description);
}

static void
end_element_cb (ParserContext *context)
{
  GList *last = NULL, *prev = NULL;
  ParserTagDescription *tag_description = NULL;
  GtkTextIter iter;

  if (context->is_formating_error)
    return;

  if (!(last = g_list_last (context->tag_stack)))
    {
      g_warning ("An excess tag </%s>. The tag will be skipped. A formatting is continued.",
                 context->name_tag->str);
      return;
    }

  tag_description = (ParserTagDescription *) last->data;

  if ((g_strcmp0 (context->name_tag->str, tag_description->name_tag) != 0))
    {
      ParserTagDescription *tag_prev = NULL;
      if ((prev = last->prev))
        {
          tag_prev = (ParserTagDescription *) prev->data;
          if ((g_strcmp0 (context->name_tag->str, tag_prev->name_tag) == 0) &&
              (tag_description->offset == tag_prev->offset))
            {
              g_warning ("The tags </%s> and </%s> have a wrong position (ones are reversed). A formatting is continued.",
                         tag_description->name_tag, tag_prev->name_tag);
              tag_description = tag_prev;
              last = prev;
            }
          else
            {
              g_critical ("The tags </%s> and </%s> have a wrong position. "
                          "Furtermore, the tags cannot be reversed. A formatting is broken. "
                          "A text will be inserted, but it will not be formatted.",
                          tag_description->name_tag, tag_prev->name_tag);
              context->is_formating_error = 1;
              return;

            }

        }
      else
        {
          context->is_formating_error = 1;
          g_critical ("An undefined error! A formatting is broken. A text will be inserted, but it will not be formatted.");
          return;

        }

    }

  gtk_text_buffer_get_iter_at_offset (context->text_buffer, &iter,
                                      tag_description->offset);
  gtk_text_buffer_apply_tag_by_name (context->text_buffer,
                                     tag_description->name_buffer_tag,
                                     &iter,
                                     &context->start_iter_buffer);

  parse_tag_description_free (tag_description);
  context->tag_stack = g_list_remove_link (context->tag_stack, last);
}

static inline void
insert_sign_buffer (gchar **dest,
                    const gchar *src,
                    const gchar **tab)
{
  gchar *str = NULL, *tmp = NULL;

  tmp = *dest;
  str = (gchar*) tab[((guchar) *src) - 128];

  while (*str)
    *tmp++ = *str++;

  *dest = tmp;
}

static inline void
insert_text_buffor (ParserContext *context)
{
  while (context->iter != context->current_text_end &&
         context->current_buffer_pos != context->buffer_end &&
         (!IS_ANGLE_OPEN (*context->iter)))
    {
      if ( (*context->iter == ' ') && (*(context->iter+1) == ' '))
        {
          *context->current_buffer_pos++ = '\n';
          /* Pomiń podwójną spacje po pierwszym słowie */
          context->iter = context->iter + 2;
          continue;

        }

      if (((guchar) *context->iter) < 127)
        *context->current_buffer_pos++ = *context->iter++;
      else
        {
          /*	    insert_sign_buffer (&context->current_buffer_pos,
           * context->iter,
           * **(array_of_pointer_to_arrays_of_character_set +1));*/
          gy_tabs_convert_character (&context->current_buffer_pos,
                                     context->iter,
                                     GY_ENCODING_ISO88592);
          context->iter++;

        }
    }
}

static inline gboolean
advance_char (ParserContext *context)
{
  context->iter++;

  if (G_UNLIKELY (context->iter == context->current_text_end))
    return FALSE;

  return TRUE;
}

static inline void
skip_spaces (ParserContext *context)
{
  do
    {
      if (!IS_SPACE (*context->iter))
        return;
    }
  while (advance_char (context));
}

static void
advance_to_name_end (ParserContext *context)
{
  do
    {
      if (IS_COMMON_NAME_END_CHAR (*context->iter))
        return;
      if (IS_SPACE (*context->iter))
        return;
    }
  while (advance_char (context));
}

gboolean
parse_context_parse (ParserContext *context,
                     const gchar   *text,
                     gint           length_text,
                     GtkTextBuffer *text_buffer)
{
  g_return_val_if_fail (context != NULL, FALSE);
  g_return_val_if_fail (text != NULL, FALSE);
  g_return_val_if_fail (text_buffer != NULL, FALSE);

  if (length_text < 0)
    context->current_text_size = strlen (text);
  else
    context->current_text_size = length_text;

  if (!context->current_text_size)
    return TRUE;

  context->state = STATE_NEUTRAL;
  context->current_text = text;
  context->current_text_end = context->current_text + context->current_text_size;
  context->iter = context->current_text;
  context->text_buffer = text_buffer;
  context->current_buffer_pos = context->buffer;
  context->buffer_end = context->current_buffer_pos + (SIZE_PARSER_BUFFER - 2);
  gtk_text_buffer_get_start_iter (context->text_buffer,
                                  &context->start_iter_buffer);
  context->is_formating_error = 0;
  memset (context->buffer, 0, SIZE_PARSER_BUFFER);

  while (context->iter != context->current_text_end)
    {
      switch (context->state)
        {
          case STATE_NEUTRAL:
          {
            if (IS_ANGLE_OPEN (*context->iter))
              {
                context->state = STATE_AFTER_OPEN_ANGLE;
                advance_char (context);

              }
            else
              {
                context->state = STATE_INSERT_TEXT;

              }
            break;

          }
          case STATE_AFTER_OPEN_ANGLE:
          {
            skip_spaces (context);
            if (IS_COMMON_NAME_END_CHAR (*context->iter))
              {
                advance_char (context);
                context->state = STATE_CLOSE_TAG;

              }
            else
              {
                context->state = STATE_OPEN_TAG;

              }
            break;

          }
          case STATE_AFTER_CLOSE_ANGLE:
          {
            if (context->iter != context->current_text_end)
              context->state = STATE_NEUTRAL;
            break;

          }
          case STATE_OPEN_TAG:
          {
            context->start_name = context->iter;
            advance_to_name_end (context);
            context->name_tag = g_string_new_len (context->start_name,
                                                  context->iter - context->start_name);
            if (IS_SPACE (*context->iter))
              {
                context->state = STATE_BEFORE_READ_ATTRIBUTE;

              }
            else
              {
                advance_char (context);
                context->state = STATE_AFTER_CLOSE_ANGLE;
                (*context->start_element) (context);
                g_string_free (context->name_tag, TRUE);
                context->name_tag = NULL;

              }
            break;

          }
          case STATE_CLOSE_TAG:
          {
            context->start_name = context->iter;
            advance_to_name_end (context);
            context->name_tag = g_string_new_len (context->start_name,
                                                  context->iter - context->start_name);
            skip_spaces (context);
            context->state = STATE_BEFORE_CLOSE_ANGLE;
            (*context->end_element) (context);
            g_string_free (context->name_tag, TRUE);
            context->name_tag = NULL;
            break;

          }
          case STATE_BEFORE_CLOSE_ANGLE:
          {
            if (IS_ANGLE_CLOSE (*context->iter))
              {
                advance_char (context);
                context->state = STATE_AFTER_CLOSE_ANGLE;

              }
            else
              {
                context->state = STATE_ERROR;

              }
            break;

          }
          case STATE_BEFORE_READ_ATTRIBUTE:
          {
            skip_spaces (context);
            context->state = STATE_READ_ATTRIBUTE;
            break;

          }
          case STATE_READ_ATTRIBUTE:
          {
            context->start_name = context->iter;
            advance_to_name_end (context);
            context->attribute_name = g_string_new_len (context->start_name,
                                                        context->iter - context->start_name);
            context->state = STATE_BEFORE_READ_VALUE_ATTR;
            break;

          }
          case STATE_BEFORE_READ_VALUE_ATTR:
          {
            skip_spaces (context);
            advance_char (context);
            skip_spaces (context);
            context->state = STATE_READ_VALUE_ATTR;
            break;

          }
          case STATE_READ_VALUE_ATTR:
          {
            context->start_name = context->iter;
            advance_to_name_end (context);
            context->attribute_value = g_string_new_len (context->start_name,
                                                         context->iter - context->start_name);
            (*context->start_element) (context);
            skip_spaces (context);
            advance_char (context);
            context->state = STATE_AFTER_CLOSE_ANGLE;

            g_string_free (context->name_tag, TRUE);
            context->name_tag = NULL;
            g_string_free (context->attribute_name, TRUE);
            context->attribute_name = NULL;
            g_string_free (context->attribute_value, TRUE);
            context->attribute_value = NULL;
            break;

          }
          case STATE_INSERT_TEXT:
          {
            insert_text_buffor (context);
            (*context->buffer_insert) (context->text_buffer,
                                       &context->start_iter_buffer,
                                       context->buffer,
                                       context->current_buffer_pos - context->buffer);
            memset (context->buffer, 0,
                    context->current_buffer_pos - context->buffer);
            context->current_buffer_pos = context->buffer;
            context->state = STATE_NEUTRAL;

            break;

          }
          case STATE_ERROR:
          {
            g_critical ("A undefined error! Parsing is broken.");
            parse_context_free_contents_parser_context (context);
            return FALSE;

          }

        }

    }
  parse_context_free_contents_parser_context (context);
  return TRUE;
}
