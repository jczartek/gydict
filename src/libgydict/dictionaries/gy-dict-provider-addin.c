/* gy-dict-provider-addin.c
 *
 * Copyright 2019 Jakub Czartek <kuba@linux.pl>
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

#include "gy-dict-provider-addin.h"

G_DEFINE_INTERFACE (GyDictProviderAddin, gy_dict_provider_addin, G_TYPE_OBJECT)

static void
gy_dict_provider_addin_default_init (GyDictProviderAddinInterface *iface)
{
}

void
gy_dict_provider_addin_load (GyDictProviderAddin *self,
                             GyDictProvider      *provider)
{
  g_return_if_fail (GY_IS_DICT_PROVIDER_ADDIN (self));
  g_return_if_fail (GY_IS_DICT_PROVIDER (provider));

  if (GY_DICT_PROVIDER_ADDIN_GET_IFACE (self)->load != NULL)
    GY_DICT_PROVIDER_ADDIN_GET_IFACE (self)->load (self, provider);
}

void
gy_dict_provider_addin_unload (GyDictProviderAddin *self,
                               GyDictProvider      *provider)
{
  g_return_if_fail (GY_IS_DICT_PROVIDER_ADDIN (self));
  g_return_if_fail (GY_IS_DICT_PROVIDER (provider));

  if (GY_DICT_PROVIDER_ADDIN_GET_IFACE (self)->unload != NULL)
    GY_DICT_PROVIDER_ADDIN_GET_IFACE (self)->unload (self, provider);
}
