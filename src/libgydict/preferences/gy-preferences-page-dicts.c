/* gy-preferences-page-dicts.c
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

#include "gy-preferences-page-dicts.h"
#include "gy-preferences-file-chooser.h"

struct _GyPreferencesPageDicts
{
  GyPreferencesPage  parent;
};

G_DEFINE_TYPE (GyPreferencesPageDicts, gy_preferences_page_dicts, GY_TYPE_PREFERENCES_PAGE)

static void
gy_preferences_page_dicts_class_init (GyPreferencesPageDictsClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-preferences-page-dicts.ui");

  g_type_ensure (GY_TYPE_PREFERENCES_FILE_CHOOSER);
}

static void
gy_preferences_page_dicts_init (GyPreferencesPageDicts *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
