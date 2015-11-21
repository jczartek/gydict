/* gy-preferences-page.h
 *
 * Copyright (C) 2015 Jakub Czartek <kuba@linux.pl>
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
#ifndef GY_PREFERENCES_PAGE_H
#define GY_PREFERENCES_PAGE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_PREFERENCES_PAGE (gy_preferences_page_get_type())

G_DECLARE_DERIVABLE_TYPE (GyPreferencesPage, gy_preferences_page, GY, PREFERENCES_PAGE, GtkBin)

struct _GyPreferencesPageClass
{
  GtkBinClass parent;
};

void gy_preferences_page_set_title (GyPreferencesPage *page,
                                    const gchar       *title);

G_END_DECLS

#endif /* GY_PREFERENCES_PAGE_H */
