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

#ifndef __GY_SETTINGS_H__
#define __GY_SETTINGS_H__

#include <gtk/gtk.h>

#define GY_SETTINGS_KEY_USE_PATH	"use-system-path"
#define GY_SETTINGS_KEY_USE_FONTS	"use-system-fonts"
#define GY_SETTINGS_KEY_PATH_PWN_ANG	"dict-pwn-angpol"
#define GY_SETTINGS_KEY_PATH_PWN_PL	"dict-pwn-polang"
#define GY_SETTINGS_KEY_PATH_DEPL_A	"dict-depl-a"
#define GY_SETTINGS_KEY_PATH_DEPL_B	"dict-depl-b"
#define GY_SETTINGS_KEY_FONT_TEXT	"font-text"
#define GY_SETTINGS_KEY_FONT_TREE	"font-list"

G_BEGIN_DECLS	

/*
 * Type macros
 */
#define GY_TYPE_SETTINGS 	    (gy_settings_get_type ())
#define GY_SETTINGS(obj) 	    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_SETTINGS, GySettings))
#define GY_IS_SETTINGS(obj) 	    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_SETTINGS))
#define GY_SETTINGS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_SETTINGS, GySettingsClass))
#define GY_IS_SETTINGS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_SETTINGS))
#define GY_SETTINGS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_SETTINGS, GySettingsClass))

typedef struct _GySettings GySettings;
typedef struct _GySettingsClass GySettingsClass;
typedef struct _GySettingsPrivate GySettingsPrivate;

struct _GySettings
{
    GObject parent_instance;

    /* instance members */
    GySettingsPrivate *priv;

};

struct _GySettingsClass
{
    GObjectClass parent_class;

    /* signals */
    void (*fonts_changed) (GySettings  *settings,
			   const gchar *text_font,
			   const gchar *list_font);
    void (*paths_changed) (GySettings  *settings,
			   gboolean     use_default,
			   const gchar *key_path,
			   const gchar *value_path);

};

/* used by GY_TYPE_SETTINGS */
GType gy_settings_get_type (void);

/*
 * Method definitions
 */
GySettings * gy_settings_get (void);
gchar * gy_settings_get_path_dictionary (GySettings  *settings, 
					 const gchar *key);
void gy_settings_set_path_dictionary (GySettings  *settings, 
				      const gchar *key, 
				      const gchar *value);
gboolean gy_settings_get_use_fonts_system (GySettings *settings);
void gy_settings_set_use_fonts_system (GySettings  *settings, 
				       const gchar *key, 
				       gboolean     value);
gboolean gy_settings_get_use_paths_system (GySettings  *settings, 
					   const gchar *key);
void gy_settings_set_use_paths_system (GySettings  *settings, 
				       const gchar *key, 
				       gboolean value);
gchar *gy_settings_get_font_text (GySettings *settings);
gchar *gy_settings_get_font_tree (GySettings *settings);
GSettings *gy_settings_get_fonts_settings (GySettings *settings);
GSettings *gy_settings_get_paths_settings (GySettings *settings);

G_END_DECLS


#endif /* end of include guard: __GY_SETTINGS_H__ */
