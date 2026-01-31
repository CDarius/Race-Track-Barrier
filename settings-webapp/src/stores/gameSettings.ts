/* eslint-disable @typescript-eslint/no-explicit-any */
import { ref } from 'vue'
import { defineStore } from 'pinia'
import type { CancelTokenSource } from 'axios';

import api from '@/api/api';
import type { ListGameSettingsResponse, GameSettingsGroup, ReadGameSettingRespose, WriteGameSettingRespose } from '@/api/settingsApi'

export const useGameSettingsStore = defineStore('gamesettings', () => {
    // State
    const groups = ref<GameSettingsGroup[]>([]);
    const groupsLoading = ref(false);
    const groupsLoadingError = ref<string | null>(null);

    // Actions
    const fetchGameSettingsList = async() => {
        groupsLoading.value = true;
        groupsLoadingError.value = null;

        try {
            const response = await api.get<ListGameSettingsResponse>('/settings');
            groups.value = response.data.groups;
        } catch(err) {
            groupsLoadingError.value = 'Failed to get settings list';
            console.error('/settings API Error:', err);            
        } finally {
            groupsLoading.value = false;
        }
    }

    // Queue for read requests
    type ReadRequest = {
        groupName: string,
        settingName: string,
        resolve: (value: ReadGameSettingRespose) => void,
        reject: (reason?: any) => void
    };
    const readQueue: ReadRequest[] = [];
    let isProcessingReadQueue = false;

    const processReadQueue = async () => {
        if (isProcessingReadQueue || readQueue.length === 0) return;
        isProcessingReadQueue = true;
        const { groupName, settingName, resolve, reject } = readQueue.shift()!;
        try {
            const result = await api.get<ReadGameSettingRespose>(`/settings/${groupName}/${settingName}`);
            resolve(result.data);
        } catch (err) {
            reject(err);
        } finally {
            isProcessingReadQueue = false;
            // Process next in queue
            if (readQueue.length > 0) {
                processReadQueue();
            }
        }
    };

    const readGameSettingValue = async(groupName: string, settingName: string): Promise<ReadGameSettingRespose> => {
        return new Promise((resolve, reject) => {
            readQueue.push({ groupName, settingName, resolve, reject });
            processReadQueue();
        });
    }

    const writeGameSettingValue = async(groupName: string, settingName: string, value: unknown, cancelTokenSource: CancelTokenSource | null = null) => {
        return await api.put<WriteGameSettingRespose>(
            `/settings/${groupName}/${settingName}`, { value: value },
            {
                cancelToken: cancelTokenSource?.token,
            }
        );
    }

    const saveSettingsToNVS = async() => {
        await api.get('/settings/storetonvs')
    }

    const reloadSettingsFromNVS = async() => {
        await api.get('/settings/restorefromnvs')
    }

    return {
        groups, groupsLoading, groupsLoadingError,
        fetchGameSettingsList,
        readGameSettingValue,
        writeGameSettingValue,
        saveSettingsToNVS,
        reloadSettingsFromNVS
    };
});