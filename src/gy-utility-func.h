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
#ifndef __GY_UTILITY_H__
#define __GY_UTILITY_H__

extern guint gydict_utility_search_entity(const  gchar ** tab_encja, const gchar * token_encja);
extern void gy_utility_delete_text_in_buffer(GtkTextBuffer *buffer);
extern gboolean gy_utility_handlers_is_blocked_by_func(gpointer instance, gpointer func, gpointer data);
extern gint gy_utility_strcmp (const gchar *p1,
			       const gchar *p2,
		      	       size_t n);
extern void gy_utility_text_buffer_insert_text_with_tags (GtkTextBuffer *buffer,
							  GtkTextIter   *iter,
							  const gchar   *text,
							  gint           len,
							  GHashTable    *table_tags);
#endif
