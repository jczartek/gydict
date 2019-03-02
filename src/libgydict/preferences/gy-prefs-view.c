/* gy-prefs-view.c
 *
 * Copyright 2018 Jakub Czartek <kuba@linux.pl>
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
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <glib/gi18n.h>
#include "gy-prefs-view.h"

struct _GyPrefsView
{
  DzlPreferencesView  parent_instance;
  GHashTable         *size_groups;
};

G_DEFINE_TYPE (GyPrefsView, gy_prefs_view, DZL_TYPE_PREFERENCES_VIEW)

static void
gy_prefs_view_register_builtin_prefs (DzlPreferences *prefs)
{
  GtkSizeGroup *path_group;

  dzl_preferences_add_page (prefs, "appearance", _("Appearance"), 0);
  dzl_preferences_add_list_group (prefs, "appearance", "basic", _("Themes"), GTK_SELECTION_NONE, 0);
  dzl_preferences_add_switch (prefs, "appearance", "basic",
                              "org.gtk.gydict", "night-mode", NULL, NULL,
                              _("Dark Mode"), _("Whether Gydict should use a dark theme"), _("dark theme"), 0);
  dzl_preferences_add_switch (prefs, "appearance", "basic",
                              "org.gtk.gydict", "show-grid-lines", NULL, NULL,
                              _("Grid Pattern"), _("Display a grid pattern underneath text"), NULL, 10);

  dzl_preferences_add_list_group (prefs, "appearance", "font", _("Font"), GTK_SELECTION_NONE, 10);
  dzl_preferences_add_font_button (prefs, "appearance", "font", "org.gtk.gydict", "font-name", _("Text View"), C_("Keywords", "Text view font"), 0);


  path_group = gy_prefs_view_get_size_group (GY_PREFS_VIEW (prefs), "paths");
  dzl_preferences_add_page (prefs, "dictionaries", _("Dictionaries"), 400);
  dzl_preferences_add_list_group (prefs, "dictionaries", "paths", _("The dictionaries paths"), GTK_SELECTION_NONE, 0);

  {
    GtkWidget *widget;
    guint      widget_id;


    widget_id = dzl_preferences_add_file_chooser (prefs, "dictionaries", "paths", "org.gtk.gydict.paths",
                                                  "gyenglishpwn-english", NULL, _("PWN English-Polish"), _("Select angpol.win"),
                                                  GTK_FILE_CHOOSER_ACTION_OPEN, NULL, 0);
    widget    = dzl_gtk_widget_find_child_typed (dzl_preferences_get_widget (prefs, widget_id), GTK_TYPE_FILE_CHOOSER_BUTTON);
    if (widget) gtk_size_group_add_widget (path_group, widget);


    widget_id = dzl_preferences_add_file_chooser (prefs, "dictionaries", "paths", "org.gtk.gydict.paths",
                                                  "gyenglishpwn-polish", NULL, _("PWN Polish-English"), _("Select polang.win"),
                                                  GTK_FILE_CHOOSER_ACTION_OPEN, NULL, 20);
    widget    = dzl_gtk_widget_find_child_typed (dzl_preferences_get_widget (prefs, widget_id), GTK_TYPE_FILE_CHOOSER_BUTTON);
    if (widget) gtk_size_group_add_widget (path_group, widget);


    widget_id = dzl_preferences_add_file_chooser (prefs, "dictionaries", "paths", "org.gtk.gydict.paths",
                                                  "gygermanpwn-german", NULL, _("PWN German-Polish"), _("Select niempol.win"),
                                                  GTK_FILE_CHOOSER_ACTION_OPEN, NULL, 30);
    widget    = dzl_gtk_widget_find_child_typed (dzl_preferences_get_widget (prefs, widget_id), GTK_TYPE_FILE_CHOOSER_BUTTON);
    if (widget) gtk_size_group_add_widget (path_group, widget);


    widget_id = dzl_preferences_add_file_chooser (prefs, "dictionaries", "paths", "org.gtk.gydict.paths",
                                                  "gygermanpwn-polish", NULL, _("PWN Polish-German"), _("Select polniem.win"),
                                                  GTK_FILE_CHOOSER_ACTION_OPEN, NULL, 40);
    widget    = dzl_gtk_widget_find_child_typed (dzl_preferences_get_widget (prefs, widget_id), GTK_TYPE_FILE_CHOOSER_BUTTON);
    if (widget) gtk_size_group_add_widget (path_group, widget);


    widget_id = dzl_preferences_add_file_chooser (prefs, "dictionaries", "paths", "org.gtk.gydict.paths",
                                                  "gydepl-german", NULL, _("DEPL German-Polish"), _("Select a.dat"),
                                                  GTK_FILE_CHOOSER_ACTION_OPEN, NULL, 50);
    widget    = dzl_gtk_widget_find_child_typed (dzl_preferences_get_widget (prefs, widget_id), GTK_TYPE_FILE_CHOOSER_BUTTON);
    if (widget) gtk_size_group_add_widget (path_group, widget);
  }
}

static void
gy_prefs_view_finalize (GObject *obj)
{
  GyPrefsView *self = GY_PREFS_VIEW (obj);

  g_clear_pointer (&self->size_groups, g_hash_table_unref);

  G_OBJECT_CLASS (gy_prefs_view_parent_class)->finalize (obj);
}

static void
gy_prefs_view_constructed (GObject *obj)
{
  G_OBJECT_CLASS (gy_prefs_view_parent_class)->constructed (obj);

  gy_prefs_view_register_builtin_prefs (DZL_PREFERENCES(obj));
}

static void
gy_prefs_view_class_init (GyPrefsViewClass *klass)
{
  GObjectClass *oclass = G_OBJECT_CLASS (klass);

  oclass->constructed = gy_prefs_view_constructed;
  oclass->finalize    = gy_prefs_view_finalize;
}

static void
gy_prefs_view_init (GyPrefsView *self)
{
}

GtkSizeGroup *
gy_prefs_view_get_size_group (GyPrefsView *self,
                              const gchar *name_group)
{
  GtkSizeGroup *ret = NULL;

  g_return_val_if_fail (GY_IS_PREFS_VIEW (self), NULL);

  if (self->size_groups == NULL)
    self->size_groups = g_hash_table_new_full (g_direct_hash, g_str_equal,
                                               g_free, g_object_unref);

  if (!(ret = g_hash_table_lookup (self->size_groups, name_group)))
    {
      ret = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
      g_hash_table_insert (self->size_groups, g_strdup(name_group), ret);
    }

  return ret;
}