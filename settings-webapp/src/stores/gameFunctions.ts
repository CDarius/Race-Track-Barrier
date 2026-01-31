import { ref } from 'vue'
import { defineStore } from 'pinia'

import api from '@/api/api';
import type { ListGameFunctionsResponse, GameFunctionsGroup } from '@/api/functionsApi'

export const useGameFunctionsStore = defineStore('gamefunctions', () => {
    // State
    const groups = ref<GameFunctionsGroup[]>([]);
    const groupsLoading = ref(false);
    const groupsLoadingError = ref<string | null>(null);

    // Actions
    const fetchGameFunctionsList = async() => {
        groupsLoading.value = true;
        groupsLoadingError.value = null;

        try {
            const response = await api.get<ListGameFunctionsResponse>('/webfunctions');
            groups.value = response.data.functions;
        } catch(err) {
            groupsLoadingError.value = 'Failed to get functions list';
            console.error('/functions API Error:', err);            
        } finally {
            groupsLoading.value = false;
        }
    }

    return {
        groups, groupsLoading, groupsLoadingError,
        fetchGameFunctionsList
    };
});