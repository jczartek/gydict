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

#ifndef __GY_PARSER_DICT_H__
#define __GY_PARSER_DICT_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS
/*
 * Type macros
 */
#define GY_TYPE_PARSER_DICT                (gy_parser_dict_get_type ())
#define GY_PARSER_DICT(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_PARSER_DICT, GyParserDict))
#define GY_IS_PARSER_DICT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_PARSER_DICT))
#define GY_PARSER_DICT_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GY_TYPE_PARSER_DICT, GyParserDictInterface))

typedef struct _GyParserDict GyParserDict; /* dummy object */
typedef struct _GyParserDictInterface GyParserDictInterface;

struct _GyParserDictInterface
{
    GTypeInterface parent_iface;

    /* Method iface */
    void (*parser_dict) (GyParserDict      *parser, 
			 GtkTextBuffer     *buffer, 
			 gint               row);

};

/* used by GY_TYPE_PARSER */
GType gy_parser_dict_get_type (void);
void gy_parser_dict_parse (GyParserDict      *parser,
			   GtkTextBuffer     *buffer,
		       	   gint	              row);

G_END_DECLS

#endif /* end of include guard: __GY_PARSER_DICT_H__ */
