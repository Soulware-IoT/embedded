/**
 * @file Buzzer.cpp
 * @brief Implements the Buzzer class.
 *
 * Drives a passive buzzer through two hardcoded melodies, advancing one note per call to
 * update() using millis()-based timing so playback never blocks the main loop:
 *   - An alarm melody ("Star Power Theme" at 175 BPM) that loops continuously while active.
 *   - A success jingle (ascending C major arpeggio) that plays once and silences itself.
 *
 * @author IoT Solution Development Team
 * @date June 14, 2026
 * @version 1.0
 */

/*
 * This file is part of the Cocina360 Device (C++ Edition) project.
 * Copyright (c) 2026 IoT Solution Development Team
 *
 * Licensed under the MIT License. See LICENSE.md for details.
 */

#include "Buzzer.h"
#include <Arduino.h>

const Command Buzzer::PLAY_ALARM_COMMAND = Command(PLAY_ALARM_COMMAND_ID);
const Command Buzzer::STOP_ALARM_COMMAND = Command(STOP_ALARM_COMMAND_ID);
const Command Buzzer::PLAY_SUCCESS_COMMAND = Command(PLAY_SUCCESS_COMMAND_ID);

#define BPM 175 ///< Melody tempo, in beats per minute.
#define DURATION          (int)(60000 / BPM) ///< Duration of a quarter note, in milliseconds.
#define DURATION_HALF_DOT (int)(45000 / BPM) ///< Duration of a dotted eighth note, in milliseconds.
#define DURATION_HALF     (int)(30000 / BPM) ///< Duration of an eighth note, in milliseconds.
#define DURATION_QUARTER  (int)(15000 / BPM) ///< Duration of a sixteenth note, in milliseconds.

#define NOTE_DS5 622 ///< Frequency (Hz) of note D#5.
#define NOTE_B4  494 ///< Frequency (Hz) of note B4.
#define NOTE_AS4 466 ///< Frequency (Hz) of note A#4.
#define NOTE_A4  440 ///< Frequency (Hz) of note A4.

/// @brief Note frequency sequence for the alarm melody (0 = rest).
static const int melody[] = {
    NOTE_B4, NOTE_B4, NOTE_DS5, NOTE_A4, 0, NOTE_DS5, NOTE_AS4,
    0, NOTE_DS5, NOTE_AS4, 0, NOTE_DS5, NOTE_AS4,
    0, NOTE_DS5, NOTE_AS4, 0, NOTE_DS5, NOTE_AS4,
    0, NOTE_DS5, NOTE_AS4, 0, NOTE_DS5, NOTE_AS4
};

/// @brief Per-note duration sequence (ms), aligned index-for-index with melody[].
static const int noteDurations[] = {
    DURATION_QUARTER, DURATION_HALF_DOT, DURATION_HALF, DURATION_HALF, DURATION, DURATION_HALF, DURATION_HALF,
    DURATION, DURATION_HALF, DURATION_HALF, DURATION, DURATION_HALF, DURATION_HALF,
    DURATION, DURATION_HALF, DURATION_HALF, DURATION, DURATION_HALF, DURATION_HALF,
    DURATION, DURATION_HALF, DURATION_HALF, DURATION, DURATION_HALF, DURATION_HALF,
};

static const int NUM_NOTES = sizeof(melody) / sizeof(melody[0]); ///< Total number of notes in the alarm melody.

#define NOTE_C5 523 ///< Frequency (Hz) of note C5.
#define NOTE_E5 659 ///< Frequency (Hz) of note E5.
#define NOTE_G5 784 ///< Frequency (Hz) of note G5.
#define NOTE_C6 1047 ///< Frequency (Hz) of note C6.

/// @brief Note frequency sequence for the one-shot success jingle: an ascending C major arpeggio.
static const int successMelody[] = {
    NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6
};

/// @brief Per-note duration sequence (ms) for the success jingle, aligned with successMelody[].
static const int successNoteDurations[] = {
    90, 90, 90, 220
};

static const int NUM_SUCCESS_NOTES = sizeof(successMelody) / sizeof(successMelody[0]); ///< Total number of notes in the success jingle.

Buzzer::Buzzer(int pin, CommandHandler* commandHandler)
    : Actuator(pin, commandHandler), isPlaying(false), loopMelody(false), noteStartTime(0),
      currentNote(0), activeMelody(nullptr), activeDurations(nullptr), activeNumNotes(0) {
    pinMode(pin, OUTPUT);
    noTone(pin);
}

void Buzzer::start(const int* melody, const int* durations, int numNotes, bool loop) {
    activeMelody = melody;
    activeDurations = durations;
    activeNumNotes = numNotes;
    loopMelody = loop;
    currentNote = 0;
    noteStartTime = 0;
    isPlaying = true;
}

void Buzzer::stop() {
    isPlaying = false;
    noTone(pin);
    currentNote = 0;
}

void Buzzer::handle(Command command) {
    if (command == PLAY_ALARM_COMMAND) {
        start(melody, noteDurations, NUM_NOTES, true);
    } else if (command == PLAY_SUCCESS_COMMAND) {
        start(successMelody, successNoteDurations, NUM_SUCCESS_NOTES, false);
    } else if (command == STOP_ALARM_COMMAND) {
        stop();
    }
    Actuator::handle(command);
}

void Buzzer::update() {
    if (!isPlaying) return;

    unsigned long now = millis();
    int duration = activeDurations[currentNote];

    if (now - noteStartTime >= (duration * 1.1)) {
        noteStartTime = now;
        if (activeMelody[currentNote] > 0) {
            tone(pin, activeMelody[currentNote], duration);
        } else {
            noTone(pin);
        }
        currentNote++;
        if (currentNote >= activeNumNotes) {
            if (loopMelody) {
                currentNote = 0;
            } else {
                // Don't call stop() here: the last note was just started via tone() with its own
                // duration and will stop itself. Calling noTone() now would cut it off early.
                isPlaying = false;
                currentNote = 0;
            }
        }
    }
}
