/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <string.h>
#include "gy-pwntabs.h"
#include "gy-parser-pwn.h"

#define SIZE_TEXT_BUFFER		512
#define __is_angle(c) 			((c) == '<' || (c) == '>' )
#define __is_space(c) 			((c) == ' ' || (c) == '\t')
#define __is_end_char(c)		((c) == '>' || (c) == ' ' || (c) == '\t' || (c) == '/')
#define __is_slash(c)			((c) == '/')
#define __is_et(c) 			((c) == '&')
#define __is_semicolon(c)		((c) == ';')
#define __is_equal(c) 			((c) == '=')
#define __is_quote(c) 			((c) == '\"')
#define __next_char			parser->__text_iter++
#define __give_char			(*parser->__text_iter)

#define __set_flag(lval, rval)		((lval) |= (rval))
#define __check_flag(lval, rval)	((lval) & (rval)) 
#define __clean_flag(lval)		((lval) ^= (lval))

typedef enum
{
  GY_PARSER_STATE_TAG_OPEN    = 1 << 0,
  GY_PARSER_STATE_TAG_CLOSE   = 1 << 1
} GyMarkupParserPwnStateFlags;

struct _GyMarkupParserPwn
{
  GyMarkupParserPwnTagStartCallback     __tag_start_cb;
  GyMarkupParserPwnTagEndCallback       __tag_end_cb;
  GyMarkupParserPwnTextCallback         __text_cb;

  gpointer                              __data;
  GDestroyNotify                        __dnotify_cb;

  gchar                                *__tag_name;
  GPtrArray                            *__attribute_name;
  GPtrArray                            *__attribute_value;

  gchar                                *__buffer;
  gchar                                *__buf_iter;	/* Aktualna pozycja w buforze */

  const gchar                          *__text;
  const gchar                          *__text_iter;	/* Aktualna pozycja w parsowanym łańcuchu */
  const gchar                          *__text_end;
  gsize                                 __text_len;

  GHashTable                           *__entity;

  GyDictEncoding                        __encoding;
  GyMarkupParserPwnStateFlags           __flag;
};

GyMarkupParserPwn* 
gy_markup_parser_pwn_new (GyMarkupParserPwnTagStartCallback      tag_start_cb,
                          GyMarkupParserPwnTagEndCallback        tag_end_cb,
                          GyMarkupParserPwnTextCallback          text_cb,
                          GHashTable                            *entity,
                          gpointer                               data,
                          GDestroyNotify                         dnotify_cb)
{
  GyMarkupParserPwn *parser = NULL;

  g_return_val_if_fail (tag_start_cb != NULL, NULL);
  g_return_val_if_fail (tag_end_cb != NULL, NULL);
  g_return_val_if_fail (text_cb != NULL, NULL);
  g_return_val_if_fail (entity != NULL, NULL);

  parser = g_new0 (GyMarkupParserPwn, 1);

  parser->__tag_start_cb = tag_start_cb;
  parser->__tag_end_cb = tag_end_cb;
  parser->__text_cb = text_cb;

  parser->__entity = g_hash_table_ref (entity);
  parser->__buffer = g_slice_alloc0 (SIZE_TEXT_BUFFER);

  parser->__data = data;
  parser->__dnotify_cb = dnotify_cb;

  return parser;
}

void 
gy_markup_parser_pwn_free (GyMarkupParserPwn *parser)
{
  g_return_if_fail ((parser != NULL) &&
                    (parser->__entity != NULL) &&
                    (parser->__buffer != NULL));

  g_clear_pointer (&parser->__entity,
                   g_hash_table_unref);
  g_slice_free1 (SIZE_TEXT_BUFFER,
                 parser->__buffer);
  parser->__buffer = NULL;

  if (parser->__dnotify_cb != NULL)
    parser->__dnotify_cb (parser->__data);

  g_free (parser);
}

static inline void
skip_space (GyMarkupParserPwn *parser)
{
  while (__is_space (*parser->__text_iter))
    __next_char;
}

