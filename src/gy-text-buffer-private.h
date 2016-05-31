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

struct GyDescriptionBufferTag
{
  gchar*          name;
  gint            size;
  gchar*          foreground;
  gint            weight;
  gchar*          family;
  PangoStyle      style;
  gdouble         scale;
  gint            rise;
  gchar*          background;
  PangoUnderline  underline;
};

#endif /* __GY_TEXT_BUFFER_PRIVATE_H__ */
