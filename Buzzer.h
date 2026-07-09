#ifndef BUZZER_H
#define BUZZER_H

/**
 * @file Buzzer.h
 * @brief Declares the Buzzer class.
 *
 * This class specializes the Modest IoT Nano-framework's Actuator to drive a passive buzzer as
 * the system's acoustic safety barrier. It plays a looping alarm melody asynchronously, in
 * non-blocking fashion, in response to PLAY_ALARM / STOP_ALARM commands.
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
    bool isPlaying; ///< Whether the alarm melody is currently active.
    unsigned long noteStartTime; ///< Timestamp (ms) at which the current note started playing.
    int currentNote; ///< Index of the note currently playing within the melody sequence.

public:
    static const int PLAY_ALARM_COMMAND_ID = 10; ///< Identifier for the play-alarm command.
    static const int STOP_ALARM_COMMAND_ID = 11; ///< Identifier for the stop-alarm command.
    static const Command PLAY_ALARM_COMMAND; ///< Command that starts the looping alarm melody.
    static const Command STOP_ALARM_COMMAND; ///< Command that silences the buzzer.

    /**
     * @brief Constructs a Buzzer bound to a digital output pin.
     * @param pin The GPIO pin connected to the passive buzzer.
     * @param commandHandler Pointer to a CommandHandler this actuator reports to (default: nullptr).
     */
    Buzzer(int pin, CommandHandler* commandHandler = nullptr);

    /**
     * @brief Handles a PLAY_ALARM or STOP_ALARM command by toggling playback state.
     * @param command The command to handle.
     */
    void handle(Command command) override;

    /**
     * @brief Advances the melody playback on a non-blocking timer. Must be called every loop.
     *
     * While playing, emits the next note of the alarm melody once its duration has elapsed,
     * looping back to the start of the sequence when it completes.
     */
    void update();
};

#endif