/* gy-text-buffer-private.h
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

#ifndef __GY_TEXT_BUFFER_PRIVATE_H__
#define __GY_TEXT_BUFFER_PRIVATE_H__

#include <gtk/gtk.h>

struct GyAttributeTag
{
  gchar*          name;
  gchar*          foreground;
  gint            weight;
  gchar*          family;
  PangoStyle      style;
  gdouble         scale;
  gint            rise;
  gchar*          background;
  PangoUnderline  underline;
};

static struct GyAttributeTag attrs_tags[] = {
    {
      .name = "big",
      .foreground = NULL,
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = PANGO_SCALE_LARGE,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "blue",
      .foreground = "#14aabc",
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "b",
      .foreground = NULL,
      .weight = PANGO_WEIGHT_BOLD,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "ph",
      .foreground = "#c1bdbd",
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "sub",
      .foreground = NULL,
      .weight = PANGO_WEIGHT_BOLD,
      .family = "serif",
      .style = 0,
      .scale = PANGO_SCALE_LARGE,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "i",
      .foreground = NULL,
      .weight = 0,
      .family = NULL,
      .style = PANGO_STYLE_ITALIC,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "small",
      .foreground = NULL,
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = PANGO_SCALE_SMALL,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "sup",
      .foreground = NULL,
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = PANGO_SCALE_X_SMALL,
      .rise = (PANGO_SCALE * 6),
      .background = NULL,
      .underline = 0
    },
    {
      .name = "search",
      .foreground = "#ffffff",
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = "#456b6a",
      .underline = 0
    },
    {
      .name = "search_next",
      .foreground = "#ffffff",
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = "#4c83cd",
      .underline = 0
    },
    {
      .name = "red",
      .foreground = "#bc4b14",
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "green",
      .foreground = "#457218",
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "gray",
      .foreground = "#8746ea",
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "acronym",
      .foreground = NULL,
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = 0
    },
    {
      .name = "link",
      .foreground = NULL,
      .weight = 0,
      .family = NULL,
      .style = 0,
      .scale = 0.0,
      .rise = 0,
      .background = NULL,
      .underline = PANGO_UNDERLINE_SINGLE
    }
};

#define ATTR_TAG(attr) (attrs_tags[iter].attr)

static void
gy_text_buffer_set_attrs_tags (GtkTextBuffer *buffer)
{
  GtkTextTag *tag = NULL;
  g_return_if_fail (GTK_IS_TEXT_BUFFER (buffer));

  for (int iter = 0, end = G_N_ELEMENTS (attrs_tags); iter < end; iter++)
    {
      tag = gtk_text_buffer_create_tag (buffer,
                                        ATTR_TAG (name),
                                        NULL, NULL);

      if (ATTR_TAG(foreground)) g_object_set (tag, "foreground", ATTR_TAG (foreground), NULL);

      if (ATTR_TAG (weight)) g_object_set (tag, "weight", ATTR_TAG (weight), NULL);

      if (ATTR_TAG (family)) g_object_set (tag, "family", ATTR_TAG (family), NULL);

      if (ATTR_TAG (style)) g_object_set (tag, "style", ATTR_TAG (style), NULL);

      if (ATTR_TAG (scale)) g_object_set (tag, "scale", ATTR_TAG (scale), NULL);

      if (ATTR_TAG (rise)) g_object_set (tag, "rise", ATTR_TAG (rise), NULL);

      if (ATTR_TAG (background)) g_object_set (tag, "background", ATTR_TAG (background), NULL);

      if (ATTR_TAG (underline)) g_object_set (tag, "underline", ATTR_TAG (underline), NULL);
    }
}

#undef ATTR_TAG

#endif /* __GY_TEXT_BUFFER_PRIVATE_H__ */
