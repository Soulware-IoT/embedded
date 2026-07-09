/**
 * @file Buzzer.cpp
 * @brief Implements the Buzzer class.
 *
 * Drives a passive buzzer through a hardcoded alarm melody ("Star Power Theme" at 175 BPM),
 * advancing one note per call to update() using millis()-based timing so playback never blocks
 * the main loop.
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

static const int NUM_NOTES = sizeof(melody) / sizeof(melody[0]); ///< Total number of notes in the melody.

Buzzer::Buzzer(int pin, CommandHandler* commandHandler) 
    : Actuator(pin, commandHandler), isPlaying(false), noteStartTime(0), currentNote(0) {
    pinMode(pin, OUTPUT);
    noTone(pin);
}

void Buzzer::handle(Command command) {
    if (command == PLAY_ALARM_COMMAND) {
        isPlaying = true;
    } else if (command == STOP_ALARM_COMMAND) {
        isPlaying = false;
        noTone(pin);
        currentNote = 0;
    }
    Actuator::handle(command);
}

void Buzzer::update() {
    if (!isPlaying) return;

    unsigned long now = millis();
    int duration = noteDurations[currentNote];

    if (now - noteStartTime >= (duration * 1.1)) {
        noteStartTime = now;
        if (melody[currentNote] > 0) {
            tone(pin, melody[currentNote], duration);
        } else {
            noTone(pin);
        }
        currentNote++;
        if (currentNote >= NUM_NOTES) {
            currentNote = 0;
        }
    }
}
