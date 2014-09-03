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

#include "gy-enum-types.h"

#include "gy-dict.h"

GType
gy_enum_types_get_type (void)
{
    static GType enum_type = 0;

    if (enum_type == 0)
    {
	static const GEnumValue enum_values[] =
	{
	    {GY_ENCODING_NONE, "GY_ENCODING_NONE", "encoding-none"},
	    {GY_ENCODING_CP1250, "GY_ENCODING_CP1250", "encoding-cp"},
	    {GY_ENCODING_ISO88592, "GY_ENCODING_ISO88592", "encoding-iso88592"},
	    {GY_ENCODING_UTF8, "GY_ENCODING_UTF8", "encoding-utf8"},
	    {0, NULL, NULL}
	};

	enum_type = g_enum_register_static (g_intern_static_string ("GyDictEncoding"),
					  enum_values);

    }

    return enum_type;
}



