// enve - 2D animations software
// Copyright (C) 2016-2019 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PATHEFFECTSMENU_H
#define PATHEFFECTSMENU_H
#include "typemenu.h"
#include "PathEffects/patheffectsinclude.h"
#include "boundingbox.h"

namespace PathEffectsMenu {
    template <typename T, typename U>
    void addPathEffectActionToMenu(const QString& text,
                                   PropertyMenu * const menu,
                                   const U& adder);
    template <typename U>
    void addPathEffectsActionToMenu(PropertyMenu * const menu,
                                    const U &adder);
    void addPathEffectsToActionMenu(PropertyMenu * const menu);
}

#endif // PATHEFFECTSMENU_H
