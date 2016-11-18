/* gy-css-provider.c
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

#define G_LOG_DOMAIN "GyCssProvider"

#include "gy-css-provider.h"

struct _GyCssProvider
{
  GtkCssProvider parent;

  GtkSettings *gtk_settings;
};

G_DEFINE_TYPE (GyCssProvider, gy_css_provider, GTK_TYPE_CSS_PROVIDER)

GtkCssProvider *
gy_css_provider_new (void)
{
  return GTK_CSS_PROVIDER (g_object_new (GY_TYPE_CSS_PROVIDER, NULL));
}

static void gy_css_provider_update (GyCssProvider *self)
{
  g_autofree gchar *theme_name = NULL;
  g_autofree char *resource_path = NULL;
  gboolean prefer_dark_theme = FALSE;
  gsize len = 0;
  guint32 flags = 0;

  g_return_if_fail (GY_IS_CSS_PROVIDER (self));
  g_return_if_fail (GTK_IS_SETTINGS (self->gtk_settings));

  g_object_get (self->gtk_settings,
                "gtk-theme-name", &theme_name,
                "gtk-application-prefer-dark-theme", &prefer_dark_theme,
                NULL);

  resource_path = g_strdup_printf ("/org/gtk/gydict/%s%s.css",
                                   theme_name,
                                   prefer_dark_theme ? "-dark" : "");

  if (!g_resources_get_info (resource_path, G_RESOURCE_LOOKUP_FLAGS_NONE, &len, &flags, NULL))
    {
      g_free (resource_path);
      resource_path = g_strdup ("/org/gtk/gydict/Default.css");
    }

  gtk_css_provider_load_from_resource (GTK_CSS_PROVIDER (self), resource_path);
}

static void
gy_css_provider_settings_notify_gtk_theme_name (GyCssProvider *self,
                                                GParamSpec    *pspec,
                                                GtkSettings   *settings)
{
  g_return_if_fail (GY_IS_CSS_PROVIDER (self));

  gy_css_provider_update (self);
}

static void
gy_css_provider_settings_notify_gtk_applicaton_prefer_dark_theme (GyCssProvider *self,
                                                                  GParamSpec    *pspec,
                                                                  GtkSettings   *settings)
{
  g_return_if_fail (GY_IS_CSS_PROVIDER (self));

  gy_css_provider_update (self);
}

static void
gy_css_provider_finalize (GObject *object)
{
  GyCssProvider *self = (GyCssProvider *)object;

  g_clear_object (&self->gtk_settings);

  G_OBJECT_CLASS (gy_css_provider_parent_class)->finalize (object);
}

static void
gy_css_provider_constructed (GObject *object)
{
  GyCssProvider *self = GY_CSS_PROVIDER (object);

  G_OBJECT_CLASS (gy_css_provider_parent_class)->constructed (object);

  self->gtk_settings = g_object_ref (gtk_settings_get_default ());

  g_signal_connect_swapped (self->gtk_settings, "notify::gtk-theme-name",
                            G_CALLBACK (gy_css_provider_settings_notify_gtk_theme_name), self);
  g_signal_connect_swapped (self->gtk_settings, "notify::gtk-application-prefer-dark-theme",
                            G_CALLBACK (gy_css_provider_settings_notify_gtk_applicaton_prefer_dark_theme), self);

  gy_css_provider_update (self);
}

static void
gy_css_provider_parsing_error (GtkCssProvider *provider,
                               GtkCssSection  *section,
                               const GError   *error)
{

  g_return_if_fail (GY_IS_CSS_PROVIDER (provider));
  g_return_if_fail (error != NULL);

  if (section != NULL)
    {
      g_autofree gchar *uri = NULL;
      GFile *file;
      file = gtk_css_section_get_file (section);
      uri = g_file_get_uri (file);

      g_critical ("Parsing Error: %s @ %u:%u: %s",
                  uri, gtk_css_section_get_start_line (section),
                  gtk_css_section_get_start_position (section), error->message);
    }
  else
    {
      g_critical ("%s", error->message);
    }
}

static void
gy_css_provider_class_init (GyCssProviderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkCssProviderClass *provider_class = GTK_CSS_PROVIDER_CLASS (klass);

  object_class->constructed = gy_css_provider_constructed;
  object_class->finalize = gy_css_provider_finalize;

  provider_class->parsing_error = gy_css_provider_parsing_error;
}

static void
gy_css_provider_init (GyCssProvider *self)
{
}