static inline void
read_tag (GyMarkupParserPwn *parser)
{
  const gchar *tmp = parser->__text_iter;

  while (!__is_end_char(*parser->__text_iter))
    __next_char;

  parser->__tag_name = g_strndup (tmp, parser->__text_iter - tmp);
}

static inline void
read_text (GyMarkupParserPwn *parser)
{
  while ((!__is_angle (*parser->__text_iter)) &&
         (!__is_et (*parser->__text_iter)) &&
         (parser->__text_iter != parser->__text_end))
  {
    if (((guchar) *parser->__text_iter) < 127)
    {
      *parser->__buf_iter++ = *parser->__text_iter++;
    }
    else
    {
      gy_tabs_convert_character (&parser->__buf_iter,
                                 parser->__text_iter,
                                 parser->__encoding);
      parser->__text_iter++;
    }
  }
}

static inline void
convert_entity (GyMarkupParserPwn *parser)
{
  gchar *entity = NULL;
  const gchar *new_entity = NULL,
              *tmp = parser->__text_iter;

  while (!__is_semicolon(*parser->__text_iter++))
    ;
  
  entity = g_strndup (tmp, parser->__text_iter - tmp - 1);
  new_entity = g_hash_table_lookup (parser->__entity,
                                    (gconstpointer) entity);
  if (new_entity)
    while (*new_entity)
      *parser->__buf_iter++ = *new_entity++;

  g_free (entity);
}

static inline void
read_attributes (GyMarkupParserPwn *parser)
{
  gchar *attribute_name = NULL,
  *attribute_value = NULL;
  const gchar *tmp = parser->__text_iter;
  parser->__attribute_name = g_ptr_array_new_with_free_func (g_free);
  parser->__attribute_value = g_ptr_array_new_with_free_func (g_free);

start:
  while (g_ascii_isalnum (*parser->__text_iter))
    __next_char;

  attribute_name = g_strndup (tmp, parser->__text_iter - tmp);
  skip_space (parser);
  __next_char; /* Pomiń znak równości */
  skip_space (parser);
  __next_char; /* Pomiń pierwszy cudzysłów */
  tmp = parser->__text_iter;
  while (g_ascii_isalnum (*parser->__text_iter) || (*parser->__text_iter == '.') || (*parser->__text_iter == ','))
    __next_char;

  attribute_value = g_strndup (tmp, parser->__text_iter - tmp);
  g_ptr_array_add (parser->__attribute_name, (gpointer) attribute_name);
  g_ptr_array_add (parser->__attribute_value, (gpointer) attribute_value);
  __next_char;
  skip_space (parser);

  if (g_ascii_isalnum (*parser->__text_iter))
  {
    tmp = parser->__text_iter;
    goto start;
  }

  if (__is_slash (*parser->__text_iter))
    __next_char;
}

