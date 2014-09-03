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

G_DEFINE_INTERFACE (GyParser, gy_parser, 0);

static void
gy_parser_default_init (GyParserInterface *klass G_GNUC_UNUSED)
{
}

void
gy_parser_lexer_buffer (GyParser      *parser,
			GtkTextBuffer *buffer,
			gint 	       row)
{
    g_return_if_fail (GY_IS_PARSER (parser));

    GY_PARSER_GET_INTERFACE (parser)->lexer_buffer (parser, buffer, row);
}
