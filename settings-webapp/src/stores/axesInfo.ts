/* eslint-disable @typescript-eslint/no-explicit-any */
import { ref } from 'vue'
import { defineStore } from 'pinia'
import type { CancelTokenSource } from 'axios';

import api from '@/api/api';
import type { AxesInfoResponse, AxisInfoResponse } from '@/api/axesInfoApi'

export const useAxesInfoStore = defineStore('axesinfo', () => {
    // State
    const axes = ref<AxisInfoResponse[]>([]);
    const axesLoading = ref(false);
    const axesLoadingError = ref<string | null>(null);

    // Actions
    const fetchAxesInfo = async() => {
        axesLoading.value = true;
        axesLoadingError.value = null;

        try {
            const response = await api.get<AxesInfoResponse>('/axesinfo');
            axes.value = response.data;
        } catch(err) {
            axesLoadingError.value = 'Failed to get the axes info';
            console.error('/axesinfo API Error:', err);            
        } finally {
            axesLoading.value = false;
        }
    }

    return {
        axes, axesLoading, axesLoadingError,
        fetchAxesInfo,
    };
});