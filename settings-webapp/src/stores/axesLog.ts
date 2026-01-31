/* eslint-disable @typescript-eslint/no-explicit-any */
import { ref } from 'vue'
import { defineStore } from 'pinia'

import api from '@/api/api';
import type { AxisLogResponse } from '@/api/axesLogApi'

export interface AxisPidSettings {
    kp: number;
    ki: number;
    kd: number;
    integralRange: number;
    integralRate: number;
}
export interface AxisLogWithInfo {
    log: AxisLogResponse;
    pidSettings: AxisPidSettings;
}

function convertFromCountToStuds(log: AxisLogResponse, counts_per_unit: number): AxisLogResponse {
    const convertedLog: AxisLogResponse = {
        ...log,
        data: log.data.map(row => {
            // Clone the row to avoid mutating the original
            const newRow = [...row];
            // Columns 3,4,7,8,9,10 are converted from counts to studs
            [2, 3, 6, 7, 8, 9].forEach(idx => {
                if (typeof newRow[idx] === 'number') {
                    newRow[idx] = newRow[idx] / counts_per_unit;
                }
            });
            // Column 5 (PWM) is converted from [-10000,10000] to [-100,100]
            if (typeof newRow[5] === 'number') {
                newRow[5] = newRow[5] / 100;
            }
            return newRow;
        })
    }

    return convertedLog;
}
export const useAxesLogStore = defineStore('axeslog', () => {
    // State
    const logs = ref<Record<string, AxisLogWithInfo[]>>({});
    
    // Actions
    const startLogging = async(axis: string, duration?: number, skip?: number) => {        
        const params: Record<string, number> = {
            ...(duration !== undefined && { duration: duration }),
            ...(skip !== undefined && { div: skip })
        };
        await api.get(`/axislog/start/${axis}`, { params });
    }

    const stopLogging = async(axis: string) => {
        await api.get(`/axislog/stop/${axis}`);
    }

    const isLogRunning = async(axis: string) => {
        const response = await api.get<{ running: boolean }>(`/axislog/running/${axis}`);
        return response.data.running;
    }

    const downloadLastLog = async(axis: string, counts_per_unit: number, pidSettings: AxisPidSettings) => {
        const response = await api.get<AxisLogResponse>(`/axislog/read/${axis}`);
        const convertedLog = convertFromCountToStuds(response.data, counts_per_unit);
        const newLog: AxisLogWithInfo = {
            log: convertedLog,
            pidSettings: pidSettings
        };

        // Add the new log to the array of logs for the axis
        if (logs.value[axis] === undefined) {
            logs.value[axis] = [];
        }
        logs.value[axis].push(newLog);
    }
    
    return {
        logs,
        startLogging,
        stopLogging,
        isLogRunning,
        downloadLastLog
    };
});