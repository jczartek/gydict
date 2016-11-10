/* gy-preferences-page-general.c
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

#include "gy-preferences-page-general.h"
#include "gy-preferences-switch.h"

struct _GyPreferencesPageGeneral
{
  GyPreferencesPage  parent;

  GtkFontButton     *font_button;
  GSettings         *settings;
};

G_DEFINE_TYPE (GyPreferencesPageGeneral, gy_preferences_page_general, GY_TYPE_PREFERENCES_PAGE)

static void
gy_preferences_page_general_constructed (GObject *object)
{
  GyPreferencesPageGeneral *self = GY_PREFERENCES_PAGE_GENERAL (object);

  g_return_if_fail (GY_IS_PREFERENCES_PAGE_GENERAL (self));

  G_OBJECT_CLASS (gy_preferences_page_general_parent_class)->constructed (object);

  g_settings_bind (self->settings,    "font-name",
                   self->font_button, "font-name",
                   G_SETTINGS_BIND_DEFAULT);

}

static void
gy_preferences_page_general_class_init (GyPreferencesPageGeneralClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = gy_preferences_page_general_constructed;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-preferences-page-general.ui");
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesPageGeneral, font_button);
  gtk_widget_class_bind_template_child (widget_class, GyPreferencesPageGeneral, settings);
}

static void
gy_preferences_page_general_init (GyPreferencesPageGeneral *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