void 
gy_markup_parser_pwn_parse (GyMarkupParserPwn *parser,
                            const gchar       *text,
                            gint               text_len,
                            GyDictEncoding     encoding)
{
  g_return_if_fail (parser != NULL);
  g_return_if_fail (text !=NULL);

  parser->__encoding = encoding;

  enum {
  STATE_NEUTRAL, STATE_OPEN_ANGLE,
  STATE_CLOSE_ANGLE, STATE_READ_TAG,
  STATE_READ_TEXT, STATE_ENTITY, 
  STATE_READ_ATTRIBUTE, STATE_END
  };

  /* Tabela skoków */
  static void *jump_table[] = {
    &&state_neutral, &&state_open_angle,
    &&state_close_angle, &&state_read_tag,
    &&state_read_text, &&state_entity,
    &&state_read_attribute, &&state_end
  };
#define JUMP_SYMBOL(state)	goto *jump_table[state]

  if (text_len < 0)
    parser->__text_len = strlen (text);
  else
    parser->__text_len = text_len;

  if (!parser->__text_len)
    return;

  parser->__text = parser->__text_iter = text;
  parser->__text_end = parser->__text + parser->__text_len;
  parser->__buf_iter = parser->__buffer;

  while (1)
  {
state_neutral:
    if (parser->__text_iter != parser->__text_end )
    {
      if (*parser->__text_iter == '<')
        JUMP_SYMBOL (STATE_OPEN_ANGLE);
      else if (*parser->__text_iter == '>') /* To nie będzie chyba potrzebne */
        JUMP_SYMBOL (STATE_CLOSE_ANGLE);
      else if (*parser->__text_iter == '&')
        JUMP_SYMBOL (STATE_ENTITY);
      else
        JUMP_SYMBOL (STATE_READ_TEXT);
    }
    else
    {
      JUMP_SYMBOL (STATE_END);
    }
state_open_angle:
    /********************************************************
     * Kroki do wykonania:
     * 1. pomiń znak '<';
     * 2. pomiń białe znaki;
     * 3. sprawdź typ tagu, następnie ustaw odpowiednią flagę;
     * 4. pomiń białe znaki;
     * 5. skocz do STATE_READ_TAG;
     ********************************************************/
    /*1*/
    __next_char;
    /*2*/
    skip_space (parser);
    /*3*/
    !__is_slash(__give_char) ? __set_flag (parser->__flag, GY_PARSER_STATE_TAG_OPEN) : __set_flag (parser->__flag, GY_PARSER_STATE_TAG_CLOSE);
    /*4*/
    skip_space (parser);
    /*5*/
    JUMP_SYMBOL (STATE_READ_TAG);
state_close_angle:
    __next_char;
    JUMP_SYMBOL (STATE_NEUTRAL);
state_read_tag:
    if (__check_flag (parser->__flag, GY_PARSER_STATE_TAG_CLOSE))
      __next_char;

    skip_space (parser);

    read_tag (parser);
    skip_space (parser);
    if (__is_angle (*parser->__text_iter))
    {
      __check_flag (parser->__flag, GY_PARSER_STATE_TAG_OPEN) ? parser->__tag_start_cb (parser->__tag_name,
                                                                                        NULL, NULL,
                                                                                        parser->__data)
      : parser->__tag_end_cb (parser->__tag_name,
                              parser->__data);
      g_clear_pointer ((gpointer *) &parser->__tag_name,
                       g_free);
      __clean_flag (parser->__flag);
      JUMP_SYMBOL (STATE_CLOSE_ANGLE);
    }
    else
    {
      JUMP_SYMBOL (STATE_READ_ATTRIBUTE);
    }
state_read_text:
    read_text (parser);
    parser->__text_cb (parser->__buffer,
                       parser->__buf_iter - parser->__buffer,
                       parser->__data);
    parser->__buf_iter = parser->__buffer;
    memset (parser->__buffer, 0, SIZE_TEXT_BUFFER);
    JUMP_SYMBOL (STATE_NEUTRAL);
state_entity:
    convert_entity (parser);
    parser->__text_cb (parser->__buffer,
                       parser->__buf_iter - parser->__buffer,
                       parser->__data);
    parser->__buf_iter = parser->__buffer;
    memset (parser->__buffer, 0, SIZE_TEXT_BUFFER);
    JUMP_SYMBOL (STATE_NEUTRAL);
state_read_attribute:
    read_attributes (parser);
    parser->__tag_start_cb (parser->__tag_name,
                            parser->__attribute_name,
                            parser->__attribute_value,
                            parser->__data);
    g_clear_pointer ((gpointer *) &parser->__tag_name,
                     g_free);
    g_ptr_array_free (parser->__attribute_name, FALSE);
    g_ptr_array_free (parser->__attribute_value, FALSE);
    parser->__attribute_name = parser->__attribute_value = NULL;
    __clean_flag (parser->__flag);
    JUMP_SYMBOL (STATE_CLOSE_ANGLE);
  }
state_end:
  return;
#undef JUMP_SYMBOL
}
