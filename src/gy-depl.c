/* gy-depl.c
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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

#define G_LOG_DOMAIN "GyDepl"

#include "config.h"
#include <glib/gi18n-lib.h>
#include <string.h>
#include "gy-pwntabs.h"
#include "gy-dict.h"
#include "gy-parsable.h"
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


#define SIZE_ENTRY 64

typedef struct _ParserContext ParserContext;
typedef struct _ParserTagDescription ParserTagDescription;
typedef struct _GyDeplPrivate GyDeplPrivate;

static void gy_parser_interface_init (GyParsableInterface *iface);
static void gy_depl_parser_dict_parse (GyParsable    *parser,
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

struct _GyDepl
{
  GyDict         __parent__;

  gchar        **array_words;
  ParserContext *context;
};

G_DEFINE_TYPE_WITH_CODE (GyDepl, gy_depl, GY_TYPE_DICT,
                         G_IMPLEMENT_INTERFACE (GY_TYPE_PARSABLE,
                                                gy_parser_interface_init));

static void
gy_depl_map (GyDict *dict,
             GError **err)
{
  g_autoptr(GFile) file = NULL;
  g_autoptr(GFileInputStream) in = NULL;
  g_autoptr(GDataInputStream) data = NULL;
  g_autoptr(GSettings) settings = NULL;
  g_autofree gchar *path = NULL;
  GtkListStore *model = NULL;
  GtkTreeIter iter;
  gchar entry[SIZE_ENTRY];
  gchar *line = NULL;
  gchar *conv = NULL;
  const gchar *id = NULL;
  gint i = 0;
  gsize offset = 0;

  GyDepl *self = GY_DEPL (dict);

  g_return_if_fail (GY_IS_DEPL (self));

  g_object_get (dict, "identifier", &id, NULL);

  settings = g_settings_new ("org.gtk.gydict.paths");
  path = g_settings_get_string (settings, id);

  file = g_file_new_for_path (path);

  if (!(in = g_file_read (file, NULL, err)))
    goto out;

  if (!(data = g_data_input_stream_new (G_INPUT_STREAM(in))))
    goto out;

  model = gtk_list_store_new (1, G_TYPE_STRING);
  self->array_words = (gchar **) g_malloc0 (55000 * sizeof (guintptr));

  while ((line = g_data_input_stream_read_line (data, NULL, NULL, err)) != NULL)
    {
      if (!(conv = g_convert_with_fallback (line, -1, "UTF-8", "ISO8859-2", NULL, NULL, NULL, err)))
        {
          g_free (line);
          goto out;
        }

      memset (entry, 0, SIZE_ENTRY);
      offset = strcspn (conv, " ");
      strncat (entry, conv, offset);
      gtk_list_store_append (model, &iter);
      gtk_list_store_set (model, &iter, 0, entry, -1);

      self->array_words[i++] = conv;
      g_free (line);
    }
  g_object_set (dict, "model", model, NULL);
  g_object_set (dict, "is-mapped", TRUE, NULL);
  return;
out:
  g_object_set (dict, "is-mapped", FALSE, NULL);
  return;
}

static gpointer
gy_depl_get_lexical_unit (GyDict *dict,
                          guint   index)
{
  GyDepl *self = GY_DEPL (dict);

  return self->array_words[index];
}

static void
finalize (GObject *object)
{
  GyDepl *self = GY_DEPL (object);

  if (self->array_words)
    {
      g_strfreev (self->array_words);
      self->array_words = NULL;
    }

  if (self->context)
    parse_context_free (&self->context);

  G_OBJECT_CLASS (gy_depl_parent_class)->finalize (object);
}

static void
gy_depl_init (GyDepl *dict)
{
  GyDepl *self = GY_DEPL (dict);

  self->context = parse_context_new ();
}

static void
gy_depl_class_init (GyDeplClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  GyDictClass *dict_class = GY_DICT_CLASS (klass);

  object_class->finalize = finalize;
  dict_class->map = gy_depl_map;
}

/************************IMPLEMENTED INTERFACE********************************/
static void
gy_parser_interface_init (GyParsableInterface *iface)
{
  iface->parse = gy_depl_parser_dict_parse;
}

static void
gy_depl_parser_dict_parse (GyParsable      *parser,
                           GtkTextBuffer   *buffer,
                           gint             row)
{
  gchar *buf = NULL;
  GyDict *dict = GY_DICT (parser);

  /* non free! */
  buf = gy_depl_get_lexical_unit (dict, (guint) row);
  parse_context_parse (GY_DEPL (parser)->context, buf, -1, buffer);
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

        *context->current_buffer_pos++ = *context->iter++;
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
