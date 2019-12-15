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

  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (object), &iter);
  gtk_text_buffer_create_mark (GTK_TEXT_BUFFER (object), "searched", &iter, FALSE);
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

/**
 * gy_text_buffer_new:
 *
 * Creates a new buffer.
 *
 * Returns
 * a GyTextBuffer
 *
 */
GyTextBuffer *
gy_text_buffer_new (void)
{
  return g_object_new (GY_TYPE_TEXT_BUFFER, NULL);
}

/**
 * gy_text_buffer_clean_buffer
 * @self: a GyTextBuffer
 *
 * Cleans a buffer.
 *
 */
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

/**
 * gy_text_buffer_insert_text_with_tags:
 * @self: a GyTextBuffer
 * @iter: an iterator in buffer
 * @text: UTF-8 text
 * @len: length of text, or -1
 * @table_tags: a hash table of tags, or NULL
 *
 * Inserts text into buffer at iter, applying the table of tags to the newly-inserted text.
 * It resembles gtk_text_buffer_insert_with_tags, but allows you to pass a hash table of tags,
 * in which tags are placed as values of the table, instead a list of tags.
 *
 */
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

/**
 * gy_text_buffer_remove_tags_by_name:
 * @self: a GyTextBuffer
 * @first_tag_name: name of the first tag to remove
 * @...: NULL-terminated list of names of tags to remove or NULL
 *
 * Removes all given tags in the whole buffer.
 */
void
gy_text_buffer_remove_tags_by_name (GyTextBuffer *self,
                                    const gchar  *first_tag_name,
                                    ...)
{
  GtkTextIter start, end;
  const gchar *tag_name;
  va_list args;

  g_return_if_fail (GY_IS_TEXT_BUFFER (self));
  g_return_if_fail (first_tag_name != NULL);

  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (self), &start);
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (self), &end);

  va_start (args, first_tag_name);
  tag_name = first_tag_name;

  while (tag_name)
    {
      gtk_text_buffer_remove_tag_by_name (GTK_TEXT_BUFFER (self), tag_name,
                                          &start, &end);
      tag_name = va_arg (args, const gchar *);
    }

  va_end (args);
}

/**
 * gy_text_buffer_get_tag_by_name:
 * @self: a GyTextBuffer
 * @name_tag
 *
 * Returns the tag named name_tag in buffer self, or NULL if no tag exist in
 * the buffer.
 *
 * Returns: (nullable) (transfer none): a #GtkTextTag, or NULL
 */
GtkTextTag *
gy_text_buffer_get_tag_by_name (GyTextBuffer  *self,
                                const gchar   *name_tag)
{
  GtkTextTagTable *ttable;

  g_return_val_if_fail (GY_IS_TEXT_BUFFER (self), NULL);
  g_return_val_if_fail (name_tag != NULL, NULL);

  ttable = gtk_text_buffer_get_tag_table (GTK_TEXT_BUFFER (self));

  return gtk_text_tag_table_lookup (ttable, name_tag);
}


