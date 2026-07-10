#ifndef BUZZER_H
#define BUZZER_H

/**
 * @file Buzzer.h
 * @brief Declares the Buzzer class.
 *
 * This class specializes the Modest IoT Nano-framework's Actuator to drive a passive buzzer.
 * It plays melodies asynchronously, in non-blocking fashion: a looping alarm melody acting as
 * the system's acoustic safety barrier (PLAY_ALARM / STOP_ALARM), and a one-shot success jingle
 * for signaling positive outcomes (PLAY_SUCCESS).
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

#include "Actuator.h"

class Buzzer : public Actuator {
private:
    bool isPlaying; ///< Whether a melody is currently active.
    bool loopMelody; ///< Whether the active melody restarts from the beginning when it finishes.
    unsigned long noteStartTime; ///< Timestamp (ms) at which the current note started playing.
    int currentNote; ///< Index of the note currently playing within the active melody sequence.
    const int* activeMelody; ///< Note frequency sequence currently being played.
    const int* activeDurations; ///< Per-note duration sequence (ms) for the active melody.
    int activeNumNotes; ///< Number of notes in the active melody.

    /**
     * @brief Begins playback of a melody from its first note.
     * @param melody Note frequency sequence (0 = rest).
     * @param durations Per-note duration sequence (ms), aligned index-for-index with melody.
     * @param numNotes Number of notes in the sequence.
     * @param loop Whether playback should restart from the beginning after the last note.
     */
    void start(const int* melody, const int* durations, int numNotes, bool loop);

    /// @brief Silences the buzzer and clears playback state.
    void stop();

public:
    static const int PLAY_ALARM_COMMAND_ID = 10; ///< Identifier for the play-alarm command.
    static const int STOP_ALARM_COMMAND_ID = 11; ///< Identifier for the stop-alarm command.
    static const int PLAY_SUCCESS_COMMAND_ID = 12; ///< Identifier for the play-success-jingle command.
    static const Command PLAY_ALARM_COMMAND; ///< Command that starts the looping alarm melody.
    static const Command STOP_ALARM_COMMAND; ///< Command that silences the buzzer.
    static const Command PLAY_SUCCESS_COMMAND; ///< Command that plays a one-shot success jingle.

    /**
     * @brief Constructs a Buzzer bound to a digital output pin.
     * @param pin The GPIO pin connected to the passive buzzer.
     * @param commandHandler Pointer to a CommandHandler this actuator reports to (default: nullptr).
     */
    Buzzer(int pin, CommandHandler* commandHandler = nullptr);
    void begin();
    /**
     * @brief Handles a PLAY_ALARM, STOP_ALARM, or PLAY_SUCCESS command by toggling playback state.
     *
     * PLAY_ALARM and PLAY_SUCCESS both interrupt whatever is currently playing and start their
     * own melody from its first note. PLAY_SUCCESS needs no matching stop: it is a one-shot
     * jingle that silences itself after its last note.
     *
     * @param command The command to handle.
     */
    void handle(Command command) override;

    /**
     * @brief Advances the melody playback on a non-blocking timer. Must be called every loop.
     *
     * While playing, emits the next note of the active melody once its duration has elapsed.
     * A looping melody (the alarm) restarts from the beginning; a one-shot melody (the success
     * jingle) silences the buzzer and clears playback state after its last note.
     */
    void update();
};

#endif