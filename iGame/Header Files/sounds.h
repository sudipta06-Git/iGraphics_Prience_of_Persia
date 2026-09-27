#ifndef SOUNDS_H
#define SOUNDS_H

#include "game_globals.h"

std::vector<char> attackSoundData;
int attackSoundLoaded = 0;

// Load the short attack sound into RAM once at startup.
// Playing from memory avoids MP3 decoding and disk/file-system work when E is pressed.
void initAttackSound(){
	std::ifstream file("attack.wav", std::ios::binary | std::ios::ate);
	if (!file.is_open())
		return;

	std::streamsize size = file.tellg();
	if (size <= 0)
		return;

	file.seekg(0, std::ios::beg);
	attackSoundData.resize((size_t)size);

	if (file.read(attackSoundData.data(), size))
		attackSoundLoaded = 1;
	else
		attackSoundData.clear();
}

void playAttackSound(){
	if (!attackSoundLoaded || attackSoundData.empty())
		return;

	// SND_MEMORY + SND_ASYNC: play the already-loaded WAV without blocking
	// the game loop. No MP3 decoding, seek, stop, or disk access occurs here.
	PlaySoundA(
		attackSoundData.data(),
		NULL,
		SND_MEMORY | SND_ASYNC | SND_NODEFAULT
		);
}


/* ---------------------------------------------------------------- */
/* Button click sound                                                  */
/* ---------------------------------------------------------------- */
int buttonSoundOpened = 0;

void initButtonSound(){
	if (buttonSoundOpened) return;

	MCIERROR err = mciSendStringA(
		"open \"click.mp3\" type mpegvideo alias buttonSound",
		NULL, 0, NULL
		);

	if (err == 0){
		mciSendStringA("setaudio buttonSound volume to 500", NULL, 0, NULL);
		buttonSoundOpened = 1;
	}
}

void playButtonSound(){
	if (!buttonSoundOpened)
		initButtonSound();

	if (!buttonSoundOpened) return;

	mciSendStringA("stop buttonSound", NULL, 0, NULL);
	mciSendStringA("seek buttonSound to start", NULL, 0, NULL);
	mciSendStringA("setaudio buttonSound volume to 500", NULL, 0, NULL);
	mciSendStringA("play buttonSound", NULL, 0, NULL);
}

void closeButtonSound(){
	if (buttonSoundOpened){
		mciSendStringA("stop buttonSound", NULL, 0, NULL);
		mciSendStringA("close buttonSound", NULL, 0, NULL);
		buttonSoundOpened = 0;
	}
}

/* ---------------------------------------------------------------- */
/* Running sound                                                      */
/* ---------------------------------------------------------------- */
int runningSoundOpened = 0;
int runningSoundPlaying = 0;

// Open the running sound once. Volume is 500/1000 = 50%.
void initRunningSound(){
	if (runningSoundOpened) return;

	MCIERROR err = mciSendStringA(
		"open \"running.mp3\" type mpegvideo alias runningSound",
		NULL, 0, NULL
		);

	if (err == 0){
		mciSendStringA("setaudio runningSound volume to 500", NULL, 0, NULL);
		runningSoundOpened = 1;
	}
}

void startRunningSound(){
	if (!runningSoundOpened)
		initRunningSound();

	if (!runningSoundOpened || runningSoundPlaying) return;

	mciSendStringA("stop runningSound", NULL, 0, NULL);
	mciSendStringA("seek runningSound to start", NULL, 0, NULL);
	mciSendStringA("setaudio runningSound volume to 500", NULL, 0, NULL);
	mciSendStringA("play runningSound repeat", NULL, 0, NULL);
	runningSoundPlaying = 1;
}

void stopRunningSound(){
	if (!runningSoundOpened || !runningSoundPlaying) return;

	mciSendStringA("stop runningSound", NULL, 0, NULL);
	mciSendStringA("seek runningSound to start", NULL, 0, NULL);
	runningSoundPlaying = 0;
}

void closeRunningSound(){
	if (runningSoundOpened){
		mciSendStringA("stop runningSound", NULL, 0, NULL);
		mciSendStringA("close runningSound", NULL, 0, NULL);
		runningSoundOpened = 0;
		runningSoundPlaying = 0;
	}
}

/* ---------------------------------------------------------------- */
/* Background music                                                   */
/* ---------------------------------------------------------------- */
int gameBgmOpened = 0;
int gameBgmPlaying = 0;

// Open the background music once. The actual playback starts when
// START GAME is clicked. Volume is 500/1000 = 50%.
void initGameBGM(){
	if (gameBgmOpened) return;

	MCIERROR err = mciSendStringA(
		"open \"gamebgm.mp3\" type mpegvideo alias gameBGM",
		NULL, 0, NULL
		);

	if (err == 0){
		mciSendStringA("setaudio gameBGM volume to 500", NULL, 0, NULL);
		gameBgmOpened = 1;
	}
}

