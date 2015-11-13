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

#include "gy-parser.h"

G_DEFINE_INTERFACE (GyParserDict, gy_parser_dict, 0);

static void
gy_parser_dict_default_init (GyParserDictInterface *klass G_GNUC_UNUSED)
{
}

void
gy_parser_dict_parse (GyParserDict      *parser,
                      GtkTextBuffer     *buffer,
                      gint           row)
{
  g_return_if_fail (GY_IS_PARSER_DICT (parser));

  GY_PARSER_DICT_GET_INTERFACE (parser)->parser_dict (parser, buffer, row);
}
