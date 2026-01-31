<template>
    <div v-if="loading"
        class="h-100 w-100 d-flex flex-column justify-content-center align-items-center">
        <h3>Loading...</h3>
        <div class="spinner-border mt-2" style="width: 3rem; height: 3rem;" role="status">
            <span class="visually-hidden">Loading...</span>
        </div>
    </div>

    <div v-else-if="loadingError" class="container-md">
        <div class="alert alert-danger d-flex align-items-center mt-4" role="alert">
            <i class="bi bi-exclamation-triangle-fill me-2"></i>
            <div>
                {{ loadingError }}
            </div>
        </div>
    </div>

    <PageScrollableLayout v-else>
        <div class="container-md my-3">
            <div class="list-group">
                <router-link v-for="axis in axesInfoStore.axes" :to="'/axistuning/' + axis.name" :key="axis.name"
                    class="list-group-item list-group-item-action">
                    {{ axis.name.toUpperCase() }}-Axis
                    <i class="bi bi-chevron-right float-end"></i>
                </router-link>
            </div>
        </div>
    </PageScrollableLayout>
</template>

<script setup lang="ts">
import { onBeforeMount, computed } from 'vue';
import { useAxesInfoStore } from '@/stores/axesInfo';
import { useGameSettingsStore } from '@/stores/gameSettings'
import { useGameFunctionsStore } from '@/stores/gameFunctions';

import PageScrollableLayout from '@/components/PageScrollableLayout.vue';

const axesInfoStore = useAxesInfoStore();
const settingStore = useGameSettingsStore();
const functionsStore = useGameFunctionsStore();

const loading = computed(() => {
    return axesInfoStore.axesLoading || settingStore.groupsLoading || functionsStore.groupsLoading;
});

const loadingError = computed(() => {
    return axesInfoStore.axesLoadingError || settingStore.groupsLoadingError || functionsStore.groupsLoadingError;
});

onBeforeMount(() => {
    const runFetches = async () => {
        if (axesInfoStore.axes == null || axesInfoStore.axes.length == 0)
            await axesInfoStore.fetchAxesInfo();
        if (settingStore.groups == null || settingStore.groups.length == 0)
            await settingStore.fetchGameSettingsList();
        if (functionsStore.groups == null || functionsStore.groups.length == 0)
            await functionsStore.fetchGameFunctionsList();
    };

    runFetches();
});

</script>