void startGameBGM(){
	if (!gameBgmOpened)
		initGameBGM();

	if (!gameBgmOpened) return;

	mciSendStringA("stop gameBGM", NULL, 0, NULL);
	mciSendStringA("seek gameBGM to start", NULL, 0, NULL);
	mciSendStringA("setaudio gameBGM volume to 500", NULL, 0, NULL);
	mciSendStringA("play gameBGM repeat", NULL, 0, NULL);
	gameBgmPlaying = 1;
}

void stopGameBGM(){
	if (!gameBgmOpened) return;

	mciSendStringA("stop gameBGM", NULL, 0, NULL);
	mciSendStringA("seek gameBGM to start", NULL, 0, NULL);
	gameBgmPlaying = 0;
}

void closeGameBGM(){
	if (gameBgmOpened){
		mciSendStringA("stop gameBGM", NULL, 0, NULL);
		mciSendStringA("close gameBGM", NULL, 0, NULL);
		gameBgmOpened = 0;
		gameBgmPlaying = 0;
	}
}

/* ================================================================== */
/* LEVEL 3 AUDIO                                                       */
/*                                                                     */
/* Two additions, both built on machinery that is already in this      */
/* file so nothing about the existing audio behaviour changes:         */
/*                                                                     */
/* 1) The final victory music, opened through MCI exactly like         */
/*    gamebgm.mp3 above (one alias, opened at most once, guarded by    */
/*    its own ...Opened flag, closed alongside the others on EXIT).    */
/*    It uses wonbgm.mp3, which already ships with the project.        */
/*                                                                     */
/* 2) A handful of short Level 3 effect sounds, loaded into RAM and    */
/*    played with PlaySoundA(SND_MEMORY | SND_ASYNC) - the exact same  */
/*    approach attack.wav already uses, so there is no MCI handle to   */
/*    leak, no decoding on the game thread, and no way to open the     */
/*    same sound twice.                                                */
/*                                                                     */
/*    EVERY ONE OF THESE FILES IS OPTIONAL. loadMemorySound() simply   */
/*    reports failure when the file is not next to the executable, the */
/*    matching play...() call then does nothing at all, and Level 3    */
/*    runs perfectly (just quieter). Drop any of these 24-bit/16-bit   */
/*    PCM .wav files in beside attack.wav to enable them:              */
/*        collapse_warn.wav   - a slab starts to crack                 */
/*        collapse.wav        - a slab drops away                      */
/*        water_rise.wav      - the flood surges to a new level        */
/*        guardian_hit.wav    - the Guardian takes a sword blow        */
/*        guardian_death.wav  - the Guardian falls                     */
/*        escape.wav          - the palace begins its final collapse   */
/*        bat.wav             - a Level 2 bat sweeps in                */
/*        bat_hit.wav         - a Level 2 bat connects                 */
/*        heart.wav           - a health heart is collected            */
/*        bite.wav            - a Level 3 piranha takes hold           */
/*        climb.wav           - hauling out of Level 3's water ('R')   */
/*    (The Guardian's own swing deliberately reuses the existing       */
/*    attack.wav through playAttackSound(), so it is never silent.)    */
/* ================================================================== */

/* ---------------------------------------------------------------- */
/* Final victory music (wonbgm.mp3)                                   */
/* ---------------------------------------------------------------- */
int victoryBgmOpened = 0;
int victoryBgmPlaying = 0;

void initVictoryBGM(){
	if (victoryBgmOpened) return;

	MCIERROR err = mciSendStringA(
		"open \"wonbgm.mp3\" type mpegvideo alias victoryBGM",
		NULL, 0, NULL
		);

	if (err == 0){
		mciSendStringA("setaudio victoryBGM volume to 600", NULL, 0, NULL);
		victoryBgmOpened = 1;
	}
}

void startVictoryBGM(){
	if (!victoryBgmOpened)
		initVictoryBGM();

	if (!victoryBgmOpened) return;

	mciSendStringA("stop victoryBGM", NULL, 0, NULL);
	mciSendStringA("seek victoryBGM to start", NULL, 0, NULL);
	mciSendStringA("setaudio victoryBGM volume to 600", NULL, 0, NULL);
	mciSendStringA("play victoryBGM repeat", NULL, 0, NULL);
	victoryBgmPlaying = 1;
}

void stopVictoryBGM(){
	if (!victoryBgmOpened) return;

	mciSendStringA("stop victoryBGM", NULL, 0, NULL);
	mciSendStringA("seek victoryBGM to start", NULL, 0, NULL);
	victoryBgmPlaying = 0;
}

