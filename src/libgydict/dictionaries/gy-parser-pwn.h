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

#ifndef __GY_MARKUP_PARSER_PWN__
#define __GY_MARKUP_PARSER_PWN__

#include <stdio.h>
#include <glib.h>
#include "gy-pwntabs.h"

typedef void (*GyMarkupParserPwnTagStartCallback) (const gchar     *tag_name,
                                                   const GPtrArray *attribute_name,
                                                   const GPtrArray *attribute_value,
                                                   gpointer         data);

typedef void (*GyMarkupParserPwnTagEndCallback) (const gchar *tag_name,
                                                 gpointer     data);

typedef void (*GyMarkupParserPwnTextCallback) (const gchar *text,
                                               gsize        text_len,
                                               gpointer     data);

typedef struct _GyMarkupParserPwn GyMarkupParserPwn;

GyMarkupParserPwn* gy_markup_parser_pwn_new (GyMarkupParserPwnTagStartCallback      tag_start_cb,
                                             GyMarkupParserPwnTagEndCallback        tag_end_cb,
                                             GyMarkupParserPwnTextCallback          text_cb,
                                             GHashTable                            *entity,
                                             gpointer                               data,
                                             GDestroyNotify                         dnotify_cb);
void gy_markup_parser_pwn_free (GyMarkupParserPwn *parser);
void gy_markup_parser_pwn_parse (GyMarkupParserPwn *parser,
                                 const gchar       *text,
                                 int                text_len,
                                 GyDictEncoding     encoding);
#endif
