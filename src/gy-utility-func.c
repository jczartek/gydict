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

#include "config.h"
#include <gtk/gtk.h>
#include "gy-utility-func.h"

guint 
gydict_utility_search_entity (const gchar **tab_encja, 
                              const gchar  *token_encja)
{
  guint i;

  for (i=0; tab_encja[i] != NULL; i++)
  {
    if (g_strcmp0 (tab_encja[i], token_encja) == 0)
        break;
  }
  return i;
}

void 
gy_utility_delete_text_in_buffer (GtkTextBuffer *buffer)
{
  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(buffer,&start,&end);
  gtk_text_buffer_delete(buffer,&start,&end);
}

gboolean 
gy_utility_handlers_is_blocked_by_func (gpointer instance, 
                                        gpointer func,
                                        gpointer data)
{
  return g_signal_handler_find (instance,
                                G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA | G_SIGNAL_MATCH_UNBLOCKED,
                                0, 0, NULL, func, data) == 0;
}

gint
gy_utility_strcmp (const gchar *p1,
                   const gchar *p2,
                   size_t       n)
{
  const guchar *s1 = (const guchar *) p1;
  const guchar *s2 = (const guchar *) p2;
  guchar c1 = '\0', c2 = '\0';

  g_return_val_if_fail (s1 != NULL, -1);
  g_return_val_if_fail (s2 != NULL, -1);

  while (n > 0)
  {
    c1 = (guchar) *s1++;
    c2 = (guchar) *s2++;

    if (*s2 == '|')
    {
      s2++;
      if (*s2 == '\0')
        return c1 - c2;
    }
    if (c1 == '\0' || c1 != c2)
      return c1 - c2;
    n--;
  }

  return c1 - c2;
}

void
gy_utility_text_buffer_insert_text_with_tags (GtkTextBuffer  *buffer,
                                              GtkTextIter    *iter,
                                              const gchar    *text,
                                              gint            len,
                                              GHashTable     *table_tags)
{
  gint 		 start_offset;
  GtkTextIter 	 start;
  GList		*list;

  g_return_if_fail (GTK_IS_TEXT_BUFFER (buffer));
  g_return_if_fail (iter != NULL);
  g_return_if_fail (text != NULL);
  g_return_if_fail (gtk_text_iter_get_buffer (iter) == buffer);

  start_offset = gtk_text_iter_get_offset (iter);
  
  gtk_text_buffer_insert (buffer, iter, text, len);

  if (table_tags == NULL)
    return;

  gtk_text_buffer_get_iter_at_offset (buffer, &start, start_offset);
  
  list = g_hash_table_get_values (table_tags);

  if (list == NULL)
    return;

  GList *l;
  for (l = list; l != NULL; l=l->next)
    gtk_text_buffer_apply_tag (buffer, (GtkTextTag*) l->data, &start, iter);

  g_list_free (list);
}
