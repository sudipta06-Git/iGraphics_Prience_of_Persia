#include "common.h"
#include "game_types.h"
#include "game_globals.h"

#include "home.h"
#include "controls.h"
#include "combat.h"
#include "sounds.h"
#include "images.h"
#include "next_level.h"

int main()
{
    setupLevel1();

    // Initialize the sound systems before the first game frame.
    initAttackSound();
    initGameBGM();
    initButtonSound();
    initRunningSound();
    // Level 3's own effects and its final victory music. Both are
    // guarded so they can never open twice, and every Level 3 effect
    // file is optional - a missing one just stays silent (see the
    // LEVEL 3 AUDIO block in sounds.h).
    initLevel3Sounds();
    initVictoryBGM();

    // iGraphics invokes iDraw(), iMouse(), iKeyboard(), and update()
    // through the callbacks defined in the functional headers.
    iInitialize(1350, 680, "Prince of Persia: Escape from the Palace");
    return 0;
}
