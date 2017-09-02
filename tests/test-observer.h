/* test-observer.h
 *
 * Copyright (C) 2017 Jakub Czartek <kuba@linux.pl>
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

#ifndef TEST_OBSERVER_H
#define TEST_OBSERVER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define TEST_TYPE_OBSERVER (test_observer_get_type())

G_DECLARE_FINAL_TYPE (TestObserver, test_observer, TEST, OBSERVER, GObject)

TestObserver *test_observer_new (void);

G_END_DECLS

#endif /* TEST_OBSERVER_H */