void closeVictoryBGM(){
	if (victoryBgmOpened){
		mciSendStringA("stop victoryBGM", NULL, 0, NULL);
		mciSendStringA("close victoryBGM", NULL, 0, NULL);
		victoryBgmOpened = 0;
		victoryBgmPlaying = 0;
	}
}

/* ---------------------------------------------------------------- */
/* Optional in-memory Level 3 effect sounds                           */
/* ---------------------------------------------------------------- */
// Returns 1 only if the whole file was read successfully. A missing
// file is NOT an error here - it just leaves the sound unloaded.
int loadMemorySound(const char *fileName, std::vector<char> &out){
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		return 0;

	std::streamsize size = file.tellg();
	if (size <= 0)
		return 0;

	file.seekg(0, std::ios::beg);
	out.resize((size_t)size);

	if (file.read(out.data(), size))
		return 1;

	out.clear();
	return 0;
}

void playMemorySound(std::vector<char> &data, int loaded){
	if (!loaded || data.empty())
		return;

	PlaySoundA(
		data.data(),
		NULL,
		SND_MEMORY | SND_ASYNC | SND_NODEFAULT
		);
}

std::vector<char> level3CollapseWarnData;   int level3CollapseWarnLoaded = 0;
std::vector<char> level3CollapseData;       int level3CollapseLoaded = 0;
std::vector<char> level3WaterRiseData;      int level3WaterRiseLoaded = 0;
std::vector<char> level3GuardianHitData;    int level3GuardianHitLoaded = 0;
std::vector<char> level3GuardianDeathData;  int level3GuardianDeathLoaded = 0;
std::vector<char> level3EscapeData;         int level3EscapeLoaded = 0;

/* Level 2's bats use the same optional-file mechanism - bat.wav as one
 * sweeps in, bat_hit.wav when one connects. Both missing is fine; the
 * bats just fly silently. */
std::vector<char> level2BatData;            int level2BatLoaded = 0;
std::vector<char> level2BatHitData;         int level2BatHitLoaded = 0;

/* Health hearts (Levels 2 and 3) - heart.wav when one is taken. */
std::vector<char> heartData;                int heartLoaded = 0;

/* Level 3's water - a piranha taking hold, and pulling out onto a ledge. */
std::vector<char> level3BiteData;           int level3BiteLoaded = 0;
std::vector<char> level3ClimbData;          int level3ClimbLoaded = 0;

int level3SoundsInitialised = 0;

// Called once from main(), before the first frame. Guarded so it can
// never load the same buffers twice even if it is called again.
void initLevel3Sounds(){
	if (level3SoundsInitialised) return;
	level3SoundsInitialised = 1;

	level3CollapseWarnLoaded   = loadMemorySound("collapse_warn.wav",  level3CollapseWarnData);
	level3CollapseLoaded       = loadMemorySound("collapse.wav",       level3CollapseData);
	level3WaterRiseLoaded      = loadMemorySound("water_rise.wav",     level3WaterRiseData);
	level3GuardianHitLoaded    = loadMemorySound("guardian_hit.wav",   level3GuardianHitData);
	level3GuardianDeathLoaded  = loadMemorySound("guardian_death.wav", level3GuardianDeathData);
	level3EscapeLoaded         = loadMemorySound("escape.wav",         level3EscapeData);

	/* Level 2's bats - optional in exactly the same way. */
	level2BatLoaded            = loadMemorySound("bat.wav",            level2BatData);
	level2BatHitLoaded         = loadMemorySound("bat_hit.wav",        level2BatHitData);
	heartLoaded                = loadMemorySound("heart.wav",          heartData);
	level3BiteLoaded           = loadMemorySound("bite.wav",           level3BiteData);
	level3ClimbLoaded          = loadMemorySound("climb.wav",          level3ClimbData);
}

void playCollapseWarnSound(){   playMemorySound(level3CollapseWarnData,  level3CollapseWarnLoaded); }
void playCollapseSound(){       playMemorySound(level3CollapseData,      level3CollapseLoaded); }
void playWaterRiseSound(){      playMemorySound(level3WaterRiseData,     level3WaterRiseLoaded); }
void playGuardianHitSound(){    playMemorySound(level3GuardianHitData,   level3GuardianHitLoaded); }
void playGuardianDeathSound(){  playMemorySound(level3GuardianDeathData, level3GuardianDeathLoaded); }
void playEscapeSound(){         playMemorySound(level3EscapeData,        level3EscapeLoaded); }
void playBatSound(){            playMemorySound(level2BatData,           level2BatLoaded); }
void playBatHitSound(){         playMemorySound(level2BatHitData,        level2BatHitLoaded); }
void playHeartSound(){          playMemorySound(heartData,               heartLoaded); }
void playBiteSound(){           playMemorySound(level3BiteData,          level3BiteLoaded); }
void playClimbSound(){          playMemorySound(level3ClimbData,         level3ClimbLoaded); }

#endif
