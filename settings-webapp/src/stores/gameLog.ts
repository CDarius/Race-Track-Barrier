/* eslint-disable @typescript-eslint/no-explicit-any */
import { ref } from 'vue'
import { defineStore } from 'pinia'

import api from '@/api/api';
import type { GetGameLogResponse } from '@/api/gameLogApi'
import { deepEqual } from '@/utils/compare';

export interface GameLog {
    cols_names: string[];
    data: {
        cycle: number;
        timestamp: number;
        first_entry: GameLogFirstEntry;
        sub_entries: GameLogSubCycle[];
    }[];
}

export class GameLogFirstEntry {
    constructor(
        public subcycle: number,
        public ballX: number,
        public ballY: number,
        public paddleL: number,
        public paddleR: number,
    ) {}
}

export class GameLogSubCycle {
    constructor(
        public subcycle: number,
        public targetBallX: number,
        public targetBallY: number,
        public targetPaddleL: number,
        public targetPaddleR: number,
        public speedX: number,
        public speedY: number
    ) {}
}

export const useGameLogStore = defineStore('gamelog', () => {
    // State
    const logs = ref<GameLog[]>([]);
    
    // Actions
    const isLogRunning = async() => {
        const response = await api.get<{ running: boolean }>('/gamelog/running');
        return response.data.running;
    }

    const downloadLastLog = async() => {
        const response = await api.get<GetGameLogResponse>("/gamelog/read", { timeout: 10000 });
        if (!response.data.data || response.data.data.length === 0) {
            // No log available
            return;
        }
        
        // Group rows by cycle
        const grouped: Record<number, { timestamp: number, first_entry: GameLogFirstEntry, sub_entries: GameLogSubCycle[] }> = {};
        for (const row of response.data.data) {
            const cycle = row[0];
            const subcycle = row[1];
            if (!grouped[cycle]) {
                grouped[cycle] = {
                    timestamp: row[6],
                    first_entry: undefined as any,
                    sub_entries: []
                };
            }
            if (subcycle === 0) {
                grouped[cycle].first_entry = new GameLogFirstEntry(
                    row[1], row[2], row[3], row[4], row[5]
                );
            } else {
                grouped[cycle].sub_entries.push(
                    new GameLogSubCycle(
                        row[1], row[2], row[3], row[4], row[5], row[6], row[7]
                    )
                );
            }
        }

        const result: GameLog = {
            cols_names: response.data.cols_names,
            data: Object.entries(grouped).map(([cycle, { timestamp, first_entry, sub_entries }]) => {
                return { cycle: Number(cycle), timestamp, first_entry, sub_entries}
            })
        };

        // Fix timestamps to start at zero
        const startTime = result.data[0].timestamp;
        for (const entry of result.data) {
            entry.timestamp -= startTime;
        }
        
        const lastLog = logs.value.length > 0 ? logs.value[logs.value.length - 1] : null;
        if (lastLog && deepEqual(lastLog, result)) {
            // Discard result if same as lastLog
            return;
        }

        logs.value.push(result);
    }
    
    return {
        logs,
        isLogRunning,
        downloadLastLog,
    };
});