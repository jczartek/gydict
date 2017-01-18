/* gy-text-buffer.c
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
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

#include "gy-text-buffer.h"
#include "gy-text-buffer-private.h"

struct _GyTextBuffer
{
  GtkTextBuffer __parent__;
};

G_DEFINE_TYPE (GyTextBuffer, gy_text_buffer, GTK_TYPE_TEXT_BUFFER)

static void
gy_text_buffer_finalize (GObject *object)
{
  G_OBJECT_CLASS (gy_text_buffer_parent_class)->finalize (object);
}

static void
gy_text_buffer_constructed (GObject *object)
{
  GtkTextIter iter;

  G_OBJECT_CLASS (gy_text_buffer_parent_class)->constructed (object);

  gy_text_buffer_set_attrs_tags (GTK_TEXT_BUFFER (object));

  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (object), &iter);
  gtk_text_buffer_create_mark (GTK_TEXT_BUFFER (object), "last_pos_start", &iter, FALSE);
  gtk_text_buffer_create_mark (GTK_TEXT_BUFFER (object), "last_pos_end", &iter, FALSE);
}

static void
gy_text_buffer_class_init (GyTextBufferClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = gy_text_buffer_constructed;
  object_class->finalize = gy_text_buffer_finalize;
}

static void
gy_text_buffer_init (GyTextBuffer *self)
{
}

GyTextBuffer *
gy_text_buffer_new (void)
{
  return g_object_new (GY_TYPE_TEXT_BUFFER, NULL);
}

void
gy_text_buffer_clean_buffer (GyTextBuffer *self)
{
  GtkTextIter begin, end;

  g_return_if_fail (GY_IS_TEXT_BUFFER (self));

  gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (self),
                              &begin, &end);
  gtk_text_buffer_delete (GTK_TEXT_BUFFER (self),
                          &begin, &end);
}

void
gy_text_buffer_insert_text_with_tags (GyTextBuffer   *self,
                                      GtkTextIter    *iter,
                                      const gchar    *text,
                                      gint            len,
                                      GHashTable     *table_tags)
{
  gint           start_offset;
  GtkTextIter    start;
  GList		      *list;
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER (self);

  g_return_if_fail (GY_IS_TEXT_BUFFER (self));
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

  for (GList *l = list; l != NULL; l=l->next)
    gtk_text_buffer_apply_tag (buffer, (GtkTextTag*) l->data, &start, iter);

  g_list_free (list);
}