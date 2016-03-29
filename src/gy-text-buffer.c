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

enum {
  PROP_0,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static void
gy_text_buffer_finalize (GObject *object)
{
  GyTextBuffer *self = (GyTextBuffer *)object;

  G_OBJECT_CLASS (gy_text_buffer_parent_class)->finalize (object);
}

static void
gy_text_buffer_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GyTextBuffer *self = GY_TEXT_BUFFER (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_text_buffer_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GyTextBuffer *self = GY_TEXT_BUFFER (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_text_buffer_class_init (GyTextBufferClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_text_buffer_finalize;
  object_class->get_property = gy_text_buffer_get_property;
  object_class->set_property = gy_text_buffer_set_property;
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