static GtkTextTag *
get_tag_for_attributes (GyTextAttrIterator *iter)
{
  GyTextAttribute *attr;
  GtkTextTag *tag;

  tag = gtk_text_tag_new (NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_LANGUAGE);
  if (attr)
    g_object_set (tag, "language", pango_language_to_string (gy_text_attribute_get_language (attr)), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_FAMILY);
  if (attr)
    g_object_set (tag, "family", gy_text_attribute_get_string (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_STYLE);
  if (attr)
    g_object_set (tag, "style", gy_text_attribute_get_int (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_WEIGHT);
  if (attr)
    g_object_set (tag, "weight", gy_text_attribute_get_int (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_VARIANT);
  if (attr)
    g_object_set (tag, "variant", gy_text_attribute_get_int (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_STRETCH);
  if (attr)
    g_object_set (tag, "stretch", gy_text_attribute_get_int (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_SIZE);
  if (attr)
    g_object_set (tag, "size", gy_text_attribute_get_int (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_FONT_DESC);
  if (attr)
    g_object_set (tag, "font-desc", gy_text_attribute_get_font_desc (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_FOREGROUND);
  if (attr)
    {
      PangoColor *color;
      GdkRGBA rgba;

      color = (PangoColor *) gy_text_attribute_get_color (attr);
      rgba.red = color->red / 65535.;
      rgba.green = color->green / 65535.;
      rgba.blue = color->blue / 65535.;
      rgba.alpha = 1.;
      g_object_set (tag, "foreground-rgba", &rgba, NULL);
    };

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_BACKGROUND);
  if (attr)
    {
      PangoColor *color;
      GdkRGBA rgba;

      color = (PangoColor *) gy_text_attribute_get_color (attr);
      rgba.red = color->red / 65535.;
      rgba.green = color->green / 65535.;
      rgba.blue = color->blue / 65535.;
      rgba.alpha = 1.;
      g_object_set (tag, "background-rgba", &rgba, NULL);
    };

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_UNDERLINE);
  if (attr)
    g_object_set (tag, "underline", gy_text_attribute_get_int (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_UNDERLINE_COLOR);
  if (attr)
    {
      PangoColor *color;
      GdkRGBA rgba;

      color = (PangoColor *) gy_text_attribute_get_color (attr);
      rgba.red = color->red / 65535.;
      rgba.green = color->green / 65535.;
      rgba.blue = color->blue / 65535.;
      rgba.alpha = 1.;
      g_object_set (tag, "underline-rgba", &rgba, NULL);
    }

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_STRIKETHROUGH);
  if (attr)
    g_object_set (tag, "strikethrough", gy_text_attribute_get_boolean (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_STRIKETHROUGH_COLOR);
  if (attr)
    {
      PangoColor *color;
      GdkRGBA rgba;

      color = (PangoColor *) gy_text_attribute_get_color (attr);
      rgba.red = color->red / 65535.;
      rgba.green = color->green / 65535.;
      rgba.blue = color->blue / 65535.;
      rgba.alpha = 1.;
      g_object_set (tag, "strikethrough-rgba", &rgba, NULL);
    }

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_RISE);
  if (attr)
    g_object_set (tag, "rise", gy_text_attribute_get_int (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_SCALE);
  if (attr)
    g_object_set (tag, "scale", gy_text_attribute_get_float (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_FALLBACK);
  if (attr)
    g_object_set (tag, "fallback", gy_text_attribute_get_boolean (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_LETTER_SPACING);
  if (attr)
    g_object_set (tag, "letter-spacing", gy_text_attribute_get_int (attr), NULL);

  attr = gy_text_attr_iterator_get (iter, GY_TEXT_ATTR_FONT_FEATURES);
  if (attr)
    g_object_set (tag, "font-features", gy_text_attribute_get_string (attr), NULL);

  return tag;
}


void
gy_text_buffer_insert_with_attributes (GyTextBuffer   *self,
                                       GtkTextIter    *iter,
                                       const gchar    *text,
                                       GyTextAttrList *attributes)
{
  GtkTextMark *mark;
  GyTextAttrIterator *attr_iter;
  GtkTextTagTable *tags;

  g_return_if_fail (GY_IS_TEXT_BUFFER (self));

  if (!attributes)
    {
      gtk_text_buffer_insert (GTK_TEXT_BUFFER (self), iter, text, -1);
      return;
    }

  // create mark with right gravity
  mark = gtk_text_buffer_create_mark (GTK_TEXT_BUFFER (self), NULL, iter, FALSE);
  attr_iter = gy_text_attr_list_get_iterator (attributes);
  tags = gtk_text_buffer_get_tag_table (GTK_TEXT_BUFFER (self));

  do
    {
      GtkTextTag *tag;
      gint start, end;

      gy_text_attr_iterator_range (attr_iter, &start, &end);

      if (end == G_MAXINT) // last chunk
        end = start - 1; // resulting in -1 to be passed to _insert

      tag = get_tag_for_attributes (attr_iter);
      gtk_text_tag_table_add (tags, tag);

      gtk_text_buffer_insert_with_tags (GTK_TEXT_BUFFER (self), iter, text + start, end - start, tag, NULL);

      gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (self), iter, mark);
    }
  while (gy_text_attr_iterator_next (attr_iter));

  gtk_text_buffer_delete_mark (GTK_TEXT_BUFFER (self), mark);
  gy_text_attr_iterator_destroy (attr_iter);
}
